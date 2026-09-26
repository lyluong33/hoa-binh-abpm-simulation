"""Stage 4: figures and summary tables of the scenario experiment.

Answers three questions, with the numbers written to results/key_findings.json:

1. Uncoupled vs coupled (one-way): does the process-based ALMaSS indicator show
   changes that the area-based indicators of an uncoupled AgriPoliS miss?
2. Two-way coupling: does the feedback from perceived biodiversity loss change
   land use, and how does that depend on the market pressure?
3. Ecological lag: how does ecological inertia delay the social response?
"""
from __future__ import annotations

import json

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt  # noqa: E402
import numpy as np  # noqa: E402
import pandas as pd  # noqa: E402

from . import config  # noqa: E402
from .paths import GENERATED_DIR, RESULTS_DIR  # noqa: E402

FIG_DIR = RESULTS_DIR / "figures"
S4 = "S4_MAIZE_PRESSURE"
PRESSURES = {"_p1.1": 1.10, "_p1.15": 1.15, "_p1.2": 1.20, "": 1.35}


def load() -> pd.DataFrame:
    return pd.read_csv(RESULTS_DIR / "scenario_results.csv")


def series(d: pd.DataFrame, scenario: str, mode: str, column: str) -> pd.Series:
    x = d[(d["scenario"] == scenario) & (d["mode"] == mode)]
    return x.set_index("year")[column]


def first_departure(a: pd.Series, b: pd.Series, tol: float) -> int | None:
    diff = (a - b).abs()
    years = diff[diff > tol].index
    return int(years[0]) if len(years) else None


def relative_change(s: pd.Series) -> float:
    return float(100 * (s.iloc[-1] / s.iloc[0] - 1))


# ---------------------------------------------------------------------------
# figures
# ---------------------------------------------------------------------------

def fig_coupled_vs_uncoupled(d: pd.DataFrame) -> None:
    fig, axes = plt.subplots(1, 2, figsize=(11, 4), sharey=True)
    for ax, scenario in zip(axes, ("S0_BASELINE", S4)):
        u = d[(d["scenario"] == scenario) & (d["mode"] == "uncoupled")].set_index("year")
        o = d[(d["scenario"] == scenario) & (d["mode"] == "oneway")].set_index("year")
        ax.plot(u.index, 100 * u["stewardship_share"] / u["stewardship_share"].iloc[0], label="uncoupled: stewardship area")
        ax.plot(u.index, u["sar_index"], label="uncoupled: species-area (semi-natural land)")
        ax.plot(o.index, 100 * o["richness"] / o["richness"].iloc[0], label="one-way: ALMaSS richness (landscape)", lw=2)
        ax.plot(o.index, 100 * o["richness_FARMLAND"] / o["richness_FARMLAND"].iloc[0],
                label="one-way: ALMaSS richness (farmland)", lw=2, ls="--")
        ax.set_title(scenario)
        ax.set_xlabel("year")
        ax.axhline(100, color="grey", lw=0.5)
    axes[0].set_ylabel("index (2025 = 100)")
    axes[1].legend(fontsize=8, loc="lower left")
    fig.suptitle("Same land use, different ecological verdict: area-based vs process-based indicators")
    fig.tight_layout()
    fig.savefig(FIG_DIR / "fig1_coupled_vs_uncoupled.png", dpi=150)
    plt.close(fig)


def fig_twoway_pressure(d: pd.DataFrame) -> None:
    fig, axes = plt.subplots(2, len(PRESSURES), figsize=(14, 6), sharex=True)
    for j, (tag, price) in enumerate(PRESSURES.items()):
        for mode, style in (("oneway", "-"), ("twoway", "--")):
            axes[0, j].plot(series(d, S4, mode + tag, "MAIZE_INT"), style, label=mode)
            axes[1, j].plot(series(d, S4, mode + tag, "richness_FARMLAND"), style, label=mode)
        axes[0, j].set_title(f"maize price x{price:.2f}")
    axes[0, 0].set_ylabel("intensive maize (ha)")
    axes[1, 0].set_ylabel("farmland richness (species)")
    axes[0, 0].legend()
    fig.suptitle("Two-way coupling: feedback damps intensification at intermediate market pressure")
    fig.tight_layout()
    fig.savefig(FIG_DIR / "fig2_twoway_pressure.png", dpi=150)
    plt.close(fig)


def fig_lag(d: pd.DataFrame, tag: str = "_p1.15") -> None:
    fig, axes = plt.subplots(1, 3, figsize=(14, 4))
    for mode, style in (("oneway", ":"), ("twoway", "-"), ("twoway_nolag", "--")):
        m = mode + tag
        axes[0].plot(series(d, S4, m, "richness_FARMLAND"), style, label=f"{mode} (observed)")
        if mode != "oneway":
            axes[0].plot(series(d, S4, m, "richness_FARMLAND_equilibrium"), style, alpha=0.4,
                         label=f"{mode} (committed equilibrium)")
        axes[1].plot(series(d, S4, m, "awareness"), style, label=mode)
        axes[2].plot(series(d, S4, m, "MAIZE_INT"), style, label=mode)
    axes[0].set_ylabel("farmland richness")
    axes[1].set_ylabel("mean awareness")
    axes[2].set_ylabel("intensive maize (ha)")
    for ax in axes:
        ax.set_xlabel("year")
    axes[0].legend(fontsize=7)
    axes[1].legend(fontsize=8)
    fig.suptitle("Ecological inertia (tau ~ 6 years) delays the perceived loss and the social response")
    fig.tight_layout()
    fig.savefig(FIG_DIR / "fig3_lag.png", dpi=150)
    plt.close(fig)


def fig_scenarios(d: pd.DataFrame) -> None:
    acts = config.activity_codes()
    last = d[(d["mode"] == "twoway") & (d["year"] == d["year"].max())].set_index("scenario")
    first = d[(d["mode"] == "twoway") & (d["year"] == d["year"].min())].iloc[0]
    fig, axes = plt.subplots(1, 3, figsize=(15, 4.5))
    for ax, land_type in zip(axes, config.LAND_TYPES):
        codes = config.activities_of(land_type)
        shares = last[codes].div(last[codes].sum(axis=1), axis=0)
        base = pd.DataFrame([first[codes] / first[codes].sum()], index=["2025 (all)"])
        pd.concat([base, shares]).plot.barh(stacked=True, ax=ax, legend=True, width=0.8)
        ax.set_title(f"{land_type}: shares in 2042")
        ax.legend(fontsize=7, loc="lower right")
    fig.tight_layout()
    fig.savefig(FIG_DIR / "fig4_scenarios_land_use.png", dpi=150)
    plt.close(fig)
    del acts


def fig_model_fit() -> None:
    fit = pd.read_csv(GENERATED_DIR / "pfg_calibration_fit.csv", header=[0, 1], index_col=0)
    data = pd.read_csv(GENERATED_DIR / "almass_sampling_dataset.csv")
    fig, axes = plt.subplots(1, 2, figsize=(10, 4))
    obs, pred = fit["observed"].values.ravel(), fit["predicted"].values.ravel()
    axes[0].loglog(obs, pred, "o")
    lim = [obs.min() * 0.8, obs.max() * 1.2]
    axes[0].plot(lim, lim, "k-", lw=0.5)
    axes[0].set_xlabel("observed PFG occupancy (thesis plots)")
    axes[0].set_ylabel("HB_PFG equilibrium")
    axes[0].set_title("Plant model calibration (4 land uses x 5 PFGs)")
    axes[1].hist(data["tau_richness"].dropna(), bins=25)
    axes[1].set_xlabel("relaxation time of richness, tau (years)")
    axes[1].set_title("Ecological inertia in 150 ALMaSS runs")
    fig.tight_layout()
    fig.savefig(FIG_DIR / "fig0_model_fit.png", dpi=150)
    plt.close(fig)


# ---------------------------------------------------------------------------
# key numbers
# ---------------------------------------------------------------------------

def key_findings(d: pd.DataFrame) -> dict:
    acts = config.activity_codes()
    findings: dict = {"q1_uncoupled_vs_oneway": {}, "q2_twoway": {}, "q3_lag": {}}
    for scenario in d["scenario"].unique():
        u = d[(d["scenario"] == scenario) & (d["mode"] == "uncoupled")].set_index("year")
        o = d[(d["scenario"] == scenario) & (d["mode"] == "oneway")].set_index("year")
        findings["q1_uncoupled_vs_oneway"][scenario] = {
            "max_land_use_difference_ha": float((u[acts] - o[acts]).abs().to_numpy().max()),
            "stewardship_share_change_pct": relative_change(u["stewardship_share"]),
            "sar_index_change_pct": float(u["sar_index"].iloc[-1] - 100),
            "richness_landscape_change_pct": relative_change(o["richness"]),
            "richness_farmland_change_pct": relative_change(o["richness_FARMLAND"]),
        }
    for tag, price in PRESSURES.items():
        one, two = (d[(d["scenario"] == S4) & (d["mode"] == m + tag)].set_index("year") for m in ("oneway", "twoway"))
        findings["q2_twoway"][f"price_x{price:.2f}"] = {
            "maize_int_2042_oneway_ha": float(one["MAIZE_INT"].iloc[-1]),
            "maize_int_2042_twoway_ha": float(two["MAIZE_INT"].iloc[-1]),
            "maize_int_change_pct": float(100 * (two["MAIZE_INT"].iloc[-1] / one["MAIZE_INT"].iloc[-1] - 1)),
            "awareness_2042_twoway": float(two["awareness"].iloc[-1]),
            "farmland_richness_2042_oneway": float(one["richness_FARMLAND"].iloc[-1]),
            "farmland_richness_2042_twoway": float(two["richness_FARMLAND"].iloc[-1]),
        }
    for tag in ("_p1.15", "_p1.2", ""):
        one = d[(d["scenario"] == S4) & (d["mode"] == "oneway" + tag)].set_index("year")
        lag = d[(d["scenario"] == S4) & (d["mode"] == "twoway" + tag)].set_index("year")
        nolag = d[(d["scenario"] == S4) & (d["mode"] == "twoway_nolag" + tag)].set_index("year")
        findings["q3_lag"][f"price_x{PRESSURES[tag]:.2f}"] = {
            "awareness_response_year_with_lag": first_departure(lag["awareness"], one["awareness"], 0.01),
            "awareness_response_year_without_lag": first_departure(nolag["awareness"], one["awareness"], 0.01),
            "land_use_response_year_with_lag": first_departure(lag["MAIZE_INT"], one["MAIZE_INT"], 0.5),
            "land_use_response_year_without_lag": first_departure(nolag["MAIZE_INT"], one["MAIZE_INT"], 0.5),
            "observed_minus_committed_farmland_richness_2033": float(
                lag.loc[2033, "richness_FARMLAND"] - lag.loc[2033, "richness_FARMLAND_equilibrium"]),
            "year_observed_farmland_richness_peaks": int(one["richness_FARMLAND"].idxmax()),
        }
    loc = d[(d["scenario"] == S4) & (d["mode"] == "twoway")].set_index("year")
    land = d[(d["scenario"] == S4) & (d["mode"] == "twoway_landscape")].set_index("year")
    findings["q2_twoway"]["perception_scale_price_x1.35"] = {
        "awareness_2042_local": float(loc["awareness"].iloc[-1]),
        "awareness_2042_landscape": float(land["awareness"].iloc[-1]),
        "maize_int_2042_local": float(loc["MAIZE_INT"].iloc[-1]),
        "maize_int_2042_landscape": float(land["MAIZE_INT"].iloc[-1]),
    }
    return findings


def summary_table(d: pd.DataFrame) -> pd.DataFrame:
    acts = config.activity_codes()
    last = d[d["year"] == d["year"].max()]
    main = last[last["mode"].isin(["uncoupled", "oneway", "twoway", "twoway_nolag"])]
    cols = ["scenario", "mode", *acts, "awareness", "stewardship_share", "sar_index", "richness",
            "richness_FARMLAND", "richness_PRODUCTION_FOREST", "richness_PROTECTION_FOREST",
            "rd_divergence", "rd_dissimilarity", "total_income_eur"]
    return main[cols].round(3)


def run() -> dict:
    FIG_DIR.mkdir(parents=True, exist_ok=True)
    d = load()
    fig_model_fit()
    fig_coupled_vs_uncoupled(d)
    fig_twoway_pressure(d)
    fig_lag(d)
    fig_scenarios(d)
    summary_table(d).to_csv(RESULTS_DIR / "summary_2042.csv", index=False)
    findings = key_findings(d)
    (RESULTS_DIR / "key_findings.json").write_text(json.dumps(findings, indent=2))
    return findings


if __name__ == "__main__":
    print(json.dumps(run(), indent=1))
