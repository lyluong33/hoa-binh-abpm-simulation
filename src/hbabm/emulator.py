"""ALMaSS sampling design, batch runs and the emulator used inside AgriPoliS.

1. ``design``: land-use compositions (shares within each land type) - all
   vertex landscapes, the surveyed baseline and Dirichlet samples.
2. ``prepare_runs`` / ``run_pending``: one ALMaSS run folder per design point,
   all starting from the baseline landscape state, executed in parallel.
3. ``collect``: per run the equilibrium indicators (mean of the last years) and
   the relaxation time tau of richness towards its equilibrium.
4. ``fit`` / ``write_emulator``: ridge-regularised quadratic response surface in
   the shares (linear + pairwise products), exported as emulator.txt.
"""
from __future__ import annotations

import itertools
import os
import subprocess
import time
from dataclasses import dataclass
from pathlib import Path

import numpy as np
import pandas as pd
from scipy.optimize import curve_fit

from . import config, rd
from .almass_inputs import landscape as ls
from .almass_inputs import run_folder
from .config import PFGS
from .paths import WORK_DIR

RUNS_DIR = WORK_DIR / "almass_design"
ALMASS_BINARY = Path(os.environ.get("HB_ALMASS_BIN", WORK_DIR / "almass_cmd"))
YEARS = 15
EQUILIBRIUM_YEARS = 4
OUTPUTS = ("richness", "rd_divergence", "rd_dissimilarity", "tree_occupancy")


# ---------------------------------------------------------------------------
# design
# ---------------------------------------------------------------------------

def design(n_random: int = 125, seed: int = 1) -> pd.DataFrame:
    rng = np.random.default_rng(seed)
    groups = [config.activities_of(lt) for lt in config.LAND_TYPES]
    rows = [run_folder.baseline_shares()]
    for vertex in itertools.product(*groups):
        rows.append({a: float(a in vertex) for g in groups for a in g})
    for _ in range(n_random):
        row = {}
        for g in groups:
            row.update(zip(g, rng.dirichlet(np.full(len(g), 0.8))))
        rows.append(row)
    frame = pd.DataFrame(rows)[config.activity_codes()]
    frame.insert(0, "run_id", range(len(frame)))
    return frame


# ---------------------------------------------------------------------------
# runs
# ---------------------------------------------------------------------------

def baseline_initial_state(equilibrium: pd.DataFrame) -> tuple[dict, dict]:
    """Every patch starts from the mean state of its land type in the baseline landscape."""
    shares = run_folder.baseline_shares()
    acts = config.activities()
    occupancy, canopy = {}, {}
    for land_type in config.LAND_TYPES:
        codes = config.activities_of(land_type)
        occ = sum(shares[a] * equilibrium.loc[a].values for a in codes)
        can = sum(shares[a] * acts[a].canopy_cover for a in codes)
        for a in codes:
            occupancy[a], canopy[a] = occ, can
    return occupancy, canopy


def prepare_runs(frame: pd.DataFrame, params, pools: pd.Series, equilibrium: pd.DataFrame,
                 runs_dir: Path = RUNS_DIR, years: int = YEARS) -> list[Path]:
    static = run_folder.build_static()
    occupancy, canopy = baseline_initial_state(equilibrium)
    land_type_of = {a: v.land_type for a, v in config.activities().items()}
    folders = []
    for row in frame.itertuples(index=False):
        shares = {a: getattr(row, a) for a in config.activity_codes()}
        allocation = ls.assign_activities(static.landscape, shares, land_type_of,
                                          np.random.default_rng(1000 + row.run_id))
        folder = runs_dir / f"run_{row.run_id:03d}"
        run_folder.write_run(folder, static, allocation, params, pools, occupancy, canopy,
                             years=years, seed=100 + row.run_id)
        folders.append(folder)
    return folders


def is_done(folder: Path, years: int = YEARS) -> bool:
    out = folder / "HB_PFG_landscape.txt"
    return out.exists() and len(out.read_text().splitlines()) >= years + 1


def run_pending(folders: list[Path], budget_s: float = 150.0, workers: int = 4) -> int:
    """Run ALMaSS in the folders that are not finished, within a time budget."""
    pending = [f for f in folders if not is_done(f)]
    running: list[tuple[subprocess.Popen, Path]] = []
    start = time.time()
    env = {**os.environ, "OMP_NUM_THREADS": "1"}
    while (pending or running) and time.time() - start < budget_s:
        while pending and len(running) < workers:
            folder = pending.pop(0)
            log = open(folder / "almass.log", "w")
            running.append((subprocess.Popen([str(ALMASS_BINARY)], cwd=folder, stdout=log,
                                             stderr=subprocess.STDOUT, env=env), folder))
        for proc, folder in list(running):
            if proc.poll() is not None:
                running.remove((proc, folder))
        time.sleep(0.2)
    for proc, _ in running:
        proc.wait()
    return sum(is_done(f) for f in folders)


# ---------------------------------------------------------------------------
# collect
# ---------------------------------------------------------------------------

def relaxation_time(series: np.ndarray) -> float:
    """Fit y(t) = y_eq + (y_0 - y_eq) exp(-t / tau); NaN if the change is too small."""
    t = np.arange(len(series), dtype=float)
    y_eq, y0 = series[-EQUILIBRIUM_YEARS:].mean(), series[0]
    if abs(y0 - y_eq) < 0.05 * max(abs(y_eq), 1e-9):
        return np.nan
    try:
        (tau,), _ = curve_fit(lambda tt, tau: y_eq + (y0 - y_eq) * np.exp(-tt / tau), t, series,
                              p0=[3.0], bounds=(0.1, 50.0))
    except RuntimeError:
        return np.nan
    return float(tau)


def collect_run(folder: Path) -> dict:
    land = pd.read_csv(folder / "HB_PFG_landscape.txt", sep="\t")
    patches = pd.read_csv(folder / "HB_PFG_patches.txt", sep="\t")
    last_years = sorted(patches["year"].unique())[-EQUILIBRIUM_YEARS - 1:]
    diversity = rd.response_diversity(patches[patches["year"].isin(last_years)], list(PFGS))
    tail = land.tail(EQUILIBRIUM_YEARS)
    return {"richness": tail["richness"].mean(), "tree_occupancy": tail["TREE"].mean(),
            "tau_richness": relaxation_time(land["richness"].values),
            "tau_tree": relaxation_time(land["TREE"].values), **diversity}


def collect(frame: pd.DataFrame, runs_dir: Path = RUNS_DIR) -> pd.DataFrame:
    results = []
    for row in frame.itertuples(index=False):
        folder = runs_dir / f"run_{row.run_id:03d}"
        if is_done(folder):
            results.append({"run_id": row.run_id, **collect_run(folder)})
    return frame.merge(pd.DataFrame(results), on="run_id")


# ---------------------------------------------------------------------------
# emulator
# ---------------------------------------------------------------------------

def feature_terms(activities: list[str]) -> list[tuple[str, ...]]:
    return [(a,) for a in activities] + list(itertools.combinations(activities, 2))


def feature_matrix(shares: pd.DataFrame, terms: list[tuple[str, ...]]) -> np.ndarray:
    return np.column_stack([np.prod([shares[a].values for a in term], axis=0) for term in terms])


@dataclass
class EmulatorFit:
    output: str
    terms: list[tuple[str, ...]]
    intercept: float
    coefficients: np.ndarray
    alpha: float
    cv_r2: float
    tau: float

    def predict(self, shares: pd.DataFrame) -> np.ndarray:
        return self.intercept + feature_matrix(shares, self.terms) @ self.coefficients


def _ridge(x: np.ndarray, y: np.ndarray, alpha: float) -> tuple[float, np.ndarray]:
    xm, ym = x.mean(axis=0), y.mean()
    xc = x - xm
    beta = np.linalg.solve(xc.T @ xc + alpha * np.eye(x.shape[1]), xc.T @ (y - ym))
    return ym - xm @ beta, beta


def fit_output(data: pd.DataFrame, output: str, tau: float, folds: int = 5,
               alphas=(1e-4, 1e-3, 1e-2, 0.1, 1.0)) -> EmulatorFit:
    acts = config.activity_codes()
    terms = feature_terms(acts)
    clean = data.dropna(subset=[output])
    x, y = feature_matrix(clean, terms), clean[output].values
    order = np.random.default_rng(0).permutation(len(y))
    best = None
    for alpha in alphas:
        pred = np.empty_like(y)
        for k in range(folds):
            test = order[k::folds]
            train = np.setdiff1d(order, test)
            b0, beta = _ridge(x[train], y[train], alpha)
            pred[test] = b0 + x[test] @ beta
        r2 = 1 - np.sum((y - pred) ** 2) / np.sum((y - y.mean()) ** 2)
        if best is None or r2 > best[1]:
            best = (alpha, r2)
    b0, beta = _ridge(x, y, best[0])
    return EmulatorFit(output, terms, b0, beta, best[0], float(best[1]), tau)


def fit(data: pd.DataFrame) -> dict[str, EmulatorFit]:
    tau_richness = float(np.nanmedian(data["tau_richness"]))
    tau_tree = float(np.nanmedian(data["tau_tree"]))
    taus = {"richness": tau_richness, "tree_occupancy": tau_tree,
            "rd_divergence": tau_richness, "rd_dissimilarity": tau_richness}
    return {o: fit_output(data, o, taus[o]) for o in OUTPUTS}


def emulator_lines(fits: dict[str, EmulatorFit], primary: str = "richness") -> list[str]:
    acts = config.activities()
    lines = ["# ALMaSS HB_PFG emulator - generated by hbabm.emulator (shares within land type)"]
    lines += [f"ACTIVITY {a} {acts[a].land_type}" for a in config.activity_codes()]
    for name, f in fits.items():
        lines.append(f"# {name}: CV R2 = {f.cv_r2:.3f}, ridge alpha = {f.alpha:g}")
        lines.append(f"OUTPUT {name} {f.tau:.4f}")
        lines.append(f"TERM {name} {f.intercept:.8g}")
        lines += [f"TERM {name} {c:.8g} {' '.join(t)}" for t, c in zip(f.terms, f.coefficients)]
    lines.append(f"PRIMARY {primary}")
    return lines
