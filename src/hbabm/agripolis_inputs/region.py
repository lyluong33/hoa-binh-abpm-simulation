"""Build the Hoa Binh AgriPoliS input directory from the Altmark template.

The template's structural parts that AgriPoliS hard-codes (livestock,
investments, quota, premium tranches, standard names) are kept but become
inactive because no Hoa Binh farm owns livestock or machinery. What changes:

* three land (soil) types FARMLAND, PRODUCTION_FOREST, PROTECTION_FOREST,
* the crop products are replaced by the nine activities (EUR/ha, hours/ha),
* the land rows and the coupled-premium row of the farm MIP,
* one typical farm per surveyed land manager (farmsdata.txt, farms/*.txt),
* globals for plot size, labour and household consumption in Vietnam,
* policy_settings.txt per scenario (see policy.py),
* awareness.txt, farm_attributes.txt and the emulator file for the SES module.
"""
from __future__ import annotations

import json
import re
import shutil
from dataclasses import dataclass
from pathlib import Path

import pandas as pd

from .. import config
from ..paths import MODELS_DIR, PROCESSED_DIR
from . import economics
from .tables import fmt, read_lines, row, write_lines

TEMPLATE = MODELS_DIR / "agripolis" / "templates" / "altmark_inputfiles"
REMOVED_PRODUCTS = ["BARLEY", "WINTER_WHEAT", "SUMMER_WHEAT", "SUGAR_BEETS", "MAIZE_SILAGE", "GRAIN_MAIZE",
                    "PASTURE", "GRASS_SILAGE", "POTATOES", "TRITICALE", "SPELT", "IDLE_ARABLE", "IDLE_GRASS"]
ROTATION_ROWS = ["crops", "wheat_winter", "wheat_summer", "surgar_beets", "triticale_min_8", "spelt_min_5",
                 "potatoes_max_10%"]
OLD_LAND_ROWS = ["Arable_Land", "Grazing_Land"]
FIRST_ACTIVITY_ID = 5
FINANCING_SHARE = {"FARMLAND": 0.4, "PRODUCTION_FOREST": 0.2, "PROTECTION_FOREST": 0.0}
# Fixed (yearly) labour contracts are switched off: upland households hire and sell
# labour by the hour (V_HIRED_LABOUR, V_OFF_FARM_LAB). The template leaves their
# objective coefficient at zero in the first MIP, which makes the MIP unbounded.
LABOUR_SUB_HOURS = 900.0      # template value of Labour_sub (hours per fixed labour object)
EXTRA_ROWS = {"no_fixed_hired_labour": "HIREDLAB", "no_fixed_off_farm_labour": "OFFFARMLAB"}


@dataclass(frozen=True)
class RegionSettings:
    plot_size_ha: float = 0.25
    household_copies: int = 6          # each surveyed household stands for this many agents
    community_copies: int = 1
    rent_eur_per_ha: tuple[float, float, float] = (100.0, 35.0, 5.0)
    land_value_eur_per_ha: tuple[float, float, float] = (3000.0, 1000.0, 0.0)
    base_equity_eur: float = 3000.0
    asset_equity_eur: float = 10000.0
    off_farm_wage_eur_per_hour: float = 0.85   # 200,000 VND/day / 8 h / 29,424 VND/EUR
    off_farm_availability: float = 0.5         # share of family hours that can find waged work
    hired_wage_eur_per_hour: float = 1.0
    # minimum consumption withdrawn per family labour unit and year: rural poverty
    # line 2022-2025 (1.5 M VND/person/month) ~ 18 M VND/yr ~ 600 EUR
    consumption_per_labour_unit_eur: float = 600.0
    periods: int = 18


# ---------------------------------------------------------------------------
# market.txt
# ---------------------------------------------------------------------------

def market_lines(initial_payment_eur: dict[str, float]) -> list[str]:
    econ = economics.activity_economics()
    lines = []
    for line in read_lines(TEMPLATE / "market.txt"):
        cells = line.split("\t")
        name = cells[4] if len(cells) > 4 else ""
        if name in REMOVED_PRODUCTS:
            continue
        lines.append(line)
        if name == "AGRI_SERVICES":   # activities follow the services row (ids 5..13)
            for i, code in enumerate(config.activity_codes()):
                e = econ[code]
                prem = initial_payment_eur.get(code, 0.0)
                lines.append(row(FIRST_ACTIVITY_ID + i, 4, 0, code, "-", fmt(e.price_eur), fmt(e.var_cost_eur),
                                 fmt(e.labour_hours), 0, 1, "ARABLE", 0, "TRUE", fmt(prem), "TRUE", fmt(prem), 1))
    return _renumber_products(lines)


def _renumber_products(lines: list[str]) -> list[str]:
    result, next_id = [], None
    for line in lines:
        cells = line.split("\t")
        if len(cells) > 5 and cells[1].strip().isdigit() and cells[2].strip() == "4":
            next_id = int(cells[1]) if next_id is None else next_id + 1
            cells[1] = str(next_id)
            line = "\t".join(cells)
        result.append(line)
    return result


# ---------------------------------------------------------------------------
# mip/*.txt
# ---------------------------------------------------------------------------

def _strip_terms(expression: str, names: list[str]) -> str:
    """Remove '+coef*NAME' / '+NAME' terms of the given product names from a row expression."""
    for name in names:
        expression = re.sub(r"\+?\s*(\(?-?[\d.]+\)?\s*\*)?\s*\b" + re.escape(name) + r"\b\s*", " ", expression)
    expression = re.sub(r"^\s*\+\s*", "", expression.strip())
    return expression or "0*SOWS"


def matrix_lines(initial_payment_eur: dict[str, float], settings: RegionSettings) -> list[str]:
    """Farm MIP rows: template rows without crops, three land rows, labour caps, payments."""
    acts_by_land = {lt: config.activities_of(lt) for lt in config.LAND_TYPES}
    lines = []
    for line in read_lines(TEMPLATE / "mip" / "matrix_new.txt"):
        name, _, expr = line.partition("\t")
        if name in ROTATION_ROWS or name == "Grazing_Land":
            continue
        if name == "Arable_Land":
            for land_type, codes in acts_by_land.items():
                lines.append(f"{land_type}\t" + " +".join(codes))
            continue
        if name == "LU_upper_limit":
            lines.append(line)
            lines += [f"{row_name}\t{column}" for row_name, column in EXTRA_ROWS.items()]
            # (1/availability) * off-farm hours <= family hours  (rhs linked to the labour capacity)
            lines.append(f"off_farm_labour_cap\t{1.0 / settings.off_farm_availability:g}*V_OFF_FARM_LAB")
            continue
        if name == "coupled_premium":
            terms = " ".join(f"+({-initial_payment_eur.get(c, 0.0):.2f})*{c}" for c in config.activity_codes())
            lines.append(f"coupled_premium\t{terms.lstrip('+')} +COUPLED_PREM_UNMOD")
            continue
        if expr and not name.startswith(("#", "_")):
            line = f"{name}\t{_strip_terms(expr, REMOVED_PRODUCTS)}"
        lines.append(line)
    return lines


def matrix_links_lines() -> list[str]:
    lines = []
    for line in read_lines(TEMPLATE / "mip" / "matrixLinks.txt"):
        cells = line.split("\t")
        if len(cells) > 2 and cells[2].strip() in REMOVED_PRODUCTS:
            continue
        lines.append(line)
        if len(cells) > 2 and cells[2].strip() == "EXCESS_LU":
            pass
    acts = config.activities()
    insert_at = next(i for i, l in enumerate(lines) if "liquidity/financing_rule" in l)
    new = [row("liquidity/financing_rule", c, "  market ", "C", FINANCING_SHARE[acts[c].land_type])
           for c in config.activity_codes()]
    return lines[:insert_at] + new + lines[insert_at:]


def capacity_links_lines(settings: RegionSettings) -> list[str]:
    lines = []
    for line in read_lines(TEMPLATE / "mip" / "capacityLinks.txt"):
        cells = line.split("\t")
        name = cells[1].strip() if len(cells) > 1 else ""
        if name in ROTATION_ROWS or name == "Grazing_Land":
            continue
        if name == "Arable_Land":
            lines += [row(lt, "land", "", lt, 1) for lt in config.LAND_TYPES]
            continue
        lines.append(line)
        if name == "LU_upper_limit":
            lines += [row(row_name, "number", 0, "", "") for row_name in EXTRA_ROWS]
            lines.append(row("off_farm_labour_cap", "reference", "", "labour", 1))
    return lines


# ---------------------------------------------------------------------------
# globals.txt, options.txt, scenario.txt, demographics.txt
# ---------------------------------------------------------------------------

def globals_lines(settings: RegionSettings) -> list[str]:
    replace = {
        "Plotsize": fmt(settings.plot_size_ha), "Number_of_soil_types": "3",
        "Names_of_soil_types": "\t".join(config.LAND_TYPES),
        "transport_costs": "5", "capital_withdraw_factor": fmt(settings.consumption_per_labour_unit_eur),
        "V_HIRED_LABOUR": fmt(-settings.hired_wage_eur_per_hour),
        "V_OFF_FARM_LAB": fmt(settings.off_farm_wage_eur_per_hour),
        # AgriPoliS derives the farm family's opportunity cost of labour from the
        # fixed off-farm labour object: OFFFARMLAB * hours_per_unit / Labour_sub.
        # With Labour_sub = 900 h this equals the hourly off-farm wage.
        # The expected wage (wage x availability) is used, consistent with the
        # off-farm labour cap in the MIP (Harris-Todaro expected income).
        "HIREDLAB": fmt(settings.hired_wage_eur_per_hour * LABOUR_SUB_HOURS),
        "OFFFARMLAB": fmt(-settings.off_farm_wage_eur_per_hour * settings.off_farm_availability
                          * LABOUR_SUB_HOURS),
    }
    lines = []
    for line in read_lines(TEMPLATE / "globals.txt"):
        cells = line.split("\t")
        key = cells[1].strip() if len(cells) > 2 else ""
        if key in replace:
            line = f"\t{key}\t{replace[key]}\t"
        lines.append(line)
    return lines


def options_lines(settings: RegionSettings) -> list[str]:
    lines = []
    for line in read_lines(TEMPLATE / "options.txt"):
        if re.match(r"\s*TEILER\s", line):
            line = "\tTEILER\t1\t\t"
        elif re.match(r"\s*RUNS\s", line):
            line = f"\tRUNS\t{settings.periods}\t"
        lines.append(line)
    return lines


def scenario_lines(name: str) -> list[str]:
    lines = []
    for line in read_lines(TEMPLATE / "scenario.txt"):
        if line.startswith("Scenario:"):
            line = f"Scenario:\t{name}\t"
        elif line.startswith("Description:"):
            line = "Description: Hoa Binh upland land-use policy scenario\t\t"
        elif line.startswith("Teiler"):
            line = "Teiler\t1\t"
        elif line.startswith("Rent_variation"):
            line = "Rent_variation\tfalse"
        lines.append(line)
    return lines


def demographics_lines(managers: pd.DataFrame) -> list[str]:
    # individual ages are not published; only their aggregate (data/processed/age_summary.json)
    ages = json.loads((PROCESSED_DIR / "age_summary.json").read_text())
    replace = {"FF_initAge_min": ages["min"], "FF_initAge_max": min(ages["max"], 75),
               "FF_initAge_mean": ages["mean"], "FF_initAge_dev": ages["sd"]}
    lines = []
    for line in read_lines(TEMPLATE / "demographics.txt"):
        key = line.split("\t")[0]
        if key in replace:
            line = f"{key}\t{fmt(round(replace[key], 2))}"
        lines.append(line)
    return lines


# ---------------------------------------------------------------------------
# farms
# ---------------------------------------------------------------------------

def farm_table(managers: pd.DataFrame, settings: RegionSettings) -> pd.DataFrame:
    """One typical farm per land manager with land (ha) by land type and capital."""
    table = managers.copy()
    table["copies"] = table["is_community_group"].map({True: settings.community_copies,
                                                        False: settings.household_copies})
    table["area_ha"] = table["area_ha"].clip(lower=settings.plot_size_ha)
    table["owned_share"] = (~table["tenure"].isin(["rental", "working contract"])).astype(float)
    value = dict(zip(config.LAND_TYPES, settings.land_value_eur_per_ha))
    table["land_assets"] = table["area_ha"] * table["land_type"].map(value) * table["owned_share"]
    table["equity"] = (table["land_assets"] + settings.base_equity_eur
                       + settings.asset_equity_eur * table["asset_index"])
    table["organisation"] = table["is_community_group"].map({True: 2, False: 1})
    return table


def farmsdata_lines(table: pd.DataFrame, settings: RegionSettings) -> list[str]:
    names = table["manager_id"].tolist()
    lines = ["#\tHoa Binh land managers (anonymised survey, Dec 2024) - generated by hbabm", "",
             row("NumOfFarms", len(names)), row("Name", *names),
             row("Farm_Type", *[3] * len(names)),
             row("Form_of_Organisation", *table["organisation"]),
             row("Weighting_Factor", *table["copies"]), "",
             row("land_input", *[fmt(a) for a in table["area_ha"]]), ""]
    rents = dict(zip(config.LAND_TYPES, settings.rent_eur_per_ha))
    for land_type in config.LAND_TYPES:
        on_type = (table["land_type"] == land_type).astype(float)
        owned = table["area_ha"] * on_type * table["owned_share"]
        rented = table["area_ha"] * on_type * (1 - table["owned_share"])
        lines += [f"\t{land_type}", row("owned_land", *[fmt(v) for v in owned]),
                  row("rented_land", *[fmt(v) for v in rented]),
                  row("initial_rental_price", *[fmt(rents[land_type])] * len(names)), ""]
    lines += [row("milk_quota", *[0] * len(names)),
              row("fam_labour_units", *[fmt(v) for v in table["labour_units"]]),
              row("off_fam_labour", *[0] * len(names)),
              row("equity_capital", *[fmt(round(v)) for v in table["equity"]]),
              row("land_assets", *[fmt(round(v)) for v in table["land_assets"]]),
              row("rel_invest_age", *[0] * len(names))]
    return lines


def farm_file_lines(name: str) -> list[str]:
    return [f"FarmName  {name}\t\t", "\t\t", "!              Investition\tAnzahl\tKapazitaet", "\t\t"]


# ---------------------------------------------------------------------------
# assemble
# ---------------------------------------------------------------------------

def write_region(out_dir: Path, scenario_name: str, policy_lines: list[str], ses_files: dict[str, list[str]],
                 settings: RegionSettings = RegionSettings(), managers: pd.DataFrame | None = None) -> Path:
    managers = managers if managers is not None else pd.read_csv(PROCESSED_DIR / "land_managers.csv")
    initial = economics.payments_eur("S0_BASELINE")
    if out_dir.exists():
        shutil.rmtree(out_dir)
    (out_dir / "farms").mkdir(parents=True)
    table = farm_table(managers, settings)
    write_lines(out_dir / "market.txt", market_lines(initial))
    write_lines(out_dir / "mip" / "matrix_new.txt", matrix_lines(initial, settings))
    write_lines(out_dir / "mip" / "matrixLinks.txt", matrix_links_lines())
    write_lines(out_dir / "mip" / "capacityLinks.txt", capacity_links_lines(settings))
    shutil.copy(TEMPLATE / "mip" / "objFuncLinks.txt", out_dir / "mip" / "objFuncLinks.txt")
    shutil.copy(TEMPLATE / "investments.txt", out_dir / "investments.txt")
    shutil.copy(TEMPLATE / "debug_mip.txt", out_dir / "debug_mip.txt")
    write_lines(out_dir / "globals.txt", globals_lines(settings))
    write_lines(out_dir / "options.txt", options_lines(settings))
    write_lines(out_dir / "scenario.txt", scenario_lines(scenario_name))
    write_lines(out_dir / "demographics.txt", demographics_lines(managers))
    write_lines(out_dir / "farmsdata.txt", farmsdata_lines(table, settings))
    for name in table["manager_id"]:
        write_lines(out_dir / "farms" / f"{name}.txt", farm_file_lines(name))
    write_lines(out_dir / "policy_settings.txt", policy_lines)
    for filename, lines in ses_files.items():
        write_lines(out_dir / filename, lines)
    return out_dir
