# i18n / bitmap font tooling

The single source of truth for smart_home UI copy is:

```
smart_home/i18n/strings.xlsx
```

It is a real `.xlsx` workbook (sheet `strings`) with one row per `strid`:

| strid | zh-CN | en-US | ja-JP | ru-RU | ar | comment |
|---|---|---|---|---|---|---|

- `strid` and `en-US` are required.
- A blank language cell means "not translated". The generator **omits** that
  row from the locale's table, so `lv_i18n_get_text` falls back to `en-US`
  (pack[0]) at runtime. Do not put an empty string in the cell unless the empty
  translation is intentional.
- `comment` is ignored by the generator.
- `{...}` placeholders must match the `en-US` value; `--strict` treats
  mismatches as errors.

## Commands

```bash
# Generate smart_home/i18n/translations/lv_i18n.c (checked in)
python3 -I sdk/tools/i18n/gen_lv_i18n.py \
  --xlsx smart_home/i18n/strings.xlsx \
  --out smart_home/i18n/translations/lv_i18n.c

# CI / pre-commit: fail if the generated C file is stale
python3 -I sdk/tools/i18n/gen_lv_i18n.py ... --check --strict

# Verify every lvTr("...")/lvTrFmt("...") literal exists in the workbook
python3 -I sdk/tools/i18n/check_keys.py \
  --xlsx smart_home/i18n/strings.xlsx --sources smart_home

# Add/update rows from a CSV/TSV without opening Excel
python3 -I sdk/tools/i18n/upsert_keys.py \
  --xlsx smart_home/i18n/strings.xlsx --input new_keys.tsv

# One-time YAML migration (only needed for the legacy tree)
python3 -I sdk/tools/i18n/migrate_yaml_to_xlsx.py
```

`lv_i18n.h` is a fixed template and is not regenerated. The C generator uses
`sdk/tools/i18n/templates/lv_i18n.c.in`.

## Bitmap fonts

Fonts are subset from the characters that actually appear in the workbook:

```bash
npm --prefix sdk/tools/i18n install      # .npmrc uses registry.npmmirror.com

python3 -I sdk/tools/i18n/gen_fonts.py \
  --xlsx smart_home/i18n/strings.xlsx \
  --fonts-dir assets/common/fonts \
  --out sdk/core/tools/fonts --jobs 4

# Validate the committed files against font_manifest.json
python3 -I sdk/tools/i18n/gen_fonts.py \
  --xlsx smart_home/i18n/strings.xlsx --out sdk/core/tools/fonts --check
```

Generated per size `20/22/24/28/32/36`:

- `NotoSC_<size>_Normal.c` — `NotoSansSC-AR-Regular.ttf`, all locales.
- `NotoSC_<size>_Bold.c` — `NotoSansSC-Bold.ttf`, with
  `--lv-fallback NotoArabic_<size>_Bold`.
- `NotoArabic_<size>_Bold.c` — `NotoSansArabic-Bold.ttf`, Arabic glyphs and
  presentation forms (`U+FB50–FDFF`, `U+FE70–FEFF`) used by LVGL's Arabic
  shaping.

Fixed ranges (ASCII, Latin-1, punctuation, kana, fullwidth) are added so
runtime data such as numbers and dates renders even when it is not in the
workbook. CJK is limited to characters found in the workbook.

After changing the workbook, regenerate both `lv_i18n.c` and the fonts; the
font manifest records the workbook/character-set hashes so `--check` catches
stale files.
