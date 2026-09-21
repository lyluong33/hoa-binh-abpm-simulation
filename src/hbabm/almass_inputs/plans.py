"""Writers for the HB management-plan and PFG parameter files read by ALMaSS."""
from __future__ import annotations

from pathlib import Path

import numpy as np
import pandas as pd

from .. import config
from ..config import PFGS
from ..pfg_model import PFGParameters


def write_management_plans(path: Path) -> None:
    lines = ["# tov action day_of_year window_days intensity probability  (generated from config/model/activities.json)"]
    for act in config.activities().values():
        for e in act.events:
            lines.append(f"{act.almass_tov}\t{e.action}\t{e.day_of_year}\t{e.window_days}\t{e.intensity}\t{e.probability}")
    path.write_text("\n".join(lines) + "\n")


def write_pfg_parameters(path: Path, params: PFGParameters, pools: pd.Series,
                         init_occupancy: dict[str, np.ndarray], init_canopy: dict[str, float]) -> None:
    acts = config.activities()
    actions = config.pfg_config()["actions"]
    lines = ["# PFG name colonisation extinction shade_beta seed_rain pool s_" + " s_".join(actions)]
    for g, name in enumerate(PFGS):
        sens = " ".join(f"{params.sensitivity[a][g]:.5f}" for a in actions)
        lines.append(f"PFG {name} {params.colonisation[g]:.5f} {params.extinction[g]:.5f} "
                     f"{params.shade_beta[g]:.5f} {params.seed_rain[g]:.3f} {int(pools[name])} {sens}")
    lines.append("# target canopy cover per vegetation type")
    lines += [f"CANOPY {a.almass_tov} {a.canopy_cover}" for a in acts.values()]
    lines.append("# initial state per vegetation type")
    for code, act in acts.items():
        occ = " ".join(f"{v:.6f}" for v in init_occupancy[code])
        lines.append(f"INIT {act.almass_tov} {occ}")
        lines.append(f"CANOPY_INIT {act.almass_tov} {init_canopy[code]:.4f}")
    lines += [f"GLOBAL CANOPY_RECOVERY {params.canopy_recovery}", f"GLOBAL T_MIN {params.t_min}",
              f"GLOBAL T_OPT {params.t_opt}", f"GLOBAL RAIN_HALF {params.rain_half}"]
    path.write_text("\n".join(lines) + "\n")
