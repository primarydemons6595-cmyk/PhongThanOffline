# Player name / HP height after restoring a player body

Status: published to Game Loose; binary hash verified. Game.exe and CoreServer.dll unchanged; world PID 105212 continued without restart.

The reported symptom differs between observers: the local female character has a normal name/HP position, while another client draws her name/HP far above her body.

Confirmed source defect: `KNpc::LoadDataFromTemplate` copies NPC-template `m_nStature`. Both normal-player `KNpc::Load` and `ResetNpcTypeName(1)` previously restored player resources without clearing this value. `GetNpcPate()` then adds the stale NPC stature to the normal player head height. Client morph diagnostics contain female-template transitions `-2 -> 456 -> -2`. This establishes a concrete failing path, but the two-client screenshot has not been reproduced directly.

Fix: clear client-only `m_nStature` in both normal-player resource restoration paths. Keep genuine NPC/morph template heights, mount adjustments, and gameplay state unchanged.

Validation: CoreClient Release build passed. An extracted production `GetNpcPate()` regression harness passed 24 checks covering stale stature 100/200/300, both sexes, mounted/on-foot restoration, and retention until restoration. It demonstrates that restored head heights match freshly loaded player heights. No live two-client visual test yet.

Package: `Deploy/ProjectContent/PlayerNameHeight20261026`, CoreClient.dll only. Evidence and guarded publication script: `analysis/player_name_height_20261026`. Server restart is not needed.
