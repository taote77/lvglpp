#!/usr/bin/env python3
"""Minimal, dependency-free .xlsx reader/writer.

Only what the i18n pipeline needs:
  * read the first (or named) sheet as a matrix of strings
  * write a single-sheet workbook using inline strings

Supports shared strings, inline strings, formula cached values and rich-text
runs.  The writer emits deterministic zips (fixed timestamp) so generated
artifacts do not churn.
"""

from __future__ import annotations

import argparse
import re
import sys
import tempfile
import zipfile
from pathlib import Path
from xml.etree import ElementTree as ET
from xml.sax.saxutils import escape, quoteattr

NS_MAIN = "{http://schemas.openxmlformats.org/spreadsheetml/2006/main}"
NS_REL = "{http://schemas.openxmlformats.org/officeDocument/2006/relationships}"
NS_PKG_REL = "{http://schemas.openxmlformats.org/package/2006/relationships}"

_OXML_ESCAPE = re.compile(r"_x([0-9A-Fa-f]{4})_")


def decode_ooxml_escapes(text: str) -> str:
    """Decode OOXML ``_xNNNN_`` escapes without double-decoding ``_x005F_``."""
    text = text.replace("_x005F_", "\x00")
    text = _OXML_ESCAPE.sub(lambda m: chr(int(m.group(1), 16)), text)
    return text.replace("\x00", "_")


def _text_of(element: ET.Element) -> str:
    return "".join(node.text or "" for node in element.iter(f"{NS_MAIN}t"))


def _column_index(reference: str) -> int:
    letters = "".join(ch for ch in reference if ch.isalpha()).upper()
    if not letters:
        raise ValueError(f"invalid cell reference: {reference!r}")
    value = 0
    for ch in letters:
        value = value * 26 + (ord(ch) - ord("A") + 1)
    return value - 1


def _row_index(reference: str) -> int:
    digits = "".join(ch for ch in reference if ch.isdigit())
    if not digits:
        raise ValueError(f"invalid cell reference: {reference!r}")
    return int(digits) - 1


def _read_shared_strings(zf: zipfile.ZipFile) -> list[str]:
    try:
        data = zf.read("xl/sharedStrings.xml")
    except KeyError:
        return []
    root = ET.fromstring(data)
    return [decode_ooxml_escapes(_text_of(si)) for si in root.findall(f"{NS_MAIN}si")]


def _sheet_target(zf: zipfile.ZipFile, sheet_name: str | None) -> str:
    try:
        workbook = ET.fromstring(zf.read("xl/workbook.xml"))
    except KeyError:
        return "xl/worksheets/sheet1.xml"

    sheets = workbook.findall(f".//{NS_MAIN}sheet")
    if not sheets:
        return "xl/worksheets/sheet1.xml"

    selected = None
    if sheet_name:
        for sheet in sheets:
            if (sheet.get("name") or "") == sheet_name:
                selected = sheet
                break
        if selected is None:
            names = [sheet.get("name") for sheet in sheets]
            raise KeyError(f"sheet {sheet_name!r} not found; available: {names}")
    else:
        selected = sheets[0]

    rel_id = selected.get(f"{NS_REL}id")
    if not rel_id:
        return "xl/worksheets/sheet1.xml"

    rels_path = "xl/_rels/workbook.xml.rels"
    try:
        rels = ET.fromstring(zf.read(rels_path))
    except KeyError:
        return "xl/worksheets/sheet1.xml"

    for rel in rels.findall(f"{NS_PKG_REL}Relationship"):
        if rel.get("Id") == rel_id:
            target = rel.get("Target", "")
            target = target.lstrip("/")
            if not target.startswith("xl/"):
                target = "xl/" + target
            return target
    return "xl/worksheets/sheet1.xml"


def read_sheet(path: str | Path, sheet_name: str | None = None) -> list[list[str]]:
    """Read a sheet into a rectangular list-of-rows of strings."""
    with zipfile.ZipFile(path) as zf:
        shared = _read_shared_strings(zf)
        target = _sheet_target(zf, sheet_name)
        root = ET.fromstring(zf.read(target))

    cells: dict[tuple[int, int], str] = {}
    max_row = -1
    max_col = -1
    next_row = 0

    for row in root.findall(f".//{NS_MAIN}row"):
        row_ref = row.get("r")
        row_idx = int(row_ref) - 1 if row_ref else next_row
        next_row = row_idx + 1
        next_col = 0
        for cell in row.findall(f"{NS_MAIN}c"):
            ref = cell.get("r")
            if ref:
                row_cell = _row_index(ref)
                col_cell = _column_index(ref)
            else:
                row_cell = row_idx
                col_cell = next_col
            next_col = col_cell + 1

            cell_type = cell.get("t")
            if cell_type == "inlineStr":
                inline = cell.find(f"{NS_MAIN}is")
                value = decode_ooxml_escapes(_text_of(inline)) if inline is not None else ""
            else:
                v = cell.find(f"{NS_MAIN}v")
                raw = (v.text or "") if v is not None else ""
                if cell_type == "s":
                    try:
                        value = shared[int(raw)]
                    except (ValueError, IndexError):
                        value = ""
                elif cell_type == "b":
                    value = "TRUE" if raw == "1" else "FALSE"
                else:
                    value = decode_ooxml_escapes(raw)

            if value:
                cells[(row_cell, col_cell)] = value
                max_row = max(max_row, row_cell)
                max_col = max(max_col, col_cell)

    if max_row < 0:
        return []

    return [[cells.get((r, c), "") for c in range(max_col + 1)] for r in range(max_row + 1)]


def _column_letters(index: int) -> str:
    if index < 0:
        raise ValueError(index)
    result = ""
    value = index + 1
    while value:
        value, rem = divmod(value - 1, 26)
        result = chr(ord("A") + rem) + result
    return result


def _cell_xml(row: int, col: int, value: str) -> str:
    ref = f"{_column_letters(col)}{row + 1}"
    if value == "":
        return ""
    return (
        f'<c r="{ref}" t="inlineStr"><is>'
        f'<t xml:space="preserve">{escape(value)}</t>'
        f"</is></c>"
    )


def _sheet_xml(rows: list[list[str]]) -> str:
    parts = [
        '<?xml version="1.0" encoding="UTF-8" standalone="yes"?>',
        f'<worksheet xmlns="{NS_MAIN[1:-1]}"><sheetData>',
    ]
    for r, row in enumerate(rows):
        cells = "".join(_cell_xml(r, c, str(value)) for c, value in enumerate(row))
        parts.append(f'<row r="{r + 1}">{cells}</row>')
    parts.append("</sheetData></worksheet>")
    return "".join(parts)


def _workbook_xml(sheet_name: str) -> str:
    name = escape(sheet_name, {'"': "&quot;"})
    return (
        '<?xml version="1.0" encoding="UTF-8" standalone="yes"?>'
        f'<workbook xmlns="{NS_MAIN[1:-1]}" '
        f'xmlns:r="{NS_REL[1:-1]}">'
        f'<sheets><sheet name="{name}" sheetId="1" r:id="rId1"/></sheets>'
        "</workbook>"
    )


_CONTENT_TYPES = (
    '<?xml version="1.0" encoding="UTF-8" standalone="yes"?>'
    '<Types xmlns="http://schemas.openxmlformats.org/package/2006/content-types">'
    '<Default Extension="rels" ContentType="application/vnd.openxmlformats-package.relationships+xml"/>'
    '<Default Extension="xml" ContentType="application/xml"/>'
    '<Override PartName="/xl/workbook.xml" '
    'ContentType="application/vnd.openxmlformats-officedocument.spreadsheetml.sheet.main+xml"/>'
    '<Override PartName="/xl/worksheets/sheet1.xml" '
    'ContentType="application/vnd.openxmlformats-officedocument.spreadsheetml.worksheet+xml"/>'
    '<Override PartName="/xl/styles.xml" '
    'ContentType="application/vnd.openxmlformats-officedocument.spreadsheetml.styles+xml"/>'
    "</Types>"
)

_ROOT_RELS = (
    '<?xml version="1.0" encoding="UTF-8" standalone="yes"?>'
    '<Relationships xmlns="http://schemas.openxmlformats.org/package/2006/relationships">'
    '<Relationship Id="rId1" '
    'Type="http://schemas.openxmlformats.org/officeDocument/2006/relationships/officeDocument" '
    'Target="xl/workbook.xml"/>'
    "</Relationships>"
)

_WORKBOOK_RELS = (
    '<?xml version="1.0" encoding="UTF-8" standalone="yes"?>'
    '<Relationships xmlns="http://schemas.openxmlformats.org/package/2006/relationships">'
    '<Relationship Id="rId1" '
    'Type="http://schemas.openxmlformats.org/officeDocument/2006/relationships/worksheet" '
    'Target="worksheets/sheet1.xml"/>'
    '<Relationship Id="rId2" '
    'Type="http://schemas.openxmlformats.org/officeDocument/2006/relationships/styles" '
    'Target="styles.xml"/>'
    "</Relationships>"
)

_STYLES = (
    '<?xml version="1.0" encoding="UTF-8" standalone="yes"?>'
    '<styleSheet xmlns="http://schemas.openxmlformats.org/spreadsheetml/2006/main">'
    '<fonts count="1"><font><sz val="11"/><name val="Calibri"/></font></fonts>'
    '<fills count="2"><fill><patternFill patternType="none"/></fill>'
    '<fill><patternFill patternType="gray125"/></fill></fills>'
    '<borders count="1"><border><left/><right/><top/><bottom/><diagonal/></border></borders>'
    '<cellStyleXfs count="1"><xf numFmtId="0" fontId="0" fillId="0" borderId="0"/></cellStyleXfs>'
    '<cellXfs count="1"><xf numFmtId="0" fontId="0" fillId="0" borderId="0" xfId="0"/></cellXfs>'
    '<cellStyles count="1"><cellStyle name="Normal" xfId="0" builtinId="0"/></cellStyles>'
    "</styleSheet>"
)


def write_sheet(path: str | Path, rows: list[list[object]], sheet_name: str = "strings") -> None:
    """Write a minimal single-sheet .xlsx workbook."""
    path = Path(path)
    path.parent.mkdir(parents=True, exist_ok=True)
    normalized = [[str(value) if value is not None else "" for value in row] for row in rows]

    with zipfile.ZipFile(path, "w", zipfile.ZIP_DEFLATED) as zf:
        def put(name: str, data: str) -> None:
            info = zipfile.ZipInfo(name, date_time=(1980, 1, 1, 0, 0, 0))
            info.compress_type = zipfile.ZIP_DEFLATED
            zf.writestr(info, data.encode("utf-8"))

        put("[Content_Types].xml", _CONTENT_TYPES)
        put("_rels/.rels", _ROOT_RELS)
        put("xl/workbook.xml", _workbook_xml(sheet_name))
        put("xl/_rels/workbook.xml.rels", _WORKBOOK_RELS)
        put("xl/styles.xml", _STYLES)
        put("xl/worksheets/sheet1.xml", _sheet_xml(normalized))


def _self_test() -> int:
    with tempfile.TemporaryDirectory() as tmp:
        path = Path(tmp) / "roundtrip.xlsx"
        rows = [
            ["strid", "zh-CN", "en-US", "ja-JP", "ru-RU", "ar", "comment"],
            ["Hello", "你好", "Hello", "こんにちは", "Привет", "مرحبا", "line1\nline2"],
            ["Empty", "", "Only English", "", "", "", ""],
        ]
        write_sheet(path, rows)
        got = read_sheet(path, "strings")
        if got != rows:
            print("round-trip mismatch", file=sys.stderr)
            print("expected:", rows, file=sys.stderr)
            print("actual:  ", got, file=sys.stderr)
            return 1
        print("xlsx_io self-test: OK")
    return 0


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--self-test", action="store_true", help="write/read a temporary workbook")
    args = parser.parse_args(argv)
    if args.self_test:
        return _self_test()
    parser.print_help()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
