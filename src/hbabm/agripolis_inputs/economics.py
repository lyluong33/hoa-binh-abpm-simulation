"""Per-hectare economics and policy payments of the nine activities in EUR."""
from __future__ import annotations

from dataclasses import dataclass

from .. import config

HOURS_PER_DAY = 8.0


@dataclass(frozen=True)
class ActivityEconomics:
    code: str
    land_type: str
    price_eur: float        # output value per ha and year
    var_cost_eur: float     # variable cost + annualised establishment cost
    labour_hours: float


def vnd_per_eur() -> float:
    return float(config.activity_economics()["vnd_per_eur"])


def activity_economics() -> dict[str, ActivityEconomics]:
    rate = vnd_per_eur()
    result = {}
    for code, spec in config.activity_economics()["activities"].items():
        establishment = spec.get("establishment_cost_annualised_vnd_per_ha_yr", 0.0) or 0.0
        result[code] = ActivityEconomics(
            code=code, land_type=spec["land_type"],
            price_eur=spec["output_value_vnd_per_ha_yr"] / rate,
            var_cost_eur=(spec["variable_cost_vnd_per_ha_yr"] + establishment) / rate,
            labour_hours=spec["labour_days_per_ha_yr"] * HOURS_PER_DAY,
        )
    missing = set(config.activity_codes()) - set(result)
    if missing:
        raise ValueError(f"economics missing for {missing}")
    return result


def scenario(scenario_id: str) -> dict:
    for spec in config.scenarios()["scenarios"]:
        if spec["id"] == scenario_id:
            return spec
    raise KeyError(scenario_id)


def payments_eur(scenario_id: str) -> dict[str, float]:
    """Full-rate coupled payment per activity (recurring + annualised one-off support), EUR/ha/yr."""
    spec = scenario(scenario_id)
    rate = vnd_per_eur()
    total = {code: 0.0 for code in config.activity_codes()}
    for key in ("payments_vnd_per_ha_yr", "annualised_support_vnd_per_ha_yr"):
        for code, value in spec.get(key, {}).items():
            total[code] += value / rate
    return total


def payment_factor(scenario_id: str, period: int) -> float:
    """Phasing factor of a scenario's payments in simulation period ``period`` (1 = 2025)."""
    factor = 1.0
    for step in sorted(scenario(scenario_id).get("phasing") or [], key=lambda s: s["from_period"]):
        if period >= step["from_period"]:
            factor = step["factor"]
    return factor
