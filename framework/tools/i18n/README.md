# i18n / bitmap font tooling

The single source of truth for smart_home UI copy is:

```
demo/smart_home/i18n/strings.xlsx
```

It is a real `.xlsx` workbook (sheet `strings`) with one row per `strid`:

| strid | zh-CN | en-US | ja-JP | ru-RU | ar | comment |
|---|---|---|---|---|---|---|

An optional `role` column (styles such as `Title` / `Body1`) is used by the
role-font workflow described below.

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
# Generate demo/smart_home/i18n/translations/lv_i18n.c (checked in)
python3 -I framework/tools/i18n/gen_lv_i18n.py \
  --xlsx demo/smart_home/i18n/strings.xlsx \
  --out demo/smart_home/i18n/translations/lv_i18n.c

# CI / pre-commit: fail if the generated C file is stale
python3 -I framework/tools/i18n/gen_lv_i18n.py ... --check --strict

# Verify every lvTr("...")/lvTrFmt("...") literal exists in the workbook
python3 -I framework/tools/i18n/check_keys.py \
  --xlsx demo/smart_home/i18n/strings.xlsx --sources demo/smart_home

# Add/update rows from a CSV/TSV without opening Excel
python3 -I framework/tools/i18n/upsert_keys.py \
  --xlsx demo/smart_home/i18n/strings.xlsx --input new_keys.tsv

# One-time YAML migration (only needed for the legacy tree)
python3 -I framework/tools/i18n/migrate_yaml_to_xlsx.py
```

`lv_i18n.h` is a fixed template and is not regenerated. The C generator uses
`framework/tools/i18n/templates/lv_i18n.c.in`.

## Semantic typography roles (`strings` + `fonts` sheets)

`gen_role_fonts.py` reads the `strings` sheet and a `fonts` sheet, and
generates one bitmap font per **(language, role)** from the characters of the
strids assigned to that role. Role names are a closed enum:
`LVGLPP_FONT_ROLE_TITLE`, `_TITLE2`, `_BODY1`, `_BODY2`
(`Label::Role` in C++).

The `roles` cell is a **multi-select dropdown**: it may be empty, hold one
role, or hold several. The control stores the selections with a delimiter
(comma, semicolon, pipe or newline); the generator accepts all of them, so
users only click options and never type separators.

```text
strings: strid | roles | en-US | zh-CN | ja-JP | ru-RU | ar | comment
fonts:   lang  | role  | family | size | weight | size_delta
```

Example (the cell itself is rendered as a multi-select dropdown in Excel/WPS):

| strid | roles | en-US |
|---|---|---|
| home_title | Title | Smart Home |
| common_energy_kwh | Title,Body2 | {:.1f} kWh |

Each selected role adds that string's characters to the role's font.
Rendering a string with a role it was not selected for intentionally produces
missing glyphs. An empty cell means the string belongs to no role font.

The repo's minimal xlsx writer creates the `roles` column values but not the
dropdown control itself; apply the Excel/WPS multi-select dropdown to that
column in the workbook. If your control stores selections in a different
format, provide a saved sample and the parser can be adapted.

- `lang = *` (or `default`) rows define the shared role table.
- A concrete locale row overrides only the cells it fills in; blanks inherit
  from `*`. `size_delta` adds to the inherited pixel size.
- Supported families map to `assets/common/fonts/*.ttf` (`NotoSansSC`,
  `NotoSansSC-AR`, `NotoSansJP`, `NotoSansArabic`).
- Generated output: `RoleFont_<locale>_<role>.c`, `font_table_<locale>.h/.c`,
  `role_fonts.h/.c` and `role_font_manifest.json`.

```bash
python3 -I framework/tools/i18n/gen_role_fonts.py \
  --xlsx demo/typography_demo/i18n/strings.xlsx \
  --out demo/typography_demo/i18n/fonts --prefix typography_demo

# verify the workbook and generated fonts are in sync
python3 -I framework/tools/i18n/gen_role_fonts.py \
  --xlsx demo/typography_demo/i18n/strings.xlsx \
  --out demo/typography_demo/i18n/fonts --prefix typography_demo --check
```

See `demo/typography_demo/README.md` for the complete example. The real
`demo/smart_home/i18n/strings.xlsx` uses the same multi-select `roles` column;
its generated fonts live in `demo/smart_home/i18n/fonts/` and are verified by
`cmake --build build --target i18n-check`.

## Bitmap fonts

Fonts are subset from the characters that actually appear in the workbook:

```bash
npm --prefix framework/tools/i18n install      # .npmrc uses registry.npmmirror.com

python3 -I framework/tools/i18n/gen_fonts.py \
  --xlsx demo/smart_home/i18n/strings.xlsx \
  --fonts-dir assets/common/fonts \
  --out framework/core/tools/fonts --jobs 4

# Validate the committed files against font_manifest.json
python3 -I framework/tools/i18n/gen_fonts.py \
  --xlsx demo/smart_home/i18n/strings.xlsx --out framework/core/tools/fonts --check
```

Generated per size `20/22/24/28/32/36`:

- `NotoSC_<size>_Normal.c` — `NotoSansSC-AR-Regular.ttf`, all locales, with
  `--lv-fallback NotoArabic_<size>_Normal`.
- `NotoSC_<size>_Bold.c` — `NotoSansSC-Bold.ttf`, with
  `--lv-fallback NotoArabic_<size>_Bold`.
- `NotoArabic_<size>_Normal.c` — `NotoSansArabic-Regular.ttf`, Arabic glyphs
  and presentation forms (`U+FB50–FDFF`, `U+FE70–FEFF`) used by LVGL's Arabic
  shaping. `NotoSansSC-AR-Regular.ttf` only carries part of these forms, so
  the fallback fills the gaps. It also chains
  `--lv-fallback lv_font_montserrat_<size>` so LVGL symbol glyphs
  (`U+F000–F8FF`, e.g. dropdown arrows) keep rendering with the Noto default.
- `NotoArabic_<size>_Bold.c` — the same set from
  `NotoSansArabic-Bold.ttf`.

Fixed ranges (ASCII, Latin-1, punctuation, kana, fullwidth) are added so
runtime data such as numbers and dates renders even when it is not in the
workbook. CJK is limited to characters found in the workbook.

After changing the workbook, regenerate both `lv_i18n.c` and the fonts; the
font manifest records the workbook/character-set hashes so `--check` catches
stale files.

## Constrained-target font profiles (ESP32)

`gen_fonts_profile.py` writes an independent per-language profile and never
modifies `framework/core/tools/fonts/`:

```bash
python3 -I framework/tools/i18n/gen_fonts_profile.py \
  --xlsx demo/smart_home/i18n/strings.xlsx \
  --langs zh-CN,en-US --sizes 16,24 --styles normal \
  --bpp 4 --compress \
  --extra-charset ports/esp32/dynamic_charset.txt \
  --out ports/esp32/fonts --emit-table
```

- `--langs` filters the workbook columns; only those glyphs are included.
- `--sizes/--styles` keep only the faces the UI uses (default `16,24`/`normal`).
- `--bpp 4 --compress` cuts flash substantially; use `--bpp 2` only if
  legibility allows.
- `--extra-charset FILE` adds characters needed by runtime data (device names,
  SSIDs, user input).
- `--emit-table` writes `font_table_<lang>.h/.c` with a `LvglppFontFace` table.
  Register it at runtime with
  `LvFontManager::setProfile(lvglpp_fonts_<lang>, lvglpp_fonts_<lang>_count);`
  (or `setProfile(nullptr, 0)` to restore the desktop table).
- `--dry-run` prints the `lv_font_conv` commands without writing anything.

Never point `--out` at `framework/core/tools/fonts/`; the desktop full set and its
`font_manifest.json` must stay unchanged.
