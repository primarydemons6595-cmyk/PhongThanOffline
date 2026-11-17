# Event extra-drop item identification

Published and hash verified (8 Studio files). World PID 48580 unchanged; Game, server binary, event configuration, and campaign snapshot hashes unchanged.

The item picker now shows the item name, ID, source table, and full type/level/series tuple. The extra-drop grid adds read-only ID and source-file columns. Numeric ID search can be combined with a source name, e.g. `questkey.txt 47`.

Confirmed against the active Server item version 001: questkey Detail 46 is Nguyen Bao, Detail 47 is Xu. These are IDs from the item fields, not physical text-line numbers. Quest items display that numeric ID; other item families display the complete Genre/Detail/Particular/Level/Series key because a single number is insufficient to identify equipment variants. Existing campaign rows resolve their source from the active catalog. Multiple matching sources are shown rather than silently choosing one.

Display metadata is marked JsonIgnore and does not alter the event configuration or native event snapshot contract. Names, item tuple values, maps, quantities, and drop percentages are retained.

Validation: Release build passed with zero warnings/errors. Read-only active-catalog tests verify Xu 47 and Nguyen Bao 46, labels, same-name/different-ID and different-source disambiguation, ID search, and serialization compatibility. The event editor was rendered and inspected with both quest items. The preview does not save its sample event.

Evidence: analysis/event_item_ids_20261026. Package: Deploy/ProjectContent/EventItemIds20261026, eight Studio DLL/PDB files across Release and Verified launch locations. Game, client/server cores, existing event JSON, and native campaign snapshot are protected by hashes.

Service indicators: each service name now has a red dot when stopped and a lime-green dot when its process is running. Status refreshes with PID detection, including changes outside Studio. Rendered and inspected against the live cluster: five native services green, stopped Event service red.
