"""Deterministic acceptance gates for Fix #1D artifacts and registry."""
from __future__ import annotations

import csv
import hashlib
import re
from pathlib import Path


project = Path(__file__).resolve().parents[1]
workspace = project.parent
inventory_path = workspace / "analysis" / "repair" / "map_exit_trap_inventory.csv"
graph_path = workspace / "analysis" / "repair" / "phongthan_world_connection_graph.csv"
registry_path = project / "Deploy" / "ProjectContent" / "script" / "phongthan" / "mapexit" / "registry.tsv"
identity_path = project / "Headers" / "PhongThanMapIdentity.h"
activation_path = workspace / "PhongThanRuntime-Staging" / "Server" / "maps" / "WorldSet.ini"


def require(condition: bool, message: str) -> None:
    if not condition:
        raise AssertionError(message)


inventory = list(csv.DictReader(inventory_path.open(encoding="utf-8-sig")))
graph = list(csv.DictReader(graph_path.open(encoding="utf-8-sig")))
registry_lines = registry_path.read_text(encoding="ascii").splitlines()
registry = list(csv.DictReader(registry_lines, delimiter="\t"))

active_text = activation_path.read_bytes().decode("gbk")
active = {int(value) for value in re.findall(r"(?mi)^\s*World\d{3}\s*=\s*(\d+)\s*$", active_text)}
require(len(active) == 102, f"active worlds: {len(active)}")
require({int(row["RuntimeMapId"]) for row in inventory} <= active, "inventory contains an inactive source world")

unique_ids = {row["TrapIdHex"] for row in inventory}
require(len(unique_ids) == 189, f"unique Region_S IDs: {len(unique_ids)}")
require(all(row["Classification"] not in ("", "UNCLASSIFIED") for row in inventory), "unclassified trap ID")

registry_ids = [row["ScriptIdHex"] for row in registry]
require(len(registry_ids) == len(set(registry_ids)), "duplicate ScriptId registration")
mapping: dict[str, str] = {}
for row in registry:
    path = row["LoosePath"].lstrip("\\/").replace("\\", "/")
    require(row["ScriptIdHex"] not in mapping, "conflicting ScriptId mapping")
    mapping[row["ScriptIdHex"]] = path
    callback = project / "Deploy" / "ProjectContent" / Path(path)
    require(callback.is_file(), f"enabled callback missing: {path}")
    require(hashlib.sha256(callback.read_bytes()).hexdigest().upper() == row["Sha256"], f"callback hash mismatch: {path}")
    require(b"function main" in callback.read_bytes() or b"function  main" in callback.read_bytes(), f"callback has no main: {path}")

for required in ("0xCE741D0E", "0x381C646E", "0x5F310810"):
    require(required in mapping, f"required regression binding missing: {required}")

identity_text = identity_path.read_text(encoding="ascii", errors="ignore")
canonical = {
    int(pt): int(runtime)
    for pt, runtime in re.findall(r"\{\s*(\d+)\s*,\s*(\d+)\s*,\s*PHONGTHAN_MAP_[A-Z_]+\s*\}", identity_text)
}
for edge in graph:
    source_pt = int(edge["SourcePtMapId"])
    destination_pt = int(edge["DestinationPtMapId"])
    require(canonical.get(source_pt) == int(edge["SourceRuntimeMapId"]), "non-canonical source identity")
    require(canonical.get(destination_pt) == int(edge["DestinationRuntimeMapId"]), "non-canonical destination identity")
    require(int(edge["DestinationRuntimeMapId"]) in active, "inactive graph destination")
    require(edge["ValidationStatus"] == "VALIDATED", "unvalidated graph edge")
    graph_path = edge["CallbackPath"].lstrip("\\/").replace("\\", "/")
    require(graph_path == mapping[edge["TrapIdHex"]], "graph/registry path mismatch")
    require(edge["ReverseLinkStatus"] in ("CONFIRMED", "ONE_WAY_AUTHENTIC"), "invalid reverse-link classification")

require(len(graph) == 154, f"validated graph edges: {len(graph)}")
require(sum(1 for row in inventory if row["Classification"] == "UNRESOLVED") == 3, "unexpected unresolved row count")
require(all(row["LooseScriptStatus"] != "LOOSE_RESOLVED" for row in inventory if row["Classification"] == "UNRESOLVED"), "unresolved callback was enabled")

sort_source = (project / "Sources" / "Core" / "Src" / "KSortScript.cpp").read_bytes()
npc_source = (project / "Sources" / "Core" / "Src" / "KNpc.cpp").read_bytes()
require(b"PhongThanScriptAliasRegistry.inl" in sort_source, "startup registry loader missing")
require(b"registry.tsv" in sort_source, "startup registry path missing")
require(b"dwPhongThanSuperTrapId" not in npc_source, "legacy per-ID CheckTrap binding remains")
require(b"ExecuteScript(m_TrapScriptID" in npc_source, "normal trap dispatch missing")

print("WORLD_EXIT_NETWORK_TEST=PASS")
print(f"ACTIVE_WORLDS={len(active)}")
print(f"UNIQUE_TRAPS={len(unique_ids)}")
print(f"REGISTRY_BINDINGS={len(registry)}")
print(f"GRAPH_EDGES={len(graph)}")
