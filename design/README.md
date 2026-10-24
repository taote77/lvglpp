# Design tokens

`design/tokens.json` is the single source of truth for the UI colour palette
and the design dimensions shared by the SDK and smart_home. Generated C++
headers are committed so a normal build needs no Python, network access or
Figma credentials.

```
design/tokens.json                  <- edit / sync from Figma
        |
        |  sync_tokens.py generate
        v
sdk/config/color_sheet.h            ColorToken values (dark + light), dimensions, opacity
sdk/config/color_sheet_light.h      explicit light-theme values (dark fallback)
```

Do not edit the generated headers by hand: `design-check` fails when they drift
from the token file.

## Quick start

```bash
# edit design/tokens.json, then
python3 -I sdk/tools/design/sync_tokens.py generate

# verify committed headers are current (used by CI)
python3 -I sdk/tools/design/sync_tokens.py generate --check
# or
cmake --build build --target design-check
```

## Updating from Figma

Two export paths are supported. Both update `design/tokens.json`, regenerate
the headers, and print a diff of the changed values. Use `--dry-run` first.

### A. Tokens Studio (recommended)

The [Tokens Studio](https://tokens.studio/) Figma plugin works on every Figma
plan:

1. In Figma, open **Tokens Studio -> Export -> JSON**.
2. Save the export outside the repository (it may contain unrelated tokens).
3. Import it:

```bash
python3 -I sdk/tools/design/sync_tokens.py import /tmp/figma-tokens.json --dry-run
python3 -I sdk/tools/design/sync_tokens.py import /tmp/figma-tokens.json
```

The importer understands the usual `$themes` export (one theme named `Dark` and
one named `Light`, each selecting token sets) and flat single-set exports. For
single-set exports pass `--theme dark` or `--theme light` to choose the target
mode. A `$value` may be a hex colour, an `rgb()/rgba()` string, or an alias such
as `{color.brand.primary}`.

### B. Figma Variables REST API

Requires a Figma Enterprise plan and a personal access token:

```bash
export FIGMA_TOKEN=...            # never commit this
python3 -I sdk/tools/design/sync_tokens.py fetch \
    --file-key <figma-file-key> --dry-run
python3 -I sdk/tools/design/sync_tokens.py fetch --file-key <figma-file-key>
```

Mode names are matched through `meta.figma.themeMap` in the token file
(default `Dark`/`Light`); override per run with `--theme-map dark=Night,light=Day`.
The file key is stored in the token file after a successful sync.

### Matching Figma names to C++ tokens

By default `CLR_ON_SURFACE_VARIANT_1` matches a Figma path whose normalised
segments end in `on/surface/variant/1` (`On Surface Variant 1`,
`color/on-surface-variant-1`, ...). Exact matches win over suffix matches, and
ambiguous matches are reported instead of guessed.

If a design token cannot be matched by name, pin it explicitly:

```json
"CLR_PRIMARY": {
  "dark": "#4986FF",
  "light": "#3760EA",
  "figma": "Brand/Primary"
}
```

Unknown Figma colours are reported and ignored unless `--add` is passed, which
creates `CLR_<PATH>` entries (useful for a first import, but review the names).

## Editing tokens directly

A colour entry needs a `dark` value and may add a `light` value. Missing light
values fall back to the dark value.

```json
"CLR_WARNING": {
  "dark": "#F5A34A",
  "light": "#ED6C02"
}
```

- Names must be `UPPER_SNAKE_CASE` and are emitted as C++ identifiers.
- Dimensions are non-negative integers; opacity values are `0.0 .. 1.0`.
- Adding a `ColorRole` in `sdk/core/style/Theme.h` also requires adding the
  matching pair to `kPalette` in `Theme.cpp`; a `static_assert` catches a missed
  role at compile time.

## Using tokens from C++

The generated `CLR_*` constants are `form::design::ColorToken` values. They
implicitly convert to the value for the active `core::ThemeMode`, so existing
call sites follow `Theme::setMode()` without being rewritten:

```cpp
// Resolves to the dark or light value at read time.
label.setTextColor(form::design::CLR_ON_SURFACE);

// Explicit values when theme switching is not wanted.
uint32_t dark  = form::design::CLR_ON_SURFACE.darkValue();
uint32_t light = form::design::CLR_ON_SURFACE.lightValue();
uint32_t same  = form::design::light::CLR_ON_SURFACE;

// Semantic roles, including roles not exposed as a single CLR_ token.
label.setTextColor(lvglpp::core::Theme::instance().color(
    lvglpp::core::ColorRole::OnSurface));
```

Because a token resolves when it is read, widgets keep the colour they were
created with. After calling `Theme::setMode()`, rebuild the UI to re-create it;
smart_home does this from the Settings page with
`sys::UiConfig::instance().requestRebuild()`. For manual testing the app also
accepts `LVGLPP_THEME=light` at startup.

`form::design::ThemeColor(color)` is a legacy no-op kept for existing call
sites; its argument has already been resolved to the active theme. New
theme-aware code can use `core::Theme` or the explicit
`ThemeColor(dark, light, style)` overload.

`ColorToken` conversions are implemented in `sdk/config/color_sheet.cpp`, so
code using the generated tokens must link `lvglpp`. The token values are not
compile-time constants; use `darkValue()/lightValue()` when a `constexpr` is
required.
