#!/usr/bin/env python3
"""Generate subset LVGL bitmap fonts from the Excel translation character set.

For every size we emit:
  * NotoSC_<size>_Normal.c   (NotoSansSC-AR-Regular: zh/en/ja/ru/ar)
  * NotoSC_<size>_Bold.c     (NotoSansSC-Bold + Arabic fallback)
  * NotoArabic_<size>_Bold.c (NotoSansArabic-Bold, only Arabic glyphs)

Only characters that actually occur in strings.xlsx (plus a small set of fixed
ranges for punctuation) are included, so the fonts stay small.
"""

from __future__ import annotations

import argparse
import hashlib
import json
import os
import re
import shutil
import subprocess
import sys
import tempfile
from concurrent.futures import ThreadPoolExecutor, as_completed
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))

from i18n_model import extract_charset, load_model  # noqa: E402

SCRIPT_DIR = Path(__file__).resolve().parent
DEFAULT_TOOL = SCRIPT_DIR / "node_modules" / ".bin" / "lv_font_conv"
DEFAULT_SIZES = (20, 22, 24, 28, 32, 36)

FIXED_RANGES = (
    "0x20-0x7E",     # ASCII
    "0xA0-0xFF",     # Latin-1 supplement
    "0x2000-0x206F", # general punctuation
    "0x3000-0x303F", # CJK punctuation
    "0x3040-0x30FF", # hiragana + katakana (Japanese UI/data)
    "0xFF00-0xFFEF", # fullwidth forms
)
ARABIC_RANGES = (
    "0xFB50-0xFDFF", # Arabic presentation forms-A
    "0xFE70-0xFEFF", # Arabic presentation forms-B
)

GLYPH_RE = re.compile(r"/\*\s*U\+([0-9A-Fa-f]+)")
MANIFEST = "font_manifest.json"
TOOL_VERSION = "lv_font_conv@1.5.3"


def _sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        for chunk in iter(lambda: handle.read(1 << 20), b""):
            digest.update(chunk)
    return digest.hexdigest()


def _sha256_text(text: str) -> str:
    return hashlib.sha256(text.encode("utf-8")).hexdigest()


def _is_arabic(codepoint: int) -> bool:
    return (
        0x0600 <= codepoint <= 0x06FF
        or 0x0750 <= codepoint <= 0x077F
        or 0x08A0 <= codepoint <= 0x08FF
        or 0xFB50 <= codepoint <= 0xFDFF
        or 0xFE70 <= codepoint <= 0xFEFF
    )


def _split_charset(charset: str) -> tuple[str, str]:
    arabic = "".join(char for char in charset if _is_arabic(ord(char)))
    other = "".join(char for char in charset if not _is_arabic(ord(char)))
    return other, arabic


def _fc_charset(path: Path) -> list[tuple[int, int]] | None:
    """Return cmap ranges via fontconfig, or None when fc-query is unavailable."""
    if not shutil.which("fc-query"):
        return None
    result = subprocess.run(
        ["fc-query", "-f", "%{charset}\n", str(path)],
        check=False,
        capture_output=True,
        text=True,
    )
    if result.returncode != 0:
        return None
    ranges: list[tuple[int, int]] = []
    for part in result.stdout.split():
        if "-" in part:
            start, end = part.split("-", 1)
            ranges.append((int(start, 16), int(end, 16)))
        else:
            value = int(part, 16)
            ranges.append((value, value))
    return ranges


def _check_source_coverage(path: Path, chars: str) -> list[str]:
    ranges = _fc_charset(path)
    if ranges is None:
        return []
    missing = []
    for char in chars:
        codepoint = ord(char)
        if not any(start <= codepoint <= end for start, end in ranges):
            missing.append(f"U+{codepoint:04X} ({char!r})")
    return missing


def _command(
    tool: Path,
    source: Path,
    size: int,
    output: Path,
    symbols: str,
    ranges: tuple[str, ...],
    fallback: str | None,
    bpp: int,
) -> list[str]:
    command = [
        str(tool),
        "--font",
        str(source),
        "--size",
        str(size),
        "--bpp",
        str(bpp),
        "--format",
        "lvgl",
        "--no-compress",
        "--autohint-off",
        "--use-color-info",
        "--output",
        str(output),
    ]
    if fallback:
        command.extend(["--lv-fallback", fallback])
    for value in ranges:
        command.extend(["--range", value])
    if symbols:
        command.extend(["--symbols", symbols])
    return command


def _glyphs(path: Path) -> set[int]:
    text = path.read_text(encoding="utf-8", errors="replace")
    return {int(match.group(1), 16) for match in GLYPH_RE.finditer(text)}


def _verify_output(path: Path, expected: str, label: str) -> list[str]:
    present = _glyphs(path)
    missing = [f"U+{ord(char):04X} ({char!r})" for char in expected if ord(char) not in present]
    if missing:
        print(f"error: {label} is missing {len(missing)} glyph(s): {', '.join(missing[:20])}", file=sys.stderr)
    return missing


def _load_manifest(path: Path) -> dict | None:
    try:
        return json.loads(path.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError):
        return None


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--xlsx", required=True)
    parser.add_argument("--sheet")
    parser.add_argument("--fonts-dir", default="assets/common/fonts")
    parser.add_argument("--out", default="sdk/core/tools/fonts")
    parser.add_argument("--tool", default=str(DEFAULT_TOOL))
    parser.add_argument("--sizes", default=",".join(str(size) for size in DEFAULT_SIZES))
    parser.add_argument("--bpp", type=int, default=8, choices=(1, 2, 3, 4, 8))
    parser.add_argument("--jobs", type=int, default=min(4, os.cpu_count() or 1))
    parser.add_argument("--check", action="store_true", help="verify outputs against the manifest")
    parser.add_argument("--dry-run", action="store_true")
    parser.add_argument("--emit-charset", help="write the exact Excel character set")
    args = parser.parse_args(argv)

    xlsx = Path(args.xlsx)
    out_dir = Path(args.out)
    fonts_dir = Path(args.fonts_dir)
    sizes = tuple(int(value) for value in args.sizes.split(",") if value.strip())

    model = load_model(xlsx, args.sheet)
    if model.errors:
        for error in model.errors:
            print(f"error: {error}", file=sys.stderr)
        return 1

    charset = "".join(char for char in extract_charset(model) if char not in "\r\n\t")
    other_chars, arabic_chars = _split_charset(charset)

    if args.emit_charset:
        Path(args.emit_charset).write_text(charset + "\n", encoding="utf-8", newline="\n")
        print(f"wrote {args.emit_charset} ({len(charset)} characters)")

    if args.check:
        manifest = _load_manifest(out_dir / MANIFEST)
        if manifest is None:
            print(f"error: missing {out_dir / MANIFEST}", file=sys.stderr)
            return 1
        expected_xlsx = _sha256(xlsx)
        expected_charset = _sha256_text(charset)
        failed = False
        if manifest.get("xlsx_sha256") != expected_xlsx:
            print("error: strings.xlsx changed; regenerate fonts", file=sys.stderr)
            failed = True
        if manifest.get("charset_sha256") != expected_charset:
            print("error: translation character set changed; regenerate fonts", file=sys.stderr)
            failed = True
        for entry in manifest.get("fonts", []):
            path = out_dir / entry["file"]
            if not path.exists():
                print(f"error: missing {path}", file=sys.stderr)
                failed = True
            elif _sha256(path) != entry["sha256"]:
                print(f"error: {path} does not match the manifest", file=sys.stderr)
                failed = True
        if failed:
            return 1
        print(f"fonts are up to date ({len(manifest.get('fonts', []))} files)")
        return 0

    tool = Path(args.tool)
    if not tool.exists():
        print(
            f"error: {tool} not found.\n"
            f"Install it with: npm --prefix {SCRIPT_DIR} install",
            file=sys.stderr,
        )
        return 1

    sources = {
        "normal": fonts_dir / "NotoSansSC-AR-Regular.ttf",
        "bold": fonts_dir / "NotoSansSC-Bold.ttf",
        "arabic_bold": fonts_dir / "NotoSansArabic-Bold.ttf",
    }
    for name, path in sources.items():
        if not path.exists():
            print(f"error: missing source font {path}", file=sys.stderr)
            return 1

    coverage_errors = False
    for label, path, chars in (
        ("normal", sources["normal"], charset),
        ("bold", sources["bold"], other_chars),
        ("arabic_bold", sources["arabic_bold"], arabic_chars),
    ):
        missing = _check_source_coverage(path, chars)
        if missing:
            print(f"error: {path.name} lacks {len(missing)} glyph(s) needed by {label}: {', '.join(missing[:20])}", file=sys.stderr)
            coverage_errors = True
    if coverage_errors:
        return 1

    tasks: list[dict] = []
    for size in sizes:
        tasks.append(
            {
                "file": f"NotoSC_{size}_Normal.c",
                "source": sources["normal"],
                "size": size,
                "symbols": charset,
                "ranges": FIXED_RANGES + ARABIC_RANGES,
                "fallback": None,
                "expected": charset,
                "label": f"NotoSC_{size}_Normal",
            }
        )
        tasks.append(
            {
                "file": f"NotoSC_{size}_Bold.c",
                "source": sources["bold"],
                "size": size,
                "symbols": other_chars,
                "ranges": FIXED_RANGES,
                "fallback": f"NotoArabic_{size}_Bold",
                "expected": other_chars,
                "label": f"NotoSC_{size}_Bold",
            }
        )
        if arabic_chars:
            tasks.append(
                {
                    "file": f"NotoArabic_{size}_Bold.c",
                    "source": sources["arabic_bold"],
                    "size": size,
                    "symbols": arabic_chars,
                    "ranges": ARABIC_RANGES,
                    "fallback": None,
                    "expected": arabic_chars,
                    "label": f"NotoArabic_{size}_Bold",
                }
            )

    if args.dry_run:
        for task in tasks:
            print(
                " ".join(
                    _command(
                        tool,
                        task["source"],
                        task["size"],
                        out_dir / task["file"],
                        task["symbols"],
                        task["ranges"],
                        task["fallback"],
                        args.bpp,
                    )
                )
            )
        return 0

    out_dir.mkdir(parents=True, exist_ok=True)
    temp_dir = Path(tempfile.mkdtemp(prefix="lvglpp_fonts_", dir=out_dir.parent))

    def run(task: dict) -> tuple[dict, list[str]]:
        output = temp_dir / task["file"]
        command = _command(
            tool,
            task["source"],
            task["size"],
            output,
            task["symbols"],
            task["ranges"],
            task["fallback"],
            args.bpp,
        )
        result = subprocess.run(command, check=False, capture_output=True, text=True)
        if result.returncode != 0:
            return task, [f"lv_font_conv failed for {task['label']}:\n{result.stderr}"]
        if not output.exists():
            return task, [f"lv_font_conv did not produce {output}"]

        # The generated header records the command line; replace the temporary
        # output path with the final one so the checked-in file is reproducible.
        text = output.read_text(encoding="utf-8", errors="replace")
        text = text.replace(str(output), str(out_dir / task["file"]))
        output.write_text(text, encoding="utf-8", newline="\n")
        return task, _verify_output(output, task["expected"], task["label"])

    failures = 0
    manifest_entries = []
    try:
        with ThreadPoolExecutor(max_workers=max(1, args.jobs)) as pool:
            futures = {pool.submit(run, task): task for task in tasks}
            for future in as_completed(futures):
                task, errors = future.result()
                if errors:
                    failures += 1
                    for error in errors:
                        print(f"error: {error}", file=sys.stderr)
                else:
                    print(f"generated {task['file']}")
                    final = out_dir / task["file"]
                    os.replace(temp_dir / task["file"], final)
                    manifest_entries.append(
                        {
                            "file": task["file"],
                            "source": task["source"].name,
                            "size": task["size"],
                            "sha256": _sha256(final),
                            "glyphs": len(_glyphs(final)),
                        }
                    )
    finally:
        shutil.rmtree(temp_dir, ignore_errors=True)

    if failures:
        print(f"error: {failures} font(s) failed; existing fonts were left untouched", file=sys.stderr)
        return 1

    manifest_entries.sort(key=lambda entry: entry["file"])
    manifest = {
        "tool": TOOL_VERSION,
        "xlsx_sha256": _sha256(xlsx),
        "charset_sha256": _sha256_text(charset),
        "charset": charset,
        "sizes": list(sizes),
        "bpp": args.bpp,
        "fonts": manifest_entries,
    }
    (out_dir / MANIFEST).write_text(json.dumps(manifest, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
    print(f"wrote {len(manifest_entries)} font(s) and {out_dir / MANIFEST}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
