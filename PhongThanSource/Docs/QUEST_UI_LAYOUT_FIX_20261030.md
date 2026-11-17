# Quest tracker and panel layout correction

Published only Loose Client/Game.exe. No Release runtime files or server
binaries were changed. Receipt and backup:
`analysis/quest_ui_layout_20261030/published.json` and `Game.exe.before`.

- Quest tracker root and scrollbar start at x=0. Text begins at x=18 (body
  x=26), leaving a separate 14-pixel strip at the left edge. Slider centre
  limits include half its height so the knob stays inside the tracker.
- KWndQuestTracker::PtInWindow accepts only its visible scrollbar and trade
  link. Text and background clicks pass to the map.
- Wnd_ProcessInput routes mouse wheel to the tracker only when no higher
  window, capture or modal owns the pointer. ScrollAt checks the actual
  tracker bounds, visibility and overflow, accumulates partial wheel deltas
  and scrolls three text lines per notch. Other windows keep wheel input.
- F11 now uses PhongThanAnchorNativePanel, matching F4's authored top=116
  plus (screenHeight-600)/2, including animation endpoints.
- NPC shop top aligns with the current F4 top instead of its bottom, with
  screen bounds retained.

Validation: Test-QuestUiLayout.ps1 passes using extracted production input
methods and native anchoring functions. Covers text click-through, scrollbar
and trade-link hit areas, hidden state, wheel direction, partial deltas,
range clamps, higher-window precedence, and panel tops at 800x600,
1024x768, 1280x720 and 1920x1080. GameClient builds with zero errors;
legacy compiler warnings remain.

Client close request did not complete within 15 seconds. Windows Computer
Use initialization failed with 'failed to write kernel assets ... path
specified ... os error 3'. The exact Loose client process was then stopped,
the executable replaced with backup/hash verification, and client reopened.
GameServer PID 178548 stayed running, WORLD_READY/105 confirmed.
No character login or screenshot verification was performed after deployment.
