"""Assemble self-contained ALMaSS run folders for the Hoa Binh landscape.

Static inputs (raster, weather, crop curves, probe files) are written once to
``<work>/almass_static`` and symlinked; each run folder then only holds the
polygon file (activity allocation), plans, PFG parameters and configuration.
Activity k (order of config/model/activities.json) is farmed by ALMaSS farm k
of type UserDefinedFarm{18+k} (the first user-defined farm type that reads the
extended rotation format), whose rotation contains only that activity.
"""
from __future__ import annotations

import json
import shutil
from dataclasses import dataclass
from pathlib import Path

import numpy as np
import pandas as pd

from .. import config
from ..paths import MODELS_DIR, PROCESSED_DIR, WORK_DIR
from ..pfg_model import PFGParameters
from . import landscape as ls
from . import plans, weather

TEMPLATES = MODELS_DIR / "almass" / "templates"
USER_DEFINED_FARM_OFFSET = 14          # tof_UserDefinedFarmN == 14 + N
FIRST_USER_DEFINED_FARM = 18           # UserDefinedFarm18.. read "stock intensity permcrops n crops"
SPECIES_HB_PFG = 4                     # TOP_HB_PFG in BatchALMaSS.ini
STATIC_FILES = ("hb_landscape.lsb", "hb_weather.pre", "curves_20230807.pre",
                *(f"Probe_ob{i}.prb" for i in range(5)))
FIRST_WEATHER_YEAR = 2024
WEATHER_YEARS = 30


@dataclass
class StaticInputs:
    folder: Path
    landscape: ls.Landscape


def build_static(folder: Path = WORK_DIR / "almass_static", spec: ls.LandscapeSpec = ls.LandscapeSpec(),
                 weather_seed: int = 11) -> StaticInputs:
    folder.mkdir(parents=True, exist_ok=True)
    land = ls.build_landscape(spec)
    if not (folder / "hb_landscape.lsb").exists():
        ls.write_lsb(land.raster, folder / "hb_landscape.lsb")
    if not (folder / "hb_weather.pre").exists():
        daily = weather.generate_daily(config.climate_normals(),
                                       range(FIRST_WEATHER_YEAR, FIRST_WEATHER_YEAR + WEATHER_YEARS), weather_seed)
        weather.write_pre(weather.to_hourly(daily), folder / "hb_weather.pre")
    shutil.copy(TEMPLATES / "curves_20230807.pre", folder / "curves_20230807.pre")
    probe = (TEMPLATES / "Probe_ob0.prb").read_text().splitlines()
    probe[2] = "1"                       # annual reporting instead of every time step
    for i in range(5):
        (folder / f"Probe_ob{i}.prb").write_text("\n".join(probe) + "\n")
    return StaticInputs(folder, land)


def farm_index() -> dict[str, int]:
    return {code: i for i, code in enumerate(config.activity_codes())}


def write_run(run_dir: Path, static: StaticInputs, allocation: dict[int, str], params: PFGParameters,
              pools: pd.Series, init_occupancy: dict[str, np.ndarray], init_canopy: dict[str, float],
              years: int, seed: int) -> Path:
    run_dir.mkdir(parents=True, exist_ok=True)
    for name in STATIC_FILES:
        link = run_dir / name
        if not link.exists():
            link.symlink_to(static.folder / name)
    farms = farm_index()
    ls.write_polyref(static.landscape, {pid: farms[a] for pid, a in allocation.items()}, run_dir / "hb_polyref.txt")
    farmref = [f"{len(farms)}"] + [f"{i}\t{USER_DEFINED_FARM_OFFSET + FIRST_USER_DEFINED_FARM + i}"
                                   for i in farms.values()]
    (run_dir / "hb_farmref.txt").write_text("\n".join(farmref) + "\n")
    for code, i in farms.items():
        tov = config.activities()[code].almass_tov
        rotation = f"0\n0\n0\n1\n{tov}\n"   # arable, default intensity, no permanent crops, 1 crop
        (run_dir / f"UserDefinedFarm{FIRST_USER_DEFINED_FARM + i}.rot").write_text(rotation)
    plans.write_management_plans(run_dir / "hb_management_plans.txt")
    plans.write_pfg_parameters(run_dir / "hb_pfg_parameters.txt", params, pools, init_occupancy, init_canopy)
    (run_dir / "TIALMaSSConfig.cfg").write_text(
        'MAP_MAP_FILE (string) = "hb_landscape.lsb"\n'
        'MAP_POLY_FILE (string) = "hb_polyref.txt"\n'
        'MAP_FARMREF_FILE (string) = "hb_farmref.txt"\n'
        'MAP_WEATHER_FILE (string) = "hb_weather.pre"\n'
        'MAP_CROPCURVES_FILE (string) = "curves_20230807.pre"\n'
        'G_FIXEDRANDOMSEQUENCE (bool) = true\n'
        f'G_FIXEDRANDOMSEED (int) = {seed}\n'
        'HB_PLANS_FILE (string) = "hb_management_plans.txt"\n'
        'HB_PFG_PARAMS_FILE (string) = "hb_pfg_parameters.txt"\n'
        'MAP_SAVE_IMAGES (bool) = false\n')
    probes = "\n".join(f"Probe_ob{i}.prb" for i in range(5))
    (run_dir / "BatchALMaSS.ini").write_text(f"5\n{probes}\n./\n{years}\n{SPECIES_HB_PFG}\n")
    return run_dir


def baseline_shares(managers: pd.DataFrame | None = None) -> dict[str, float]:
    """Within-land-type activity shares of the surveyed management units (baseline landscape)."""
    managers = managers if managers is not None else pd.read_csv(PROCESSED_DIR / "land_managers.csv")
    acts = config.activities()
    shares = {}
    for land_type in config.LAND_TYPES:
        codes = config.activities_of(land_type)
        counts = managers["initial_activity"].value_counts().reindex(codes, fill_value=0).astype(float)
        counts += 0.5            # small prior so that no activity is absent from the baseline
        shares.update((counts / counts.sum()).to_dict())
    assert set(shares) == set(acts)
    return shares


def save_json(obj, path: Path) -> None:
    path.write_text(json.dumps(obj, indent=2))
