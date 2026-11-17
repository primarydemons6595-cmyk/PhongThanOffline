"""Build the deterministic Fix #1D inventory from mounted Region_S data.

This tool is intentionally read-only with respect to runtime data.  It scans
only worlds activated by Server/maps/WorldSet.ini and decodes the trap section
exactly as KRegion::LoadServerTrap does.
"""
from __future__ import annotations

import argparse
import csv
import re
import struct
from collections import defaultdict
from pathlib import Path


def read_cp936(path: Path) -> str:
    return path.read_bytes().decode("gbk")


def disk_name(value: str) -> str:
    """Mirror Test-GameplayDataRegistry.ps1's CP936-to-Latin1 disk mapping."""
    value = value.strip().strip('"').replace("/", "\\").lstrip("\\")
    if value.lower().startswith("root\\"):
        value = value[5:]
    return "\\".join(
        part.encode("gbk").decode("latin1") if any(ord(c) > 127 for c in part) else part
        for part in value.split("\\")
    )


def active_worlds(path: Path) -> list[int]:
    text = read_cp936(path)
    count_matches = re.findall(r"(?mi)^\s*Count\s*=\s*(\d+)\s*$", text)
    if len(count_matches) != 1:
        raise ValueError(f"Expected exactly one Count in {path}")
    count = int(count_matches[0])
    slots = {
        int(m.group(1)): int(m.group(2))
        for m in re.finditer(r"(?mi)^\s*World(\d{3})\s*=\s*(\d+)\s*$", text)
    }
    if len(slots) != count or set(slots) != set(range(count)):
        raise ValueError(f"World slot mismatch: Count={count}, slots={len(slots)}")
    ids = [slots[n] for n in range(count)]
    if len(ids) != len(set(ids)):
        raise ValueError("Duplicate active runtime map ID")
    return ids


def map_metadata(path: Path) -> tuple[dict[int, str], dict[int, str]]:
    directories: dict[int, str] = {}
    names: dict[int, str] = {}
    for line in read_cp936(path).splitlines():
        match = re.match(r"^\s*(\d+)(_name)?\s*=\s*([^;\r\n]+?)\s*$", line)
        if not match:
            continue
        runtime_id = int(match.group(1))
        if match.group(2):
            names[runtime_id] = match.group(3).strip()
        elif runtime_id > 0:
            directories[runtime_id] = match.group(3).strip()
    return directories, names


def canonical_map_ids(path: Path) -> dict[int, int]:
    text = path.read_text(encoding="ascii", errors="ignore")
    rows = re.findall(
        r"\{\s*(\d+)\s*,\s*(\d+)\s*,\s*PHONGTHAN_MAP_[A-Z_]+\s*\}", text
    )
    result: dict[int, int] = {}
    for pt_text, runtime_text in rows:
        pt_id, runtime_id = int(pt_text), int(runtime_text)
        if runtime_id in result:
            raise ValueError(f"Duplicate canonical runtime ID {runtime_id}")
        result[runtime_id] = pt_id
    if not result:
        raise ValueError(f"No canonical map identities in {path}")
    return result


def trap_records(path: Path) -> list[tuple[int, int]]:
    data = path.read_bytes()
    if len(data) < 52:
        raise ValueError(f"Short Region_S: {path}")
    section_count = struct.unpack_from("<I", data, 0)[0]
    if not 6 <= section_count <= 64:
        raise ValueError(f"Invalid section count {section_count}: {path}")
    header_size = 4 + section_count * 8
    if len(data) < header_size:
        raise ValueError(f"Short Region_S header: {path}")
    sections = list(struct.iter_unpack("<II", data[4:header_size]))
    trap_offset, trap_length = sections[1]
    start, end = header_size + trap_offset, header_size + trap_offset + trap_length
    if end > len(data):
        raise ValueError(f"Trap section outside Region_S: {path}")
    payload = data[start:end]
    if not payload:
        return []
    if len(payload) < 12:
        raise ValueError(f"Short KTrapFileHead: {path}")
    count, reserved1, reserved2 = struct.unpack_from("<III", payload)
    if reserved1 or reserved2 or len(payload) != 12 + count * 8:
        raise ValueError(f"Invalid trap section: {path}")
    records: list[tuple[int, int]] = []
    for x, y, cells, reserved, trap_id in struct.iter_unpack("<BBBBI", payload[12:]):
        # Match KRegion::LoadServerTrap: malformed spans are ignored.
        if reserved or y >= 32 or x + cells - 1 >= 16:
            continue
        if trap_id:
            records.append((trap_id, cells))
    return records


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--server-root", type=Path, required=True)
    parser.add_argument("--map-identity", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()

    active = active_worlds(args.server_root / "maps" / "WorldSet.ini")
    if len(active) != 102:
        raise ValueError(f"Expected 102 active worlds, found {len(active)}")
    directories, names = map_metadata(args.server_root / "settings" / "WorldSet.ini")
    identities = canonical_map_ids(args.map_identity)

    aggregate: dict[tuple[int, int], dict[str, object]] = defaultdict(
        lambda: {"cells": 0, "regions": set(), "sources": set()}
    )
    scanned_regions = 0
    for runtime_id in active:
        if runtime_id not in identities:
            raise ValueError(f"Active runtime map {runtime_id} has no canonical PT identity")
        if runtime_id not in directories:
            raise ValueError(f"Active runtime map {runtime_id} has no map directory metadata")
        relative = disk_name(directories[runtime_id])
        candidates = [
            args.server_root / "maps" / f"{relative}_S",
            args.server_root / "maps" / relative,
        ]
        source = next((p for p in candidates if p.is_dir()), None)
        if source is None:
            raise ValueError(f"No Region_S directory for runtime map {runtime_id}: {relative}")
        region_files = sorted(source.glob("v_*/*_Region_S.dat"))
        for region_file in region_files:
            scanned_regions += 1
            region_key = region_file.relative_to(args.server_root).as_posix()
            for trap_id, cell_count in trap_records(region_file):
                row = aggregate[(runtime_id, trap_id)]
                row["cells"] = int(row["cells"]) + cell_count
                row["regions"].add(region_key)
                row["sources"].add(source.relative_to(args.server_root).as_posix())

    args.output.parent.mkdir(parents=True, exist_ok=True)
    fields = [
        "RuntimeMapId", "PtMapId", "MapName", "RegionDataSource", "TrapIdHex",
        "TrapReferenceCount", "RegionReferenceCount", "LooseScriptStatus",
        "Classification", "Provenance", "DestinationPtMapId",
        "DestinationRuntimeMapId", "DestinationX", "DestinationY", "ValidationStatus",
    ]
    with args.output.open("w", newline="", encoding="utf-8-sig") as stream:
        writer = csv.DictWriter(stream, fieldnames=fields)
        writer.writeheader()
        for runtime_id, trap_id in sorted(aggregate):
            info = aggregate[(runtime_id, trap_id)]
            writer.writerow({
                "RuntimeMapId": runtime_id,
                "PtMapId": identities[runtime_id],
                "MapName": names.get(runtime_id, directories[runtime_id]),
                "RegionDataSource": ";".join(sorted(info["sources"])),
                "TrapIdHex": f"0x{trap_id:08X}",
                "TrapReferenceCount": info["cells"],
                "RegionReferenceCount": len(info["regions"]),
                "LooseScriptStatus": "NOT_EVALUATED",
                "Classification": "UNCLASSIFIED",
                "Provenance": "MOUNTED_REGION_S",
                "ValidationStatus": "PENDING_CALLBACK_RESOLUTION",
            })

    unique = len({trap_id for _, trap_id in aggregate})
    print(f"ACTIVE_WORLDS={len(active)}")
    print(f"REGION_FILES={scanned_regions}")
    print(f"MAP_TRAP_ROWS={len(aggregate)}")
    print(f"UNIQUE_TRAP_IDS={unique}")
    print(f"OUTPUT={args.output}")


if __name__ == "__main__":
    main()
