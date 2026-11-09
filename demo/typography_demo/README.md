# typography_demo — semantic roles + per-language bitmap fonts

A demo showing how **UI copy, typography roles and bitmap fonts** are all
driven by one Excel workbook:

- each strid has a multi-select `roles` cell (empty / one / several), so the
  same string can be used at several typography sizes
- the `fonts` sheet maps each role to a **font family, pixel size and weight**
- role names are an enum: `Label::Role::{Title, Title2, Body1, Body2}`
- the generator produces one subset bitmap font per **(language, role)** and
  emits C tables using `LvglppFontRole`
- using a role the string was not assigned to intentionally renders missing
  glyphs
- the UI only asks for a role; it never mentions `FontSizeXX`

## Workbook layout

`i18n/strings.xlsx` has two sheets.

### `strings`

| strid | roles | en-US | zh-CN | ja-JP | ru-RU | ar | comment |
|---|---|---|---|---|---|---|---|
| typo_title | Title | Typography | 排版系统 | タイポグラフィ | Типографика | الطباعة | page title |
| typo_body1 | Body1 | ... | ... | ... | ... | ... | body copy |

The `roles` cell is a multi-select dropdown (Excel/WPS): it can be empty, hold
one role, or hold several. The control stores selections with a delimiter
(comma/semicolon/pipe/newline), which the generator parses. An empty cell
means the string is not included in any role font.

### `fonts`

| lang | role | family | size | weight | size_delta |
|---|---|---|---|---|---|
| `*` | Title | NotoSansSC | 32 | bold | 0 |
| `*` | Title2 | NotoSansSC | 28 | bold | 0 |
| `*` | Body1 | NotoSansSC | 20 | normal | 0 |
| `*` | Body2 | NotoSansSC | 16 | normal | 0 |
| zh-CN | Title / Title2 / Body1 / Body2 | NotoSansSC | | | |
| en-US | Title / Title2 / Body1 / Body2 | NotoSansSC | | | |
| ru-RU | Title / Title2 / Body1 / Body2 | NotoSansSC | | | |
| ja-JP | Title / Title2 / Body1 / Body2 | NotoSansJP | | | |
| ar | Title / Title2 / Body1 / Body2 | NotoSansArabic | | | 2 |

- `*` (or `default`) rows define the shared role table.
- A concrete locale row overrides only the fields it fills in; blank cells
  inherit from the `*` row.
- `size_delta` adds to the inherited size (Arabic is 2 px larger here).
- Supported families are mapped to the TTFs in `assets/common/fonts/` by
  `gen_role_fonts.py` (`NotoSansSC`, `NotoSansSC-AR`, `NotoSansJP`,
  `NotoSansArabic`).

## Regenerate

```bash
# lv_i18n translation table
python3 -I framework/tools/i18n/gen_lv_i18n.py \
  --xlsx demo/typography_demo/i18n/strings.xlsx \
  --out demo/typography_demo/i18n/translations/lv_i18n.c --strict

# per-(language, role) bitmap fonts + C role tables
python3 -I framework/tools/i18n/gen_role_fonts.py \
  --xlsx demo/typography_demo/i18n/strings.xlsx \
  --out demo/typography_demo/i18n/fonts \
  --prefix typography_demo

# CI-style verification
python3 -I framework/tools/i18n/gen_role_fonts.py \
  --xlsx demo/typography_demo/i18n/strings.xlsx \
  --out demo/typography_demo/i18n/fonts \
  --prefix typography_demo --check
```

The generated files are committed:

```
i18n/fonts/
├── RoleFont_<locale>_<role>.c   # role subset bitmap font
├── font_table_<locale>.h/.c     # LvglppRoleFont {role, size, font}*
├── role_fonts.h/.c              # locale -> role table registry
└── role_font_manifest.json      # hashes for --check
```

## Runtime

`role_fonts.h` exposes:

```c
const lv_font_t *typography_demo_role_font(const char *locale, LvglppFontRole role);
```

`initTranslations()` installs a `Label` role resolver, so the UI only names a
role:

```cpp
using Role = widgets::Label::Role;
auto *title = store_.emplace<widgets::Label>(tr("typo_title"), color, root, Role::Title);
auto *body  = store_.emplace<widgets::Label>(tr("typo_body1"), color, root, Role::Body1);
```

Switching the language calls `UiConfig::requestRebuild()`; the activity
rebuilds and every label resolves the font for the new locale. Arabic also
requests RTL; its fonts include Arabic presentation forms because
`LV_USE_ARABIC_PERSIAN_CHARS` rewrites text before rendering.

## Build and run

```bash
cmake --build build --target typography_demo -j$(nproc)
cd build/target/bin
./typography_demo
```

The language buttons switch the locale at runtime. For screenshots or
automation the initial locale can be selected with:

```bash
TYPOGRAPHY_DEMO_LANG=ja-JP ./typography_demo
```

Add a new role by adding rows to `fonts` and using the role name in the
`strings` sheet; add a language override by adding a row with that locale tag.
