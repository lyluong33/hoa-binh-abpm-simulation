"""Pattern-oriented calibration of the awareness value AW_VALUE.

AW_VALUE (EUR/ha at awareness 1) is the only free behavioural parameter of the
SES extension. It is chosen so that, in the first simulated year of the
baseline, the farm agents reproduce the activities reported in the survey
(``initial_activity``): the share of surveyed land managers whose dominant
activity in AgriPoliS equals the surveyed one is maximised. Perennial
activities are partly pinned by the change limits, so the choice is informative
mainly for maize intensity and forest use.
"""
from __future__ import annotations

import pandas as pd

from . import config
from .agripolis_inputs import policy, region, ses_files
from .paths import PROCESSED_DIR, RESULTS_DIR
from .scenario_runs import AGRIPOLIS_BINARY, SCENARIO_DIR

import subprocess

CANDIDATES = (0.0, 25.0, 50.0, 75.0, 100.0, 150.0, 200.0, 300.0)
PERIODS = 2


def run_candidate(aw_value: float, managers: pd.DataFrame) -> pd.DataFrame:
    name = f"CAL_aw{aw_value:g}"
    folder = SCENARIO_DIR / name
    files = {"ses.txt": ses_files.ses_lines(ses_files.SesOptions(aw_value=aw_value)),
             "farm_attributes.txt": ses_files.attribute_lines(managers)}
    region.write_region(folder / "inputfiles", name, policy.policy_lines("S0_BASELINE", PERIODS), files,
                        region.RegionSettings(periods=PERIODS), managers)
    with open(folder / "agripolis.log", "w") as log:
        subprocess.run([str(AGRIPOLIS_BINARY), str(folder / "inputfiles") + "/"], cwd=folder,
                       stdout=log, stderr=subprocess.STDOUT, check=True)
    return pd.read_csv(folder / f"outputfiles_{name}" / "ses_farms.dat", sep="\t")


def match_score(farms: pd.DataFrame, managers: pd.DataFrame) -> float:
    first = farms[farms["iteration"] == 0]
    acts = config.activity_codes()
    dominant = first.set_index("farm_name")[acts].groupby(level=0).mean().idxmax(axis=1)
    surveyed = managers.set_index("manager_id")["initial_activity"]
    return float((dominant.reindex(surveyed.index) == surveyed).mean())


def calibrate() -> pd.DataFrame:
    managers = pd.read_csv(PROCESSED_DIR / "land_managers.csv")
    rows = [{"aw_value": v, "match": match_score(run_candidate(v, managers), managers)} for v in CANDIDATES]
    table = pd.DataFrame(rows)
    RESULTS_DIR.mkdir(exist_ok=True)
    table.to_csv(RESULTS_DIR / "calibration_aw_value.csv", index=False)
    return table


if __name__ == "__main__":
    print(calibrate().to_string(index=False))
