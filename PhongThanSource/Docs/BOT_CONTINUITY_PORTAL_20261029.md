# Bot combat continuity and Bắc Hải portal alignment — 2026-10-29

The same server controller applies to the complete bot population. Per-character snapshots are diagnostic evidence, not individual behavior overrides.

## Combat and movement

- Attack approach now uses the actual selected skill's `CanCastSkill` result and attack radius. A queued target no longer automatically means “attacking”; unavailable/rejected attacks report `combat_blocked`.
- Nearby party members may fight independently. Regroup distance increased to prevent the captain-follow branch repeatedly cancelling combat approach.
- Scheduled rest clears combat targets before party/quest decisions, preventing stale target reservations and movement during rest.
- An active quest may require a lower-level monster; its target is exempt from the normal low-level training cutoff, while the upper danger cutoff remains. The level-11 character with task 993=259 was otherwise unable to select its level-5 quest target.
- Four bounded route searches are shared fairly per second. An explicitly opted-in partial A* route follows only verified cells and replans from its reached endpoint. It never issues a direct run across an unsearched gap. Ordinary route callers retain full-path behavior.
- Portal approaches resolve actual traversable trap cells around approximate UI route markers. A clear path to a marker alone does not establish successful map transfer.
- The normal NPC death dispatcher now accepts a loaded server bot as a valid owner despite its deliberately negative connection index. Expected role identity, live actor and same-map checks remain. Quest progress is still changed by the real Lua death callback, not by bot code.
- `MobKills` counts actual direct monster-death callbacks for each bot in the current server session. Studio displays “Quái hạ (phiên)”; diagnosis snapshots include actual skill acceptance, target life, experience and task progress.

An observation window over all 52 bots after the shared corrections recorded 145 confirmed monster kills and movement/level/experience/kill progress for 47 bots. It also identified a remaining stagnant beginner at the Bắc Hải gate; this was investigated separately rather than treated as success for every bot.

## Bắc Hải → Sùng Thành Đại Doanh

The old `0x066A79DB` trigger occupied cells around 1922,2783 and 1925–1927,2783 in the server's `v_086/120_region_s.dat`. The visible transition ring is in the client's `v_087/120_region_c.dat`; its position evidence and the live nearby player's position showed the activation zone was offset from the visible ring.

The correction removes those old trigger records and adds the same script ID across 60 cells at the ring in `maps/±±º£/v_087/120_region_s.dat`. It preserves the existing callback and its destination `NewWorld(2,1547,3121)`. Obstacle, NPC and object payloads remain byte-identical. The runtime map loader's existing `_S`/unsuffixed lookup is respected. Canonical publication copies are under `Deploy/ProjectContent/maps`.

The client ui3/ui4 route marker and their canonical copies now use cell 1930,2788 (MPS 61776,89232), inside the activation zone. Bot topology is regenerated from that same route file. No client executable or sprite was changed.

Evidence and backups:

- `analysis/bot_aigent_20261029/bac_hai_portal_alignment.json`: original and corrected region hashes, backups and footprint evidence.
- `test_bac_hai_portal.cjs`: validates both mounted map layouts, region section boundaries, removal of old cells, new route/visual/player inclusion and unchanged non-trap payloads.
- `live_bac_hai_crossing.json`: live observation, when generated. A static data check alone does not establish the player's successful passage.

## Studio and speech

Studio was updated after its older `BotLive` model rejected the new `Sleeping` field. Additive read-only diagnostics may now be ignored by older-compatible dashboard readers; persisted configuration remains strict. The application is reopened visibly and close guards are respected.

Local speech uses fresh activity, canonical map names, brief colloquial instructions and paired history. Repeated copies of the user's message or the bot's last replies are retried once. Model-generated action intents still pass through authoritative game handlers. These changes do not imply flawless human-like language, full negotiated commerce, or every crafting adapter is complete.

Validation includes native route/blocked-corner/partial-route/real-trigger selection tests, 83 real Lua 4 death-progress assertions (including server bots, stale identities and unloaded players), managed social/history/dashboard checks and native public-channel checks. The native build retains its existing large-image linker warning.
