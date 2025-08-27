#!/bin/sh
#Create lv_i18n.c and lv_i18n.h from the yml files.

SCRIPT_DIR=$(cd "$(dirname "$0")" && pwd)

echo ${SCRIPT_DIR}

lv_i18n compile -t "${SCRIPT_DIR}/*.yml" -o "${SCRIPT_DIR}"

# lv_i18n compile -t "./*.yml" -o .
