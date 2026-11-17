#!/usr/bin/env python3
"""Recover only the two proven missing PT item callbacks from script.pak."""
import hashlib
import importlib.util
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
TOOL = ROOT / "PhongThanPakTool" / "PhongThanPakToolV5.py"
PAK = ROOT / "Client_Original" / "data" / "script.pak"
OUT = ROOT / "PhongThanSource" / "Deploy" / "ProjectContent" / "script" / "item"
CLIENT_LOOSE = ROOT / "PhongThanRuntime-Staging" / "Client" / "script" / "item"

spec = importlib.util.spec_from_file_location("ptpak", TOOL)
ptpak = importlib.util.module_from_spec(spec)
spec.loader.exec_module(ptpak)

ENTRIES = [
    ("\\script\\item\\\u53d8\u52a8\u7269.lua",
     "D0B457797CF4AAF0DF63411343B642E5139CE96D65CB4F0D2B62B32B86CA0D5B",
     "93F8904C4B1048E00D801F8A46E9DB1A7A734F8CFFC7B6DCCC74C3CC6AF7B20D"),
    ("\\script\\item\\\u6307\u5357\u7b26.lua",
     "18711CD21DCE982BADD1C786D48639FEAE4643B8AA840A3147DB4076A3F79B32",
     "404227D73AB5FD11643AA92ACC44D7C1A53598C153383015DE0F0B741CE7092D"),
]

pak = ptpak.KingsoftPak(PAK).open()
try:
    by_id = {entry["id"]: entry for entry in pak.entries}
    OUT.mkdir(parents=True, exist_ok=True)
    for logical, original_hash, expected_adapted in ENTRIES:
        raw_path = logical.encode("gbk")
        file_id = ptpak.filename_to_id(raw_path.decode("latin1"))
        entry = by_id.get(file_id)
        if entry is None:
            raise RuntimeError("missing exact script.pak entry 0x%08X" % file_id)
        data = pak.extract_entry(entry)
        actual_hash = hashlib.sha256(data).hexdigest().upper()
        if actual_hash != original_hash:
            raise RuntimeError("hash mismatch for 0x%08X" % file_id)
        if b"function" not in data or b"main" not in data:
            raise RuntimeError("callback has no main(): 0x%08X" % file_id)
        # Existing PT loose data represents CP936 path bytes as Latin-1 glyphs.
        loose_name = raw_path.split(b"\\")[-1].decode("latin1")
        adapted_source = CLIENT_LOOSE / loose_name
        if not adapted_source.is_file():
            raise RuntimeError("missing current canonical loose adaptation: %s" % adapted_source)
        adapted = adapted_source.read_bytes()
        adapted_hash = hashlib.sha256(adapted).hexdigest().upper()
        if adapted_hash != expected_adapted:
            raise RuntimeError("adapted loose hash mismatch for 0x%08X" % file_id)
        if b"function" not in adapted or b"main" not in adapted:
            raise RuntimeError("adapted callback has no main(): 0x%08X" % file_id)
        destination = OUT / loose_name
        destination.write_bytes(adapted)
        print("0x%08X %s original=%s adapted=%s" %
              (file_id, loose_name, actual_hash, adapted_hash))
finally:
    pak.close()
