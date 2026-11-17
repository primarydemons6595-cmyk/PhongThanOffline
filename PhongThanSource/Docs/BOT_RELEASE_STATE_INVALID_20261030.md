# Release bot creation: `character_state_invalid`

User's Release runs on a separate server. No remote filesystem or logs were
available and no local game services were restarted for this fix.

The production fresh-bot path used `GetFileAttributesA` and Windows INI profile
APIs on `settings/npc/player/newplayeriniXX.ini`. Release stores these resources
inside authenticated PAKs. Missing loose files caused `NewState` to return false;
the caller displayed the generic `character_state_invalid` for every failure.

`PhongThanBotInitialState.h` now holds the exact production builder and loads
templates with Engine's `KIniFile`, supporting disk resources in Dev and PAK in
Release. All required stat keys must exist; invalid/failed reads cannot silently
generate a character using fabricated fallback stats. Identity, starter weapon,
spawn, state schema and normal atomic save remain unchanged.

The loader distinguishes template missing/invalid, read/write failures, invalid
size/checksum/schema and identity mismatches. It only creates a fresh state when
the file does not exist. Existing corrupt/mismatched saves are never deleted or
overwritten to hide an error.

Validation:

- `Build-PhongThan.ps1 -RuntimeProfile SecureRelease -Targets CoreServer`, full
  Core rebuild; zero errors, existing LNK4084 size warning. No Dev DLL deployed.
- `SecureBotCreationProbe.cpp` includes the production builder, uses the actual
  shipped Engine/Lua/zlib and authenticated PAKs in an isolated fixture. Six
  profession/sex combinations pass on Base and Base + 1.01 (12 cases), including
  `VyKemDau`; requires no loose templates and checks canonical template stats,
  name/account, profession/sex, revival/entry and one valid starter weapon.
- Fixture setup mounts beside its EXE (Engine mounts from executable directory)
  and includes zlib required for compressed entries. These are probe dependencies,
  not additional production changes; no Engine update is required by this fix.
- Installer tested on a disposable directory: correct hash, automatic original
  DLL backup, mutable bot/character sentinels preserved, corrupt payload rejected.
- ZIP contents rechecked against the DLL hash.

Delivery: `PhongThanRelease/Hotfix/Server-BotCreate-20261030.zip`, containing only
CoreServer.dll, its checksum manifest, installer and instructions. Apply on the
user's server after a normal GameServer save/stop; no client update, settings
extraction, roster deletion or character reset. The installer does not start,
stop or force-kill a game process. Actual remote character recovery remains to
be confirmed after the user applies it. Detailed verification is in
`analysis/release_bot_state/verification.json`.

This is a targeted native hotfix, not a claim that every runtime artifact was
recertified. Full Release publishing must still rebuild/certify its complete
source fingerprint. The existing certified-output identity was not forged or
updated to pretend a full Release build occurred.
