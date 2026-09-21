"""Stage 1: calibrate the plant model and prepare the shared ALMaSS inputs."""
from __future__ import annotations

import json

import pandas as pd

from . import config, pfg_model
from .almass_inputs import run_folder
from .config import PFGS
from .paths import GENERATED_DIR, PROCESSED_DIR


def load_targets():
    composition = pd.read_csv(PROCESSED_DIR / "pfg_composition_by_land_use.csv", index_col=0)
    richness = pd.read_csv(PROCESSED_DIR / "richness_by_land_use.csv", index_col=0)
    pools = pd.read_csv(PROCESSED_DIR / "pfg_species_pools.csv", index_col=0)["pool_size"]
    return composition, richness, pools


def calibrate_and_save(out_dir=GENERATED_DIR) -> pfg_model.CalibrationResult:
    out_dir.mkdir(parents=True, exist_ok=True)
    composition, richness, pools = load_targets()
    result = pfg_model.calibrate(composition, richness, pools)
    p = result.params
    summary = {
        "rmse_log_occupancy": result.rmse_log,
        "groups": {g: {"colonisation": float(p.colonisation[i]), "extinction": float(p.extinction[i]),
                       "shade_beta": float(p.shade_beta[i]), "seed_rain": float(p.seed_rain[i]),
                       "sensitivity": {a: float(v[i]) for a, v in p.sensitivity.items()}}
                   for i, g in enumerate(PFGS)},
        "globals": {"canopy_recovery": p.canopy_recovery, "t_min": p.t_min, "t_opt": p.t_opt,
                    "rain_half": p.rain_half},
        "equilibrium_occupancy": result.equilibrium.round(6).to_dict(orient="index"),
        "equilibrium_richness": dict(zip(result.equilibrium.index,
                                         pfg_model.richness(result.equilibrium.values, pools).round(2))),
    }
    (out_dir / "pfg_calibration.json").write_text(json.dumps(summary, indent=2))
    pd.concat({"observed": result.observed, "predicted": result.predicted}, axis=1).to_csv(
        out_dir / "pfg_calibration_fit.csv")
    return result


def load_calibration(path=GENERATED_DIR / "pfg_calibration.json") -> tuple[pfg_model.PFGParameters, pd.DataFrame]:
    """Calibrated parameters and equilibrium occupancy per activity."""
    data = json.loads(path.read_text())
    spec = {"actions": config.pfg_config()["actions"], "groups": data["groups"], "globals": data["globals"]}
    equilibrium = pd.DataFrame(data["equilibrium_occupancy"]).T[list(PFGS)]
    return pfg_model.PFGParameters.from_config(spec), equilibrium


if __name__ == "__main__":
    res = calibrate_and_save()
    print(f"PFG calibration: RMSE(log occupancy) = {res.rmse_log:.3f}")
    run_folder.build_static()
