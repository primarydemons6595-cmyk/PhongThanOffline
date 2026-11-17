# Summon HUD mouse routing fix

The image-less KPtCompanionHud root used Trans=1, which invoked image-alpha
hit testing against an empty image. Represent3 returns zero for that image;
TopChildFromPoint therefore never reached the sword button.

Use Trans=0 and DummyWnd=1 on this root so its visible children define its
interactive region. Empty space and hidden panels pass through to the world.
The original Normal/Locked modes, sprite and tooltips are unchanged.

Validation: SummonHudHitTests extracts production PtInWindow and
TopChildFromPoint, reproduces the old failure and checks sword reachability
with and without a pet, hidden panels and empty space. The original button
handler test also passes. GameClient build: zero errors, one warning.

Published only PhongThanRuntime-Loose/Client/Game.exe and reopened that client.
Server PID 178548 remained running, WORLD_READY/105 confirmed. Receipt and
previous executable: analysis/summon_hud_hit_20261030/. Client responds with
window title Phong Than Online. No live character login/click verification
was performed; routing was verified using production hit-test methods.
