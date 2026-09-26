# ODD description of the Hòa Bình social-ecological model

This document describes the coupled AgriPoliS and ALMaSS model following the ODD protocol (Grimm et al. 2020). The parameters live in `config/`, and each item names its source file in parentheses.

## 1. Purpose and patterns

The model answers two questions:

- How do forest policy and market shifts in the Hòa Bình uplands change land use and understorey plant diversity?
- How does the coupling of ecological processes to the economic model (one-way, two-way, with or without ecological lag) change these answers?

Three patterns evaluate the model:

- plant functional group (PFG) composition and species richness by land use, from the 40 vegetation plots
- slashing intensity and frequency by land use, from the slashing survey
- the main activity of each of the 41 surveyed land managers

## 2. Entities, state variables and scales

| Entity | State variables | Source |
|---|---|---|
| Land-manager agent (AgriPoliS `RegFarmInfo` with `ses::FarmState`) | area of each land type, owned and rented; family labour; capital; awareness `a`; asset index; land-use certificate; area of the nine activities in the previous year | `data/processed/land_managers.csv` |
| AgriPoliS plot | land type (`FARMLAND`, `PRODUCTION_FOREST` or `PROTECTION_FOREST`), 0.25 ha, rent | `region.py` |
| ALMaSS parcel (`HB_PFG_Patch`) | vegetation type `tov_HB*`; canopy cover; occupancy `o_g` of the five PFGs; total disturbance of the year | `HB_PFG` |
| ALMaSS landscape | 1 m grid of 1.2 × 1.2 km; 780 parcels of 40 × 40 m; stream, road and village | `landscape.py` |
| Emulator (`EcoEmulator`) | activity shares within each land type; equilibrium indicators `V*`; lagged indicators `V`; 2025 reference values | `emulator.txt` |

AgriPoliS runs in yearly steps over 18 years, from 2025 to 2042, and ALMaSS in daily steps.

## 3. Process overview and scheduling

Each AgriPoliS year (`RegManagerInfo::step`) runs five steps:

1. Land market: rental auction on shadow prices.
2. Investment: no livestock or machinery investment takes place.
3. Production: every farm solves its MIP. Just before the solver, `ses::Extension::BeforeSolve` adds `a × AW_VALUE` to the objective coefficients of the stewardship activities and sets the bounds of the perennial activities.
4. `ses::Extension::EndOfPeriod`: the emulator computes the indicators from the land-use composition, the extension writes its outputs and awareness is updated.
5. AgriPoliS market update and farm output.

Each ALMaSS day runs four steps:

1. The farms carry out the `HB_ScheduledPlan` schedule of each parcel.
2. `HB_PFG_Population_Manager::DoFirst` computes the season factor and the landscape-mean occupancy.
3. Each parcel applies the management operations of the day, with a loss of intensity times group sensitivity, updates its canopy and grows by the Levins-type equation.
4. On 31 December, the yearly outputs are written.

## 4. Design concepts

- Decision-making: each agent maximises cash income plus the non-market value `a × AW_VALUE` per ha under labour, capital and land constraints (the AgriPoliS MIP).
- Adaptation: awareness changes through social learning, extension and experienced biodiversity loss; only a loss relative to 2025 feeds back (loss aversion).
- Sensing: a manager senses the richness of the land type of their surveyed activity (`PERCEIVED`); a sensitivity variant uses the whole landscape.
- Interaction: indirect through the rental market; social learning within a radius of 40 plots; seed rain in ALMaSS through the landscape-mean occupancy.
- Stochasticity: farm locations; the agents reached by the extension campaign; acacia clear-felling, with probability 1/6 per year; weather; the location of activities in the landscape.
- Observation: `ses_farms.dat`, `ses_landscape.dat` and `sector.dat` from AgriPoliS; `HB_PFG_patches.txt` and `HB_PFG_landscape.txt` from ALMaSS.

## 5. Initialisation

- Agents: initial awareness `a₀`, asset index, land-use certificate and initial activity of each manager (`data/processed/land_managers.csv`). Both indices were computed from the survey before anonymisation:
  - `a₀ = 0.7 × clip((s_reg - 0.25) / 0.40) + 0.15 × E + 0.15 × P`
  - `s_reg`: share of the manager's ecosystem-service scores given to pest control, climate regulation, erosion control and soil fertility
  - `E`: 1 if the manager received extension advice, otherwise 0
  - `P`: 1 if the manager knows a protected species, otherwise 0
  - Asset index: mean of five normalised components: log area, land documents, labour, tourism income and education.
- Farm-family age: aggregate age of the 41 managers (`data/processed/age_summary.json`), with the maximum capped at 75 years.
- Perennials in the first year: the change limits use the surveyed activity as the previous-year level.
- ALMaSS design runs: every parcel starts from the mean occupancy and canopy of its land type in the baseline landscape, so each run is a transient from which τ is estimated.

## 6. Input data

- Activity economics, climate normals and scenarios: `config/research/*.json`, with sources and field descriptions in `docs/research/scenario.md`.
- Management schedules: `config/model/activities.json`, from the slashing survey:

| Land use | Median intensity (1 to 5) | Median frequency (per year) |
|---|---|---|
| Annual cropland | 5 | 4 |
| Perennial cropland | 4 | 5 |
| Production forest | 2 | 1 |
| Protection forest | 1 | 0, with 64% of records not slashed |

- Canopy cover: median `Avg_canopy` of the vegetation plots by land use.

## 7. Submodels

### Activities and economics (`economics.py`)

- Prices and costs converted from VND to EUR; labour as days × 8 hours.
- Establishment costs as an equal annuity at 8%.
- Policy payments as activity premiums in the `coupled_premium` row of the MIP.

### Labour (`region.py`)

- Off-farm work by the hour at 0.85 EUR/h, up to 50% of family hours; hired labour at 1 EUR/h.
- Fixed labour contracts switched off.
- Opportunity cost of family labour in the exit rule: the expected wage, unused because of `NO_EXIT`.

### Change limits (`ses.json`)

| Activity | Maximum expansion per year | Maximum contraction per year | Needs a land-use certificate |
|---|---|---|---|
| `ORCHARD` | 10% | 10% | yes |
| `ACACIA` | 15% | 1/6 | yes |
| `NATIVE_MIX` | 5% | 4% | yes |

The expansion limit is multiplied by the asset factor `0.25 + 0.75 × asset index`.

### Awareness (`SesExtension.cpp`)

```
a' = clip(a + 0.1 (ā - a) + R e (1 - a) - 0.05 (a - a₀) + F max(0, (V_ref - V) / V_ref) (1 - a))
```

- `ā`: mean awareness of the agents within 40 plots
- `R`: 1 if the extension campaign reaches the agent that year, otherwise 0
- `e`: campaign effect, 0.08 in `S5_INTEGRATED`
- `F`: feedback strength `AW_FEEDBACK`, 0 in `uncoupled` and `oneway` runs and 3 in `twoway` runs
- `V`, `V_ref`: perceived richness in the current year and in 2025
- `clip`: limits a value to [0, 1]

### HB_PFG (`HB_PFG.h`)

```
do_g/dt = c_g S(t) exp(-β_g × canopy) (o_g + r_g ō_g) (1 - o_g) - e_g o_g
```

- `c_g`, `e_g`: colonisation and extinction rates of group `g`
- `β_g`: shade response
- `ō_g`, `r_g`: landscape-mean occupancy and seed-rain weight
- `S(t) = clip((T - 12) / 12) × clip(rain_30 / 30)`, with `T` the daily temperature and `rain_30` the rain of the last 30 days
- Operation of intensity `I`: `o_g ← o_g (1 - I s_{g,operation})`, with `s` the sensitivity of the group to the operation
- Canopy: relaxes towards the target of the vegetation type at 0.2 per year; clear-felling sets it to 0, and an operation with `I ≥ 0.8` cuts it to the target
- Calibration: `c_g`, `β_g` and two scale factors, for disturbance sensitivity and extinction, fitted by least squares on log observed occupancy (`pfg_model.calibrate`); the results are in `data/generated/pfg_calibration.json`

### Emulator (`emulator.py`)

- Ridge regression on the 9 shares and their 36 pairwise products, with the penalty α chosen by 5-fold cross-validation.
- τ: median of the per-run τ from exponential fits to the transients.

### Response diversity (`rd.py`, Ross et al. 2023)

- Log growth rate of each group against the total disturbance of the year: a quadratic polynomial and its derivative.
- Divergence and mean pairwise dissimilarity of the derivatives.

## 8. Calibration and verification

| Item | Result |
|---|---|
| `HB_PFG` against the vegetation plots | RMSE(log occupancy) = 0.106 |
| `AW_VALUE` = 150 EUR/ha | matches 73% of the surveyed activities, against 68% without awareness |
| Emulator against direct ALMaSS runs at the 2042 compositions | error below 0.3% |
| One-way coupling | land use unchanged, 0 ha difference |
| Automated tests | 19 pytest tests |
