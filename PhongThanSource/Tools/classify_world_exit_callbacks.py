"""Classify exact Region_S callbacks and materialize validated loose exits."""
from __future__ import annotations

import argparse
import csv
import hashlib
import re
import shutil
from collections import defaultdict
from pathlib import Path

from build_world_exit_inventory import active_worlds, canonical_map_ids, disk_name, map_metadata, read_cp936


FIXED_BINDINGS = {
    0xCE741D0E: ("script/phongthan/gate/supertrap.lua", "SUPERTRAP_OR_OTHER_TRANSITION", "ADAPTED"),
    0x381C646E: ("script/phongthan/mapexit/chongcheng_camp_to_outskirts.lua", "NORMAL_EXIT", "ADAPTED"),
    0x5F310810: ("script/phongthan/mapexit/chongcheng_outskirts_to_camp.lua", "NORMAL_EXIT", "ADAPTED"),
}


def parse_rect(path: Path) -> tuple[int, int, int, int]:
    text = read_cp936(path)
    main = re.search(r"(?mis)^\s*\[MAIN\]\s*(.*?)(?=^\s*\[|\Z)", text)
    if not main:
        raise ValueError(f"Missing [MAIN] in {path}")
    match = re.search(r"(?mi)^\s*rect\s*=\s*(-?\d+)\s*,\s*(-?\d+)\s*,\s*(-?\d+)\s*,\s*(-?\d+)", main.group(1))
    if not match:
        raise ValueError(f"Missing MAIN.rect in {path}")
    return tuple(map(int, match.groups()))


def classify(payload: bytes) -> tuple[str, tuple[int, int, int] | None, str]:
    # Lua keywords/API names are ASCII; comments/messages contain several
    # legacy Vietnamese/Chinese encodings and must not influence semantics.
    text = payload.decode("latin1")
    calls = [tuple(map(int, match)) for match in re.findall(
        r"(?i)\bNewWorld\s*\(\s*(\d+)\s*,\s*(\d+)\s*,\s*(\d+)\s*\)", text
    )]
    any_calls = bool(re.search(r"(?i)\bNewWorld\s*\(", text))
    if len(calls) == 1:
        return "NORMAL_EXIT", calls[0], "DIRECT_FIXED_NEWWORLD"
    if len(calls) > 1:
        return "SUPERTRAP_OR_OTHER_TRANSITION", None, "MULTI_DESTINATION_NEWWORLD"
    if any_calls:
        return "UNKNOWN", None, "NON_LITERAL_NEWWORLD"
    return "NON_EXIT", None, "NO_NEWWORLD"


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--inventory", type=Path, required=True)
    parser.add_argument("--catalog", type=Path, required=True)
    parser.add_argument("--payload-root", type=Path, required=True)
    parser.add_argument("--server-root", type=Path, required=True)
    parser.add_argument("--map-identity", type=Path, required=True)
    parser.add_argument("--project-content", type=Path, required=True)
    parser.add_argument("--graph", type=Path, required=True)
    parser.add_argument("--registry", type=Path, required=True)
    args = parser.parse_args()

    rows = list(csv.DictReader(args.inventory.open(encoding="utf-8-sig")))
    catalog = {int(row["TrapIdHex"], 16): row for row in csv.DictReader(args.catalog.open(encoding="utf-8-sig"))}
    active = set(active_worlds(args.server_root / "maps" / "WorldSet.ini"))
    identities = canonical_map_ids(args.map_identity)
    pt_to_runtime = {pt: runtime for runtime, pt in identities.items()}
    directories, names = map_metadata(args.server_root / "settings" / "WorldSet.ini")

    payloads: dict[int, bytes] = {}
    classifications: dict[int, tuple[str, tuple[int, int, int] | None, str]] = {}
    for trap_id in sorted({int(row["TrapIdHex"], 16) for row in rows}):
        entry = catalog.get(trap_id)
        if not entry or entry["Status"] != "RESOLVED":
            classifications[trap_id] = ("UNRESOLVED", None, "NOT_FOUND_IN_ORIGINAL_PACKAGE_SET")
            continue
        payload = (args.payload_root / f"{trap_id:08X}.bin").read_bytes()
        payloads[trap_id] = payload
        classifications[trap_id] = classify(payload)
    # Fix #1B is a known multi-destination transition with its own maintained binding.
    classifications[0xCE741D0E] = ("SUPERTRAP_OR_OTHER_TRANSITION", None, "FIX_1B_SUPERTRAP")

    validation: dict[int, tuple[str, int | None, int | None, int | None, int | None]] = {}
    enabled: dict[int, tuple[str, str, str]] = {}
    recovered_root = args.project_content / "script" / "phongthan" / "mapexit" / "recovered"
    recovered_root.mkdir(parents=True, exist_ok=True)
    for trap_id, (kind, destination, _reason) in sorted(classifications.items()):
        if trap_id in FIXED_BINDINGS:
            loose_path, fixed_kind, provenance = FIXED_BINDINGS[trap_id]
            full_path = args.project_content / Path(loose_path)
            if not full_path.is_file():
                raise ValueError(f"Missing established loose callback: {full_path}")
            enabled[trap_id] = (loose_path.replace("/", "\\"), fixed_kind, provenance)
            if fixed_kind != "NORMAL_EXIT":
                validation[trap_id] = ("VALIDATED_OTHER_TRANSITION", None, None, None, None)
                continue
        if kind != "NORMAL_EXIT" or destination is None:
            validation[trap_id] = ("NOT_APPLICABLE", None, None, None, None)
            continue
        pt_id, x, y = destination
        runtime_id = pt_to_runtime.get(pt_id)
        if runtime_id is None:
            validation[trap_id] = ("INVALID_DESTINATION_ID", pt_id, None, x, y)
            continue
        if runtime_id not in active:
            validation[trap_id] = ("DESTINATION_WORLD_INACTIVE", pt_id, runtime_id, x, y)
            continue
        relative = disk_name(directories[runtime_id])
        world_file = args.server_root / "maps" / f"{relative}.wor"
        if not world_file.is_file():
            validation[trap_id] = ("DESTINATION_WOR_MISSING", pt_id, runtime_id, x, y)
            continue
        left, top, right, bottom = parse_rect(world_file)
        region_x, region_y = x // 16, y // 32
        if not (left <= region_x <= right and top <= region_y <= bottom):
            validation[trap_id] = ("DESTINATION_OUTSIDE_WOR_RECT", pt_id, runtime_id, x, y)
            continue
        source_dir = next((p for p in (
            args.server_root / "maps" / f"{relative}_S",
            args.server_root / "maps" / relative,
        ) if p.is_dir()), None)
        region_file = source_dir / f"v_{region_y:03d}" / f"{region_x:03d}_Region_S.dat" if source_dir else None
        # KRegion::Load explicitly accepts sparse cells inside the authoritative
        # WOR rectangle and initializes an empty region.  Absence of a combined
        # Region_S file therefore is not an invented bounds expansion.
        region_status = "VALIDATED" if region_file is not None and region_file.is_file() else "VALIDATED_SPARSE_REGION"
        validation[trap_id] = (region_status, pt_id, runtime_id, x, y)
        if trap_id not in enabled:
            # KSortScript lowercases the discovered relative filename before
            # hashing it; aliases must use that exact canonical hash domain.
            relative_path = f"script/phongthan/mapexit/recovered/{trap_id:08x}.lua"
            destination_path = args.project_content / Path(relative_path)
            source_payload = payloads[trap_id]
            if destination_path.exists() and destination_path.read_bytes() != source_payload:
                raise ValueError(f"Recovered callback collision: {destination_path}")
            destination_path.write_bytes(source_payload)
            enabled[trap_id] = (relative_path.replace("/", "\\"), "NORMAL_EXIT", "BYTE_IDENTICAL")

    for row in rows:
        trap_id = int(row["TrapIdHex"], 16)
        kind, _destination, reason = classifications[trap_id]
        status, pt_id, runtime_id, x, y = validation[trap_id]
        row["LooseScriptStatus"] = "LOOSE_RESOLVED" if trap_id in enabled else (
            "ORIGINAL_PACKED_AVAILABLE" if trap_id in payloads else "UNRESOLVED"
        )
        row["Classification"] = kind
        if trap_id in enabled:
            row["Provenance"] = enabled[trap_id][2]
        elif trap_id in payloads:
            row["Provenance"] = "ORIGINAL_SCRIPT_PAK_NOT_ENABLED"
        else:
            row["Provenance"] = "NO_AUTHORITATIVE_CALLBACK_LOCATED"
        row["DestinationPtMapId"] = pt_id if pt_id is not None else ""
        row["DestinationRuntimeMapId"] = runtime_id if runtime_id is not None else ""
        row["DestinationX"] = x if x is not None else ""
        row["DestinationY"] = y if y is not None else ""
        row["ValidationStatus"] = f"{status}:{reason}"

    with args.inventory.open("w", newline="", encoding="utf-8-sig") as stream:
        writer = csv.DictWriter(stream, fieldnames=rows[0].keys())
        writer.writeheader()
        writer.writerows(rows)

    args.registry.parent.mkdir(parents=True, exist_ok=True)
    with args.registry.open("w", newline="", encoding="ascii") as stream:
        stream.write("ScriptIdHex\tLoosePath\tClassification\tProvenance\tSha256\r\n")
        for trap_id, (path, kind, provenance) in sorted(enabled.items()):
            digest = hashlib.sha256((args.project_content / Path(path)).read_bytes()).hexdigest().upper()
            stream.write(f"0x{trap_id:08X}\t\\{path}\t{kind}\t{provenance}\t{digest}\r\n")

    exit_rows = [row for row in rows if row["Classification"] == "NORMAL_EXIT" and row["ValidationStatus"].startswith("VALIDATED")]
    edges = {(int(row["PtMapId"]), int(row["DestinationPtMapId"])) for row in exit_rows}
    graph_fields = [
        "SourcePtMapId", "SourceRuntimeMapId", "SourceMapName", "TrapIdHex",
        "DestinationPtMapId", "DestinationRuntimeMapId", "DestinationMapName",
        "DestinationX", "DestinationY", "CallbackPath", "Provenance",
        "ReverseLinkStatus", "ValidationStatus",
    ]
    args.graph.parent.mkdir(parents=True, exist_ok=True)
    with args.graph.open("w", newline="", encoding="utf-8-sig") as stream:
        writer = csv.DictWriter(stream, fieldnames=graph_fields)
        writer.writeheader()
        for row in sorted(exit_rows, key=lambda item: (int(item["PtMapId"]), item["TrapIdHex"])):
            source_pt, destination_pt = int(row["PtMapId"]), int(row["DestinationPtMapId"])
            trap_id = int(row["TrapIdHex"], 16)
            writer.writerow({
                "SourcePtMapId": source_pt,
                "SourceRuntimeMapId": row["RuntimeMapId"],
                "SourceMapName": row["MapName"],
                "TrapIdHex": row["TrapIdHex"],
                "DestinationPtMapId": destination_pt,
                "DestinationRuntimeMapId": row["DestinationRuntimeMapId"],
                "DestinationMapName": names.get(int(row["DestinationRuntimeMapId"]), ""),
                "DestinationX": row["DestinationX"],
                "DestinationY": row["DestinationY"],
                "CallbackPath": enabled[trap_id][0],
                "Provenance": enabled[trap_id][2],
                "ReverseLinkStatus": "CONFIRMED" if (destination_pt, source_pt) in edges else "ONE_WAY_AUTHENTIC",
                "ValidationStatus": "VALIDATED",
            })

    counts = defaultdict(int)
    for trap_id, (kind, _destination, _reason) in classifications.items():
        counts[kind] += 1
    print(f"UNIQUE_TRAPS={len(classifications)}")
    for key in sorted(counts):
        print(f"CLASS_{key}={counts[key]}")
    print(f"ENABLED_BINDINGS={len(enabled)}")
    print(f"GRAPH_EDGES={len(exit_rows)}")
    print(f"INVALID_DESTINATIONS={sum(1 for value in validation.values() if value[0].startswith('INVALID') or value[0].endswith('MISSING') or value[0].endswith('INACTIVE'))}")


if __name__ == "__main__":
    main()
