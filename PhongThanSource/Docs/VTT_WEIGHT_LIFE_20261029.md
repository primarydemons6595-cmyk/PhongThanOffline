# Loose update — 2026-10-29

Published package: `Deploy/ProjectContent/VttWeightLife20261029` (29 files, including the client Engine dependency correction below).
Target: `PhongThanRuntime-Loose` only. This publication does not deploy to Release.
Backups and evidence: `analysis/vtt_weight_life_20261029`.

## Changes

- Thong Thien: dedicated enemy acquisition, original four-skill selection and chase logic for the four VTT boss templates. Automated production-code tests cover targeting, attacks, chase, cooldowns, peaceful players and animation completion. Actual combat against a logged-in player remains to be verified in game.
- VTT barriers: three close visual rows, corrected sprite anchors, smaller gate centered on the aperture, expanded collision cells, 64-bit barrier mask shared by client/server. Generated flood checks find no safe-area leaks on the four maps. Visual alignment still needs in-game review.
- Carry capacity: shared displayed/server capacity, stack weight, permanent bonus and equipped attribute 186. Overweight blocks walk/run/jump. Na Tra's exchange validates and charges all costs before increasing permanent capacity. Master reputation uses dedicated task 5350; no migration of an unidentified historical master-reputation field is claimed.
- Life books: original books 58/62 and 59/128 can be learned with duplicate/rollback handling. Sixteen recipe books validate dedicated life-profession rank and preserve the book on failure. The life-skill panel reads those ranks. Full profession training, rank advancement and crafting are not completed by this patch.
- Sieu Do: duplicate-death receipts distinguish each revived life of an NPC and each player; kills after revival can count inside the circle.
- Ke Tuc: missing `Task_51` title restored as `Kế Tục` in both ui3/ui4 taskinfo.ini. Existing progress records remain untouched. Reproducible update: `Tools/fix_ke_tuc_title_20261029.py`.

## Verification and publication

Dev CoreServer, CoreClient and Game builds succeeded. Boss selector, carry capacity, life books, life tokens and Lua kill receipts passed their focused harnesses. Arena tests: 133 passed; client barrier tests: 1854 passed. Generated SPR frames decode successfully.

World 18176 saved and stopped cleanly (exit 0). Nine saved character files were unchanged during copying. World restarted as PID 156344. Lua registry: 9249 loaded, 0 failed, alias registry PASS. All 28 deployed files match package SHA256 hashes. Client must be reopened to load updated binaries and task titles.

See `publication.json`, `restart.json`, `ke_tuc_title/publication.json`, build logs and test executables in the evidence directory. The main publication receipt contains the initial 26 files; the title receipt adds the two UI files. The client Engine receipt below adds the final dependency.

## Client entry point correction

The first publication omitted the matching client Engine.dll. CoreClient imports `PtReleaseEnabled` and `PtReleaseNextName` from Engine; the old Loose client Engine exported neither, causing the Windows entry-point error before client initialization. Updated only the Loose client's Engine from the current Dev build and added it to the package manifest (now 29 files). The build directory named `Engine/Release` is the compiler output location; the runtime profile is Dev and the Loose client retains `loose_only.flag`.

The new Engine retains all 1269 named exports of the old DLL. `Tools/verify_client_local_imports.py` checks all 27 adjacent PE images: 1406 local imports, zero unresolved symbols. Actual Loose Game.exe launch succeeded, PID 156396, responding window titled `Phong Than Online`; core initialization and script loading logs advanced. This verifies startup, not gameplay combat or account login. Backup, hashes and import report are in `analysis/vtt_weight_life_20261029/client_engine_fix`.
