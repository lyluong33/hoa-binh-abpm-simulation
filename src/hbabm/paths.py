"""Filesystem locations used by the pipeline."""
from __future__ import annotations

import os
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[2]
CONFIG_DIR = REPO_ROOT / "config"
MODEL_CONFIG_DIR = CONFIG_DIR / "model"
RESEARCH_CONFIG_DIR = CONFIG_DIR / "research"
PROCESSED_DIR = REPO_ROOT / "data" / "processed"
GENERATED_DIR = REPO_ROOT / "data" / "generated"
RESULTS_DIR = REPO_ROOT / "results"
MODELS_DIR = REPO_ROOT / "models"
WORK_DIR = Path(os.environ.get("HB_WORK_DIR", "/tmp/hbabm_work"))
