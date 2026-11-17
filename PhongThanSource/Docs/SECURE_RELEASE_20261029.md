# Loose development and secure release

The shared native source supports two explicit runtime profiles. `Dev` is the
default and preserves loose resources and diagnostic logs. `SecureRelease`
requires the signed encrypted PAK catalog and suppresses diagnostic files.
Changing profiles forces a native rebuild to avoid mixing PCH/OBJ files. Builds
write to Source/Output; certified secure binaries are retained separately in
Source/Output/SecureRelease and do not overwrite the deployed Loose runtime.

## Studio workflow

Use `PhongThanRuntime-Loose/ManagerStudio/Release/PhongThanStudio.exe`, tab
`PAK / Release`. Keep editing and testing the Client/Server Loose trees.
Initialize the publisher identity once, build the Release runtime, then create
the Client and Server Bases. A Base is immutable; the tool refuses to overwrite
an existing Base. Select a side, compare Loose with Release, and create the next
patch after changes. Unchanged inputs return `NO_CHANGES` without consuming a
version folder.

```
PhongThanRelease/
  Client Base/                  # initial release, version 1.00
  Server Base/
  Update/
    Client 1.01/                # created only when there are changes
    Server 1.01/
    Client 1.02/
```

Copy the **contents** of each matching update folder over the installed Client
or Server, in version order. Retain all predecessor PAKs; each patch includes the
new signed Data/release.catalog. Copy all files in the patch, including that
catalog and any updated executables/configuration. Close the Client or stop the
Server before applying a patch and restart afterwards. Mounts are fixed for the
process lifetime. Copy/paste is not an atomic live deployment; a future updater
must stage and verify a complete update before committing it. `Update.exe` is
not implemented in this change.

Patches contain added and modified resources, deletion tombstones, and changed
allowlisted runtime/operational files. Effective state includes earlier patches,
so a later scan does not repack changes already published. The catalog pins the
ordered complete chain; missing, modified, incorrectly ordered, or wrong-role
packages are rejected. Newer entries override earlier entries. Real legacy hash
collisions retain path-specific identities. Do not manually rename PAKs or mix
Client and Server packages.

CharacterStore, drop serial reservations, access tokens, and live service state
are excluded from patches. A new Server Base initializes fresh persistence;
never replace a running server's state with a fresh Base. Use Start-Server.cmd
and Stop-Server.cmd in Server Base. World startup can take several minutes.
Static gameplay tables remain edited in Loose then published as a patch.

## Format and protection

PTSEC002 uses a signed encrypted manifest and independently authenticated
256 KiB chunks, optionally compressed. Public resource IDs and package names do
not reveal original paths. Each archive gets a derived AES-256 key from a durably
reserved unique archive ID. GCM nonces use separate monotonically numbered
manifest/chunk counters under that archive key. Authentication completes before
plaintext is exposed; chunk hashes inside the signed manifest also prevent a
holder of the data key from signing replacement content. ECDSA publication keys
are separate from encryption keys. Only the public verification key and runtime
data key are compiled into the game; the private signing key stays with Studio.

Runtime reads decrypt in memory, cache at most one chunk per open virtual stream,
and wipe transient chunk buffers. No directory of decrypted resources is
created. Gameplay parsers may retain the data they actively need. Protection
against legacy unpackers does not prevent an attacker from reverse engineering
the executable or inspecting memory on a machine they control.

Back up the complete `ManagerStudio/ReleasePrivate` publisher identity and
archive counter securely. DPAPI ties the key bundle to the publishing Windows
identity. Do not reset the counter or restore an older counter under the same
key. Moving publishing to another machine requires deliberate key migration;
copying the DPAPI file alone is not a migration mechanism. Private publisher
files and the generated key header do not belong in a distributable.

## Package layout and measurements

The current Base uses 19 Client PAKs (including Data/serverlist.pak) and 3 Server
PAKs. Related resource groups are split at 512 MiB of input data per part. There
is one combined lookup table after mount, rather than searching every PAK on
each read. Patches append new files and do not rewrite Base archives.

Tests read 2,000 resource paths (6,895,108 returned bytes).
Both single and multiple package layouts had zero read failures and similar
read latency. Final warm runs measured 282–290 ms of reads for the single PAK
layout and 276–286 ms for multiple PAKs. Peak working memory was roughly 56 MiB
versus 81 MiB. Mount was about 423–433 ms and 427–439 ms respectively; one
hundred tiny synthetic patches mounted in 494–500 ms. First runs were slower
and excluded from these warm ranges. These are engineering samples,
not a cold-start, FPS, or original VNG performance comparison. Evidence lives
under analysis/secure_release_20261028; rerun on target hardware for deployment
sizing. Consolidating old patches into a new Base is a future maintenance step.

## Validation and character creation fix

24 managed secure-package/pipeline checks, 13 optimized VC6 reader checks, and
194 Studio checks pass. Existing Kingsoft extraction tools can extract their
original PAK positive control but do not extract plaintext from PTSEC002.
The multi-PAK World smoke test loaded 105 maps and stopped with exit code 0,
without root diagnostic .log/.jsonl files.

The Release character creation failure came from Bishop reading newplayerini
templates with Windows profile APIs, and the shared spawn resolver reading
revivepos.ini from disk. Those resources live inside PAK in Release. Release now
uses the authenticated resource reader and initializes Bishop's Engine resource
root. Dev retains its existing disk profile reader. A native production-code
probe checks all 3 professions x 2 genders and original spawn coordinates. A
real Goddess service probe creates, rejects duplicate names, lists persisted
characters, then deletes its own test characters for all six combinations.
Client error handling now reserves the name error for invalid/duplicate names;
other failures use the service error path.

Both full native builds (19 targets per profile) pass. The Dev profile also
passes the same six character-template/spawn cases against the unchanged Loose
settings. The default build stamp is left at Dev; no newly built native binaries
were deployed into Loose. The Release Event service returned all 18 events from
its authenticated status endpoint and created no diagnostic files in its root.

Full interactive client gameplay is not yet certified by these probes. The
headless client startup experiment reached UiStart but stalled in the legacy
sprite drawing loop; treat that as an outstanding validation limitation.

## 2026-10-30: prevent omitted Event updates from stale build output

Runtime files come from the certified SecureRelease output, not deployed Dev
binaries. Server 1.01 was generated using the older certified Event/Core DLLs,
which matched the Base; newer DLLs in Loose were therefore absent from the
patch. Resource hashing counts are inventory progress, not the number of files
that a delta must contain.

Build-SecureRelease now records a SHA256 identity of native and managed build
sources, headers, project files and build scripts. Base/Patch and Studio Check
reject missing or stale source identities and instruct the publisher to build
runtime Release. Source changes during a Release build also reject publication.
Legacy certificates require one new Release build. No Dev binaries are copied
automatically into the encrypted Release.

Studio Check now previews runtime/operational differences alongside resources.
Patch output logs each ADD/CHANGE/UNCHANGED runtime file. Thirty secure pipeline
checks pass, including stale Event source, changed shared Studio.Core, legacy
certificates, preview without publishing, and Event dependency inclusion.
The existing Server 1.01 is preserved. After building runtime Release again,
create the next Server patch and apply both versions in order.
