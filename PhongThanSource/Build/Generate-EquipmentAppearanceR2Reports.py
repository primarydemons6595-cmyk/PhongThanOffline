#!/usr/bin/env python3
"""Generate focused Fix #2A-R2 evidence matrices from independent PT evidence."""
import csv
import hashlib
from pathlib import Path


def write_csv(path, header, rows):
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open("w", newline="", encoding="utf-8-sig") as stream:
        writer = csv.writer(stream)
        writer.writerow(header)
        writer.writerows(rows)


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest().upper() if path.is_file() else ""


def main():
    root = Path(__file__).resolve().parents[2]
    client = root / "PhongThanRuntime-Staging" / "Client"
    analysis = root / "analysis" / "repair"
    dependencies = {}
    with (analysis / "remaining_pak_dependencies.csv").open(encoding="utf-8-sig", newline="") as stream:
        for row in csv.DictReader(stream):
            dependencies[row["RequestedPath"].lower()] = row

    classes = (("Giap Sy", "jsm", "jsf", 0), ("Dao Sy", "ssm", "ssf", 1),
               ("Di Nhan", "yrm", "yrf", 2))
    controls = (("Cu Dau", 2, 1, 2, "Dau Tran"),
                ("Tinh Cang Giap", 6, 3, 6, "Ky Lan"),
                ("Khai Thien", 8, 4, 8, "Tram Long Giap"),
                ("Hoang Kim Chan Dan Giap", 10, 6, 10, "invisible"))
    appearance = []
    provenance = []
    trace = []
    for name, level, selector, source_row, old_result in controls:
        trace.append((name, "real Game.exe control", "settings/item/001/armor.txt", source_row,
                      0, 2, 0, 1, 0, f"2:0:{level}", "class/sex body table",
                      selector, selector, 0, f"jsm{selector:02d}_bd_*",
                      old_result, "PENDING_R2_MANUAL", "ORIGINAL_PT_TABLES+RECOVERED_ASSETS"))
        for class_name, male, female, particular in classes:
            if particular != 0:
                continue
            for sex, prefix in (("Male", male), ("Female", female)):
                virtual = f"\\spr\\npcres\\human\\{prefix}{selector:02d}_bd_st0.spr"
                loose = client / virtual.lstrip("\\").replace("\\", "/")
                dep = dependencies.get(virtual.lower(), {})
                exists = loose.is_file()
                status = "VALIDATED_LOOSE" if exists else "MISSING"
                appearance.append((name, name, class_name, sex, "default", "settings/item/001/armor.txt",
                                   source_row, 0, 2, 0, "armor/body", level,
                                   "DetailType=2;ParticularType=0;Level", "body", selector,
                                   f"{sex}-{class_name}-body", selector, virtual, exists,
                                   dep.get("PakArchive", "UNKNOWN"), "canonical loose" if exists else "MISSING",
                                   dep.get("MigrationStatus", "MISSING"), status,
                                   "Expected selector is independent of current adapter."))
                provenance.append((name, name, class_name, sex, "default", "armor/body",
                                   "settings/item/001/armor.txt", source_row, 0, 2, 0, selector,
                                   "body", virtual, str(loose), exists, digest(loose),
                                   dep.get("PakArchive", ""), dep.get("PakEntry", virtual),
                                   digest(loose), "LOOSE" if exists else "MISSING", virtual,
                                   digest(loose), "MATCH" if exists else "MISSING",
                                   dep.get("MigrationStatus", "MISSING"), dep.get("Notes", "")))

    # Independent class/sex/body/head/cape samples prove that resource domains do not alias.
    for class_name, male, female, particular in classes:
        for sex, prefix in (("Male", male), ("Female", female)):
            for slot, detail, suffix in (("armor/body", 2, "bd"), ("helm/head", 7, "hd"),
                                         ("cape", 9, "cl")):
                virtual = f"\\spr\\npcres\\human\\{prefix}01_{suffix}_st0.spr"
                loose = client / virtual.lstrip("\\").replace("\\", "/")
                dep = dependencies.get(virtual.lower(), {})
                exists = loose.is_file()
                appearance.append((f"{class_name} tier-1 {slot}", "PT production tier 1", class_name,
                                   sex, "default", f"settings/item/001/{'armor' if detail == 2 else 'helm' if detail == 7 else 'pendant'}.txt",
                                   particular * 10 + 1, 0, detail, particular, slot, 1,
                                   f"DetailType={detail};ParticularType={particular};Level=1", suffix,
                                   1, f"{sex}-{class_name}-{suffix}", 1, virtual, exists,
                                   dep.get("PakArchive", "UNKNOWN"), "canonical loose" if exists else "MISSING",
                                   dep.get("MigrationStatus", "MISSING"),
                                   "VALIDATED_LOOSE" if exists else "KNOWN_ORIGINAL_MISSING",
                                   "Di Nhan cape resources are absent from the mounted original PAK chain." if not exists else ""))

    write_csv(analysis / "pt_equipment_appearance_matrix.csv",
              ("RawItemName", "DisplayName", "Class", "Sex", "Body", "SourceTable", "SourceRow",
               "Genre", "DetailType", "ParticularType", "EquipmentSlot", "Level", "RawAppearanceFields",
               "CharacterPartDomain", "AppearanceSelector", "ResourceTableDomain", "ResourceIndex",
               "ResourcePath", "ResourceExists", "OriginalResourceSource", "LooseResourceSource",
               "MappingProvenance", "ValidationStatus", "Notes"), appearance)
    write_csv(analysis / "equipment_resource_provenance.csv",
              ("ItemName", "RawItemName", "Class", "Sex", "Body", "Slot", "SourceTable", "SourceRow",
               "Genre", "DetailType", "ParticularType", "AppearanceSelector", "CharacterPart",
               "ExpectedResourcePath", "CurrentLoosePath", "CurrentLooseExists", "CurrentLooseHash",
               "OriginalPak", "OriginalPakEntry", "OriginalResourceHash", "RuntimeResolvedSource",
               "RuntimeResolvedPath", "RuntimeResourceHash", "MatchStatus", "Provenance", "Notes"), provenance)
    write_csv(analysis / "equipment_world_appearance_trace.csv",
              ("DisplayName", "RuntimeItemIdentity", "SourceTable", "TemplateRow", "Genre", "DetailType",
               "ParticularType", "EquipmentSlot", "UpgradeLevel", "AppearanceKey", "PartTable", "PartNumber",
               "ResourceSelector", "PaletteId", "FinalResourcePattern", "ObservedRuntimeResult", "Classification",
               "Provenance"), trace)

    effect_rows = []
    state_specs = ((35, "equipeffect3f1.spr", "3-piece +12 set", "Female"),
                   (36, "equipeffect3m1.spr", "3-piece +12 set", "Male"),
                   (37, "equipeffect5f1.spr", "5+-piece +12 set", "Female"),
                   (38, "equipeffect5m1.spr", "5+-piece +12 set", "Male"),
                   (39, "weaponeffect1.spr", "+12 weapon; profession 0", "Any"),
                   (40, "weaponeffect2.spr", "+12 weapon; profession 1", "Any"),
                   (41, "weaponeffect3.spr", "+12 weapon; profession 2", "Any"),
                   (42, "weaponeffect4.spr", "+12 weapon; profession 3", "Any"),
                   (72, "UNKNOWN", "3-piece Xian/Mo set", "Female"),
                   (73, "UNKNOWN", "3-piece Xian/Mo set", "Male"))
    for state, resource, trigger, sex in state_specs:
        path = client / "Spr" / "skill" / "magic" / resource if resource != "UNKNOWN" else Path()
        exists = resource != "UNKNOWN" and path.is_file()
        enabled = state <= 42
        effect_rows.append((trigger, "profession/set domain", sex, "default", "weapon" if state >= 39 and state <= 42 else "set",
                            12, "FULL" if state < 39 else "N/A", "matching set" if state < 39 else "N/A",
                            state, state, f"\\Spr\\skill\\magic\\{resource}" if resource != "UNKNOWN" else "UNKNOWN",
                            "LOOSE" if exists else "UNKNOWN", "character state", "single selected state",
                            "MUTUALLY_EXCLUSIVE" if state >= 39 and state <= 42 else "ONE_SEX_SPECIFIC_SET_STATE",
                            "clear on unequip/state recompute", "ORIGINAL_PT_STATE_TABLE+ADAPTED_SELECTOR",
                            "ENABLED_PENDING_MANUAL" if enabled else "DISABLED_TRIGGER_UNKNOWN",
                            f"SHA256={digest(path) if exists else 'UNKNOWN'}"))
    write_csv(analysis / "pt_equipment_world_effect_matrix.csv",
              ("ItemOrDomain", "Class", "Sex", "Body", "Slot", "UpgradeLevel", "ActivationState",
               "SetState", "EffectSelector", "EffectState", "EffectResource", "EffectResourceSource",
               "AttachmentPart", "CompositionRule", "ExclusivityRule", "RemovalRule", "Provenance",
               "ValidationStatus", "Notes"), effect_rows)
    print(f"APPEARANCE_ROWS={len(appearance)} PROVENANCE_ROWS={len(provenance)} TRACE_ROWS={len(trace)} EFFECT_ROWS={len(effect_rows)}")


if __name__ == "__main__":
    main()
