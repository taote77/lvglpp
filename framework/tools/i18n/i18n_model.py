#!/usr/bin/env python3
"""Translation workbook model, validation and lv_i18n C rendering."""

from __future__ import annotations

import re
from dataclasses import dataclass, field
from pathlib import Path

from xlsx_io import read_sheet

# Pack order matters: en-US is first and is used as lv_i18n's runtime fallback.
LOCALES: tuple["Locale", ...] = (
    ("en-US", "en_us", ("en-us", "en", "english")),
    ("ar", "ar", ("ar", "arabic")),
    ("ja-JP", "ja_jp", ("ja-jp", "ja", "jp", "japanese")),
    ("ru-RU", "ru_ru", ("ru-ru", "ru", "russian")),
    ("zh-CN", "zh_cn", ("zh-cn", "zh", "ch", "cn", "chinese")),
)

STRID_HEADERS = {"strid", "code", "key"}
COMMENT_HEADERS = {"comment", "comments", "note", "notes", "context", "description"}

_CJK_RE = re.compile(r"[㐀-鿿぀-ヿ가-힯]")


@dataclass(frozen=True)
class Locale:
    tag: str
    symbol: str
    aliases: tuple[str, ...]


LOCALES = tuple(Locale(*item) for item in LOCALES)  # type: ignore[arg-type]


@dataclass
class Row:
    strid: str
    values: dict[str, str] = field(default_factory=dict)
    comment: str = ""
    source_row: int = 0


@dataclass
class Model:
    rows: list[Row]
    locales: tuple[Locale, ...] = LOCALES
    warnings: list[str] = field(default_factory=list)
    errors: list[str] = field(default_factory=list)

    def locale(self, tag: str) -> Locale | None:
        for locale in self.locales:
            if locale.tag == tag:
                return locale
        return None


def _norm_header(value: str) -> str:
    return value.strip().lstrip("﻿").lower()


def _find_column(headers: list[str], aliases: set[str] | tuple[str, ...]) -> list[int]:
    wanted = {alias.lower() for alias in aliases}
    return [index for index, header in enumerate(headers) if _norm_header(header) in wanted]


def _format_fields(text: str) -> list[str]:
    """Return the ``{...}`` field specs in *text*, ignoring escaped braces."""
    text = text.replace("{{", "\x00").replace("}}", "\x01")
    fields: list[str] = []
    index = 0
    while index < len(text):
        if text[index] == "{":
            end = text.find("}", index + 1)
            if end != -1:
                fields.append(text[index + 1 : end])
                index = end + 1
                continue
        index += 1
    return fields


def load_model(xlsx_path: str | Path, sheet_name: str | None = None) -> Model:
    rows = read_sheet(xlsx_path, sheet_name)
    model = Model(rows=[])

    header_index = -1
    for index, row in enumerate(rows):
        if any(str(cell).strip() for cell in row):
            header_index = index
            break
    if header_index < 0:
        model.errors.append("workbook is empty")
        return model

    raw_headers = [str(cell).strip().lstrip("﻿") for cell in rows[header_index]]
    headers = [_norm_header(header) for header in raw_headers]

    strid_columns = _find_column(raw_headers, STRID_HEADERS)
    if not strid_columns:
        model.errors.append("missing 'strid' column (aliases: code, key)")
        return model
    strid_col = strid_columns[0]

    locale_columns: dict[str, int] = {}
    for locale in LOCALES:
        candidates = _find_column(raw_headers, locale.aliases)
        if not candidates:
            model.errors.append(f"missing column for locale {locale.tag}")
            continue
        exact = [index for index in candidates if _norm_header(raw_headers[index]) == locale.tag.lower()]
        locale_columns[locale.tag] = exact[0] if exact else candidates[0]
        if len(candidates) > 1:
            model.warnings.append(
                f"multiple columns match {locale.tag}: "
                + ", ".join(raw_headers[index] for index in candidates)
                + f"; using {raw_headers[locale_columns[locale.tag]]!r}"
            )

    known_columns = {strid_col, *locale_columns.values()}
    for index, header in enumerate(headers):
        if index in known_columns or not header:
            continue
        if header in COMMENT_HEADERS:
            continue
        model.warnings.append(f"ignoring unknown column {raw_headers[index]!r}")

    comment_columns = _find_column(raw_headers, COMMENT_HEADERS)
    comment_col = comment_columns[0] if comment_columns else None

    seen: dict[str, int] = {}
    for row_index in range(header_index + 1, len(rows)):
        raw = rows[row_index]
        if not any(str(cell).strip() for cell in raw):
            continue

        def cell(column: int | None) -> str:
            if column is None or column >= len(raw):
                return ""
            return str(raw[column])

        strid = cell(strid_col).strip()
        excel_row = row_index + 1
        if not strid:
            model.errors.append(f"row {excel_row}: empty strid")
            continue
        if strid in seen:
            model.errors.append(f"row {excel_row}: duplicate strid {strid!r} (first at row {seen[strid]})")
            continue
        seen[strid] = excel_row

        values = {locale.tag: cell(locale_columns.get(locale.tag)) for locale in LOCALES}
        if not values.get("en-US", "").strip():
            model.errors.append(f"row {excel_row}: {strid!r} has no en-US value")

        model.rows.append(
            Row(
                strid=strid,
                values=values,
                comment=cell(comment_col),
                source_row=excel_row,
            )
        )

    _validate_placeholders(model)
    return model


def _validate_placeholders(model: Model) -> None:
    for row in model.rows:
        reference = _format_fields(row.values.get("en-US", ""))
        for locale in LOCALES:
            if locale.tag == "en-US":
                continue
            value = row.values.get(locale.tag, "")
            if not value:
                continue
            fields = _format_fields(value)
            if len(fields) != len(reference):
                model.warnings.append(
                    f"{row.strid}: {locale.tag} has {len(fields)} placeholder(s), "
                    f"en-US has {len(reference)}"
                )
            elif fields != reference:
                model.warnings.append(
                    f"{row.strid}: {locale.tag} placeholder specs differ from en-US "
                    f"({fields!r} vs {reference!r})"
                )
            if locale.tag == "ar" and _CJK_RE.search(value):
                model.warnings.append(f"{row.strid}: ar value contains CJK characters (untranslated?)")


def c_string_literal(value: str) -> str:
    out: list[str] = ['"']
    for char in value:
        code = ord(char)
        if char == "\\":
            out.append("\\\\")
        elif char == '"':
            out.append('\\"')
        elif char == "\n":
            out.append("\\n")
        elif char == "\r":
            out.append("\\r")
        elif char == "\t":
            out.append("\\t")
        elif code < 0x20:
            out.append(f"\\{code:03o}")
        else:
            out.append(char)
    out.append('"')
    return "".join(out)


def extract_charset(model: Model) -> str:
    chars = set()
    for row in model.rows:
        for value in row.values.values():
            for char in value:
                if char in "\n\r\t" or ord(char) >= 0x20:
                    chars.add(char)
    return "".join(sorted(chars, key=ord))


_PLURAL_BODIES = {
    "en-US": """\
    uint32_t n = op_n(num); UNUSED(n);
    uint32_t i = op_i(n); UNUSED(i);
    uint32_t v = op_v(n); UNUSED(v);

    if ((i == 1 && v == 0)) return LV_I18N_PLURAL_TYPE_ONE;
    return LV_I18N_PLURAL_TYPE_OTHER;""",
    "ar": """\
    uint32_t n = op_n(num); UNUSED(n);

    uint32_t n100 = n % 100;
    if ((n == 0)) return LV_I18N_PLURAL_TYPE_ZERO;
    if ((n == 1)) return LV_I18N_PLURAL_TYPE_ONE;
    if ((n == 2)) return LV_I18N_PLURAL_TYPE_TWO;
    if (((3 <= n100 && n100 <= 10))) return LV_I18N_PLURAL_TYPE_FEW;
    if (((11 <= n100 && n100 <= 99))) return LV_I18N_PLURAL_TYPE_MANY;
    return LV_I18N_PLURAL_TYPE_OTHER;""",
    "ja-JP": """\
    uint32_t n = op_n(num); UNUSED(n);

    return LV_I18N_PLURAL_TYPE_OTHER;""",
    "ru-RU": """\
    uint32_t n = op_n(num); UNUSED(n);
    uint32_t v = op_v(n); UNUSED(v);
    uint32_t i = op_i(n); UNUSED(i);
    uint32_t i10 = i % 10;
    uint32_t i100 = i % 100;
    if ((v == 0 && i10 == 1 && i100 != 11)) return LV_I18N_PLURAL_TYPE_ONE;
    if ((v == 0 && (2 <= i10 && i10 <= 4) && (!(12 <= i100 && i100 <= 14)))) return LV_I18N_PLURAL_TYPE_FEW;
    if ((v == 0 && i10 == 0) || (v == 0 && (5 <= i10 && i10 <= 9)) || (v == 0 && (11 <= i100 && i100 <= 14))) return LV_I18N_PLURAL_TYPE_MANY;
    return LV_I18N_PLURAL_TYPE_OTHER;""",
    "zh-CN": """\
    uint32_t n = op_n(num); UNUSED(n);

    return LV_I18N_PLURAL_TYPE_OTHER;""",
}

DATA_MARKER = "/*__I18N_DATA__*/"


def _render_locale(model: Model, locale: Locale) -> list[str]:
    lines = [f"static lv_i18n_phrase_t {locale.symbol}_singulars[] = {{"]
    for row in model.rows:
        value = row.values.get(locale.tag, "")
        if value == "":
            continue
        lines.append(f"    {{{c_string_literal(row.strid)}, {c_string_literal(value)}}},")
    lines.append("    {NULL, NULL} // End mark")
    lines.append("};")
    lines.extend(["", "", ""])
    lines.append(f"static uint8_t {locale.symbol}_plural_fn(int32_t num)")
    lines.append("{")
    body = _PLURAL_BODIES.get(locale.tag)
    if body is None:
        raise ValueError(f"no plural rule for {locale.tag}")
    lines.extend(body.splitlines())
    lines.append("}")
    lines.append("")
    lines.append(f"static const lv_i18n_lang_t {locale.symbol}_lang = {{")
    lines.append(f'    .locale_name = "{locale.tag}",')
    lines.append(f"    .singulars = {locale.symbol}_singulars,")
    lines.append("")
    lines.append(f"    .locale_plural_fn = {locale.symbol}_plural_fn")
    lines.append("};")
    lines.append("")
    return lines


def render_lv_i18n_c(model: Model, template: str) -> str:
    if DATA_MARKER not in template:
        raise ValueError(f"template does not contain {DATA_MARKER}")

    blocks: list[str] = []
    for locale in LOCALES:
        blocks.extend(_render_locale(model, locale))

    blocks.append("const lv_i18n_language_pack_t lv_i18n_language_pack[] = {")
    for locale in LOCALES:
        blocks.append(f"    &{locale.symbol}_lang,")
    blocks.append("    NULL // End mark")
    blocks.append("};")

    rendered = template.replace(DATA_MARKER, "\n".join(blocks))
    if not rendered.endswith("\n"):
        rendered += "\n"
    return rendered
