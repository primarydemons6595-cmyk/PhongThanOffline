# Four-arena population and Thien Hung repair

Published only to `PhongThanRuntime-Loose/Server`. No Release files or binaries were changed.

All four mission scripts incorrectly started registration timer 3005 and displayed/stopped combat timer 3006. Timer 3005 belongs to arena 3 (Hoa), whose callback calls RunMission(3). In worlds containing missions 1, 2 or 4, this cannot advance the correct mission. The live lifecycle log also recorded timer 3005 running in worlds 1067, 1068 and 1070. Registration timers are now 3001/3003/3005/3007 and combat timers 3002/3004/3006/3008, matching the existing timer callback files. Heartbeat 3009 is retained.

Existing normal-monster data was present: 250 points per arena, all within the current spawn domains. No replacement monsters, drops, schedules or unconditional boss spawns were added. Monsters remain tied to an open mission. Guardians appear in combat; the boss requires all four guardians and the existing gate-opening condition.

Each arena had a doctor placement but no Thien Hung placement. Added one template-155 Thien Hung per arena, 128 world units beside the existing doctor, checking the connecting path against collision and VTT fence/door cells. Display coordinates: Tho 165,204; Thuy 219,222; Hoa 206,212; Phong 209,189. These nearby coordinates are authored, not claimed as recovered original coordinates. The script copies the existing recovered original Thien Hung callbacks from `npc_recovered_batch/1021_b5deef29.lua`, adapting only the top-level map guard to the four arenas. Existing doctor and other region content are preserved.

Validation: the actual engine Lua 4 runtime executed each mission and its separate registration/combat callback states, checked spawn failure retry, 250 normal monsters, four guardians, gate-restricted single boss, no duplicate tick spawns, timer cleanup, a second round and Thien Hung dialog/map guards. Content verification checked all nine published SHA256 hashes, preserved region sections, four unique NPC placements and all 1000 normal spawn points.

Backup: `analysis/vtt_repair_20261031/backup_20261031_134413`. The Loose world was already stopped and remains stopped. Native world population and in-game visual acceptance were not performed in this patch.
