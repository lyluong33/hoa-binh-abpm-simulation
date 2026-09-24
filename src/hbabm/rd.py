"""Response diversity of the plant functional groups (Ross et al. 2023).

Ross et al. measure response diversity as the diversity of species' responses
to an environmental driver: fit each species' growth rate as a smooth function
of the driver, take the derivatives (the responses) at every observed driver
value, and summarise across species by

* dissimilarity - how different the responses are (here: mean pairwise
  absolute difference of the derivatives), and
* divergence    - [(max - min) - | |max| - |min| |] / (max - min), which is 1
  when some groups respond positively and others negatively (compensation)
  and 0 when all respond in the same direction.

Here the "species" are the five PFGs, the driver is the yearly management
disturbance experienced by a patch (sum of operation intensities, from
HB_PFG_patches.txt) and the growth rate is log(o_{t+1} / o_t) of the patch.
"""
from __future__ import annotations

import numpy as np
import pandas as pd

EPS = 1e-6


def growth_table(patches: pd.DataFrame, groups: list[str]) -> pd.DataFrame:
    """Per patch and year: driver (disturbance in year t+1) and log growth of each PFG."""
    patches = patches.sort_values(["polyref", "year"])
    nxt = patches.groupby("polyref").shift(-1)
    table = pd.DataFrame({"polyref": patches["polyref"], "year": patches["year"],
                          "driver": nxt["disturbance"]})
    for g in groups:
        table[g] = np.log((nxt[g] + EPS) / (patches[g] + EPS))
    return table.dropna()


def responses(table: pd.DataFrame, groups: list[str], degree: int = 2) -> pd.DataFrame:
    """Derivative d(growth)/d(driver) of each PFG at every observed driver value."""
    x = table["driver"].values
    derivs = {}
    for g in groups:
        if np.ptp(x) < EPS:
            derivs[g] = np.zeros_like(x)
            continue
        coef = np.polyfit(x, table[g].values, deg=min(degree, len(np.unique(x)) - 1))
        derivs[g] = np.polyval(np.polyder(coef), x)
    return pd.DataFrame(derivs, index=table.index)


def divergence(derivatives: np.ndarray) -> np.ndarray:
    """Row-wise divergence of a (n_points, n_groups) array of responses."""
    hi, lo = derivatives.max(axis=1), derivatives.min(axis=1)
    span = hi - lo
    with np.errstate(invalid="ignore", divide="ignore"):
        value = (span - np.abs(np.abs(hi) - np.abs(lo))) / span
    return np.where(span > EPS, value, 0.0)


def dissimilarity(derivatives: np.ndarray) -> np.ndarray:
    """Row-wise mean pairwise absolute difference of responses."""
    n = derivatives.shape[1]
    diffs = [np.abs(derivatives[:, i] - derivatives[:, j]) for i in range(n) for j in range(i + 1, n)]
    return np.mean(diffs, axis=0)


def response_diversity(patches: pd.DataFrame, groups: list[str]) -> dict[str, float]:
    table = growth_table(patches, groups)
    if table.empty:
        return {"rd_divergence": np.nan, "rd_dissimilarity": np.nan}
    derivs = responses(table, groups).values
    return {"rd_divergence": float(np.mean(divergence(derivs))),
            "rd_dissimilarity": float(np.mean(dissimilarity(derivs)))}
