"""Stage 2: ALMaSS design runs -> sampling data set -> emulator.txt.

Run repeatedly (``python -m hbabm.pipeline_emulator``) until all runs are done;
each call works for at most ``--budget`` seconds so it fits short job slots.
"""
from __future__ import annotations

import argparse
import json

import pandas as pd

from . import emulator
from .paths import GENERATED_DIR
from .pipeline_almass import load_calibration, load_targets

DESIGN_CSV = GENERATED_DIR / "almass_design.csv"
SAMPLE_CSV = GENERATED_DIR / "almass_sampling_dataset.csv"
EMULATOR_TXT = GENERATED_DIR / "emulator.txt"
EMULATOR_JSON = GENERATED_DIR / "emulator_fit.json"


def main(budget: float, workers: int) -> None:
    if DESIGN_CSV.exists():
        frame = pd.read_csv(DESIGN_CSV)
    else:
        frame = emulator.design()
        frame.to_csv(DESIGN_CSV, index=False)
    params, equilibrium = load_calibration()
    _, _, pools = load_targets()
    folders = [emulator.RUNS_DIR / f"run_{i:03d}" for i in frame["run_id"]]
    if not all((f / "BatchALMaSS.ini").exists() for f in folders):
        emulator.prepare_runs(frame, params, pools, equilibrium)
    done = emulator.run_pending(folders, budget_s=budget, workers=workers)
    print(f"ALMaSS design runs finished: {done}/{len(folders)}")
    if done < len(folders):
        return
    data = emulator.collect(frame)
    data.to_csv(SAMPLE_CSV, index=False)
    fits = emulator.fit(data)
    EMULATOR_TXT.write_text("\n".join(emulator.emulator_lines(fits)) + "\n")
    EMULATOR_JSON.write_text(json.dumps({o: {"cv_r2": f.cv_r2, "alpha": f.alpha, "tau": f.tau}
                                         for o, f in fits.items()}, indent=2))
    for o, f in fits.items():
        print(f"{o:18s} CV R2 = {f.cv_r2:.3f}  tau = {f.tau:.2f} y")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--budget", type=float, default=150.0)
    parser.add_argument("--workers", type=int, default=4)
    args = parser.parse_args()
    main(args.budget, args.workers)
