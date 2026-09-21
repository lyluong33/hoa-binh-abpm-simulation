"""Typed access to the JSON configuration files."""
from __future__ import annotations

import json
from dataclasses import dataclass
from functools import lru_cache
from pathlib import Path
from typing import Any

from .paths import MODEL_CONFIG_DIR, RESEARCH_CONFIG_DIR

LAND_TYPES = ("FARMLAND", "PRODUCTION_FOREST", "PROTECTION_FOREST")
PFGS = ("ANNUAL", "GRASS", "FORB_FERN", "WOODY", "TREE")


def load_json(path: Path) -> dict[str, Any]:
    with open(path, encoding="utf-8") as handle:
        return json.load(handle)


@dataclass(frozen=True)
class ManagementEvent:
    """One scheduled management operation of an activity (per year)."""

    action: str          # cut | herbicide | burn | sow | harvest | clearfell
    day_of_year: int     # 1..365, earliest day
    window_days: int     # the operation may be done within this window
    intensity: float     # 0..1, share of vegetation removed / damaged
    probability: float = 1.0  # yearly probability (e.g. clear-fell once per rotation)


@dataclass(frozen=True)
class Activity:
    code: str
    land_type: str
    almass_tov: str
    thesis_land_use: str
    canopy_cover: float
    stewardship: bool
    events: tuple[ManagementEvent, ...]


@lru_cache(maxsize=1)
def activities() -> dict[str, Activity]:
    raw = load_json(MODEL_CONFIG_DIR / "activities.json")["activities"]
    result = {}
    for code, spec in raw.items():
        events = tuple(ManagementEvent(**event) for event in spec["events"])
        result[code] = Activity(
            code=code,
            land_type=spec["land_type"],
            almass_tov=spec["almass_tov"],
            thesis_land_use=spec["thesis_land_use"],
            canopy_cover=spec["canopy_cover"],
            stewardship=spec["stewardship"],
            events=events,
        )
    return result


def activity_codes() -> list[str]:
    return list(activities())


def activities_of(land_type: str) -> list[str]:
    return [code for code, act in activities().items() if act.land_type == land_type]


@lru_cache(maxsize=1)
def activity_economics() -> dict[str, Any]:
    return load_json(RESEARCH_CONFIG_DIR / "activity_economics.json")


@lru_cache(maxsize=1)
def scenarios() -> dict[str, Any]:
    return load_json(RESEARCH_CONFIG_DIR / "scenarios.json")


@lru_cache(maxsize=1)
def climate_normals() -> dict[str, Any]:
    return load_json(RESEARCH_CONFIG_DIR / "climate_normals.json")


@lru_cache(maxsize=1)
def pfg_config() -> dict[str, Any]:
    return load_json(MODEL_CONFIG_DIR / "pfg.json")


@lru_cache(maxsize=1)
def awareness_config() -> dict[str, Any]:
    return load_json(MODEL_CONFIG_DIR / "awareness.json")
