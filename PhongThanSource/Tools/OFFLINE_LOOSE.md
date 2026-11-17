# Offline loose exporter

`build_offline_loose.py` builds a conservative content superset for the current
server and client. It does NOT patch startup, deploy binaries, copy accounts,
change source encoding, remove PAKs or certify complete gameplay restoration.

## Commands (workspace root)

```powershell
python PhongThanSource/Tests/test_offline_loose.py
python PhongThanSource/Tools/build_offline_loose.py --output analysis/repair/loose_plan_NEW
python PhongThanSource/Tools/build_offline_loose.py --output OfflineLoose_NEW --materialize
# Continue an interrupted export; hash-check existing bytes, refuse changed files:
python PhongThanSource/Tools/build_offline_loose.py --output OfflineLoose_NEW --materialize --resume
# Or export the exact reviewed plan (changed input bytes are blocked):
python PhongThanSource/Tools/build_offline_loose.py --from-plan analysis/repair/loose_plan_NEW --output OfflineLoose_FROM_PLAN_NEW
# Retry only frame-compressed sprites in an existing generated export:
python PhongThanSource/Tools/build_offline_loose.py --retry-frame-sprites --output OfflineLoose_EXISTING
# Byte-correct C literals / selected-data dependencies and historical recovery:
python PhongThanSource/Tools/complete_offline_loose.py --output OfflineLoose_EXISTING
python PhongThanSource/Tools/complete_offline_loose.py --output OfflineLoose_EXISTING --materialize
python PhongThanSource/Tools/complete_offline_loose.py --output OfflineLoose_EXISTING --retry
# Optional future ID store (DEFERRED; not the user's named-only workflow):
python PhongThanSource/Tools/build_loose_id_store.py --output OfflineLoose_EXISTING
```

The output directory must not exist and must not overlap source, original
client, recovery input, staging or persistent state. No deletion/overwrite mode.
Use `--runtime`, `--recovery`, `--workspace` for explicit alternate input roots.
Both roles are resolved independently using their actual `package.ini` and PAK
indices. Only content roots are exported, not executables, package.ini, logs,
database configuration, account data or CharacterStore.

## Selection and evidence

- Engine `KPakList::FindElemFile`: first package containing an ID wins.
  Native XPackFile::Open validates the entire archive. An invalid entry rejects
  the entire PAK (current ui.pak has ID0). Accepted archives use native binary
  search, including duplicate midpoint selection and unreachable unsorted IDs.
  Rejected packages are reported, never silently used as active content.
- Server `KCore.cpp`: PAK then loose. Existing loose files shadowed by a PAK are
  recorded, not silently copied over the winning PAK version.
- Client `PhongThanClient.cpp`: loose then PAK. Existing client overrides stay.
- The manifest uses EXACT_MAPPINGS paths only after verifying filename hash
  using signed ANSI bytes (ASCII case handling matches Engine). Original spelling
  and runtime output spelling are distinct; no blanket Unicode rename.
- Static absolute C/C++ literals add exact ASCII source-required names, enabling
  matching of previously unresolved IDs. Non-ASCII literals require existing
  proven mapping; formatted/macro-built/relative paths are not invented.
- Current runtime content roots add files absent from original recovery, e.g.
  authored NPC/spawn/UI compatibility content. File bytes are preserved.
- Decode actual selected PAK bytes: raw and bounded NRV2B supported. Historical
  cache payloads are not accepted as current-version proof: runtime PAK indices
  differ from recovery input. Frame-compressed SPR (0x11) is expanded using
  KSprite/XPackFile contracts: original header, palette and decoded frame bytes
  are preserved; the frame storage table becomes standard loose SPR offsets.
  Dimensions, palette, frame bounds and RLE are validated. Ambiguous indexed/RGB
  interpretation and unsupported compression fail closed. Known
  source names resolve IDs previously listed in04 directly against current PAKs.
- `03_RecoveredLoose` file bytes are NOT authoritative for version selection.
  Old or merged copies cannot override current package-chain precedence.
  Current PAK indices contain104898 entries while recovery inventory has109982;
  stale inventory entry indices/SHA must never replace directly decoded current data.

## Outputs

- `Server_manifest.csv`, `Client_manifest.csv`: output path, selected PAK/index/ID,
  origin, SHA256, size, status, whether runtime loose was shadowed.
- `*_blocked.csv`: missing payloads, unsafe/ambiguous encoding, decode failures.
- `*_unknown_ids.csv`: winning PAK IDs not covered by resolved output names.
- `*_skipped_packages.csv`: archives rejected by native Open validation.
- `*_source04_mappings.csv`: exact ASCII source filenames that identify historical
  unresolved04 IDs, with current selected PAK/index/SHA and old-versus-current
  payload comparison. Historical payloads are not silently copied as current.
  Historical packages are matched by filename ID, even when the winning version
  moved packages. Active runtime loose overrides are included and explicitly
  distinguished from current PAK payloads in the comparison.
- `source_references.csv`, `*_source_gaps.csv`: source literal evidence and gaps.
  These scan the shared tree conservatively for both roles; optional/debug/opposite
  build references may appear. They are not asserted to be actual live file opens.
- `mapping_rejected.csv`: invalid path/hash mappings in the recovery reports.
- `summary.json`: statistics and explicit startup blockers.
- With `--materialize`: verified `Server/` and `Client/` loose content trees.

## Limits / acceptance

Current user decision (2026-10-10): recover original/proven named paths ONLY;
do not modify loaders. The partial ID-addressed export and journal are deferred
under `completion/deferred_id_store/`, outside the named Server/Client roots.
Do not count/deploy that partial store. No production source was changed.

Final named merge and targeted historical inventory check:

```powershell
python PhongThanSource/Tools/finalize_named_loose.py --output OfflineLoose_20261009_final --materialize --verify-supplements
```

This retains base active-version reports, checks original inventory filenames
by exact ID and extracted SHA, refuses ambiguous historical versions/changed
outputs, and stream-hashes the new supplements without rechecking unchanged
base export. `--verify` optionally rechecks every merged named output. Reports are
`completion/*_named_effective_manifest.csv`, `*_named_unresolved.csv` and
`named_summary.json`. Current merged counts: Server115019 / Client84345;
1012 historical fallback paths per role. Remaining106 references per role
include4 directory references,101 absent file payloads and1 unsupported SPR.
Missing shared-source references include legacy/optional/non-live code paths;
this is not a claim of106 mandatory missing runtime files.

Final recheck: all1800 Server and1669 Client supplement files match manifest
SHA256. The repeated full-base hash scan was stopped without changing content;
unchanged base verification remains the earlier completed export/resume result.

The completion tools add reports under `completion/`, leaving the original
active-version manifests intact. `CURRENT_PAK_NAME_RECOVERED` means a proven
filename/hash against today's native winner. `HISTORICAL_EXACT_SHA_FALLBACK`
means verified recovery03 bytes filling an absent path, NOT a currently loaded
version. C literals are decoded as bytes (hex/octal escapes included); disk
aliases use the host Windows ANSI codepage, matching native fopen semantics.
Shared-source missing references are conservative, not proof of live necessity.

`_loose_id/XX/XXXXXXXX.bin` is an optional ID-addressed content store, NOT a
guessed original name or a PAK. It preserves native filename-ID routing and
captures data absent from filename recovery. Existing source still needs a
loader adapter to consume it. CSV journals support hash-verified resume. On
NTFS, verified identical outputs are hard-linked inside the export to avoid
duplicate data: editing an alias can change its linked ID copy. Copy the export
normally to make independent deployment files if that is unwanted. A READY
marker is emitted only for complete successful ID export; it is revoked before
revalidation. It does NOT certify startup/gameplay or original server logic.

Optional offline-only x64 decoder: `Build/Build-OfflineNrv2b.ps1`. This builds
`Tools/offline_nrv2b.dll`, not a game binary.100 real NRV entries matched the
independent Python path; malformed-input tests also pass. Missing/wrong-host
DLL falls back to Python. The C decoder bounds all input/output accesses and
uses no allocation; source encoding and game protocols are untouched.

Completed workspace export: `OfflineLoose_20261009_final` (2026-10-10).
Server113219 resolved/1106 blocked; Client82676 resolved/1106 blocked.
All4274 selectively retried server frame SPRs passed. Remaining known-name
blocks have no current PAK/runtime loose payload. Source04 mappings37 per role.
See summary/manifests rather than stale preliminary outputs. This completed
export operation is still NOT certification of full PAK-free startup/gameplay.

Decoded entries are capped at64MiB, stricter than the native256MiB per-frame
limit. Over-cap entries remain blocked. Selective frame retry preflights free
space, hashes outputs, refuses changed existing files and updates manifests;
it does not repeat the complete map export. It records the stored payload SHA
as well as the expanded loose SHA. It can resume completed identical outputs.
NRV backreferences use at most64KiB temporary slices, not offset-sized repeated
buffers. Memory exhaustion is recorded as a blocked entry rather than silently
accepting incomplete data. Resume verifies completed role manifests and rechecks
unfinished role inputs/outputs; it never overwrites changed existing output.

This is NOT a complete runtime dependency tracer: dynamic resource names, Lua
includes assembled at runtime, optional features and table-generated paths need
closure and separate offline runtime tests. Unknown IDs stay named by ID in the
report, not fabricated into filesystem names. Full original content is not the
same as complete original server logic.

Current GameClient directly requires the package registry and PAK Ui/Setting.ini;
CoreServer requires registry opening AND direct PAK itemversion.ini lookup;
KIniFile::LoadPakEntry also uses unsigned-ID PAK lookup. `pak_free_ready` stays false even if all
known names export successfully. Do not remove PAKs from staging. A future
explicit loose-only startup change needs isolated runtime acceptance, including
maps, character/monster sprites, skills, shop, sound, UI and Lua dependencies.
