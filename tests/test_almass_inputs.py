import struct

import numpy as np
import pytest

from hbabm import config
from hbabm.almass_inputs import landscape as ls
from hbabm.almass_inputs import plans, weather


def test_largest_remainder_preserves_total():
    counts = ls.largest_remainder(np.array([0.333, 0.333, 0.334]), 10)
    assert counts.sum() == 10 and counts.min() >= 3


def test_landscape_zoning_follows_shares():
    land = ls.build_landscape(ls.LandscapeSpec(width_m=400, height_m=400, parcel_m=40, valley_floor_m=80))
    n = len(land.parcels)
    shares = [len(land.parcels_of(lt)) / n for lt in config.LAND_TYPES]
    assert shares == pytest.approx([0.25, 0.35, 0.40], abs=0.05)
    # farmland is closer to the valley floor than protection forest
    elev = {lt: np.mean([p.elevation for p in land.parcels_of(lt)]) for lt in config.LAND_TYPES}
    assert elev["FARMLAND"] < elev["PRODUCTION_FOREST"] < elev["PROTECTION_FOREST"]


def test_assign_activities_respects_land_types():
    land = ls.build_landscape(ls.LandscapeSpec(width_m=400, height_m=400, parcel_m=40, valley_floor_m=80))
    acts = {a: v.land_type for a, v in config.activities().items()}
    shares = {a: 1.0 for a in acts}
    alloc = ls.assign_activities(land, shares, acts, np.random.default_rng(0))
    for parcel in land.parcels:
        assert acts[alloc[parcel.poly_id]] == parcel.land_type


def test_lsb_header(tmp_path):
    raster = np.arange(6, dtype=np.int32).reshape(2, 3)
    ls.write_lsb(raster, tmp_path / "m.lsb")
    data = (tmp_path / "m.lsb").read_bytes()
    assert data[:12] == ls.LSB_MAGIC
    assert struct.unpack("<ii", data[12:20]) == (3, 2)
    assert len(data) == 20 + 6 * 4


def test_plan_file_lists_every_scheduled_event(tmp_path):
    plans.write_management_plans(tmp_path / "p.txt")
    lines = [l for l in (tmp_path / "p.txt").read_text().splitlines() if not l.startswith("#")]
    expected = sum(len(a.events) for a in config.activities().values())
    assert len(lines) == expected
    assert all(len(l.split("\t")) == 6 for l in lines)


def test_weather_reproduces_monthly_rain():
    normals = config.climate_normals()
    daily = weather.generate_daily(normals, range(2001, 2031), seed=3)
    monthly = daily.groupby(daily["date"].dt.month)["rain"].sum() / 30
    assert monthly.sum() == pytest.approx(sum(normals["monthly"]["rain_mm"]), rel=0.15)
    assert daily["t_mean"].mean() == pytest.approx(np.mean(normals["monthly"]["t_mean"]), abs=0.5)
