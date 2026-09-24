import numpy as np
import pandas as pd

from hbabm import rd


def test_divergence_extremes():
    same_sign = np.array([[1.0, 2.0, 3.0]])
    opposite = np.array([[-2.0, 0.5, 2.0]])
    assert rd.divergence(same_sign)[0] == 0.0
    assert rd.divergence(opposite)[0] == 1.0


def test_dissimilarity_zero_for_identical_responses():
    assert rd.dissimilarity(np.array([[0.3, 0.3, 0.3]]))[0] == 0.0


def test_response_diversity_detects_compensation():
    rows = []
    rng = np.random.default_rng(0)
    for patch in range(40):
        driver = rng.uniform(0, 5)
        a, b = 0.5, 0.5
        for year in range(4):
            rows.append({"polyref": patch, "year": year, "disturbance": driver, "A": a, "B": b})
            a *= np.exp(-0.1 * driver + 0.2)   # declines with disturbance
            b *= np.exp(0.1 * driver - 0.2)    # increases with disturbance
    result = rd.response_diversity(pd.DataFrame(rows), ["A", "B"])
    assert result["rd_divergence"] > 0.9
