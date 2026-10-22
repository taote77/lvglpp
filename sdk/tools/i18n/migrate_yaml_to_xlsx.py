#!/usr/bin/env python3
"""One-time migration: smart_home YAML translations -> strings.xlsx.

This is the only script in the pipeline that uses PyYAML.  BaseLoader is used
deliberately: safe_load turns values like ``On``/``Off``/``Yes``/``No`` into
booleans, which would silently change the generated translations.
"""

from __future__ import annotations

import argparse
import sys
from pathlib import Path

import yaml

sys.path.insert(0, str(Path(__file__).resolve().parent))

from i18n_model import LOCALES, load_model  # noqa: E402
from xlsx_io import read_sheet, write_sheet  # noqa: E402

LANGUAGES = ("en-US", "zh-CN", "ja-JP", "ru-RU", "ar")
FILE_BY_LANGUAGE = {
    "en-US": "en-US.yml",
    "zh-CN": "zh-CN.yml",
    "ja-JP": "ja-JP.yml",
    "ja": "ja.yml",
    "ru-RU": "ru-RU.yml",
    "ar": "ar.yml",
}

KEY_FIXES = {"UniformityCalibration:": "UniformityCalibration"}

# Extras that only exist in ja.yml / ar.yml need an explicit en-US value.
MANUAL_EN = {
    "HeatModLabel": "Heating Module",
    "Machine error. Please restart.": "Machine error. Please restart.",
    "If error still occurs after restart, please contact support.": (
        "If error still occurs after restart, please contact support."
    ),
    "Restart": "Restart",
    "ChangeLangWarnTitle": "Change System Language",
    "ChangeLangWarnConfirm": "Change",
    "ChangeLangWarnCancel": "Cancel",
    "ChangeLangWarnConfirmText": "Change the system language?",
    "ChangeLangWarnConfirmTitle": "Change System Language",
    "ChangeLangWarnConfirmConfirm": "Change",
    "ChangeLangWarnConfirmCancel": "Cancel",
    "ChangeLangWarnConfirmConfirmText": "Change the system language?",
    "ChangeLangWarnConfirmConfirmTitle": "Change System Language",
    "ChangeLangWarnConfirmConfirmConfirm": "Change",
    "ChangeLangWarnConfirmConfirmCancel": "Cancel",
}


class UniqueKeyLoader(yaml.BaseLoader):
    def construct_mapping(self, node, deep=False):
        seen = set()
        for key_node, _ in node.value:
            key = self.construct_object(key_node, deep=deep)
            if key in seen:
                raise ValueError(f"duplicate key {key!r}")
            seen.add(key)
        return super().construct_mapping(node, deep=deep)


def load_language(path: Path, locale: str) -> dict[str, str]:
    with path.open("r", encoding="utf-8") as handle:
        data = yaml.load(handle, Loader=UniqueKeyLoader)
    if not isinstance(data, dict):
        raise ValueError(f"{path}: expected a mapping")
    values = data.get(locale)
    if not isinstance(values, dict):
        raise ValueError(f"{path}: missing top-level locale key {locale!r}")
    return {str(key): ("" if value is None else str(value)) for key, value in values.items()}


def normalize_keys(values: dict[str, str], warnings: list[str]) -> dict[str, str]:
    result: dict[str, str] = {}
    for key, value in values.items():
        fixed = str(key).strip()
        if fixed in KEY_FIXES:
            warnings.append(f"normalized key {fixed!r} -> {KEY_FIXES[fixed]!r}")
            fixed = KEY_FIXES[fixed]
        if fixed in result and result[fixed] != value:
            warnings.append(f"key {fixed!r}: keeping first non-empty value, second was {value!r}")
            if not result[fixed] and value:
                result[fixed] = value
        elif value:
            result[fixed] = value
        elif fixed not in result:
            result[fixed] = ""
    return result


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--input-dir", default="smart_home/i18n/translations")
    parser.add_argument("--out", default="smart_home/i18n/strings.xlsx")
    args = parser.parse_args(argv)

    input_dir = Path(args.input_dir)
    warnings: list[str] = []

    loaded: dict[str, dict[str, str]] = {}
    for locale in ("en-US", "zh-CN", "ja-JP", "ja", "ru-RU", "ar"):
        path = input_dir / FILE_BY_LANGUAGE[locale]
        if not path.exists():
            print(f"error: missing {path}", file=sys.stderr)
            return 1
        loaded[locale] = normalize_keys(load_language(path, locale), warnings)

    # Fold the legacy ja.yml pack into ja-JP. ja-JP wins on conflicts because
    # it is the locale LvTranslator will use after this migration.
    ja_jp = dict(loaded["ja-JP"])
    ja_conflicts: list[tuple[str, str, str]] = []
    for key, value in loaded["ja"].items():
        if key in ja_jp:
            if ja_jp[key] != value:
                ja_conflicts.append((key, ja_jp[key], value))
        elif value:
            ja_jp[key] = value
    loaded["ja-JP"] = ja_jp

    ordered: list[str] = []
    for key in loaded["en-US"]:
        if key not in ordered:
            ordered.append(key)
    for locale in ("zh-CN", "ja-JP", "ru-RU", "ar"):
        for key in loaded[locale]:
            if key not in ordered:
                ordered.append(key)

    notes: dict[str, str] = {}
    rows: list[list[str]] = [["strid", "zh-CN", "en-US", "ja-JP", "ru-RU", "ar", "comment"]]
    for key in ordered:
        en_value = loaded["en-US"].get(key, "")
        if not en_value:
            en_value = MANUAL_EN.get(key, key if " " in key else "")
            if en_value:
                notes[key] = "en-US synthesized during YAML migration"
            else:
                warnings.append(f"no en-US value for {key!r}; using strid as English text")
                en_value = key
        # Column order must match the header:
        # strid | zh-CN | en-US | ja-JP | ru-RU | ar | comment
        row = [key]
        for locale in ("zh-CN", "en-US", "ja-JP", "ru-RU", "ar"):
            if locale == "en-US":
                row.append(en_value)
            else:
                row.append(loaded[locale].get(key, ""))
        row.append(notes.get(key, ""))
        rows.append(row)

    out_path = Path(args.out)
    write_sheet(out_path, rows, sheet_name="strings")

    # Verify the workbook round-trips before reporting success.
    readback = read_sheet(out_path, "strings")
    if readback != rows:
        print("error: workbook round-trip mismatch", file=sys.stderr)
        for index, (expected, actual) in enumerate(zip(rows, readback)):
            if expected != actual:
                print(f"  row {index + 1}:\n    expected {expected!r}\n    actual   {actual!r}", file=sys.stderr)
                break
        return 1

    model = load_model(out_path, "strings")
    for warning in warnings + model.warnings:
        print(f"warning: {warning}", file=sys.stderr)
    for error in model.errors:
        print(f"error: {error}", file=sys.stderr)
    if model.errors:
        return 1

    print(f"wrote {out_path}: {len(ordered)} strids")
    for locale in LOCALES:
        count = sum(1 for row in model.rows if row.values.get(locale.tag, "") != "")
        print(f"  {locale.tag:>6}: {count} entries")
    if ja_conflicts:
        print("ja.yml conflicts resolved in favor of ja-JP:")
        for key, kept, dropped in ja_conflicts:
            print(f"  {key!r}: kept {kept!r}, dropped {dropped!r}")
    if warnings:
        print(f"{len(warnings)} warning(s) reported above", file=sys.stderr)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
