#!/usr/bin/env python3
"""Upsert translation rows in strings.xlsx from a CSV/TSV file.

The input file must have a header row using the same column names as the
workbook (at minimum ``strid``).  Empty cells do not overwrite existing values.
"""

from __future__ import annotations

import argparse
import csv
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))

from xlsx_io import read_sheet, write_sheet  # noqa: E402


def _read_input(path: Path) -> tuple[list[str], list[list[str]]]:
    text = path.read_text(encoding="utf-8-sig")
    sample = text[:4096]
    delimiter = "\t" if sample.count("\t") > sample.count(",") else ","
    rows = list(csv.reader(text.splitlines(), delimiter=delimiter))
    if not rows:
        raise ValueError("input file is empty")
    header = [cell.strip() for cell in rows[0]]
    return header, rows[1:]


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--xlsx", required=True)
    parser.add_argument("--input", required=True, help="CSV/TSV with a strid column")
    parser.add_argument("--sheet", default="strings")
    args = parser.parse_args(argv)

    xlsx_path = Path(args.xlsx)
    workbook = read_sheet(xlsx_path, args.sheet)
    if not workbook:
        print("error: workbook is empty", file=sys.stderr)
        return 1
    header = [cell.strip() for cell in workbook[0]]
    if "strid" not in header:
        print("error: workbook has no strid column", file=sys.stderr)
        return 1
    strid_index = header.index("strid")

    input_header, input_rows = _read_input(Path(args.input))
    if "strid" not in input_header:
        print("error: input has no strid column", file=sys.stderr)
        return 1
    unknown = [column for column in input_header if column and column not in header]
    if unknown:
        print(f"error: unknown columns {unknown}; workbook has {header}", file=sys.stderr)
        return 1

    rows_by_strid = {row[strid_index]: row for row in workbook[1:] if len(row) > strid_index and row[strid_index]}
    added = 0
    updated = 0

    for raw in input_rows:
        if not any(cell.strip() for cell in raw):
            continue
        values = {input_header[index]: cell for index, cell in enumerate(raw) if index < len(input_header)}
        key = values.get("strid", "").strip()
        if not key:
            continue
        existing = rows_by_strid.get(key)
        if existing is None:
            existing = [""] * len(header)
            existing[strid_index] = key
            workbook.append(existing)
            rows_by_strid[key] = existing
            added += 1
        else:
            updated += 1
        for column, value in values.items():
            if column == "strid" or value == "":
                continue
            existing[header.index(column)] = value

    write_sheet(xlsx_path, workbook, sheet_name=args.sheet)
    print(f"upserted {added} new row(s), updated {updated} existing row(s) in {xlsx_path}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
