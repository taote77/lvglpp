#!/usr/bin/env bash
# Sync the portable lvglpp subset into the smart-speaker ESP-IDF project.
#
# Usage:
#   ports/esp32/adapters/smart_speaker/sync_to_smart_speaker.sh [project-dir]
#
# Default project dir: /mnt/c/workspace/esp32/smart-speaker
#
# The ESP target uses the project's managed lvgl/lvgl component, so this script
# copies only sdk/core (without the large Noto .c font files), sdk/config and
# ports/esp32. It also copies the board adapters into main/.
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
LVGLPP_ROOT="$(cd "${SCRIPT_DIR}/../../../.." && pwd)"
PROJECT_DIR="${1:-/mnt/c/workspace/esp32/smart-speaker}"
COMPONENT_DIR="${PROJECT_DIR}/components/lvglpp"

if [[ ! -d "${PROJECT_DIR}/components/lcd" ]]; then
    echo "error: ${PROJECT_DIR} does not look like the smart-speaker project" >&2
    exit 1
fi

mkdir -p "${COMPONENT_DIR}/sdk" "${COMPONENT_DIR}/ports"

# sdk/core minus the generated Noto font .c files (ESP uses LV_FONT_DEFAULT or
# a generated profile, never the desktop full set).
rsync -a --delete \
    --exclude '*.c' \
    --exclude '__pycache__' \
    "${LVGLPP_ROOT}/sdk/core/" "${COMPONENT_DIR}/sdk/core/"

rsync -a --delete \
    --exclude '__pycache__' \
    "${LVGLPP_ROOT}/sdk/config/" "${COMPONENT_DIR}/sdk/config/"

rsync -a --delete \
    --exclude '__pycache__' \
    "${LVGLPP_ROOT}/ports/esp32/" "${COMPONENT_DIR}/ports/esp32/"

cp "${SCRIPT_DIR}/components_lvglpp_CMakeLists.txt" "${COMPONENT_DIR}/CMakeLists.txt"
cp "${SCRIPT_DIR}/SmartSpeakerAdapters.h" "${PROJECT_DIR}/main/"
cp "${SCRIPT_DIR}/SmartSpeakerAdapters.cpp" "${PROJECT_DIR}/main/"

# Shared face/candle pages: one source in lvglpp/demo/face_ui, copied into the
# app component. FacePage/CandlePage are compiled by main/CMakeLists.txt.
mkdir -p "${PROJECT_DIR}/main/face_ui"
cp "${LVGLPP_ROOT}/demo/face_ui/FaceTypes.h" "${PROJECT_DIR}/main/face_ui/"
cp "${LVGLPP_ROOT}/demo/face_ui/FacePage.h" "${PROJECT_DIR}/main/face_ui/"
cp "${LVGLPP_ROOT}/demo/face_ui/FacePage.cc" "${PROJECT_DIR}/main/face_ui/"
cp "${LVGLPP_ROOT}/demo/face_ui/CandlePage.h" "${PROJECT_DIR}/main/face_ui/"
cp "${LVGLPP_ROOT}/demo/face_ui/CandlePage.cc" "${PROJECT_DIR}/main/face_ui/"

echo "synced lvglpp -> ${COMPONENT_DIR}"
echo "copied SmartSpeakerAdapters.{h,cpp} -> ${PROJECT_DIR}/main/"
echo "copied FacePage/CandlePage -> ${PROJECT_DIR}/main/face_ui/"
