#!/usr/bin/env python3
"""Generate per-language, per-role LVGL bitmap fonts from a workbook.

The workbook has two sheets:

``strings``
    One column per locale plus a ``roles`` cell written by the spreadsheet's
    multi-select dropdown. It may be empty, hold one role, or hold several
    (the control stores them with comma / semicolon / pipe / newline
    separators). The same strid can therefore be used at several typography
    sizes.

``fonts``
    The typography table: one row per role and language.  ``lang`` may be
    ``*`` (or ``default``) for the shared definition; a concrete locale row
    overrides only the columns it fills in::

        lang  role    family          size  weight  size_delta
        *     Title   NotoSansSC      32    bold    0
        *     Body1   NotoSansSC      20    normal  0
        ja-JP Title   NotoSansJP
        ar    Title   NotoSansArabic              2

Each strid may declare one or more roles (``Body1,Body2``). Its characters are
included in every role font it lists. A role font contains only the strings
assigned to that role, so rendering a string with a role it was not assigned
to intentionally produces missing glyphs.

The output directory receives:

  * ``RoleFont_<locale>_<role>.c``
  * ``font_table_<locale>.h/.c``
  * ``role_fonts.h/.c``       (locale -> role table registry)
  * ``role_font_manifest.json``

Example:

    python3 -I framework/tools/i18n/gen_role_fonts.py \\
        --xlsx demo/typography_demo/i18n/strings.xlsx \\
        --out demo/typography_demo/i18n/fonts \\
        --prefix typography_demo
"""

from __future__ import annotations

import argparse
import hashlib
import json
import re
import subprocess
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))

from i18n_model import Locale, c_string_literal, load_model  # noqa: E402
from xlsx_io import read_sheet  # noqa: E402

SCRIPT_DIR = Path(__file__).resolve().parent
DEFAULT_TOOL = SCRIPT_DIR / "node_modules" / ".bin" / "lv_font_conv"
MANIFEST = "role_font_manifest.json"
GLYPH_RE = re.compile(r"/\*\s*U\+([0-9A-Fa-f]+)")

# Role names accepted by the workbook and their C enum constants. Extend
# LvglppFontRole (FontProfile.h) and Label::Role together with this map.
ROLE_ENUMS = {
    "Title": "LVGLPP_FONT_ROLE_TITLE",
    "Title2": "LVGLPP_FONT_ROLE_TITLE2",
    "Body1": "LVGLPP_FONT_ROLE_BODY1",
    "Body2": "LVGLPP_FONT_ROLE_BODY2",
}

# Role fonts are strict subsets: only the characters actually used by the
# role's strings (passed through --symbols) plus basic ASCII/punctuation for
# mixed runtime data. Language-wide ranges (kana, Arabic presentation forms,
# ...) would defeat the purpose and make every role font huge.
COMMON_RANGES = (
    "0x20-0x7E",     # ASCII; translated punctuation arrives through --symbols
)

# LV_USE_ARABIC_PERSIAN_CHARS rewrites Arabic text to presentation forms
# before rendering, so those glyphs must be present even though the workbook
# stores logical Arabic.
LOCALE_RANGES = {
    "ar": ("0xFB50-0xFDFF", "0xFE70-0xFEFF"),
}

# Font family -> TTF files per weight.  The demo's fonts.json can add or
# override entries through its optional "families" object.
DEFAULT_FAMILIES = {
    "NotoSansSC": {
        "normal": "NotoSansSC-Regular.ttf",
        "bold": "NotoSansSC-Bold.ttf",
    },
    "NotoSansSC-AR": {
        "normal": "NotoSansSC-AR-Regular.ttf",
        "bold": "NotoSansSC-Bold.ttf",
    },
    "NotoSansJP": {
        "normal": "NotoSansJP-Regular.ttf",
        "bold": "NotoSansJP-Bold.ttf",
    },
    "NotoSansArabic": {
        "normal": "NotoSansArabic-Regular.ttf",
        "bold": "NotoSansArabic-Bold.ttf",
    },
}


def _sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        for chunk in iter(lambda: handle.read(1 << 20), b""):
            digest.update(chunk)
    return digest.hexdigest()


def _identifier(value: str) -> str:
    """Turn a role/prefix into a safe C identifier fragment."""
    cleaned = re.sub(r"[^0-9A-Za-z_]", "_", value.strip())
    if not cleaned or cleaned[0].isdigit():
        cleaned = "_" + cleaned
    return cleaned


def _resolve_locale(locales: tuple[Locale, ...], value: str) -> Locale | None:
    wanted = value.strip().lower()
    for locale in locales:
        if wanted == locale.tag.lower() or wanted in locale.aliases:
            return locale
    return None


FONT_TABLE_HEADERS = {
    "lang": {"lang", "language", "locale"},
    "role": {"role", "style", "typography"},
    "family": {"family", "font", "font_family"},
    "size": {"size", "px", "pixel_size", "font_size"},
    "weight": {"weight", "font_weight", "variation"},
    "size_delta": {"size_delta", "delta", "px_delta"},
}


def _column_indices(header: list[str]) -> dict[str, int]:
    normalized = [str(cell).strip().lower() for cell in header]
    indices: dict[str, int] = {}
    for key, aliases in FONT_TABLE_HEADERS.items():
        for index, name in enumerate(normalized):
            if name in aliases:
                indices[key] = index
                break
    return indices


def _load_font_table(xlsx: Path, sheet_name: str,
                     locales: tuple[Locale, ...]) -> tuple[dict, dict]:
    """Parse the ``fonts`` sheet into default and per-language role maps."""
    rows = read_sheet(xlsx, sheet_name)
    if not rows:
        raise ValueError(f"workbook has no {sheet_name!r} sheet")

    columns = _column_indices(rows[0])
    missing = [key for key in ("lang", "role") if key not in columns]
    if missing:
        raise ValueError(f"{sheet_name!r} sheet is missing column(s): {', '.join(missing)}")

    defaults: dict[str, dict] = {}
    overrides: dict[str, dict[str, dict]] = {}

    for row_index, row in enumerate(rows[1:], start=2):
        def cell(key: str) -> str:
            index = columns.get(key)
            if index is None or index >= len(row):
                return ""
            return str(row[index]).strip()

        role = cell("role")
        if not role:
            continue

        entry: dict = {}
        family = cell("family")
        if family:
            entry["family"] = family
        weight = cell("weight").lower()
        if weight:
            if weight not in ("normal", "bold"):
                raise ValueError(f"fonts row {row_index}: weight must be normal or bold")
            entry["weight"] = weight
        for key, field_name in (("size", "size"), ("size_delta", "size_delta")):
            value = cell(key)
            if value:
                try:
                    entry[field_name] = int(value)
                except ValueError as exc:
                    raise ValueError(f"fonts row {row_index}: invalid {key} {value!r}") from exc
        if entry.get("size", 1) <= 0:
            raise ValueError(f"fonts row {row_index}: size must be positive")

        lang = cell("lang").lower()
        if lang in ("", "*", "default", "all"):
            defaults.setdefault(role, {}).update(entry)
            continue

        locale = _resolve_locale(locales, lang)
        if locale is None:
            raise ValueError(f"fonts row {row_index}: unknown language {lang!r}")
        overrides.setdefault(locale.tag, {}).setdefault(role, {}).update(entry)

    if not defaults:
        raise ValueError(f"{sheet_name!r} sheet defines no default ('*') roles")

    for role, entry in defaults.items():
        if "size" not in entry:
            raise ValueError(f"role {role!r} has no size in the fonts sheet")
        entry.setdefault("family", "NotoSansSC")
        entry.setdefault("weight", "normal")
        if entry["family"] not in DEFAULT_FAMILIES:
            raise ValueError(f"role {role!r}: unknown family {entry['family']!r}")

    for lang, roles in overrides.items():
        for role in roles:
            if role not in defaults:
                raise ValueError(f"fonts sheet: {lang!r} overrides unknown role {role!r}")

    return defaults, overrides


def _descriptor(defaults: dict, overrides: dict, locale: Locale,
                role: str) -> tuple[str, int, str]:
    """Resolve (family, pixel size, weight) for one language/role pair."""
    base = defaults[role]
    language = overrides.get(locale.tag, {}).get(role, {})

    family = language.get("family", base["family"])
    weight = str(language.get("weight", base["weight"])).lower()
    size = int(language.get("size", base["size"]))
    size += int(language.get("size_delta", base.get("size_delta", 0)))

    if weight not in ("normal", "bold"):
        raise ValueError(f"role {role!r}: weight must be normal or bold")
    if size <= 0:
        raise ValueError(f"role {role!r}: size must be positive")
    if family not in DEFAULT_FAMILIES:
        raise ValueError(f"role {role!r}: unknown family {family!r}")
    return family, size, weight


def _row_roles(row) -> set[str]:
    """Roles in the multi-select cell (empty / one / several)."""
    raw = (row.role or "").strip()
    if not raw:
        return set()
    return {part.strip() for part in re.split(r"[,;|，、\n]+", raw) if part.strip()}


def _role_charset(model, locale: Locale, role: str, extra: str) -> str:
    """Characters of the strings assigned to *role* in this locale."""
    chars = {char for char in extra if char not in "\r\n\t"}
    for row in model.rows:
        if role not in _row_roles(row):
            continue
        for char in row.values.get(locale.tag, ""):
            if char not in "\r\n\t":
                chars.add(char)
    chars.discard("\n")
    chars.discard("\r")
    chars.discard("\t")
    return "".join(sorted(chars, key=ord))


def _glyphs(path: Path) -> set[int]:
    text = path.read_text(encoding="utf-8", errors="replace")
    return {int(match.group(1), 16) for match in GLYPH_RE.finditer(text)}


def _verify_output(path: Path, expected: str, label: str) -> bool:
    present = _glyphs(path)
    missing = [char for char in expected if ord(char) not in present]
    if missing:
        sample = ", ".join(f"U+{ord(char):04X} ({char!r})" for char in missing[:20])
        print(f"error: {label} is missing {len(missing)} glyph(s): {sample}", file=sys.stderr)
        return False
    return True


def _command(tool: Path, source: Path, size: int, bpp: int, compress: bool,
             output: Path, symbols: str, ranges: tuple[str, ...]) -> list[str]:
    command = [
        str(tool),
        "--font", str(source),
        "--size", str(size),
        "--bpp", str(bpp),
        "--format", "lvgl",
        "--autohint-off",
        "--use-color-info",
        "--output", str(output),
    ]
    if not compress:
        command.append("--no-compress")
    for value in ranges:
        command.extend(["--range", value])
    if symbols:
        command.extend(["--symbols", symbols])
    return command


def _table_symbol(prefix: str, locale: Locale) -> str:
    return f"{_identifier(prefix)}_role_fonts_{locale.symbol}"


def _emit_language_table(out_dir: Path, prefix: str, locale: Locale,
                         entries: list[tuple[str, int, str]]) -> tuple[str, str]:
    """Write font_table_<locale>.h/.c; return the two file names."""
    table = _table_symbol(prefix, locale)
    guard = f"{_identifier(prefix).upper()}_ROLE_FONT_TABLE_{locale.symbol.upper()}_H"
    header_name = f"font_table_{locale.symbol}.h"
    source_name = f"font_table_{locale.symbol}.c"

    lines = [
        f"#ifndef {guard}",
        f"#define {guard}",
        "",
        '#include "core/tools/fonts/FontProfile.h"',
        "",
    ]
    for _role, _size, symbol in entries:
        lines.append(f"LV_FONT_DECLARE({symbol});")
    lines.extend([
        "",
        f"#define {table}_count {len(entries)}U",
        f"extern const LvglppRoleFont {table}[];",
        "",
        f"#endif // {guard}",
        "",
    ])
    (out_dir / header_name).write_text("\n".join(lines), encoding="utf-8", newline="\n")

    source = [
        f'#include "{header_name}"',
        "",
        f"const LvglppRoleFont {table}[] = {{",
    ]
    for role, size, symbol in entries:
        source.append(f"    {{{ROLE_ENUMS[role]}, {size}, &{symbol}}},")
    source.extend([
        "};",
        "",
    ])
    (out_dir / source_name).write_text("\n".join(source), encoding="utf-8", newline="\n")
    return header_name, source_name


def _emit_registry(out_dir: Path, prefix: str,
                   tables: list[tuple[Locale, list[tuple[str, int, str]]]],
                   default_role: str) -> tuple[str, str]:
    """Write role_fonts.h/.c with the locale -> role-font registry."""
    ident = _identifier(prefix)
    guard = f"{ident.upper()}_ROLE_FONTS_H"
    header_name = "role_fonts.h"
    source_name = "role_fonts.c"

    header = [
        f"#ifndef {guard}",
        f"#define {guard}",
        "",
        '#include "core/tools/fonts/FontProfile.h"',
        "",
        "#ifdef __cplusplus",
        'extern "C" {',
        "#endif",
        "",
        "typedef struct LvglppLanguageRoleFonts {",
        "    const char           *locale;",
        "    const LvglppRoleFont *fonts;",
        "    unsigned              count;",
        "} LvglppLanguageRoleFonts;",
        "",
        f"extern const LvglppLanguageRoleFonts {ident}_language_role_fonts[];",
        f"extern const unsigned {ident}_language_role_fonts_count;",
        "",
        "/** Resolve a role font for a locale. Falls back to en-US and then to",
        " *  the default role. Returns NULL only when no fonts were generated. */",
        f"const lv_font_t *{ident}_role_font(const char *locale, LvglppFontRole role);",
        "",
        "#ifdef __cplusplus",
        "}",
        "#endif",
        "",
        f"#endif // {guard}",
        "",
    ]
    (out_dir / header_name).write_text("\n".join(header), encoding="utf-8", newline="\n")

    source = [
        f'#include "{header_name}"',
        "",
    ]
    for locale, _entries in tables:
        source.append(f'#include "font_table_{locale.symbol}.h"')
    source.extend([
        "",
        "#include <string.h>",
        "",
        f"const LvglppLanguageRoleFonts {ident}_language_role_fonts[] = {{",
    ])
    for locale, _entries in tables:
        table = _table_symbol(prefix, locale)
        source.append(f'    {{{c_string_literal(locale.tag)}, {table}, {table}_count}},')
    source.extend([
        "};",
        f"const unsigned {ident}_language_role_fonts_count =",
        f"    sizeof({ident}_language_role_fonts) / sizeof({ident}_language_role_fonts[0]);",
        "",
        "static const LvglppLanguageRoleFonts *find_table(const char *locale)",
        "{",
        "    if (locale != NULL)",
        "    {",
        f"        for (unsigned i = 0; i < {ident}_language_role_fonts_count; ++i)",
        "        {",
        f"            if (strcmp({ident}_language_role_fonts[i].locale, locale) == 0)",
        "            {",
        f"                return &{ident}_language_role_fonts[i];",
        "            }",
        "        }",
        "    }",
        "    return NULL;",
        "}",
        "",
        "static const lv_font_t *find_role(const LvglppRoleFont *fonts, unsigned count,",
        "                                  LvglppFontRole role)",
        "{",
        "    if (fonts == NULL)",
        "    {",
        "        return NULL;",
        "    }",
        "    for (unsigned i = 0; i < count; ++i)",
        "    {",
        "        if (fonts[i].role == role)",
        "        {",
        "            return fonts[i].font;",
        "        }",
        "    }",
        "    return NULL;",
        "}",
        "",
        f"const lv_font_t *{ident}_role_font(const char *locale, LvglppFontRole role)",
        "{",
        "    const LvglppLanguageRoleFonts *table = find_table(locale);",
        "    if (table == NULL)",
        "    {",
        '        table = find_table("en-US");',
        "    }",
        "    if (table == NULL)",
        "    {",
        "        return NULL;",
        "    }",
        "",
        "    const lv_font_t *font = find_role(table->fonts, table->count, role);",
        "    if (font == NULL)",
        "    {",
        f"        font = find_role(table->fonts, table->count, {ROLE_ENUMS[default_role]});",
        "    }",
        "    if (font == NULL && table->count > 0)",
        "    {",
        "        font = table->fonts[0].font;",
        "    }",
        "    return font;",
        "}",
        "",
    ])
    (out_dir / source_name).write_text("\n".join(source), encoding="utf-8", newline="\n")
    return header_name, source_name


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--xlsx", required=True)
    parser.add_argument("--out", required=True)
    parser.add_argument("--sheet", help="strings sheet (default: first sheet)")
    parser.add_argument("--fonts-sheet", default="fonts",
                        help="typography role table sheet (default: fonts)")
    parser.add_argument("--langs", default="", help="comma-separated locale tags (default: all)")
    parser.add_argument("--fonts-dir", default="assets/common/fonts")
    parser.add_argument("--tool", default=str(DEFAULT_TOOL))
    parser.add_argument("--bpp", type=int, default=4, choices=(1, 2, 3, 4, 8))
    parser.add_argument("--compress", action="store_true")
    parser.add_argument("--extra-charset", help="file with additional characters")
    parser.add_argument("--prefix", default="lvglpp", help="C identifier prefix for the registry")
    parser.add_argument("--dry-run", action="store_true")
    parser.add_argument("--check", action="store_true", help="verify outputs against the manifest")
    args = parser.parse_args(argv)

    xlsx = Path(args.xlsx)
    out_dir = Path(args.out)
    fonts_dir = Path(args.fonts_dir)

    model = load_model(xlsx, args.sheet)
    if model.errors:
        for error in model.errors:
            print(f"error: {error}", file=sys.stderr)
        return 1

    try:
        defaults, overrides = _load_font_table(xlsx, args.fonts_sheet, model.locales)
    except ValueError as exc:
        print(f"error: {exc}", file=sys.stderr)
        return 1

    roles = list(defaults)
    unsupported = [role for role in roles if role not in ROLE_ENUMS]
    if unsupported:
        print(
            f"error: role(s) {unsupported} are not in LvglppFontRole; extend "
            "FontProfile.h, Label::Role and ROLE_ENUMS together",
            file=sys.stderr,
        )
        return 1
    default_role = "Body1" if "Body1" in roles else roles[0]

    # Every role listed in the strings sheet must exist in the fonts sheet.
    # An empty role cell is allowed: the string simply belongs to no role font.
    unassigned = []
    for row in model.rows:
        roles_for_row = _row_roles(row)
        if not roles_for_row:
            unassigned.append(row.strid)
        for role in roles_for_row:
            if role not in defaults:
                print(f"error: {row.strid} uses unknown role {role!r}", file=sys.stderr)
                return 1
    if unassigned:
        print(
            f"warning: {len(unassigned)} string(s) have no role and will be "
            f"missing from every role font (e.g. {', '.join(unassigned[:5])})",
            file=sys.stderr,
        )

    if args.langs:
        locales: list[Locale] = []
        for value in args.langs.split(","):
            locale = _resolve_locale(model.locales, value)
            if locale is None:
                print(f"error: unknown locale {value!r}", file=sys.stderr)
                return 1
            if locale not in locales:
                locales.append(locale)
    else:
        locales = list(model.locales)

    extra = ""
    if args.extra_charset:
        extra = Path(args.extra_charset).read_text(encoding="utf-8")

    if args.check:
        manifest_path = out_dir / MANIFEST
        try:
            manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
        except (OSError, json.JSONDecodeError):
            print(f"error: missing or invalid {manifest_path}", file=sys.stderr)
            return 1
        failed = False
        if manifest.get("xlsx_sha256") != _sha256(xlsx):
            print("error: the workbook changed; regenerate role fonts", file=sys.stderr)
            failed = True
        for key in ("fonts", "tables", "registry"):
            for entry in manifest.get(key, []):
                path = out_dir / entry["file"]
                if not path.exists():
                    print(f"error: missing {path}", file=sys.stderr)
                    failed = True
                elif _sha256(path) != entry["sha256"]:
                    print(f"error: {path} does not match the manifest", file=sys.stderr)
                    failed = True
        if failed:
            return 1
        print(f"role fonts are up to date ({len(manifest.get('fonts', []))} files)")
        return 0

    tool = Path(args.tool)
    if not args.dry_run and not args.check and not tool.exists():
        print(
            f"error: {tool} not found.\n"
            f"Install it with: npm --prefix {SCRIPT_DIR} install",
            file=sys.stderr,
        )
        return 1

    tasks = []
    tables: list[tuple[Locale, list[tuple[str, int, str]]]] = []
    for locale in locales:
        entries: list[tuple[str, int, str]] = []
        for role in roles:
            try:
                family, size, weight = _descriptor(defaults, overrides, locale, role)
            except ValueError as exc:
                print(f"error: {exc}", file=sys.stderr)
                return 1
            symbols = _role_charset(model, locale, role, extra)
            if not symbols:
                continue
            source = fonts_dir / DEFAULT_FAMILIES[family][weight]
            if not source.exists():
                print(f"error: missing source font {source}", file=sys.stderr)
                return 1
            symbol = f"RoleFont_{locale.symbol}_{_identifier(role)}"
            output = out_dir / f"{symbol}.c"
            ranges = COMMON_RANGES + LOCALE_RANGES.get(locale.tag, ())
            tasks.append({
                "locale": locale,
                "role": role,
                "family": family,
                "size": size,
                "weight": weight,
                "source": source,
                "symbol": symbol,
                "output": output,
                "symbols": symbols,
                "ranges": ranges,
            })
            entries.append((role, size, symbol))
        if entries:
            tables.append((locale, entries))

    if not tasks:
        print("error: no (locale, role) pair has translated strings", file=sys.stderr)
        return 1

    if args.dry_run:
        for task in tasks:
            command = _command(tool, task["source"], task["size"], args.bpp, args.compress,
                               task["output"], task["symbols"], task["ranges"])
            print(" ".join(command))
        print(f"would write {len(tasks)} role font(s) to {out_dir}")
        return 0

    out_dir.mkdir(parents=True, exist_ok=True)
    manifest_fonts = []
    for task in tasks:
        command = _command(tool, task["source"], task["size"], args.bpp, args.compress,
                           task["output"], task["symbols"], task["ranges"])
        result = subprocess.run(command, check=False, capture_output=True, text=True)
        if result.returncode != 0:
            print(f"error: lv_font_conv failed for {task['output'].name}:\n{result.stderr}",
                  file=sys.stderr)
            return 1
        if not task["output"].exists():
            print(f"error: lv_font_conv did not produce {task['output']}", file=sys.stderr)
            return 1
        if not _verify_output(task["output"], task["symbols"], task["output"].name):
            return 1
        manifest_fonts.append({
            "file": task["output"].name,
            "locale": task["locale"].tag,
            "role": task["role"],
            "family": task["family"],
            "size": task["size"],
            "weight": task["weight"],
            "source": task["source"].name,
            "sha256": _sha256(task["output"]),
        })
        print(f"generated {task['output']}")

    table_files = []
    for locale, entries in tables:
        table_files.extend(_emit_language_table(out_dir, args.prefix, locale, entries))
    registry_files = list(_emit_registry(out_dir, args.prefix, tables, default_role))

    manifest = {
        "tool": "lv_font_conv",
        "xlsx_sha256": _sha256(xlsx),
        "fonts_sheet": args.fonts_sheet,
        "prefix": args.prefix,
        "default_role": default_role,
        "langs": [locale.tag for locale, _entries in tables],
        "roles": roles,
        "bpp": args.bpp,
        "compress": args.compress,
        "fonts": manifest_fonts,
        "tables": [
            {"file": name, "sha256": _sha256(out_dir / name)}
            for name in table_files
        ],
        "registry": [
            {"file": name, "sha256": _sha256(out_dir / name)}
            for name in registry_files
        ],
    }
    (out_dir / MANIFEST).write_text(
        json.dumps(manifest, ensure_ascii=False, indent=2) + "\n",
        encoding="utf-8",
        newline="\n",
    )
    print(f"wrote {len(manifest_fonts)} role font(s), {len(table_files)} table file(s), "
          f"registry and {out_dir / MANIFEST}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
