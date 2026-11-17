#!/usr/bin/env python3
"""Targeted recovery of original PT player equipment SPRs referenced by tables."""
import argparse
import csv
import hashlib
import importlib.util
from pathlib import Path


PART_WORDS = ("躯体", "头部", "披风")
FAMILY_WORDS = ("甲士", "术士", "异人")


def sha256(data):
    return hashlib.sha256(data).hexdigest().upper()


def load_pak_tool(path):
    spec = importlib.util.spec_from_file_location("ptpak", path)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


def decoded_name(path):
    try:
        return path.name.encode("cp1252").decode("gbk")
    except (UnicodeEncodeError, UnicodeDecodeError):
        return path.name


def collect_paths(npcres):
    requested = set()
    tables = []
    for table in npcres.glob("*.txt"):
        label = decoded_name(table)
        if not any(word in label for word in PART_WORDS):
            continue
        if not any(word in label for word in FAMILY_WORDS):
            continue
        tables.append((table, label))
        text = table.read_bytes().decode("gbk")
        for line in text.splitlines()[1:]:
            for value in line.split("\t")[1:]:
                value = value.strip()
                if value.lower().endswith(".spr") and value.isascii():
                    requested.add("\\spr\\npcres\\human\\" + value)
    return sorted(requested, key=str.lower), tables


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--project-root", required=True)
    args = parser.parse_args()
    root = Path(args.project_root).resolve()
    original = root / "Client_Original"
    client = root / "PhongThanRuntime-Staging" / "Client"
    canonical = root / "PhongThanSource" / "Deploy" / "ProjectContent"
    tool = load_pak_tool(root / "PhongThanPakTool" / "PhongThanPakToolV5.py")
    virtual_paths, tables = collect_paths(client / "settings" / "npcres")
    package_lines = (original / "package.ini").read_text(encoding="cp1252").splitlines()
    pak_names = []
    for line in package_lines:
        if "=" not in line or not line.split("=", 1)[0].strip().isdigit():
            continue
        pak_names.append(line.split("=", 1)[1].strip())
    requested_ids = {tool.filename_to_id(path): path for path in virtual_paths}
    sources = {}
    for pak_name in pak_names:
        pak_path = original / "data" / pak_name
        if not pak_path.exists():
            continue
        pak = tool.PakV5(pak_path).open()
        try:
            for entry in pak.entries:
                if entry["id"] in requested_ids and entry["id"] not in sources:
                    sources[entry["id"]] = (pak_path, entry)
        finally:
            pak.close()
    rows = []
    recovered = missing = 0
    by_pak = {}
    for file_id, virtual in requested_ids.items():
        source = sources.get(file_id)
        if not source:
            rows.append((virtual, "SPR", "MISSING", "", "", False,
                         "equipment body/head/cape", "MISSING", "hash not present"))
            missing += 1
            continue
        by_pak.setdefault(source[0], []).append((virtual, source[1]))
    for pak_path, work in by_pak.items():
        pak = tool.PakV5(pak_path).open()
        try:
          for virtual, entry in work:
            converted = entry["method"] == 0x11000000
            data = (pak.reconstruct_fragmented_spr(entry)
                    if converted else pak.payload(entry))
            relative = virtual.lstrip("\\").replace("\\", "/")
            for destination in (canonical / relative, client / relative):
                destination.parent.mkdir(parents=True, exist_ok=True)
                destination.write_bytes(data)
            digest = sha256(data)
            provenance = "CONVERTED" if converted else "BYTE_IDENTICAL"
            note = (f"SHA256={digest}; reconstructed from original frame-compressed PAK entry"
                    if converted else f"SHA256={digest}; exact original logical entry bytes")
            rows.append((virtual, "SPR", "LOOSE", pak_path.name, virtual, True,
                         "equipment body/head/cape", provenance, note))
            recovered += 1
        finally:
            pak.close()
    report = root / "analysis" / "repair" / "remaining_pak_dependencies.csv"
    report.parent.mkdir(parents=True, exist_ok=True)
    with report.open("w", newline="", encoding="utf-8-sig") as stream:
        writer = csv.writer(stream)
        writer.writerow(("RequestedPath", "ResourceType", "RuntimeSource", "PakArchive",
                         "PakEntry", "LooseExists", "NeededFor", "MigrationStatus", "Notes"))
        writer.writerows(rows)
    print(f"TABLES={len(tables)} REQUESTED={len(virtual_paths)} RECOVERED={recovered} MISSING={missing}")
    if missing:
        raise SystemExit(2)


if __name__ == "__main__":
    main()
