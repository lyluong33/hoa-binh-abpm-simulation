"""Stochastic hourly weather for ALMaSS from monthly climate normals.

Daily values: mean temperature = seasonal cycle through the monthly means
(periodic interpolation) plus AR(1) anomalies; rain occurrence is a two-state
Markov chain matched to the monthly number of rain days; wet-day amounts are
gamma distributed with the monthly mean intensity. Hours: sinusoidal diurnal
temperature cycle between Tmin and Tmax, rain concentrated in the afternoon
(convective monsoon showers).
"""
from __future__ import annotations

from pathlib import Path

import numpy as np
import pandas as pd

DAYS_IN_MONTH = np.array([31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31])
HEADER = "year\tmonth\tday\thour\tmean_temp\twind_speed\twind_direction\tprecip\tsoiltemp\t" \
         "snowdepth\thumidity\tradiation"
RAIN_HOURS = np.array([13, 14, 15, 16, 17])


def _daily_cycle(monthly: np.ndarray, doy: np.ndarray) -> np.ndarray:
    mid = np.cumsum(DAYS_IN_MONTH) - DAYS_IN_MONTH / 2.0
    x = np.concatenate([mid - 365, mid, mid + 365])
    y = np.tile(np.asarray(monthly, dtype=float), 3)
    return np.interp(doy, x, y)


def generate_daily(normals: dict, years: range, seed: int = 1) -> pd.DataFrame:
    rng = np.random.default_rng(seed)
    monthly = normals["monthly"]
    dates = pd.date_range(f"{years.start}-01-01", f"{years.stop - 1}-12-31", freq="D")
    dates = dates[~((dates.month == 2) & (dates.day == 29))]   # ALMaSS uses 365-day years
    doy = np.tile(np.arange(1, 366), len(years)).astype(float)
    month = dates.month.values - 1

    anomaly = np.zeros(len(dates))
    for i in range(1, len(dates)):
        anomaly[i] = 0.7 * anomaly[i - 1] + rng.normal(0.0, 1.1)
    t_mean = _daily_cycle(monthly["t_mean"], doy) + anomaly
    diurnal = _daily_cycle(np.subtract(monthly["t_max"], monthly["t_min"]), doy)

    rain_days = np.asarray(monthly.get("rain_days") or np.maximum(1, np.array(monthly["rain_mm"]) / 10.0))
    p_wet = np.clip(rain_days / DAYS_IN_MONTH, 0.02, 0.95)
    persistence = 0.35
    wet = np.zeros(len(dates), dtype=bool)
    for i in range(len(dates)):
        p = p_wet[month[i]]
        p_ww = p + persistence * (1 - p)
        p_dw = p * (1 - p_ww) / (1 - p) if p < 1 else 1.0
        wet[i] = rng.random() < (p_ww if i and wet[i - 1] else p_dw)
    intensity = np.asarray(monthly["rain_mm"]) / np.maximum(rain_days, 0.5)
    shape = 0.8
    amount = np.where(wet, rng.gamma(shape, intensity[month] / shape), 0.0)

    humidity = np.asarray(monthly["rel_humidity"], dtype=float)[month]
    sun = monthly.get("sunshine_hours")
    sunshine = (np.asarray(sun, dtype=float) / DAYS_IN_MONTH)[month] if sun else np.full(len(dates), 4.5)
    return pd.DataFrame({"date": dates, "t_mean": t_mean, "t_range": diurnal, "rain": amount,
                         "humidity": np.clip(humidity + np.where(wet, 5, -3), 40, 100),
                         "sunshine_h": sunshine})


def to_hourly(daily: pd.DataFrame, seed: int = 2) -> pd.DataFrame:
    rng = np.random.default_rng(seed)
    hours = np.arange(24)
    rows = []
    for day in daily.itertuples(index=False):
        temp = day.t_mean + 0.5 * day.t_range * np.sin((hours - 9) / 24 * 2 * np.pi)
        precip = np.zeros(24)
        if day.rain > 0:
            share = rng.dirichlet(np.ones(len(RAIN_HOURS)))
            precip[RAIN_HOURS] = day.rain * share
        # clear-sky proxy: sunshine hours spread over the daylight bell (MJ/m2 -> W/m2 average)
        daylight = np.clip(np.sin((hours - 6) / 12 * np.pi), 0, None)
        radiation = daylight / daylight.sum() * (day.sunshine_h * 1.6 + 6.0) * 1e6 / 3600
        for h in hours:
            rows.append((day.date.year, day.date.month, day.date.day, h, temp[h], 1.8, 180.0,
                         precip[h], day.t_mean, 0.0, day.humidity, radiation[h]))
    return pd.DataFrame(rows, columns=HEADER.split("\t"))


def write_pre(hourly: pd.DataFrame, path: Path) -> None:
    with open(path, "w", newline="\n") as handle:
        handle.write(f"2\n{len(hourly)}\n{HEADER}\n")
        np.savetxt(handle, hourly.values, fmt=["%d", "%d", "%d", "%d"] + ["%.3f"] * 8, delimiter="\t")
