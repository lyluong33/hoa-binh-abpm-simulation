# Policy scenarios for the Mai Châu and Đà Bắc uplands, 2025 to 2042

The agent-based model of this repository (AgriPoliS for land-use decisions, coupled to an ALMaSS plant model) runs six policy scenarios, `S0_BASELINE` to `S5_INTEGRATED`. This document defines them and describes the three JSON files in `config/research/` that carry their inputs.

## Study system

### Communes

| Former commune | Former district | Commune since 1 July 2025 | Merged units |
|---|---|---|---|
| Mai Hịch | Mai Châu | Bao La | Mai Hịch, Xăm Khòe, Bao La |
| Mai Hạ | Mai Châu | Mai Hạ | Mai Hạ, Chiềng Châu, Vạn Mai |
| Tiền Phong | Đà Bắc | Tiền Phong | Tiền Phong, part of Vầy Nưa |
| Cao Sơn | Đà Bắc | Cao Sơn | Cao Sơn, Tân Minh |

Hòa Bình province merged into Phú Thọ province on 1 July 2025 (Resolution 202/2025/QH15). The district level ended on the same day, and Resolution 1676/NQ-UBTVQH15 formed the new communes.

### Agents and time

The agents are the land managers surveyed for Luong (2025): households and community forest groups. A community group manages 300 to 400 ha of protection forest. The model runs one period per year, from period 1 (2025) to period 18 (2042).

### Activities

| Activity | Land type | Practice |
|---|---|---|
| `MAIZE_INT` | `FARMLAND` | maize or cassava with clearance, burning and herbicide |
| `MAIZE_LOW` | `FARMLAND` | low-input annual crops with manual weeding |
| `ORCHARD` | `FARMLAND` | fruit trees with understorey slashing |
| `FALLOW` | `FARMLAND` | unmanaged regrowth |
| `ACACIA` | `PRODUCTION_FOREST` | acacia plantation, 6-year rotation |
| `NATIVE_MIX` | `PRODUCTION_FOREST` | native timber and bamboo, 25-year rotation |
| `REGEN` | `PRODUCTION_FOREST` | natural regeneration zoning |
| `PROTECT_STRICT` | `PROTECTION_FOREST` | patrol, no cutting |
| `PROTECT_USE` | `PROTECTION_FOREST` | informal understorey use, firewood and grazing |

## Conventions

- Money: expected value in VND per ha per year at 2025 prices, equal to the legal rate times a delivery rate.
- Exchange rate: 29,424 VND/EUR, the 2025 annual average.
- One-off support: an equal annuity at 8% per year over the support period or the rotation, `A = S * r / (1 - (1 + r)^-n)`.
- Capital recovery factors: `CRF(6) = 0.21632`, `CRF(20) = 0.10185`, `CRF(25) = 0.09368`.
- Timing: every scenario pays the `S0_BASELINE` values in 2025, and scenario payments and price shifts start in 2026.

## Shared assumptions

PFES (payment for forest environmental services, Vietnamese chi trả DVMTR) pays forest owners per ha from fees on hydropower, water supply and tourism. The state also pays protection funds per ha of protection forest. Every number below carries one of the confidence flags used in the JSON files:

- `verified`: taken from the cited legal text or source.
- `approximate`: from a secondary source, or rounded or converted.
- `assumption`: a modelling choice without a direct source.

| Assumption | Value | Basis | Confidence |
|---|---|---|---|
| PFES unit rate, 2025 | 200,000 VND/ha/yr (range 140,000 to 240,000) | Tân Minh (Đà Bắc, now in Cao Sơn) 2017: 237,000 (GIZ/SNRD pilot report); Đà Bắc 2013 to 2014: 65,000 (Phuong et al. 2016); Hòa Bình average: 143,500 (Luong 2025, citing Vu 2024) | `approximate` |
| State protection funds | 600,000 VND/ha/yr | Decree 58/2024/ND-CP Art. 9: 500,000 for households and communities owning protection forest, × 1.2 in region II and × 1.5 in region III communes; × 1.2 is used and × 1.5 gives the sensitivity value of 750,000 | `verified` rate, `assumption` region factor |
| Delivery rate in `S0_BASELINE` | 0.5 | Tuyên Quang province had paid out 28% of its 2022 plan for forest protection support in ethnic-minority areas by May 2023 | `assumption` |
| `PROTECT_USE` payment | 0.5 × `PROTECT_STRICT` | payments depend on annual acceptance testing (Decree 58/2024 Art. 5.4(b) and Art. 19.5), and informal extraction risks withheld payments | `assumption` |
| PFES on `REGEN` | 0.9 × PFES unit rate | the K1 coefficient, which weights PFES by forest condition, for poor and young natural forest (Decree 91/2024/ND-CP) | `assumption` |
| PFES on plantations | 0 | `ACACIA` and `NATIVE_MIX` get no PFES; 0.9 × PFES for `NATIVE_MIX` is the sensitivity value | `assumption` |

## Scenario summary

Values are expected VND per ha per year.

| ID | Scenario | `PROTECT_STRICT` | `PROTECT_USE` | `REGEN` | Annualised support | Price multipliers | Extension campaign | Confidence |
|---|---|---|---|---|---|---|---|---|
| `S0_BASELINE` | Policy status quo 2024 to 2025 | 500,000 | 250,000 | 180,000 | `ACACIA` 1,676,000; `NATIVE_MIX` 726,000 | none | none | `approximate` |
| `S1_PFES_CARBON` | Higher PFES rate and forest carbon | 817,000 from 2028 | 408,500 | 482,000 | as S0 | none | none | `approximate` |
| `S2_NATIVE_PLANTING` | Native and large-timber planting | as S0 | as S0 | as S0 | `NATIVE_MIX` 1,786,000; `ACACIA` 0 | `NATIVE_MIX` 1.10 | none | `approximate` |
| `S3_REGEN_ZONING` | Regeneration zoning and full protection funds | 800,000 | 400,000 | 580,000 | `REGEN` 1,730,000; `ACACIA` and `NATIVE_MIX` as S0 | none | none | `approximate` |
| `S4_MAIZE_PRESSURE` | Hypothetical maize price boom | as S0 | as S0 | as S0 | as S0 | `MAIZE_INT` 1.35; `MAIZE_LOW` 1.20 | none | `assumption` |
| `S5_INTEGRATED` | S1, S2 and S3 with an extension campaign | 1,117,000 from 2028 | 558,500 | 882,000 | `NATIVE_MIX` 1,786,000; `ACACIA` 0; `REGEN` 1,730,000 | `NATIVE_MIX` 1.10 | 90% coverage, 0.08 per year from 2026 | `approximate` |

## Returns before own labour

The net return is output minus variable cost minus annualised establishment, plus payments and support, in million VND per ha per year.

| Activity | Market only | `S0_BASELINE` | `S5_INTEGRATED` from 2028 | Labour days/ha/yr |
|---|---|---|---|---|
| `MAIZE_INT` | 13.0 | 13.0 | 13.0 | 110 |
| `MAIZE_LOW` | 13.0 | 13.0 | 13.0 | 140 |
| `ORCHARD` | 15.0 | 15.0 | 15.0 | 93 |
| `ACACIA` | 4.0 | 5.7 | 4.0 | 28 |
| `NATIVE_MIX` | 3.1 | 3.8 | 5.6 | 30 |
| `REGEN` | 1.2 | 1.4 | 3.8 in the first 6 years, 2.1 after | 6 |
| `PROTECT_STRICT` | 0.38 | 0.88 | 1.50 | 1 |
| `PROTECT_USE` | 2.5 | 2.75 | 3.06 | 12 |
| `FALLOW` | 0 | 0 | 0 | 0 |

Forest payments stay one order of magnitude below the maize return.

## S0_BASELINE: policy status quo 2024 to 2025

### Policy basis

- PFES: Forestry Law 16/2017/QH14, Arts. 61 to 65.
- PFES fees: Decree 156/2018/ND-CP, 36 VND/kWh of hydropower, 52 VND/m3 of clean water, 50 VND/m3 of industrial water and at least 1% of eco-tourism revenue.
- PFES amendments: Decree 91/2024/ND-CP, with K coefficients, more industrial payers and regulation between the provinces of one river basin.
- State protection funds and protection contracts: Decree 58/2024/ND-CP Arts. 9 and 19, in force since 15 July 2024.
- Planting support: Decree 58/2024 Art. 14, 15 M VND/ha per cycle plus 0.5 M VND/ha per 4 years for extension, equal for every species.
- Region classes: Decision 861/QĐ-TTg lists the region I, II and III communes for 2021 to 2025.
- Rice support: Decree 58/2024 Art. 21, 15 kg per person per month and at most 300 kg per household per year; the model leaves it out.

### Payments

- `PROTECT_STRICT`: 500,000 (17 EUR) = PFES 200,000 + 0.5 × 600,000 state protection funds.
- `PROTECT_USE`: 250,000.
- `REGEN`: 180,000 = 0.9 × PFES.
- `ACACIA` support: 1,676,000 = 0.5 × 15.5 M × `CRF(6)`.
- `NATIVE_MIX` support: 726,000 = 0.5 × 15.5 M × `CRF(25)`.

### Eligibility

- PFES and protection funds: forest inside the PFES payment map, and no forest loss at the annual acceptance test.
- Planting support: ethnic-minority households, poor households of the Kinh majority and communities in mountainous communes.
- Planting land: allocated, leased or stably used without dispute, and without double funding.
- Tenure in the survey: 35% of managers hold a land-use right certificate, 7.5% a forest land certificate and 7.5% a lease; 17% manage protection forest as a community and 32.5% hold no document.
- Managers without a certificate can be excluded from planting support.

### Mechanism

- Forest payments stay below 5% of the net return of `MAIZE_INT`.
- Support is paid per cycle, so the 6-year `ACACIA` rotation receives 2.3 times the annualised support of the 25-year `NATIVE_MIX` rotation.

### Expected effect

- Community protection forest stays, with continued extraction of forest products.
- Production forest keeps shifting to short-rotation acacia.
- Maize and cassava on slopes keep the highest cash return.

## S1_PFES_CARBON: higher PFES rate and forest carbon payments

### Policy basis

- Forest carbon as a service: Forestry Law 2017 Art. 61 lists forest carbon sequestration and storage as a forest environmental service.
- Basin regulation: Decree 91/2024 lets the Vietnam Forest Protection and Development Fund redistribute PFES revenue between the provinces of one river basin to narrow per-ha gaps (Appendix VII).
- Carbon payments: Decree 180/2026/ND-CP, in force since 1 July 2026, pays per tCO2 or per credit, directly or through the fund.
- Carbon conditions: a registered carbon project, measurement, reporting and verification (MRV), and confirmation by the Ministry of Agriculture and Environment.
- Carbon reference value: Decree 107/2022/ND-CP set up the emission-reduction payment agreement (ERPA) pilot of six North Central Coast provinces, which paid 166,711 VND/ha in Thanh Hóa in 2023.

Note: Decree 91/2024 creates no carbon payer and no carbon rate, and the ERPA pilot does not cover Hòa Bình.

The Hòa Bình PFES rate is low because the 36 VND/kWh of the Hòa Bình hydropower plant is shared over the whole upstream Đà basin, including Sơn La, Lai Châu and Điện Biên. Basin regulation is the legal route to a higher rate.

### Payments

- `PROTECT_STRICT`: 817,000 at full implementation = PFES 350,000 from 2026 + state protection funds 300,000 + carbon 167,000 from 2028.
- PFES 350,000: between the Hòa Bình rate of 200,000 and the national average of 570,000 (4,156 bn VND over 7.3 M ha of forest supplying services).
- Carbon start in 2028: two years for project registration and MRV (`assumption`).
- `PROTECT_USE`: 408,500.
- `REGEN`: 482,000 = 0.9 × 350,000 + 167,000.
- Phasing of `PROTECT_STRICT`: 500,000 in 2025, 650,000 in 2026 and 2027, 817,000 from 2028 (factors 0.612, 0.796 and 1.0).

### Eligibility

- Natural forest supplying services in the Đà river basin.
- Carbon payments only for forest inside a registered carbon project of the province or of the forest owner.

### Mechanism

- A higher payment for strict protection.
- A wider gap between `PROTECT_STRICT` and `PROTECT_USE`, which raises the cost of understorey clearing, firewood collection and grazing.
- A paid `REGEN`.

### Expected effect

- Less `PROTECT_USE`, and more `REGEN` on households short of labour.
- Less understorey clearing in protection forest.
- Little change on farmland, because 0.8 M VND/ha stays far below the 13 M VND/ha of maize.

## S2_NATIVE_PLANTING: native and large-timber planting

### Policy basis

- Planting support: Decree 58/2024 Art. 14, 15 M VND/ha per cycle for every species.
- Large-timber credit: Art. 15, interest-rate support for up to 12 years on up to 70% of the loan, at a rate set by the provincial People's Council.
- Certification: Art. 16, up to 0.4 M VND/ha for sustainable forest management or FSC (Forest Stewardship Council) certification.
- Species differential before 2024: Decision 38/2016/QĐ-TTg paid 8 M VND/ha for native and large-timber species and 5 M VND/ha for small timber; Decree 58/2024 removed the difference.
- Provincial scheme: the Hòa Bình production-forest scheme 2020 to 2025, with vision to 2035, under Decision 327/QĐ-TTg.
- Scheme targets: 3,000 ha per year converted from small to large timber, 6,000 ha per year of new large timber, FSC on 50% of the area and 150 m3/ha per cycle.
- Scheme outcome: the large-timber area grew from 525 ha in 2019 to 20,000 ha in 2023.
- Multi-use forest values: Decision 208/QĐ-TTg (2024).

### Payments

- `NATIVE_MIX` support: 1,786,000 at full delivery = 1,452,000 planting support (15.5 M × `CRF(25)`) + 297,000 interest support + 37,000 certification.
- Interest support: 3% per year on 14 M VND for 12 years, as an annuity over 25 years (`assumption`).
- Certification: 0.4 M VND one-off, as an annuity over 25 years.
- `NATIVE_MIX` price multiplier: 1.10 for certified large timber (`assumption`).
- `ACACIA` support: 0, because the province directs new planting projects to large timber under Art. 14.4 (`assumption`).
- Forest payments: as S0.

### Eligibility

- As S0.
- Interest support: a bank loan and a planting design.
- The 32.5% of surveyed managers without any land document have little access.

### Mechanism

- The `NATIVE_MIX` return before own labour rises from 3.8 M to 5.6 M VND/ha per year, above unsupported `ACACIA` at 4.0 M.
- In S0, `ACACIA` (5.7 M) leads `NATIVE_MIX` (3.8 M).
- Extension advice shapes planting: Luong (2025) finds a Spearman correlation of 0.68 between how often officials recommend a species and how often it grows on the plots.
- Most recommended species: acacia (22 reports), Chukrasia tabularis (13) and luồng bamboo (11).

### Expected effect

- Part of the acacia area converts to Chukrasia, bamboo and mixed native stands.
- Longer rotations, less full clearing after establishment and continuous canopy.
- Limits: 8% discounting, upfront capital and land certificates.

## S3_REGEN_ZONING: regeneration zoning and full protection funds

### Policy basis

- Zoning in protection forest: Decree 58/2024 Art. 10 with Art. 6.2, 1 M VND/ha per year for 6 years.
- Zoning with supplementary planting: Art. 13, 8 M VND/ha on average, one-off, on production natural-forest land.
- Protection of production natural forest: Art. 12, 500,000 VND/ha per year times the region factor.
- Higher zoning rate: Decree 42/2026/ND-CP sets 2.5 M VND/ha per year for 6 years in special-use forest; its reach into protection forest through Art. 10 is not confirmed, so `payment_components` lists it as a sensitivity value only.
- Funding: Programme 1719 (Decision 1719/QĐ-TTg), the national target programme for ethnic-minority and mountainous areas 2021 to 2025, and its merged successor for 2026 to 2035.

### Payments

- `PROTECT_STRICT`: 800,000 = PFES 200,000 + 600,000 state protection funds at full delivery.
- `PROTECT_USE`: 400,000.
- `REGEN`: 580,000 = PFES 180,000 + 400,000 protection support (600,000 from year 7 of zoning, averaged over the 18-year horizon).
- `REGEN` support: 1,730,000 = 8 M × `CRF(6)` over the 6-year zoning period.
- `ACACIA` 1,676,000 and `NATIVE_MIX` 726,000: as S0.

Note: the model pays the `REGEN` support in every period; the 6-year limit in `payment_components` is not applied.

### Eligibility

- Land planned as protection forest or as production natural forest.
- A 6-year zoning design with annual acceptance testing.
- Ethnic-minority households, poor households and communities in mountainous communes.

### Mechanism

- Natural regeneration becomes a paid option on remote, steep and degraded production land, where acacia and maize return little or carry landslide risk.
- In Cao Sơn, 6 of 10 respondents worry about landslides.

### Expected effect

- More `REGEN`, more connectivity between protection-forest blocks and less erosion.
- Better-funded protection of protection forest.

## S4_MAIZE_PRESSURE: hypothetical maize price boom

S4 is a market scenario, not a policy. It tests the other scenarios under price pressure. The Crop Production Law 31/2018/QH14 Art. 71 requires anti-erosion practice on sloping land, with no payment, cross-compliance or enforcement tied to maize.

### Parameters

- `MAIZE_INT` price multiplier: 1.35, dry grain from 6,200 to 8,400 VND/kg.
- `MAIZE_LOW` price multiplier: 1.20.
- Forest payments and support: as S0.
- Sensitivity runs: both multipliers rescaled so that `MAIZE_INT` takes 1.05, 1.10, 1.15 or 1.20.

### Mechanism

- The `MAIZE_INT` return rises from 13 M to 23 M VND/ha per year.
- Maize expands into fallow, with more herbicide and burning.
- Luong (2025) records 25% of plots used against the official land-use plan.

Note: land types are fixed in the model, so maize cannot expand onto forest land.

### Expected effect

- `FALLOW` and `MAIZE_LOW` shift to `MAIZE_INT`.
- Less functional plant diversity on farmland and more erosion.

## S5_INTEGRATED: S1, S2 and S3 with an extension campaign

### Policy basis

- The legal basis of S1, S2 and S3.
- Awareness spending: Decree 58/2024 Arts. 5.3 and 9.3 allow training on forest protection, law dissemination and community commitments, and PFES revenue can fund communication.
- National Biodiversity Strategy (Decision 149/QĐ-TTg, 28 January 2022): forest cover of 42 to 43%, protected areas on 9% of land and restoration of at least 20% of degraded natural ecosystems by 2030.

### Payments

- `PROTECT_STRICT`: 1,117,000 from 2028 = PFES 350,000 + state protection funds 600,000 + carbon 167,000.
- `PROTECT_USE`: 558,500.
- `REGEN`: 882,000 = PFES 315,000 + carbon 167,000 + protection support 400,000.
- Annualised support: `NATIVE_MIX` 1,786,000, `ACACIA` 0 and `REGEN` 1,730,000.
- `NATIVE_MIX` price multiplier: 1.10.
- Phasing of `PROTECT_STRICT`: 500,000 in 2025, 950,000 in 2026 and 2027, 1,117,000 from 2028 (factors 0.448, 0.850 and 1.0).

### Extension campaign

- Coverage: 90% of land managers each year from 2026 (`assumption`).
- Effect: the awareness `a` (0 to 1) of a reached manager rises by `0.08 × (1 - a)` per year (`assumption`).
- Exposure in the survey: 37 of 40 managers received official planting recommendations, and 87.5% received a forest-protection message.
- Training in the survey: 1 of 40 managers attended a biodiversity or ecosystem-service training.
- Content: the value of protection forest, native species, low-clearing cultivation and overlooked services such as pollination and pest control.

### Mechanism

- Payments combine with changes in awareness and norms.
- Awareness is the main route to change on farmland (from `MAIZE_INT` to `MAIZE_LOW`, keeping `FALLOW`), because the income gap to maize stays large.

### Expected effect

- The largest landscape effect of the six scenarios: less `PROTECT_USE`, more `REGEN` and `NATIVE_MIX` and less full clearing.

## Open uncertainties

1. PFES unit rate: 200,000 VND/ha rests on the 2013 and 2017 data points and the Luong (2025) average.
2. Region factor: × 1.2 is used; the class of the four communes under Decision 861/QĐ-TTg is not confirmed, and the 2025 mergers require a new classification.
3. Delivery rates: 0.5 in S0 and 1.0 in S3 and S5 rest on evidence from other provinces.
4. Carbon: Decree 180/2026 pays per tCO2 and per project; 167,000 VND/ha per year is the Thanh Hóa ERPA value, not a Phú Thọ rate.
5. Decree 42/2026: its effect on protection-forest zoning is not confirmed.
6. Interest support: the rate under Art. 15 is an assumption; the Phú Thọ People's Council sets the actual rate.
7. Economics: the non-timber values of `REGEN` and `PROTECT_*`, remote orchard yields and native timber prices are assumptions.
8. Climate: the model uses the Mai Châu station for the whole landscape, including the Đà Bắc communes.

## Input files

### `config/research/activity_economics.json`

The file holds the market economics of the nine activities per ha per year, in 2024 to 2025 VND. Payments and support live in `scenarios.json`.

| Field | Content |
|---|---|
| `vnd_per_eur` | 29,424, the rate the model uses for every VND to EUR conversion |
| `rural_wage_vnd_per_day` | 200,000, the reference wage for hired farm labour (range 150,000 to 250,000) |
| `exchange_rate`, `rural_wage` | source, year, confidence and alternative values of the two numbers above |
| `annualisation` | annuity method, discount rate 0.08, CRF values and the rationale for 8% |
| `activities` | one object per activity |
| `summary_net_before_own_labour_vnd_per_ha_yr` | output minus variable cost minus annualised establishment, per activity |

Note: `RegionSettings` in `src/hbabm/agripolis_inputs/region.py` hard-codes the wage as 0.85 EUR per hour (200,000 / 8 / 29,424).

`src/hbabm/agripolis_inputs/economics.py` reads five fields of each activity object:

| Field | Content | Read by the model |
|---|---|---|
| `land_type` | `FARMLAND`, `PRODUCTION_FOREST` or `PROTECTION_FOREST` | yes |
| `output_value_vnd_per_ha_yr` | annualised gross output | yes, as the product price |
| `variable_cost_vnd_per_ha_yr` | annualised variable cost | yes |
| `establishment_cost_annualised_vnd_per_ha_yr` | establishment cost as an 8% annuity over the rotation | yes, added to the variable cost |
| `labour_days_per_ha_yr` | annualised labour | yes, at 8 hours per day |
| `establishment_cost_vnd_per_ha` | one-off establishment cost | no |
| `rotation_years`, `years_to_first_income` | rotation length and first year with income | no |
| `mature_values`, `yield_ramp`, `cycle_values` | yearly flows before annualisation | no |
| `notes`, `sources`, `confidence` | derivation, references and confidence flag | no |

| Activity | Output | Variable cost | Establishment, one-off | Establishment, annuity | Labour days | Rotation (years) | Confidence |
|---|---|---|---|---|---|---|---|
| `MAIZE_INT` | 28,000,000 | 15,000,000 | 0 | 0 | 110 | 1 | `approximate` |
| `MAIZE_LOW` | 19,000,000 | 6,000,000 | 0 | 0 | 140 | 1 | `assumption` |
| `ORCHARD` | 35,105,000 | 13,977,000 | 60,000,000 | 6,111,000 | 93 | 20 | `assumption` |
| `FALLOW` | 0 | 0 | 0 | 0 | 0 | none | `verified` |
| `ACACIA` | 9,542,000 | 2,070,000 | 16,000,000 | 3,461,000 | 28 | 6 | `approximate` |
| `NATIVE_MIX` | 7,246,000 | 2,254,000 | 20,000,000 | 1,874,000 | 30 | 25 | `assumption` |
| `REGEN` | 1,200,000 | 0 | 0 | 0 | 6 | none | `assumption` |
| `PROTECT_STRICT` | 400,000 | 20,000 | 0 | 0 | 1 | none | `assumption` |
| `PROTECT_USE` | 2,500,000 | 0 | 0 | 0 | 12 | none | `assumption` |

Derivations:

- `MAIZE_INT`: 4.5 t/ha of dry grain at 6,200 VND/kg; inputs from the 2013 Đà Bắc survey of Phuong et al. (2016) times 1.25, plus herbicide.
- `MAIZE_LOW`: 2.8 t/ha of maize plus 1.6 M VND of legume intercrop; no herbicide and more weeding labour.
- `ORCHARD`: 60 M VND/ha gross at maturity, for example 6 t/ha of longan at 10,000 VND/kg; 15%, 35%, 55% and 75% of full yield in years 4 to 7, and full yield from year 8.
- `ACACIA`: 70 M VND/ha sold standing at year 6 (95 m3 at 740,000 VND/m3); establishment from the 2007 to 2013 Đà Bắc costs times 1.5.
- `NATIVE_MIX`: half luồng bamboo, harvested from year 5, and half native timber such as Chukrasia tabularis, sold standing for 200 M VND/ha at year 25.
- `REGEN`, `PROTECT_STRICT`, `PROTECT_USE`: non-timber forest products only, such as bamboo shoots, firewood, medicinal plants and, for `PROTECT_USE`, grazing.

### `config/research/scenarios.json`

| Field | Content |
|---|---|
| `vnd_per_eur` | 29,424; the model reads the rate from `activity_economics.json` |
| `global_assumptions` | the shared assumptions, each with `value`, `range`, `basis` and `confidence` |
| `scenarios` | one object per scenario |

The model reads six fields of each scenario object:

| Field | Content | Read by the model |
|---|---|---|
| `id` | scenario code | yes |
| `payments_vnd_per_ha_yr` | recurring expected payment per activity | yes |
| `annualised_support_vnd_per_ha_yr` | one-off support as an 8% annuity, per activity | yes |
| `price_multipliers` | output price factor per activity | yes |
| `phasing` | list of `from_period` and `factor` | yes |
| `extension_campaign` | `coverage`, `awareness_effect_per_year` and `start_period`, or `null` | yes |
| `name_en`, `name_vi` | scenario name in English and Vietnamese | no |
| `legal_basis` | legal documents with article and URL | no |
| `payment_components` | each payment split into components with nominal rate, delivery rate, start period and confidence | no |
| `phasing_note`, `extension_campaign_note` | derivation of the phasing factors and of the campaign values | no |
| `eligibility` | who qualifies | no |
| `conditions_vi`, `mechanism_vi`, `expected_effect_vi` | conditions, mechanism and expected effect, in Vietnamese | no |
| `evidence_confidence` | overall confidence flag of the scenario | no |

`src/hbabm/agripolis_inputs/policy.py` turns a scenario into the AgriPoliS file `policy_settings.txt`:

- Premium: each activity receives its payment plus its annualised support, in EUR per ha, as a premium in the yearly optimisation of every agent.
- Period 1 (2025): every scenario uses the `S0_BASELINE` values.
- From period 2 (2026): the scenario values apply, times the `phasing` factor in force.
- Phasing reach: the factor multiplies every payment and support value of the scenario, although the factors come from the `PROTECT_STRICT` totals.
- Price multipliers: a one-off level shift in period 2 that stays in force.
- Missing entries: an activity missing from a payment map gets 0, and an activity missing from `price_multipliers` keeps 1.0.

`src/hbabm/scenario_runs.py` passes `extension_campaign` to the social-ecological (SES) extension of AgriPoliS. From `start_period`, each year every agent is reached with probability `coverage`, and the awareness `a` of a reached agent rises by `awareness_effect_per_year × (1 - a)`.

### `config/research/climate_normals.json`

The file holds the monthly climate normals of the Mai Châu station from QCVN 02:2022/BXD, the national standard of natural-condition data for construction, as given in the climate box of the English Wikipedia page on Mai Châu district. The averaging period of QCVN 02:2022 is not stated. Mai Châu has a mean temperature of 23.4 °C and 1,756 mm of rain on 127 days per year, with 2.8 to 5.5 rain days per month from November to March.

| Field | Content |
|---|---|
| `monthly` | 12 values each for `t_mean`, `t_max` and `t_min` (°C, mean daily values), `rain_mm` (mm per month), `rain_days` (days per month), `rel_humidity` (%) and `sunshine_hours` (h per month) |
| `annual` | annual values, with the stated totals and the sums of the months for rain and sunshine |
| `quality_notes` | defects of the source table; record extremes are left out |
| `station_assignment` | Mai Châu station for Mai Hịch and Mai Hạ; Hòa Bình station for Tiền Phong and Cao Sơn, with -0.55 °C per 100 m of elevation for Cao Sơn |
| `alternate_station` | monthly normals of the Hòa Bình station |
| `context` | provincial climate summary from Luong (2025) |
| `source`, `period`, `confidence`, `units` | provenance, averaging period, confidence flag and units |

`src/hbabm/almass_inputs/weather.py` generates the stochastic hourly ALMaSS weather from `monthly`, and `src/hbabm/pfg_model.py` calibrates the plant model on one year of weather generated from the same normals. The model does not read `alternate_station`.

## References

- GIZ/SNRD. Report on the pilot of distribution of fund from PFES via bank accounts. https://snrd-asia.org/wp-content/uploads/2024/11/9-Report-on-the-pilot-of-PFES-banking_EN.pdf
- Luong, T. K. L. (2025). Local ecological knowledge of land managers on functional plant diversity in Hoa Binh province, Vietnam. MSc thesis, KU Leuven.
- Phuong et al. (2016). Household opportunity costs of protecting and developing forest lands in Son La and Hoa Binh Provinces. International Journal of the Commons 10(2). https://thecommonsjournal.org/articles/10.18352/ijc.620
- Vu, H. L. (2024). MSc thesis, KU Leuven, cited in Luong (2025).

The URLs of the legal documents are in the `legal_basis` fields of `scenarios.json`, and the sources of the activity economics are in the `sources` fields of `activity_economics.json`.
