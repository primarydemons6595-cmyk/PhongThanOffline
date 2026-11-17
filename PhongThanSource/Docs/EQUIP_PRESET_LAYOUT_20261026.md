# Equipment preset UI correction

Published to Game Loose. Game.exe hash verified; protected client DLL, UI INIs, and CoreServer.dll unchanged. World PID 105212 continued without restart.

F4 equipment slots already receive an offset of (+38,+38). The ChangeEquipSet icon previously retained its unshifted INI coordinates (171,30). Apply the same offset to the icon: (209,68), immediately above Instrument at (205,103), with a 7-pixel gap below the 28-pixel icon. Other inventory control positions stay unchanged.

The preset panel now uses the existing original selector SPR and text field instead of a plain text button. Its ten-item popup opens below the field, highlights the selected preset, and selects that preset's existing saved items. Add the original shortcut hint, red for unsaved and green for saved; preset ten uses Ctrl+0. Translate original short color tags locally for the text renderer. The frame uses its actual 284x372 SPR size and remains centered.

Equipment serial persistence, per-character storage, server acknowledgments, missing-item preflight, and timeout handling are unchanged. Save and Clear still affect only the selected preset.

Validation: GameClient Release build passed. The extracted equipment-controller harness passed 23 checks including the prior persistence/identity/ack cases and new dropdown placement, preset selection, saved status, and Ctrl+0 cases. Rendered original frame/slot/button/selector art to analysis/equip_preset_layout_20261026/preset_preview.png and inspected it. This is an asset composition, not a live game screenshot.

Package: Deploy/ProjectContent/EquipPresetLayout20261026, Game.exe only. No server restart required.
