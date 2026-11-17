# NPC drop — NEW_PROJECT_RULES_V1

This is a new project rule set explicitly requested by the user, NOT recovered
original PT probabilities, MagicRate semantics, or DropRateFile1/2/3 selection.
Original NPC/drop/item resources are unchanged. This policy uses real native
item tuples and the existing object/pickup protocol.

## Effective file / Studio

Server only: `settings/phongthan/NpcDropPolicy.ini` (must be in loose_index.bin).
Studio: **Quản lý Server → Rớt đồ quái/Boss**, a separate tab. Load, edit ASCII
policy, validate probabilities and template IDs, stage, inspect Client/Server
Sync patch, Apply with backups while runtime is closed. No Client policy copy.
Native server reads this small file at each eligible death: edits to an existing
indexed policy take effect on subsequent deaths. New paths still need index
registration; Studio handles this. Restart needed for binary changes, not policy.

## Rule contract

- Main: Version=1, Enabled=0/1, DefaultTable=<ASCII name>.
- `[Template_N]`: optional native zero-based NPC template ID, Enabled=0/1,
  Table=<name>. Missing Enabled means enabled; missing/empty Table uses default.
  Empty default leaves unbound templates without generic drops.
- `[Table_name]`: Draws=1..10, Range=1..100000000, Count=1..128.
- `[name_1]` .. `[name_Count]`: Genre, Detail, Particular, Level, Series, Luck,
  Weight, Quantity are all required integer fields.
- Name: 1..31 ASCII letters/digits/underscore, case-sensitive.
- Every draw rolls in `[0,Range)`, selects one entry cumulatively. Probability
  is Weight/Range. Sum weights must not exceed Range; leftover means no drop.
  Weight=0 never wins. Draws are independent, repeated entries can win.
- Quantity=1..20 emits separate real pickup objects, NOT a stacked inventory
  grant. Maximum 200 object attempts/death. No fake item on generation failure.
- Genres 0 equipment, 1 medicine, 3 material, 4 quest, 6 recipe, 7 skillbook,
  8 IB item use the existing `PhongThanQuestItemTuple` native resolver. Level and
  Series must match the generated identity: materials/quests/recipes use native
  Level=0; medicine/equipment require positive Level. Series=0 is unrestricted
  query semantics; positive Series must match. Luck=0 only, no synthesized random
  affix/+12/set item. Unsupported/invalid identities fail closed.
- This path applies only to kind_normal deaths with valid connected player
  ownership on the same subworld. Summons return before the hook. Existing
  quest/event death scripts are still separate and may produce additional drops.
- No global boss inference from display name. Give each boss its own explicit
  Template_N binding if different loot/draw counts are wanted.

## Initial example (new tuning, not historical PT balance)

Two draws: Tiểu Hồng đơn `(1,0,0,1,0,0)` 12%/draw, Tiểu Hoàn đơn
`(1,3,0,1,0,0)` 8%, Quẻ Khôn `(3,15,0,0,0,0)` 5%, nothing 75%.
Thus generic objects/death: zero 56.25%, one 37.5%, two 6.25%. This deliberately
small starter policy is NOT a complete reconstructed boss equipment loot table.
Edit/add tables and bindings in Studio; do not alter original PAK-derived INIs
to pretend their scale semantics have been proven.

## Fake corpse fix

2402/2703 deployed NPC templates have CorpseIdx=14, while current ObjData
template14 is Item `# Khôi Giáp Sĩ`. Client `KNpc::OnDeath` previously created
one client-only object with world ID0 and item ID0 for this corpse. It now creates
a corpse only when the configured object kind is exactly Body. No renumbering,
replacement corpse sprite, geometry, actual loot or pickup behavior is invented.

Diagnostics: Server/npc_drop_trace.log (PLAN, NO_DROP, DROP_OBJECT, *_REJECT).
Build tests: Test-NpcDropRules.ps1; Test-NpcDropLooseContract.ps1.
Manual acceptance still requires real Game.exe kill/pickup, not mocked tests.
