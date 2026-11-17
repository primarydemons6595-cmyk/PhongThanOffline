# Bot dialogue and survival correction — 2026-10-29

## Observed faults

The AI checkbox enabled the local worker, but earlier versions primarily classified intent. The native reply then often used the generic greeting. Quest wording was also missing from intent rules. A model timeout, invalid JSON, or rejected answer could return to the same greeting.

Conversation history was previously supplied as serialized data rather than alternating user/assistant messages. Vietnamese in nested assistant JSON was escaped, making subsequent small-model replies less reliable. The corrected history preserves actual Vietnamese characters and the last four paired turns.

## Current behavior

- Social and quest questions may receive a generated Vietnamese answer. The prompt receives fresh native activity and map facts. Unavailable quest progress is explicitly unknown.
- Economic actions and party membership remain authoritative native operations. Model text cannot grant items, commit trades, complete tasks, or assert successful actions.
- Unsupported claims are rejected. Direct quest questions have a contextual fallback based on the current activity, including when local AI is disabled. This fallback is not reported as model output.
- Logs distinguish `reply=local_model`, `server_context`, and `server_rules`, with request ID and fallback reason. They do not include the private question text.
- The worker starts and warms the model when AI is enabled. Disabling AI terminates its model process; normal server dialogue remains available.

This machine uses Qwen3-1.7B Q8_0 with llama.cpp b11960 Vulkan. The engine and model downloads are SHA256 verified. Observed GPU usage was about 2.1 GiB including the existing desktop load. This is a machine-specific observation, not a CPU-only performance promise. `Deploy/Install-BotLocalModel.ps1 -Profile gpu-context` installs this profile; `-Profile cpu` selects the existing 0.6B CPU profile. Stop the local worker before reinstalling an engine. Reinstallation does not change the AI checkbox.

## Death/revival fault

The normal NPC death animation transitions into `do_revive` while life remains zero. The bot previously revived only from `do_death`, leaving it stuck after that transition.

The bot now accepts either death/wait state, invokes the normal remote-town revival, and confirms positive life and the actual starter-town position before recording success. It rests for 25–60 seconds, avoids the lethal map for 15 minutes, and uses conservative level selection for 30 minutes. The risk state survives restarts; following a party leader also honors the avoided map.

The live log recorded successful natural-death revival confirmations at 15:18:57, 15:19:19, and 15:20:19. A separate disposable-bot probe did not encounter a natural death and therefore did not establish a result; it archived only its own fixture.

## Validation and limits

Managed builds and 45 checks passed, including an AI-off contextual quest response and accurate provenance. Native build passed with the existing linker warning. The dialogue probe uses the live file bridge and records each answer's actual source; it sends no message to players. Its final two turns were both model-generated (868 ms and 438 ms): it acknowledged uncertainty about the unspecified quest, then described its actual travel to a training area. Model benchmark replies and timings are under `analysis/bot_aigent_20261029`.

Small-model answers still need factual validation and contextual fallback. Complete daily-event adapters, negotiated player trading, and arbitrary quest execution are separate unfinished capabilities; conversational replies do not imply those actions exist.
