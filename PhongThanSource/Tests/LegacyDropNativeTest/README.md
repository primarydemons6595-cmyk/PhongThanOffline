# Legacy drop native preflight

Update 2026-10-18: the user has authorized a separate, opt-in Loose experiment.
Read `analysis/legacy_drop_20261018_live/LIVE_TEST_REPORT.md` and
`LIVE_TEST_GUIDE.md`. OFF is the default; the production gate below stays closed.
Use `Test-LivePolicy.ps1`, `Test-QuestRegression.ps1` and the native log analyzer
for the added runtime path. Player pickup/quest runtime evidence is separate from
the unowned in-process GameServer spawn/death/ground self-test.

Current acceptance is **BLOCKED**, not a production drop implementation. Read
`analysis/legacy_drop_20261017/phase2/PHASE2_RESOLUTION_REPORT.md` from workspace root.

For the focused execution-policy investigation, read
`analysis/legacy_drop_20261017/LEGACY_DROP_EXECUTION_SPEC.md` and
`analysis/legacy_drop_20261017/execution_policy/EXECUTION_POLICY_REPORT.md`.
Run `Test-ExecutionPolicy.ps1` in this directory for deterministic/boundary/native
catalog tests without a new audit or million-draw simulation. Its exit 0 means
the experimental module tests pass; `execution_policy/acceptance.json` retains
BLOCKED while the original-data execution contract is incomplete.
`LegacyDropExecutionPolicy.h` is the shared C++98 implementation for explicit
profile/Treasure hypotheses, zero-scale level bounds, synthetic currency and
preflight budgets. Positive MagicRate and nonzero scales remain rejected.

Run from the workspace:

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass -File PhongThanSource/Tests/LegacyDropNativeTest/Run-All.ps1
```

The runner uses the existing VC6 compiler, copies Loose Client/Server data to
isolated fixtures, runs the three simulation sizes and real native item probes,
compares the complete MagicScript catalog using pre-fix and current source,
builds both Cores, runs the unchanged item baseline, and tests preflight exports
in the newly built CoreServer.dll. It never deploys, invokes an NPC death hook,
opens a game client, connects to a database or modifies runtime data.

Exit 1 indicates a failed test/build/check; exit 2 indicates that the checks ran
but the acceptance gate is BLOCKED. PowerShell launch wrappers can normalize a
nonzero child exit; the structured `phase2/acceptance.json` is authoritative.

Shared sources live in `Sources/Core/Src`:

- `LegacyDropEngine.h`: strict bounded parser, resolver, RNG, prepared lab
  selector, fail-closed quarantine, rejected-plan invalidation, death-token unit.
- `LegacyDropNativeLoader.h`: bounded reads through the real KPakFile loader.
- `LegacyDropItemAdapter.h`: verified raw-to-native identities and native item
  generation validation, using ItemGen/ItemSet rather than a mock.
- `LegacyDropPreflight.cpp`: inert diagnostics compiled into CoreServer, without
  executing world/item initialization or changing KNpc::DoDeath.

`BuildLabPlan` requires an explicitly selected slot and an initialized V1 test
quarantine. Any affected slot blocks the entire NPC plan. Missing/truncated or
invalid manifests reject all plans. Failed planning also invalidates a previously
prepared plan, so it cannot keep producing selections after a rejection.

`BuildProductionPlan` deliberately fails with `PRODUCTION_POLICY_NOT_APPROVED`.
Lab Bernoulli row selection does not supply approved level/scales, MagicRate,
money, Treasure or automatic slot semantics. Those fields remain unmodified;
they are not silently treated as zero or ignored by a production path.

Real generation probes use catalog level candidates and explicit probe level 1
for script/book items. They do not establish a drop-level formula. Likewise,
LoadLibrary preflight is not a running GameServer/world/pickup integration test.
