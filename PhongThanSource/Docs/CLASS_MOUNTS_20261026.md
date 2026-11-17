# Class cloak, wings, and named Dao Si mounts

Status: published and hash verified (21 files). Previous world saved and exited with code 0; world PID 114276 ready with 105 maps and 4,044 entities. Character files preserved. Live visual check after the fix remains pending.

User reports concern DaoGia and explicitly identifies Phi Tuoc, Bich Ngoc Tuoc, Van Trung Tuoc, Lam Gian Tuoc, novice Phi Tuoc, Kim Phung Hoang, Thanh Loan, Chu Tuoc, Tu Duc Minh, Bich Vu Minh Ton, the cranes, and Phi Dieu.

Confirmed defects:

- The compatibility tool substituted warrior cape SPRs into summoner resource tables. Runtime read-only inspection confirmed a female summoner with `jsf04_cl_rs0.spr` bound in part 9. Clear these table cells and suppress equipped-cloak composition for the four male/female sorcerer/summoner actor nodes. Warrior and ordinary NPC composition stays unchanged. The compatibility tool now prevents recurrence.
- Existing horse-item appearance keys select inconsistent or absent sprite rows. Stage 82 explicit reviewed record overrides, retaining every other override. Tu Duc Minh and its quest variant select ssx07; Bich Vu Minh Ton and its quest variant select ssx08. The named basic bird family selects ssx01, Phi Dieu selects ssx02, the two cranes select ssx03, and Kim Phung Hoang/Thanh Loan/Chu Tuoc select ssx05. Retain existing palette values for moved rows. These mappings are a repair based on user identification and inspected original model art, not a claim that the old generic HorsePart data matched these models.
- Male/female summoner wing rows 7 and 8 used yrx06 despite exact original yrx07/08 assets already existing in Loose. Reconnect each row's hh/ht layers and refresh their metadata. The sixteen SPR hashes match the highest-priority update0_171-171.pak entries.

Important audit correction: blank middle wing layers are not sufficient proof of a missing wing. The provided package has no hb sprites for many summoner wing rows; hh/ht already form the two wings. Keep intentional missing components empty. Read-only inspection of the live female summoner found both hh/ht bound and present in the draw order; its current model composed with both wings offline. Thus the exact reported transient one-sided symptom has not yet been reproduced live. Do not fabricate a third component or duplicate a whole mount into an empty part.

Evidence: analysis/class_mounts_20261026 includes original-table proofs, shared-wing proofs, read-only live binding log, model contact sheets, production-method native tests, and package manifest. CoreClient build passed; 126 extracted production mapping/cloak checks passed. All 1,248 referenced mounted-action SPR cells resolve to valid assets with eight directional groups; all 21 packaged file hashes were verified. Original assets were decoded and visually inspected. A live post-fix class/sex/direction check is still required.

Package: Deploy/ProjectContent/ClassMounts20261026. Includes CoreClient.dll, selected client resource tables, and paired client/server PhongThanHorseVisual.txt. CoreServer.dll and Game.exe are unchanged. Server mapping cache currently initializes at world start, so publication requires a clean world restart after character logout. Existing per-character equipment sets are untouched.
