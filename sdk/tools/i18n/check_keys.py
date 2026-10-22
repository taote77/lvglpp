#!/usr/bin/env python3
"""Check that translation keys used by compiled sources exist in the workbook."""

from __future__ import annotations

import argparse
import re
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))

from i18n_model import load_model  # noqa: E402

SOURCE_SUFFIXES = {".cc", ".cpp", ".h", ".hpp"}

# These directories are not part of smart_home/CMakeLists.txt's target_sources.
EXCLUDED_PARTS = {
    "page/home",
    "page/devices",
    "page/energy",
    "page/rooms",
    "page/scenes",
    "page/settings",
    # The translator implementation contains declarations/templates, not UI keys.
    "i18n/LvTranslator",
}

LITERAL_PATTERNS = (
    re.compile(r'\blvTr\s*\(\s*"((?:[^"\\]|\\.)*)"'),
    re.compile(r'\blvTrFmt\s*\(\s*"((?:[^"\\]|\\.)*)"'),
    re.compile(r'\blvTrStr\s*\(\s*"((?:[^"\\]|\\.)*)"'),
    re.compile(r'(?<![A-Za-z0-9_])_\s*\(\s*"((?:[^"\\]|\\.)*)"'),
)

DYNAMIC_PATTERN = re.compile(r'\b(?:lvTr|lvTrFmt|lvTrStr)\s*\(\s*([^"\s][^)]*)\)')

DEFAULT_DYNAMIC_ALLOWLIST = {
    "empty_info",
    "status_normal",
    "status_abnormal",
    "status_uninstalled",
    "status_error",
    "auto_fill_open_tips",
    "auto_fill_close_tips",
}


def _unescape_c(value: str) -> str:
    def replace(match: re.Match[str]) -> str:
        return {
            "n": "\n",
            "r": "\r",
            "t": "\t",
            '"': '"',
            "\\": "\\",
        }.get(match.group(1), match.group(1))

    return re.sub(r"\\(.)", replace, value)


def _iter_sources(paths: list[str]):
    for raw in paths:
        path = Path(raw)
        if path.is_file():
            yield path
            continue
        if not path.is_dir():
            continue
        for file in sorted(path.rglob("*")):
            if file.suffix not in SOURCE_SUFFIXES:
                continue
            relative = file.as_posix()
            if any(part in relative for part in EXCLUDED_PARTS):
                continue
            if "/build" in relative or "/.git/" in relative:
                continue
            yield file


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--xlsx", required=True)
    parser.add_argument("--sheet")
    parser.add_argument("--sources", nargs="+", required=True)
    parser.add_argument(
        "--allow-dynamic",
        nargs="*",
        default=sorted(DEFAULT_DYNAMIC_ALLOWLIST),
        help="dynamic keys that cannot be extracted from source literals",
    )
    parser.add_argument("--report-dynamic", action="store_true", default=True)
    args = parser.parse_args(argv)

    model = load_model(args.xlsx, args.sheet)
    if model.errors:
        for error in model.errors:
            print(f"error: {error}", file=sys.stderr)
        return 1

    known = {row.strid for row in model.rows}
    missing: dict[str, list[str]] = {}
    dynamic: dict[str, list[str]] = {}
    checked = 0

    for file in _iter_sources(args.sources):
        text = file.read_text(encoding="utf-8", errors="replace")
        checked += 1
        for pattern in LITERAL_PATTERNS:
            for match in pattern.finditer(text):
                key = _unescape_c(match.group(1))
                if key == "":
                    continue
                if key not in known:
                    missing.setdefault(key, []).append(str(file))
        for match in DYNAMIC_PATTERN.finditer(text):
            expression = match.group(1).strip()
            dynamic.setdefault(expression, []).append(str(file))

    for key, files in sorted(missing.items()):
        print(f"missing key {key!r} in {', '.join(sorted(set(files)))}", file=sys.stderr)

    allow = set(args.allow_dynamic)
    missing_dynamic = sorted(key for key in allow if key not in known)
    for key in missing_dynamic:
        print(f"missing dynamic key {key!r}", file=sys.stderr)

    if args.report_dynamic and dynamic:
        print("dynamic keys (verify manually):")
        for expression, files in sorted(dynamic.items()):
            print(f"  {expression}  ({', '.join(sorted(set(files)))})")

    if missing or missing_dynamic:
        print(
            f"failed: {len(missing)} missing literal key(s), "
            f"{len(missing_dynamic)} missing dynamic key(s), {checked} file(s) scanned",
            file=sys.stderr,
        )
        return 1

    print(f"ok: {checked} file(s) scanned, all literal keys present")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
