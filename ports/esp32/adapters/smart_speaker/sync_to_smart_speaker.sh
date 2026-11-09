#!/usr/bin/env bash
# Sync the portable lvglpp subset into the smart-speaker ESP-IDF project.
#
# Usage:
#   ports/esp32/adapters/smart_speaker/sync_to_smart_speaker.sh [project-dir]
#
# Default project dir: /mnt/c/workspace/esp32/smart-speaker
#
# The ESP target uses the project's managed lvgl/lvgl component, so this script
# copies only framework/core (without the large Noto .c font files), framework/config and
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

# framework/core minus the generated Noto font .c files (ESP uses LV_FONT_DEFAULT or
# a generated profile, never the desktop full set).
rsync -a --delete \
    --exclude '*.c' \
    --exclude '__pycache__' \
    "${LVGLPP_ROOT}/framework/core/" "${COMPONENT_DIR}/framework/core/"

rsync -a --delete \
    --exclude '__pycache__' \
    "${LVGLPP_ROOT}/framework/config/" "${COMPONENT_DIR}/framework/config/"

rsync -a --delete \
    --exclude '__pycache__' \
    "${LVGLPP_ROOT}/ports/esp32/" "${COMPONENT_DIR}/ports/esp32/"

cp "${SCRIPT_DIR}/components_lvglpp_CMakeLists.txt" "${COMPONENT_DIR}/CMakeLists.txt"
cp "${SCRIPT_DIR}/SmartSpeakerAdapters.h" "${PROJECT_DIR}/main/"
cp "${SCRIPT_DIR}/SmartSpeakerAdapters.cpp" "${PROJECT_DIR}/main/"

# Shared face/candle pages: one source in lvglpp/demo/smart_speaker, copied into the
# app component. FacePage/CandlePage are compiled by main/CMakeLists.txt.
mkdir -p "${PROJECT_DIR}/main/smart_speaker_ui"
cp "${LVGLPP_ROOT}/demo/smart_speaker/FaceTypes.h" "${PROJECT_DIR}/main/smart_speaker_ui/"
cp "${LVGLPP_ROOT}/demo/smart_speaker/FacePage.h" "${PROJECT_DIR}/main/smart_speaker_ui/"
cp "${LVGLPP_ROOT}/demo/smart_speaker/FacePage.cc" "${PROJECT_DIR}/main/smart_speaker_ui/"
cp "${LVGLPP_ROOT}/demo/smart_speaker/CandlePage.h" "${PROJECT_DIR}/main/smart_speaker_ui/"
cp "${LVGLPP_ROOT}/demo/smart_speaker/CandlePage.cc" "${PROJECT_DIR}/main/smart_speaker_ui/"

echo "synced lvglpp -> ${COMPONENT_DIR}"
echo "copied SmartSpeakerAdapters.{h,cpp} -> ${PROJECT_DIR}/main/"
echo "copied FacePage/CandlePage -> ${PROJECT_DIR}/main/smart_speaker_ui/"
