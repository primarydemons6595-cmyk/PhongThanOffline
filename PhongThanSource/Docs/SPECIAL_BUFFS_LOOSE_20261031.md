# Linh Nguyen and Thanh Long Lenh — Loose only

Scope: source and `PhongThanRuntime-Loose`; no files deployed to `PhongThanRelease`.

The GM handbook entry 16226 now credits the existing IBBuff 418 counter directly. The original limit of 30 and pet-adoption eligibility remain enforced. Previously generated inventory items 8.418.0 can be used one at a time to credit the same counter; failed removal refunds the counter.

The F2 item is 8.7445.2. Its configured Lua use file was absent. The new callback and native handler activate one of the existing original IB buff rows: 7446 for physical professions or 7447 for spell professions. Five attributes and the one-day VN duration are generated from the current `settings/item/001/ibitem.txt` rows. This reconstructs the missing use behavior; it does not claim recovery of the original Lua script. Active variants cannot stack or consume another token. Full buff slots, locked items, trading and expiration reject use without consuming an item.

Buff identity and expiry use the existing persistent IBBuff task slots. Attribute application runs during the normal full player stat rebuild on both client and server. Relog restores the attributes, and expiry removes them through a full rebuild.

Validation passed:

- Dev build, including CoreServer and CoreClient.
- Native tests using extracted production persistence and eligibility functions and the actual new item/stat handlers: cap/adoption, legacy conversion, failed removal, four professions, all five attributes, no accumulation, relog, expiry, full slots and item guards.
- Actual engine Lua 4 parser: full staged handbook, quantities 1/10/30, unrelated menu/result delegation, and the Thanh Long use callback.
- Published four files with SHA256 verification using `Deploy/Publish-SpecialBuffs-Loose.ps1`; backups are under `analysis/buff_fix_20261031/backup_20261031_133313`.

The Loose server was already stopped. The Loose client was closed for replacing CoreClient.dll. No world startup or manual in-game visual verification was performed for this patch.
