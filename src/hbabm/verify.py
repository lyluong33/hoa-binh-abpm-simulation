"""Stage 5: independent checks of the coupled model.

* Emulator vs ALMaSS: the 2042 land-use composition of each scenario (two-way
  run) is simulated directly in ALMaSS (15 years); its equilibrium richness is
  compared with the emulator's equilibrium value.
* One-way coupling must not change land use (checked in analysis.key_findings).
"""
from __future__ import annotations

import pandas as pd

from . import config, emulator
from .paths import GENERATED_DIR, RESULTS_DIR, WORK_DIR
from .pipeline_almass import load_calibration, load_targets

CHECK_DIR = WORK_DIR / "almass_verify"


def compositions() -> pd.DataFrame:
    d = pd.read_csv(RESULTS_DIR / "scenario_results.csv")
    last = d[(d["mode"] == "twoway") & (d["year"] == d["year"].max())].set_index("scenario")
    rows = []
    for i, (scenario, row) in enumerate(last.iterrows()):
        shares = {}
        for land_type in config.LAND_TYPES:
            codes = config.activities_of(land_type)
            total = row[codes].sum()
            shares.update({c: row[c] / total for c in codes})
        rows.append({"run_id": i, "scenario": scenario, **shares,
                     "emulator_richness": row["richness_equilibrium"],
                     "emulator_richness_FARMLAND": row["richness_FARMLAND_equilibrium"]})
    return pd.DataFrame(rows)


def run(budget: float = 150.0) -> pd.DataFrame | None:
    frame = compositions()
    params, equilibrium = load_calibration()
    _, _, pools = load_targets()
    folders = emulator.prepare_runs(frame, params, pools, equilibrium, runs_dir=CHECK_DIR)
    if emulator.run_pending(folders, budget_s=budget) < len(folders):
        return None
    checked = emulator.collect(frame, runs_dir=CHECK_DIR)
    result = checked[["scenario", "emulator_richness", "richness", "emulator_richness_FARMLAND",
                      "richness_FARMLAND"]].rename(columns={"richness": "almass_richness",
                                                            "richness_FARMLAND": "almass_richness_FARMLAND"})
    result["error_pct"] = 100 * (result["emulator_richness"] / result["almass_richness"] - 1)
    result["error_FARMLAND_pct"] = 100 * (result["emulator_richness_FARMLAND"] / result["almass_richness_FARMLAND"] - 1)
    result.round(3).to_csv(RESULTS_DIR / "verification_emulator_vs_almass.csv", index=False)
    return result


if __name__ == "__main__":
    out = run()
    print("not finished" if out is None else out.round(2).to_string(index=False))
