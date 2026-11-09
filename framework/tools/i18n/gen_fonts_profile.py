#!/usr/bin/env python3
"""Generate a per-language LVGL bitmap-font profile for constrained targets.

Unlike gen_fonts.py (the desktop full set under framework/core/tools/fonts/), this
script writes only the selected languages/sizes/styles into a separate output
directory. It never modifies the desktop font set.

Example (ESP32):

    python3 -I framework/tools/i18n/gen_fonts_profile.py \
        --xlsx demo/smart_home/i18n/strings.xlsx \
        --langs zh-CN,en-US --sizes 16,24 --styles normal \
        --bpp 4 --compress \
        --out ports/esp32/fonts --emit-table

The optional --extra-charset file contains characters that must be included
for runtime data (device names, SSIDs, user input).
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

from i18n_model import Locale, load_model  # noqa: E402

SCRIPT_DIR = Path(__file__).resolve().parent
DEFAULT_TOOL = SCRIPT_DIR / "node_modules" / ".bin" / "lv_font_conv"
DEFAULT_SIZES = (16, 24)
DEFAULT_STYLES = ("normal",)
MANIFEST = "esp_font_manifest.json"
GLYPH_RE = re.compile(r"/\*\s*U\+([0-9A-Fa-f]+)")

COMMON_RANGES = (
    "0x20-0x7E",     # ASCII
    "0xA0-0xFF",     # Latin-1 supplement
    "0x2000-0x206F", # general punctuation
)

LANGUAGE_RANGES = {
    "zh-CN": ("0x3000-0x303F", "0xFF00-0xFFEF"),
    "ja-JP": ("0x3000-0x303F", "0x3040-0x30FF", "0xFF00-0xFFEF"),
    "ru-RU": ("0x0400-0x04FF",),
    "ar":    ("0xFB50-0xFDFF", "0xFE70-0xFEFF"),
    "en-US": (),
}


def _sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        for chunk in iter(lambda: handle.read(1 << 20), b""):
            digest.update(chunk)
    return digest.hexdigest()


def _resolve_locale(locales: tuple[Locale, ...], value: str) -> Locale | None:
    wanted = value.strip().lower()
    for locale in locales:
        if wanted == locale.tag.lower() or wanted in locale.aliases:
            return locale
    return None


def _language_charset(model, locale: Locale, extra: str) -> str:
    chars = set(char for char in extra if char not in "\r\n\t")
    for row in model.rows:
        for char in row.values.get(locale.tag, ""):
            if char not in "\r\n\t":
                chars.add(char)
    # Whitespace is covered by the fixed ranges; keep the --symbols argument
    # free of characters that are awkward to quote.
    chars.discard("\n")
    chars.discard("\r")
    chars.discard("\t")
    return "".join(sorted(chars, key=ord))


def _source_font(fonts_dir: Path, locale: Locale, style: str) -> Path:
    if locale.tag == "ar":
        name = "NotoSansArabic-Bold.ttf" if style == "bold" else "NotoSansArabic-Regular.ttf"
    else:
        name = "NotoSansSC-Bold.ttf" if style == "bold" else "NotoSansSC-AR-Regular.ttf"
    return fonts_dir / name


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


def _emit_table(out_dir: Path, locale: Locale, sizes: list[int], styles: list[str]) -> None:
    symbol = locale.symbol
    guard = f"LVGLPP_FONT_TABLE_{symbol.upper()}_H"
    header = [
        f"#ifndef {guard}",
        f"#define {guard}",
        "",
        '#include "core/tools/fonts/FontProfile.h"',
        "",
    ]
    declarations = []
    rows = []
    for size in sizes:
        normal = f"Noto_{symbol}_{size}_Normal"
        bold = f"Noto_{symbol}_{size}_Bold"
        if "normal" in styles:
            declarations.append(f"LV_FONT_DECLARE({normal});")
            normal_ref = f"&{normal}"
        else:
            normal_ref = "NULL"
        if "bold" in styles:
            declarations.append(f"LV_FONT_DECLARE({bold});")
            bold_ref = f"&{bold}"
        else:
            bold_ref = "NULL"
        rows.append(f"    {{{size}, {normal_ref}, {bold_ref}}},")

    header.extend(declarations)
    header.extend([
        "",
        f"extern const LvglppFontFace lvglpp_fonts_{symbol}[];",
        f"extern const unsigned lvglpp_fonts_{symbol}_count;",
        "",
        f"#endif // {guard}",
        "",
    ])
    (out_dir / f"font_table_{symbol}.h").write_text("\n".join(header), encoding="utf-8")

    source = [
        f'#include "font_table_{symbol}.h"',
        "",
        f"const LvglppFontFace lvglpp_fonts_{symbol}[] = {{",
        *rows,
        "};",
        f"const unsigned lvglpp_fonts_{symbol}_count = sizeof(lvglpp_fonts_{symbol}) / sizeof(lvglpp_fonts_{symbol}[0]);",
        "",
    ]
    (out_dir / f"font_table_{symbol}.c").write_text("\n".join(source), encoding="utf-8")


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--xlsx", required=True)
    parser.add_argument("--langs", default="", help="comma-separated locale tags (default: all)")
    parser.add_argument("--sizes", default=",".join(str(size) for size in DEFAULT_SIZES))
    parser.add_argument("--styles", default=",".join(DEFAULT_STYLES), help="normal,bold")
    parser.add_argument("--bpp", type=int, default=4, choices=(1, 2, 3, 4, 8))
    parser.add_argument("--compress", action="store_true", help="enable LVGL font compression")
    parser.add_argument("--extra-charset", help="file with additional characters")
    parser.add_argument("--fonts-dir", default="assets/common/fonts")
    parser.add_argument("--out", required=True)
    parser.add_argument("--tool", default=str(DEFAULT_TOOL))
    parser.add_argument("--emit-table", action="store_true", help="write font_table_<lang>.h/.c")
    parser.add_argument("--dry-run", action="store_true")
    args = parser.parse_args(argv)

    xlsx = Path(args.xlsx)
    out_dir = Path(args.out)
    fonts_dir = Path(args.fonts_dir)
    sizes = [int(value) for value in args.sizes.split(",") if value.strip()]
    styles = [value.strip().lower() for value in args.styles.split(",") if value.strip()]

    if not sizes:
        print("error: --sizes is empty", file=sys.stderr)
        return 1
    if any(style not in ("normal", "bold") for style in styles):
        print("error: --styles must be normal and/or bold", file=sys.stderr)
        return 1

    model = load_model(xlsx)
    if model.errors:
        for error in model.errors:
            print(f"error: {error}", file=sys.stderr)
        return 1

    if args.langs:
        locales = []
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

    tool = Path(args.tool)
    if not args.dry_run and not tool.exists():
        print(
            f"error: {tool} not found.\n"
            f"Install it with: npm --prefix {SCRIPT_DIR} install",
            file=sys.stderr,
        )
        return 1

    tasks = []
    for locale in locales:
        symbols = _language_charset(model, locale, extra)
        ranges = COMMON_RANGES + LANGUAGE_RANGES.get(locale.tag, ())
        for size in sizes:
            for style in styles:
                source = _source_font(fonts_dir, locale, style)
                if not source.exists():
                    print(f"error: missing source font {source}", file=sys.stderr)
                    return 1
                style_name = "Bold" if style == "bold" else "Normal"
                output = out_dir / f"Noto_{locale.symbol}_{size}_{style_name}.c"
                tasks.append((locale, size, style, source, output, symbols, ranges))

    if args.dry_run:
        for locale, size, style, source, output, symbols, ranges in tasks:
            print(" ".join(_command(tool, source, size, args.bpp, args.compress, output, symbols, ranges)))
        return 0

    out_dir.mkdir(parents=True, exist_ok=True)
    manifest_entries = []
    for locale, size, style, source, output, symbols, ranges in tasks:
        command = _command(tool, source, size, args.bpp, args.compress, output, symbols, ranges)
        result = subprocess.run(command, check=False, capture_output=True, text=True)
        if result.returncode != 0:
            print(f"error: lv_font_conv failed for {output.name}:\n{result.stderr}", file=sys.stderr)
            return 1
        if not output.exists():
            print(f"error: lv_font_conv did not produce {output}", file=sys.stderr)
            return 1
        if not _verify_output(output, symbols, output.name):
            return 1
        manifest_entries.append({
            "file": output.name,
            "locale": locale.tag,
            "size": size,
            "style": style,
            "source": source.name,
            "sha256": _sha256(output),
        })
        print(f"generated {output}")

    if args.emit_table:
        for locale in locales:
            _emit_table(out_dir, locale, sizes, styles)

    manifest = {
        "tool": "lv_font_conv",
        "xlsx_sha256": _sha256(xlsx),
        "langs": [locale.tag for locale in locales],
        "sizes": sizes,
        "styles": styles,
        "bpp": args.bpp,
        "compress": args.compress,
        "fonts": manifest_entries,
    }
    (out_dir / MANIFEST).write_text(json.dumps(manifest, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
    print(f"wrote {len(manifest_entries)} font(s) and {out_dir / MANIFEST}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
