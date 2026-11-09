#!/usr/bin/env python3
"""Generate lv_i18n.c from an Excel translation workbook."""

from __future__ import annotations

import argparse
import csv
import difflib
import sys
from pathlib import Path

# ``python3 -I`` does not put the script directory on sys.path.
sys.path.insert(0, str(Path(__file__).resolve().parent))

from i18n_model import LOCALES, extract_charset, load_model, render_lv_i18n_c

SCRIPT_DIR = Path(__file__).resolve().parent
DEFAULT_TEMPLATE = SCRIPT_DIR / "templates" / "lv_i18n.c.in"


def _report(model) -> None:
    for error in model.errors:
        print(f"error: {error}", file=sys.stderr)
    for warning in model.warnings:
        print(f"warning: {warning}", file=sys.stderr)


def _dump_csv(path: Path, model) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open("w", newline="", encoding="utf-8") as handle:
        writer = csv.writer(handle)
        writer.writerow(["strid", *(locale.tag for locale in LOCALES), "comment"])
        for row in model.rows:
            writer.writerow([row.strid, *(row.values.get(locale.tag, "") for locale in LOCALES), row.comment])


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--xlsx", required=True, help="translation workbook (.xlsx)")
    parser.add_argument("--out", required=True, help="generated lv_i18n.c path")
    parser.add_argument("--template", default=str(DEFAULT_TEMPLATE), help="lv_i18n.c template")
    parser.add_argument("--sheet", help="sheet name (default: first sheet)")
    parser.add_argument("--check", action="store_true", help="fail if --out is not up to date")
    parser.add_argument("--strict", action="store_true", help="treat warnings as errors")
    parser.add_argument("--emit-charset", help="write the union of translated characters")
    parser.add_argument("--dump-csv", help="write a review-friendly CSV copy")
    parser.add_argument("--stats", action="store_true", help="print per-locale row counts")
    args = parser.parse_args(argv)

    model = load_model(args.xlsx, args.sheet)
    _report(model)
    if model.errors or (args.strict and model.warnings):
        return 1

    template = Path(args.template).read_text(encoding="utf-8")
    rendered = render_lv_i18n_c(model, template)

    out_path = Path(args.out)
    if args.check:
        if not out_path.exists():
            print(f"error: {out_path} does not exist", file=sys.stderr)
            return 1
        current = out_path.read_text(encoding="utf-8")
        if current != rendered:
            diff = list(
                difflib.unified_diff(
                    current.splitlines(),
                    rendered.splitlines(),
                    fromfile=str(out_path),
                    tofile="generated",
                    lineterm="",
                )
            )
            print("\n".join(diff[:120]), file=sys.stderr)
            if len(diff) > 120:
                print(f"... ({len(diff) - 120} more diff lines)", file=sys.stderr)
            print(f"error: {out_path} is stale; run gen_lv_i18n.py without --check", file=sys.stderr)
            return 1
        print(f"{out_path} is up to date")
    else:
        out_path.parent.mkdir(parents=True, exist_ok=True)
        out_path.write_text(rendered, encoding="utf-8", newline="\n")
        print(f"wrote {out_path}")

    if args.emit_charset:
        charset = extract_charset(model)
        charset_path = Path(args.emit_charset)
        charset_path.parent.mkdir(parents=True, exist_ok=True)
        charset_path.write_text(charset + "\n", encoding="utf-8", newline="\n")
        print(f"wrote {charset_path} ({len(charset)} unique characters)")

    if args.dump_csv:
        _dump_csv(Path(args.dump_csv), model)
        print(f"wrote {args.dump_csv}")

    if args.stats:
        for locale in LOCALES:
            count = sum(1 for row in model.rows if row.values.get(locale.tag, "") != "")
            print(f"{locale.tag:>6}: {count} entries")
        print(f"  total: {len(model.rows)} strids")

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
