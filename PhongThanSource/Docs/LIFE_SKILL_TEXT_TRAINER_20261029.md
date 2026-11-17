# Life skill text and trainer audit — 2026-10-29

## Display fix published to Loose

The life-skill panel passed original INI text directly to KWndText. Its local newline replacement looked for an actual newline between brackets, while the data contains the literal `<\n>` token. KWndText's automatic conversion only triggers on full `<color>` tags, so short `<c=yel>` tags also remained visible.

`Headers/PhongThanLifeText.h` now strips the leading localization marker and calls the existing Engine text encoder. UiLifeSkills passes the explicit encoded byte length to SetText, preserving zero-valued RGB bytes. The energy and strength captions use the same conversion to remove visible `$` markers. Values remain `--` because living energy/strength mechanics are not implemented.

Validation uses the actual Engine.dll encoder and all 29 original panel fields (nine names, eighteen descriptions, two resource captions), including newline counts, color controls and localization markers: 183 checks, zero failures. Dev GameClient build: zero errors and warnings. Local PE dependency audit: 1406 imports, zero missing symbols.

Closed the Loose client gracefully and replaced only its Game.exe; the running server was not restarted. Updated the Game.exe artifact/hash in VttWeightLife20261029. Evidence and previous executable: `analysis/life_ui_20261029`.

## Sinh Hoat Su is not a completed trainer

The three runtime NPCs at maps 1002, 1003 and 1004 still bind the authored guide scripts `npc_restore/1002_18.lua`, `1003_15.lua` and `1004_15.lua`. They do not execute a complete learning flow.

The NPC Lua library contains six hash-verified teacher reference scripts from the Phuc Hung and Tamhepc community PAKs. These are not marked ACTIVE_VNG_CHAIN or functionally reviewed. Their presence does not establish a complete original VNG implementation, nor coverage of all nine displayed professions.

Those scripts call AddLiveSkill and GetLiveSkillProficiency, neither of which is registered in the current server. GetLiveSkillLevel exists and reads the dedicated life-profession state. The earlier two living-book fixes and sixteen recipe-book checks therefore do not constitute a working trainer, profession progression or crafting backend. No unsupported teacher transaction was enabled during this display fix.

Machine-readable evidence: `analysis/life_ui_20261029/trainer-audit.json`.
