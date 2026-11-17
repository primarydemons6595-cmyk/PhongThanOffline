# Bot carry and stalled-objective recovery — 2026-10-30

The Lễ Quan incident was caused by two interacting paths: the starter reward
callback could grant hundreds of potion units without a carry check, and the
training loop returned immediately when overloaded. Its only recovery discarded
unneeded unlocked white equipment; a character carrying protected gift gear and
excess potions could remain blocked indefinitely.

## Changes

- `CanCarryNormalItem(genre, detail, particular, level, series, luck [, units [, binding]])`
  previews the real native template, releases it, and checks projected carry at
  80% of actual equipped capacity. The optional binding argument is 0..2, **not
  a percentage**. It never inserts inventory items or advances quest receipts.
- Bot starter gifts stop before exceeding that budget. Progress advances only
  after each successful insertion. The current level-gift-card integration and
  its issuance receipt are preserved; the card is checked separately and is not
  issued again when its receipt already exists. Human gift behavior is unchanged.
- Lễ Quan offers server bots an actual nearby buyback menu using normal `Sale(24)`.
  Bots select the issued callback and sell through native `SellItem`, including
  normal ownership, price, inventory-position and lock checks. They can also use
  an already registered nearby merchant. No remote shop state is fabricated.
- Under carry pressure, spare ordinary level-1 red/mana potions may be sold while
  retaining at least 10 health and 5 mana units across carried inventory. Native
  stack splitting preserves the reserve; the same-container hand placement is
  required by the ordinary item-move protocol. Rare equipment, useful gear,
  required quest items and locked medicine are excluded. Unneeded unlocked white
  gear is sold first when a nearby merchant is available, before the previously
  authorized emergency ground-drop fallback.
- Normal looting stops accumulating health stock at 20 units and mana stock at
  10 units. Ordinary potion pickup respects the 80% budget; urgent recovery and
  actual quest needs retain their separate priority.
- Nearby sales have a 20-second progress budget and a randomized 30–60 second
  retry delay. Failed gift callbacks yield other work for 3 minutes. Starter
  trial phases without actual task/kill-counter progress yield other work after
  3 minutes, retrying after a randomized 2–4 minutes. Existing story, Mao, daily,
  route-edge and shop budgets remain active.
- Bots pause autonomous work during a live trade/character lock. Unrecoverable
  carry blocks report `blocked_overload` and a bounded explicit console reason.

## Verification

`analysis/bot_aigent_20261029/test_carry_recovery.ps1` compiles VC6 native tests:

- actual Lễ Quan Lua plus the current level-card include: safe gift batches,
  exact partial resume, no duplicate gear/potions/card, card capacity/receipt,
  wrong-NPC/trade/lock guards and unchanged human menu;
- surplus-sale conservation for both recovery kinds, totals 1..500 and every
  possible carried stack, plus progress-budget expiry and tick-counter wrap;
- full 256×256 route cache validation, failed-edge alternatives and NPC leases;
- actual Mao/Thiếu Hạo callback and real 49-script story catalog regressions.

Core Server Release builds with zero errors and the existing LNK4084 image-size
warning. The verified DLL is staged separately to avoid publishing an untested
concurrent rebuild. Publication must recheck the actual current world and use
its graceful save event; startup does not create that event until initialization
finishes. Never force-kill a world to replace the DLL.

## Live validation still required

At preparation time the server was initially off, then a separate launch changed
the active GameServer PID while publication was being prepared. The starter gift
also changed from Xu to a level-gift card concurrently; the tests were updated
against the actual current scripts without reverting that change.

Do not call this deployed until runtime DLL SHA256 matches the verified build.
Do not call the stuck group recovered until native sale logs and live carry /
position changes confirm it. A bot far from every merchant, without safely
discardable gear, still needs a real escape/help mechanism; this change does not
invent teleportation or delete protected possessions. Full event/ticket/trade/
OTT/boss adapters remain separate unfinished work.


## 2026-10-30 08:42 (+07): carry recovery PUBLISHED and VERIFIED

User confirmed other gift/Vo Su changes finished and instructed continuation.
Preserved both. Saved/stopped164688 cleanly; published verified DLL
F882A4954CCF75D0C5CE1577E93CB49B33B9F23636F8DABEBEA8789CC266F109.
ConsoleHost144856 -> GameServer142980 READY. Event150864 launched Hidden;
Runningfalse->true CAS via authenticated API, preserving AI/memory/profiles;
revision104,52 online. Watcher46513 FINISHED four-minute observation.
ALL15 previously overloaded bots recovered under200 AND moved;0currently
onlineoverloaded. Real CARRY logs nativeSellItem+splits confirm. Hac419->163,
Lam363->133, Phong377->135; Hac traveled1007. Initialidle was randomizedrest;
all resumed without forcing their schedule. Detailed result:
analysis/bot_aigent_20261029/carry-recovery-result.json and live.json.
Publication/verification manifests now mark live validation true.
No pending publishers/watchers. Other event/trade/boss adapters remain incomplete.


## Recurring overload resolved ? 2026-10-30 11:19 (+07)

The user reported HacAnh9808 and QuanGamer00 stuck again. They were203/200 and
202/200 after real quest rewards (5town scrolls and a skillbook). Remaining spare
gear was ordinary blue quality1 with canonical known power0 and useful=false;
the old blanket rare protection prevented NPC sale. No locked/valuable gear was
removed and blue gear was NOT dropped on the ground.

Implemented for ALL bots:
- Allow only zero-power, non-useful, unupgraded/non-set ordinary blue gear for
  normal NPC sale; real ownership, sale/quest/lock checks stay active. Normal
  pickup no longer treats this category as an important item overriding80%carry.
- If blocked with no safe white drop or nearby merchant, use actually owned town
  scrolls via normal EatItem / travel Lua validation and charging. Spend only the
  calculated quantity needed to reachcapacity, at most3, and retain>=1 scroll;
  reject the plan if it cannot reachcapacity with that supply. Retry failures
  after60s. Never grant scrolls, bypass travel restrictions, or perform freewarp.
- Match shop bindings to the actual loaded NPC script, not obsolete aliases
  replaced by placeholder guidance dialogs. Register actual LeQuan metadata/
  issuedPTLeQuanSellSurplus callback. Restrict selection to this NPC's callback.
- Under carrypressure continue another sale trip after5s if surplus remains.
  After3actualpathfailures, unofferedcallback or expiry, exclude that NPC10min
  perbot and choose a different merchant after1s. Cache multiple badNPCs so two
  unavailable shops cannot cause an endlessalternation.

All4native suitesPASS (including new escape-count/reserve edgecases); final
Corebuild0errors,2existingwarnings (LNK4084image size, C4651PCH profilemacro).
Final runtime/staged DLLSHA256:
00C8B7BAE0B54E1D7933C6DEAA591C8CCFCFD7A254BE86E38988EC1564656843.
Saved/restartednormally; live world173792/ConsoleHost151216,Event150864,
configuration104Runningtrue,52configured bots online.

Real logs/snapshots proved:
- Quan used2ownedscrolls202->200, then sold6spareitems200->153, money7250->7625;
  resumedquests/training. Finalsnapshot159/200,money7700,map1005,travel_training.
- Hac used3ownedscrolls203->200, resumedcombat, then actual1007shopwasunreachable.
  Finalbuild loggedmerchant_path_unreachable_reselect11:15:48, selected another
  route11:15:50, reachedM?nhT?n1015 and sold5items11:17:38..48. Weight200->158,
  money7925->8175; resumedquest_travel and moved towardanotherportal.
- All8initiallylockedgiftitems percharacter are still owned. No individual profile,
  coordinates, quest flags or inventory records were manually patched.

Evidence: analysis/bot_aigent_20261029/recurring-overload-result.json,
recurring-overload-escape-result.json, recurring-overload-merchant-result.json,
carry-recovery-live.json and activity.log. Initial recovery of15bots fromtheearlier
startergiftincident remains valid but is separate evidence.

Limits: This resolves observed overload/shop-route cases; bots without a usable
escape item or safely releasable possessions may still report a carryblock.
Other event/trade/OTT/ticket/boss-assistance adapters remain unfinished. No claim
that every possible blocked task is now autonomous.
