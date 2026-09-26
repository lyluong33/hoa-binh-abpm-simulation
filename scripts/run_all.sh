#!/usr/bin/env bash
# Full pipeline from data/processed: calibration -> ALMaSS design runs -> emulator ->
# AgriPoliS scenarios -> analysis -> verification; model stages rerun in time-boxed slices.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
export PYTHONPATH="$ROOT/src"
cd "$ROOT"
python3 -m hbabm.pipeline_almass                      # 1. plant model calibration, static ALMaSS inputs
until python3 -m hbabm.pipeline_emulator | tee /dev/stderr | grep -q "CV R2"; do :; done   # 2. emulator
python3 -m hbabm.calibration                          # 3. AW_VALUE calibration (update config/model/ses.json)
until python3 -m hbabm.scenario_runs | tee /dev/stderr | grep -q "written"; do :; done      # 4. scenarios
python3 -m hbabm.analysis > /dev/null                 # 5. figures + key findings
until python3 -m hbabm.verify | tee /dev/stderr | grep -q "error_pct"; do :; done           # 6. verification
python3 -m pytest -q
