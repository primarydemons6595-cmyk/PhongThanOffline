# Beginner gift policy, 2026-10-16

Canonical scripts live in `Deploy/ProjectContent/script/player/playerlogin.lua`
and `Deploy/ProjectContent/script/item/camnangtanthu.lua`. They are published to
Server and Client in both PhongThanRuntime-Loose and PhongThanRuntime-Staging.
Republish with `node PhongThanSource/Deploy/Publish-BeginnerGift.mjs` from the
workspace root. Each publication saves the previous bytes under
`analysis/repair/beginner-gift-<UTC timestamp>`.

The first original backup is `analysis/repair/beginner-gift-2026-10-16T07-03-02-452Z`.

Only an unissued level-1 character gets a bag at login. Level >1 never receives
a new bag. An issued bag opens at level >=10, once per character. A legacy or
transferred guide without the new issue task does not authorize rewards.
The bag is MagicScript 9173; native inventory identity is genre 6/detail 9173,
while the original VNG Lua tuple is `(6,1,9173,1,0,0)`.

Per user choice, the available level-20 quest sets replace the originally
requested level-10 sets. All five pieces are included: armor, helmet, belt,
boots, cape/token. The armor requires level 20; the other pieces retain their
original table requirements. Opening the bag at 10 does not bypass those.

| GetSeries career | Quest set | Quest weapon(s), level requirement 10 | Gold mount |
|---|---|---|---|
| 0, Giap Si | Cu Dau, particular 6 | Tram Tuong dao 27; Tam Tiem dao 28 | Trac Ma 24 |
| 1, Dao Si | Van Trung, particular 7 | Tich Lich kiem 29 | Trac Tuoc 25 |
| 2, Di Nhan | Khuyen Van, particular 8 | Do The phu 30 | Trac Diep 26 |

Everyone also gets 100 small red potions, 100 small blue potions and 100 copper
cash through AddCopperCash (the copper wallet, not Earn/silver). Gold mounts
are bound on creation. Quest equipment keeps its original no-trade/no-sale
template flags and is additionally bound on creation.

Important source contracts:

- Use GetSeries for career, not GetFaction. Equipment Series is a different
  field: quest sets use 1/6/11. Weapon/mount template marker 1000 resolves to
  neutral series 5; passing 1000 to AddNormalItem fails identity validation.
- Task 4700: 0 unissued, 1 issued, 2 completed. Task 4701: career+1. Task 4702:
  delivered reward-unit count, including the final copper step. These project
  tasks avoid original VNG 252/253 and native 4800-4999 reservations.
- QuestExchange issues the bag and task together. Rewards use registered
  AddNormalItem (seventh argument binds), AddNormalItemPile and AddCopperCash.
  No invented PTStarterGetFaction/PTStarterGrantAtomic hooks remain.
- The claim is deliberately resumable, not a single native transaction.
  Each successful item advances saved progress; a failure leaves the bag and
  permits only the missing rewards on retry. Career cannot change mid-claim.
  Bag consumption occurs after every reward, with a retry guard for copper.
  This tests callback/relogin-style retry behavior, not crash-durable atomicity
  during an asynchronous database write.
- Old Staging login dependencies on absent custom IP/admin scripts and the
  account-123456 test rewards were removed. Login now runs the gift policy.
- Existing guide IDs 4813/9500 and the separate native developer item picker
  61000 are not migrated into eligible newcomer gifts.

Validation passed:

- `Tests/Native/BeginnerGiftLuaTests.cpp` with the real engine Lua 4 VM and
  mocked game APIs: levels 0/1/2 for issue, 9/10/20 for opening, all careers,
  invalid/missing bag, callback rechecks, binding arguments, duplicate claims,
  generation/bag-full partial retry, potion retry, career change, wallet and
  bag-consume failure retries. Does not simulate a live network session.
- `node PhongThanSource/Tests/test-beginner-gift-catalog.mjs`: 92 template
  checks, four copies in sync, quest flags, bag flags, direct task collisions.
- `Tests/Native/BeginnerGiftTableTests.cpp`: engine disk-first loading with
  PAK fallback finds both stack-100 potion identities and bag 9173 in each
  runtime Server/Client root. Staging potion tables are read from PAK.

Restart the server and client to reload scripts/catalog. These are loose
content updates; deployments prioritizing PAK must publish the changed content
to their package chain or use the existing disk-first/Loose content mode.
No C++ gameplay change or production binary replacement was made. Live in-game
acceptance remains to be run with new characters in each career.
