import pandas as pd

from hbabm import analysis, emulator


def test_first_departure():
    a = pd.Series([1.0, 1.0, 1.2, 1.5], index=[2025, 2026, 2027, 2028])
    b = pd.Series([1.0, 1.0, 1.0, 1.0], index=a.index)
    assert analysis.first_departure(a, b, 0.1) == 2027
    assert analysis.first_departure(b, b, 0.1) is None


def test_relaxation_time_recovers_tau():
    import numpy as np
    t = np.arange(15)
    series = 10 + (5 - 10) * np.exp(-t / 4.0)
    assert abs(emulator.relaxation_time(series) - 4.0) < 0.5


def test_ridge_emulator_reproduces_linear_surface():
    import numpy as np
    from hbabm import config
    rng = np.random.default_rng(1)
    rows = []
    for _ in range(80):
        row = {}
        for lt in config.LAND_TYPES:
            codes = config.activities_of(lt)
            row.update(zip(codes, rng.dirichlet(np.ones(len(codes)))))
        row["y"] = 3 * row["MAIZE_INT"] + 2 * row["PROTECT_STRICT"]
        rows.append(row)
    fit = emulator.fit_output(pd.DataFrame(rows), "y", tau=1.0)
    assert fit.cv_r2 > 0.99
