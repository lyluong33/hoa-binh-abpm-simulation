"""Synthetic upland valley landscape (Mai Chau / Da Bac type) for ALMaSS.

The raster is a valley running west-east: stream, road and village on the
valley floor, then farmland on the lower slopes, production forest on the
mid slopes and protection forest on the ridges - the elevation zoning seen in
the thesis plots (median slope 5 deg for orchards, 20-22 deg for forests).
Parcels are square polygons of ``parcel_m`` x ``parcel_m`` metres (1 m cells).

Only the *composition* of activities within each land type is varied between
ALMaSS runs; the raster and the legal land-type zoning stay fixed, as they do
in AgriPoliS (land types are soil types there).
"""
from __future__ import annotations

import struct
from dataclasses import dataclass, field
from pathlib import Path

import numpy as np

from ..config import LAND_TYPES

TOLE_FIELD = 20
TOLE_URBAN = 8
TOLE_ROAD = 122
TOLE_STREAM = 207
LSB_MAGIC = b"An LSB File."
COUNTRY_CODE = "DK"       # ALMaSS only uses it to pick country-specific crop defaults
LATITUDE, LONGITUDE = 20.62, 105.00   # Mai Hich, Mai Chau


@dataclass(frozen=True)
class LandscapeSpec:
    width_m: int = 1200
    height_m: int = 1200
    parcel_m: int = 40
    land_type_shares: tuple[float, float, float] = (0.25, 0.35, 0.40)  # farmland, production, protection
    valley_floor_m: int = 120        # stream + road + village band in the middle
    ridge_elevation_m: float = 900.0
    floor_elevation_m: float = 450.0


@dataclass
class Parcel:
    poly_id: int
    land_type: str
    centroid: tuple[int, int]
    area_m2: int
    elevation: float
    slope_deg: float


@dataclass
class Landscape:
    spec: LandscapeSpec
    raster: np.ndarray                    # int32 polygon ids, shape (height, width)
    parcels: list[Parcel]
    other_polygons: list[tuple[int, int, int, tuple[int, int]]] = field(default_factory=list)
    # (poly_id, tole, area, centroid)

    def parcels_of(self, land_type: str) -> list[Parcel]:
        return [p for p in self.parcels if p.land_type == land_type]


def build_landscape(spec: LandscapeSpec = LandscapeSpec()) -> Landscape:
    height, width, size = spec.height_m, spec.width_m, spec.parcel_m
    raster = np.zeros((height, width), dtype=np.int32)
    centre = height // 2
    half_floor = spec.valley_floor_m // 2

    stream_id, road_id, village_id = 0, 1, 2
    other = []
    # valley floor: stream (6 m), road (8 m), rest village / home gardens (non-farmed)
    raster[centre - half_floor:centre + half_floor, :] = village_id
    raster[centre - 3:centre + 3, :] = stream_id
    raster[centre + 10:centre + 18, :] = road_id
    for pid, tole in ((stream_id, TOLE_STREAM), (road_id, TOLE_ROAD), (village_id, TOLE_URBAN)):
        ys, xs = np.nonzero(raster == pid)
        other.append((pid, tole, int(len(xs)), (int(xs.mean()), int(ys.mean()))))

    # slopes: parcels ordered by distance from the valley floor get land types in
    # the order farmland -> production forest -> protection forest
    slope_rows = [r for r in range(0, height, size)
                  if r + size <= centre - half_floor or r >= centre + half_floor]
    cells = [(r, c) for r in slope_rows for c in range(0, width - size + 1, size)]
    distance = np.array([abs(r + size / 2 - centre) for r, _ in cells])
    order = np.argsort(distance, kind="stable")
    cuts = np.cumsum(spec.land_type_shares)[:-1] * len(cells)
    max_distance = distance.max()

    parcels = []
    next_id = 3
    for rank, idx in enumerate(order):
        r, c = cells[idx]
        land_type = LAND_TYPES[int(np.searchsorted(cuts, rank, side="right"))]
        raster[r:r + size, c:c + size] = next_id
        rel = distance[idx] / max_distance
        elevation = spec.floor_elevation_m + rel * (spec.ridge_elevation_m - spec.floor_elevation_m)
        parcels.append(Parcel(next_id, land_type, (c + size // 2, r + size // 2), size * size,
                              round(elevation, 1), round(5.0 + 25.0 * rel, 1)))
        next_id += 1
    return Landscape(spec, raster, sorted(parcels, key=lambda p: p.poly_id), other)


def write_lsb(raster: np.ndarray, path: Path) -> None:
    height, width = raster.shape
    with open(path, "wb") as handle:
        handle.write(LSB_MAGIC)
        handle.write(struct.pack("<ii", width, height))
        handle.write(raster.astype("<i4").tobytes())


def write_polyref(landscape: Landscape, farm_of_parcel: dict[int, int], path: Path) -> None:
    """Polygon reference file (ALMaSS 2020/21 format with location header)."""
    rows = []  # (tole, id, area, farm, centroid x, centroid y, elevation, slope, pollen/nectar curve)
    for pid, tole, area, (cx, cy) in landscape.other_polygons:
        rows.append((tole, pid, area, -1, cx, cy, landscape.spec.floor_elevation_m, 0.0, -1))
    for p in landscape.parcels:
        rows.append((TOLE_FIELD, p.poly_id, p.area_m2, farm_of_parcel[p.poly_id],
                     p.centroid[0], p.centroid[1], p.elevation, p.slope_deg, -3))
    header = "PolyType\tPolyRefNum\tArea\tFarmRef\tUnSprayedMarginRef\tSoilType\tOpenness\t" \
             "CentroidX\tCentroidY\tElevation\tSlope\tAspect\tPollenNectarCurve"
    with open(path, "w", newline="\n") as handle:
        handle.write(f"{COUNTRY_CODE} Latitude {LATITUDE} Longitude {LONGITUDE}\n{len(rows)}\n{header}\n")
        for tole, pid, area, farm, cx, cy, elev, slope, curve in rows:
            # no unsprayed margin, soil type 2, openness 0, aspect 0
            handle.write(f"{tole}\t{pid}\t{area}\t{farm}\t-1\t2\t0\t{cx}\t{cy}\t{elev}\t{slope}\t0\t{curve}\n")


def assign_activities(landscape: Landscape, shares: dict[str, float], activity_land_type: dict[str, str],
                      rng: np.random.Generator) -> dict[int, str]:
    """Allocate parcels to activities; ``shares`` are fractions within each land type."""
    allocation = {}
    for land_type in LAND_TYPES:
        parcels = landscape.parcels_of(land_type)
        codes = [a for a, lt in activity_land_type.items() if lt == land_type]
        weights = np.array([max(shares.get(a, 0.0), 0.0) for a in codes])
        if weights.sum() <= 0:
            raise ValueError(f"no activity share for land type {land_type}")
        counts = largest_remainder(weights / weights.sum(), len(parcels))
        labels = np.repeat(codes, counts)
        rng.shuffle(labels)
        allocation.update({p.poly_id: str(label) for p, label in zip(parcels, labels)})
    return allocation


def largest_remainder(fractions: np.ndarray, total: int) -> np.ndarray:
    raw = fractions * total
    counts = np.floor(raw).astype(int)
    for idx in np.argsort(-(raw - counts))[: total - counts.sum()]:
        counts[idx] += 1
    return counts
