import pandas as pd
import pytest

from hbabm import config
from hbabm.agripolis_inputs import economics, policy, region, ses_files
from hbabm.paths import PROCESSED_DIR


def test_payments_are_converted_to_eur():
    pay = economics.payments_eur("S0_BASELINE")
    rate = economics.vnd_per_eur()
    assert pay["PROTECT_STRICT"] == pytest.approx(500000 / rate)
    assert pay["MAIZE_INT"] == 0.0


def test_phasing_reproduces_baseline_at_policy_start():
    s0 = economics.payments_eur("S0_BASELINE")["PROTECT_STRICT"]
    s1_start = policy.payments_in_period("S1_PFES_CARBON", 1)["PROTECT_STRICT"]
    s1_full = policy.payments_in_period("S1_PFES_CARBON", 10)["PROTECT_STRICT"]
    assert s1_start == pytest.approx(s0)
    assert s1_full > s0


def test_policy_file_fixes_land_rows_and_prices():
    lines = policy.policy_lines("S4_MAIZE_PRESSURE", 5)
    text = "\n".join(lines)
    for land_type in config.LAND_TYPES:
        assert f"rhs_row:{land_type}=EQ;" in text
    assert "MAIZE_INT=1.35;" in text


def test_strip_terms_removes_products_only():
    expr = "(-2)*BARLEY +(-2)*PASTURE +0.3*SOWS +(-10000)*LU_UPPER_LIMIT"
    out = region._strip_terms(expr, ["BARLEY", "PASTURE"])
    assert "BARLEY" not in out and "PASTURE" not in out
    assert "0.3*SOWS" in out and "LU_UPPER_LIMIT" in out


def test_matrix_has_one_row_per_land_type():
    rows = [l.split("\t")[0] for l in region.matrix_lines(economics.payments_eur("S0_BASELINE"),
                                                          region.RegionSettings())]
    for land_type in config.LAND_TYPES:
        assert rows.count(land_type) == 1
    assert "Arable_Land" not in rows and "crops" not in rows


def test_farm_table_minimum_area_and_equity():
    managers = pd.read_csv(PROCESSED_DIR / "land_managers.csv")
    table = region.farm_table(managers, region.RegionSettings())
    assert (table["area_ha"] >= region.RegionSettings().plot_size_ha).all()
    assert (table["equity"] > 0).all()


def test_ses_file_lists_every_activity():
    lines = ses_files.ses_lines(ses_files.SesOptions())
    names = [l.split("\t")[1] for l in lines if l.startswith("ACTIVITY")]
    assert names == config.activity_codes()
