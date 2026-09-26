"""Stage 3: Vietnamese policy scenarios with uncoupled and coupled AgriPoliS.

Model configurations (``MODES``):

* ``uncoupled`` - AgriPoliS + SES extension without ALMaSS. Ecology can only be
  judged from land use: the stewardship-area share and a species-area
  relationship on semi-natural land (the approach of AgriPoliS' own
  environmental module).
* ``oneway``    - the ALMaSS emulator runs inside AgriPoliS and reports the
  process-based indicators every period; no feedback (land use must equal
  ``uncoupled``).
* ``twoway``    - as ``oneway`` and the experienced loss of richness on the land
  type a manager works on raises his awareness (AW_FEEDBACK), which can change
  land-use decisions (``_landscape`` variant: loss of landscape richness).
* ``twoway_nolag`` - ``twoway`` with the ecological relaxation time set to
  (almost) zero, to isolate the role of ecological inertia.
"""
from __future__ import annotations

import os
import re
import subprocess
import time
from dataclasses import dataclass, field
from pathlib import Path

import numpy as np
import pandas as pd

from . import config
from .agripolis_inputs import policy, region, ses_files
from .paths import GENERATED_DIR, PROCESSED_DIR, RESULTS_DIR, WORK_DIR

AGRIPOLIS_BINARY = Path(os.environ.get("HB_AGRIPOLIS_BIN", WORK_DIR / "agp24"))
SCENARIO_DIR = WORK_DIR / "scenarios"
PERIODS = 18
FIRST_YEAR = 2025
FEEDBACK = 3.0
SAR_Z = 0.25
SEMI_NATURAL = ("FALLOW", "NATIVE_MIX", "REGEN", "PROTECT_STRICT", "PROTECT_USE")
MODES = ("uncoupled", "oneway", "twoway", "twoway_nolag")


@dataclass(frozen=True)
class RunSpec:
    scenario: str
    mode: str
    feedback: float = FEEDBACK
    tag: str = ""
    price_scale: float | None = None   # sensitivity: size of the maize price shift
    perception: str = "local"

    @property
    def name(self) -> str:
        return f"{self.scenario}__{self.mode}{self.tag}"


def emulator_lines(mode: str) -> list[str]:
    lines = (GENERATED_DIR / "emulator.txt").read_text().splitlines()
    if mode == "twoway_nolag":
        lines = [re.sub(r"^(OUTPUT \S+) \S+$", r"\1 0.01", l) for l in lines]
    return lines


def ses_options(spec: RunSpec) -> ses_files.SesOptions:
    campaign = policy.economics.scenario(spec.scenario).get("extension_campaign") or {}
    feedback = spec.feedback if spec.mode.startswith("twoway") else 0.0
    return ses_files.SesOptions(
        feedback=feedback, perception=spec.perception,
        ext_coverage=campaign.get("coverage", 0.0),
        ext_effect=campaign.get("awareness_effect_per_year", 0.0),
        ext_start=campaign.get("start_period", 1 << 30) - 1 if campaign else 1 << 30,
    )


def prepare(spec: RunSpec, managers: pd.DataFrame) -> Path:
    folder = SCENARIO_DIR / spec.name
    files = {"ses.txt": ses_files.ses_lines(ses_options(spec)),
             "farm_attributes.txt": ses_files.attribute_lines(managers)}
    if spec.mode != "uncoupled":
        files["emulator.txt"] = emulator_lines(spec.mode)
    region.write_region(folder / "inputfiles", spec.name,
                        policy.policy_lines(spec.scenario, PERIODS, spec.price_scale), files,
                        region.RegionSettings(periods=PERIODS), managers)
    return folder


def output_dir(spec: RunSpec) -> Path:
    return SCENARIO_DIR / spec.name / f"outputfiles_{spec.name}"


def is_done(spec: RunSpec) -> bool:
    farms = output_dir(spec) / "ses_farms.dat"
    return farms.exists() and farms.read_text().count(f"\n{PERIODS - 1}\t") > 0


def run_all(specs: list[RunSpec], budget_s: float = 150.0, workers: int = 4) -> int:
    managers = pd.read_csv(PROCESSED_DIR / "land_managers.csv")
    pending = [s for s in specs if not is_done(s)]
    running: list[tuple[subprocess.Popen, RunSpec]] = []
    start = time.time()
    while (pending or running) and time.time() - start < budget_s:
        while pending and len(running) < workers:
            spec = pending.pop(0)
            folder = prepare(spec, managers)
            log = open(folder / "agripolis.log", "w")
            running.append((subprocess.Popen([str(AGRIPOLIS_BINARY), str(folder / "inputfiles") + "/"],
                                             cwd=folder, stdout=log, stderr=subprocess.STDOUT), spec))
        for proc, spec in list(running):
            if proc.poll() is not None:
                running.remove((proc, spec))
        time.sleep(0.5)
    for proc, _ in running:
        proc.wait()
    return sum(is_done(s) for s in specs)


# ---------------------------------------------------------------------------
# collect
# ---------------------------------------------------------------------------

def land_use(spec: RunSpec) -> pd.DataFrame:
    farms = pd.read_csv(output_dir(spec) / "ses_farms.dat", sep="\t")
    acts = config.activity_codes()
    table = farms.groupby("iteration")[acts].sum()
    table["awareness"] = farms.groupby("iteration")["awareness"].mean()
    table["awareness_sd"] = farms.groupby("iteration")["awareness"].std()
    return table


def area_indicators(table: pd.DataFrame) -> pd.DataFrame:
    acts = config.activities()
    total = table[list(acts)].sum(axis=1)
    stewardship = table[[a for a, v in acts.items() if v.stewardship]].sum(axis=1)
    semi_natural = table[list(SEMI_NATURAL)].sum(axis=1)
    return pd.DataFrame({"stewardship_share": stewardship / total,
                         "sar_index": 100 * (semi_natural / semi_natural.iloc[0]) ** SAR_Z})


def collect(specs: list[RunSpec]) -> pd.DataFrame:
    frames = []
    for spec in specs:
        if not is_done(spec):
            continue
        table = land_use(spec).join(area_indicators(land_use(spec)))
        eco_file = output_dir(spec) / "ses_landscape.dat"
        if eco_file.exists():
            eco = pd.read_csv(eco_file, sep="\t").set_index("iteration")
            table = table.join(eco[[c for c in eco.columns if not c.startswith("share_")]])
        sector = pd.read_csv(output_dir(spec) / "sector.dat", sep="\t", skipinitialspace=True)
        sector.columns = [c.strip() for c in sector.columns]
        table["total_income_eur"] = sector.groupby("period")["total_income"].sum().values[: len(table)]
        table = table.reset_index().rename(columns={"index": "iteration"})
        table.insert(0, "mode", spec.mode + spec.tag)
        table["price_scale"] = spec.price_scale if spec.price_scale else np.nan
        table.insert(0, "scenario", spec.scenario)
        table["year"] = FIRST_YEAR + table["iteration"]
        frames.append(table)
    return pd.concat(frames, ignore_index=True)


def default_specs() -> list[RunSpec]:
    scenarios = [s["id"] for s in config.scenarios()["scenarios"]]
    specs = [RunSpec(s, m) for s in scenarios for m in MODES]
    specs += [RunSpec(s, "twoway", f, f"_fb{f:g}") for s in ("S0_BASELINE", "S4_MAIZE_PRESSURE") for f in (1.0, 6.0)]
    specs += [RunSpec("S4_MAIZE_PRESSURE", m, FEEDBACK, f"_p{p:g}", p)
              for p in (1.05, 1.10, 1.15, 1.20) for m in ("oneway", "twoway", "twoway_nolag")]
    specs += [RunSpec(s, "twoway", FEEDBACK, "_landscape", perception="landscape")
              for s in ("S0_BASELINE", "S4_MAIZE_PRESSURE")]
    return specs


if __name__ == "__main__":
    import argparse
    parser = argparse.ArgumentParser()
    parser.add_argument("--budget", type=float, default=150.0)
    parser.add_argument("--workers", type=int, default=4)
    args = parser.parse_args()
    specs = default_specs()
    done = run_all(specs, args.budget, args.workers)
    print(f"scenario runs finished: {done}/{len(specs)}")
    if done == len(specs):
        RESULTS_DIR.mkdir(exist_ok=True)
        collect(specs).to_csv(RESULTS_DIR / "scenario_results.csv", index=False)
        print("results/scenario_results.csv written")
