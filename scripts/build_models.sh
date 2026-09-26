#!/usr/bin/env bash
# Build AgriPoliS (needs GLPK >= 4.52) and ALMaSS (needs OpenMP) out of source.
# Usage: GLPK_LIBRARY_DIR=/path/to/libglpk scripts/build_models.sh [work_dir]
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
WORK="${1:-${HB_WORK_DIR:-/tmp/hbabm_work}}"
mkdir -p "$WORK/build_agripolis" "$WORK/build_almass"
cmake -S "$ROOT/models/agripolis" -B "$WORK/build_agripolis" -DGLPK_LIBRARY_DIR="${GLPK_LIBRARY_DIR:-/usr/lib}"
cmake --build "$WORK/build_agripolis" -j4
cmake -S "$ROOT/models/almass/source_code" -B "$WORK/build_almass" -DCMAKE_BUILD_TYPE=Release
cmake --build "$WORK/build_almass" -j4
cp "$WORK/build_agripolis/src/agp24" "$WORK/agp24"
cp "$WORK/build_almass/almass_cmd" "$WORK/almass_cmd"
echo "binaries: $WORK/agp24 $WORK/almass_cmd"
