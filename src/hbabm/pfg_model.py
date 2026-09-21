"""Python reference implementation and calibration of the HB_PFG plant model.

The equations are identical to ``models/almass/source_code/HB_PFG`` (see the
header there); this vectorised version runs all activities at once and is used

* to calibrate colonisation rates and shade responses against the thesis plots
  (occupancy targets = PFG share x plot richness / regional pool size), and
* to provide equilibrium states that initialise ALMaSS runs.
"""
from __future__ import annotations

from dataclasses import dataclass

import numpy as np
import pandas as pd
from scipy.optimize import least_squares

from . import config
from .almass_inputs.weather import generate_daily
from .config import PFGS

DAYS = 365


@dataclass
class PFGParameters:
    colonisation: np.ndarray     # (G,)
    extinction: np.ndarray       # (G,)
    shade_beta: np.ndarray       # (G,)
    seed_rain: np.ndarray        # (G,)
    sensitivity: dict[str, np.ndarray]  # action -> (G,)
    canopy_recovery: float
    t_min: float
    t_opt: float
    rain_half: float

    @classmethod
    def from_config(cls, spec: dict | None = None) -> "PFGParameters":
        spec = spec or config.pfg_config()
        groups = [spec["groups"][g] for g in PFGS]
        vec = lambda key: np.array([g[key] for g in groups], dtype=float)  # noqa: E731
        return cls(
            colonisation=vec("colonisation"), extinction=vec("extinction"),
            shade_beta=vec("shade_beta"), seed_rain=vec("seed_rain"),
            sensitivity={a: np.array([g["sensitivity"][a] for g in groups]) for a in spec["actions"]},
            **spec["globals"],
        )


def season_factor(daily_weather: pd.DataFrame, params: PFGParameters) -> np.ndarray:
    warmth = np.clip((daily_weather["t_mean"].values - params.t_min) / (params.t_opt - params.t_min), 0, 1)
    rain30 = pd.Series(daily_weather["rain"].values).rolling(30, min_periods=1).sum().values
    moisture = np.clip(rain30 / params.rain_half, 0, 1)
    return warmth * moisture


def _event_calendar(activities: list[str], rng: np.random.Generator, years: int) -> list[list[tuple]]:
    """Per day of the simulation: list of (activity index, action, intensity)."""
    calendar = [[] for _ in range(years * DAYS)]
    acts = config.activities()
    for i, code in enumerate(activities):
        for event in acts[code].events:
            for year in range(years):
                if event.probability < 1.0 and rng.random() >= event.probability:
                    continue
                calendar[year * DAYS + event.day_of_year - 1].append((i, event.action, event.intensity))
    return calendar


def simulate(activities: list[str], params: PFGParameters, occupancy0: np.ndarray, canopy0: np.ndarray,
             season: np.ndarray, years: int, landscape_mean: np.ndarray | None = None,
             seed: int = 0) -> tuple[np.ndarray, np.ndarray]:
    """Run the patch model for each activity; returns yearly occupancy (Y, A, G) and canopy (Y, A).

    ``landscape_mean`` fixes the seed rain (calibration); if None it is the mean of the simulated patches.
    """
    rng = np.random.default_rng(seed)
    acts = config.activities()
    target = np.array([acts[a].canopy_cover for a in activities])
    occ = occupancy0.copy()
    canopy = canopy0.copy()
    calendar = _event_calendar(activities, rng, years)
    yearly_occ, yearly_can = [], []
    col_rate = params.colonisation / DAYS
    ext_rate = params.extinction / DAYS
    for day in range(years * DAYS):
        for i, action, intensity in calendar[day]:
            occ[i] *= np.maximum(0.0, 1.0 - intensity * params.sensitivity[action])
            if action == "clearfell":
                canopy[i] = 0.0
            elif intensity >= 0.8:
                canopy[i] = min(canopy[i], target[i])
        canopy += (target - canopy) * params.canopy_recovery / DAYS
        mean = landscape_mean if landscape_mean is not None else occ.mean(axis=0)
        light = np.exp(-np.outer(canopy, params.shade_beta))
        s = season[day % len(season)]
        occ = np.clip(occ + col_rate * s * light * (occ + params.seed_rain * mean) * (1 - occ) - ext_rate * occ, 0, 1)
        if day % DAYS == DAYS - 1:
            yearly_occ.append(occ.copy())
            yearly_can.append(canopy.copy())
    return np.array(yearly_occ), np.array(yearly_can)


def occupancy_targets(composition: pd.DataFrame, richness: pd.DataFrame, pools: pd.Series) -> pd.DataFrame:
    """Observed occupancy o_g = share_g x mean richness / pool_g, by land use."""
    shares = composition[list(PFGS)]
    return shares.mul(richness["richness_mean"], axis=0).div(pools[list(PFGS)], axis=1)


@dataclass
class CalibrationResult:
    params: PFGParameters
    predicted: pd.DataFrame
    observed: pd.DataFrame
    equilibrium: pd.DataFrame     # occupancy per activity at the end of the calibration run
    rmse_log: float


def calibrate(composition: pd.DataFrame, richness: pd.DataFrame, pools: pd.Series,
              spec: dict | None = None) -> CalibrationResult:
    spec = spec or config.pfg_config()
    cal = spec["calibration"]
    base = PFGParameters.from_config(spec)
    observed = occupancy_targets(composition, richness, pools).loc[list(cal["targets"])]
    activities = config.activity_codes()
    years = cal["years"]
    weather = generate_daily(config.climate_normals(), range(2001, 2002), seed=cal["weather_seed"])
    season = season_factor(weather, base)
    obar = observed.values.mean(axis=0)
    canopy0 = np.array([config.activities()[a].canopy_cover for a in activities])
    occ0 = np.tile(obar, (len(activities), 1))

    def predict(theta: np.ndarray) -> tuple[pd.DataFrame, np.ndarray]:
        params = _with_theta(base, theta)
        occ, _ = simulate(activities, params, occ0, canopy0, season, years, landscape_mean=obar)
        final = occ[-3:].mean(axis=0)   # average of the last years (clear-fell noise)
        per_activity = pd.DataFrame(final, index=activities, columns=list(PFGS))
        rows = {lu: sum(w * per_activity.loc[a] for a, w in mix.items()) for lu, mix in cal["targets"].items()}
        return pd.DataFrame(rows).T[list(PFGS)], final

    def residuals(theta: np.ndarray) -> np.ndarray:
        predicted, _ = predict(theta)
        return (np.log(predicted.values + 1e-4) - np.log(observed.values + 1e-4)).ravel()

    # theta = [log colonisation (5), shade beta (5), log sensitivity scale, log extinction scale]
    theta0 = np.concatenate([np.log(base.colonisation), base.shade_beta, [np.log(0.2), 0.0]])
    bounds = (np.concatenate([np.full(5, np.log(0.05)), np.full(5, -3.0), [np.log(0.01), np.log(0.2)]]),
              np.concatenate([np.full(5, np.log(50.0)), np.full(5, 6.0), [np.log(1.0), np.log(10.0)]]))
    fit = least_squares(residuals, theta0, bounds=bounds, diff_step=0.05, max_nfev=60)
    predicted, final = predict(fit.x)
    return CalibrationResult(
        params=_with_theta(base, fit.x), predicted=predicted, observed=observed,
        equilibrium=pd.DataFrame(final, index=activities, columns=list(PFGS)),
        rmse_log=float(np.sqrt(np.mean(fit.fun ** 2))),
    )


def _with_theta(base: PFGParameters, theta: np.ndarray) -> PFGParameters:
    """Apply calibrated values; sensitivities and extinction keep their prior ratios between groups."""
    sensitivity_scale, extinction_scale = np.exp(theta[10]), np.exp(theta[11])
    return PFGParameters(
        colonisation=np.exp(theta[:5]), extinction=base.extinction * extinction_scale, shade_beta=theta[5:10],
        seed_rain=base.seed_rain,
        sensitivity={a: np.clip(v * sensitivity_scale, 0, 1) for a, v in base.sensitivity.items()},
        canopy_recovery=base.canopy_recovery,
        t_min=base.t_min, t_opt=base.t_opt, rain_half=base.rain_half,
    )


def richness(occupancy: np.ndarray, pools: pd.Series) -> np.ndarray:
    return occupancy @ pools[list(PFGS)].values.astype(float)
