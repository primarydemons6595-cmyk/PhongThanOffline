# Event runtime separated from Studio

`Sources/EventRuntime/PhongThan.Event.Runtime.csproj` contains only the Event
and Bot service runtime, operational configuration, scheduling and its client
protocol. Its assembly/namespace is `PhongThan.Event.Runtime`. It has no project
reference to Studio. EventService references this project directly; Studio.Core
also references it for development tools. The runtime contains no SecurePak,
release keys, ReleasePipeline, build tooling, workspace editor or map editor.

The next Release build includes `PhongThan.Event.Runtime.dll` instead of
`PhongThanStudio.Core.dll`. The secure output certificate excludes Studio files,
and the pipeline rejects certificates that still list Studio dependencies.
The Event build checks its dependency manifest to prevent this dependency from
returning unnoticed. Existing release folders and signed patches are unchanged
by this source refactor; build a new runtime and patch when ready to publish.

An older installed Base may still have the formerly distributed
PhongThanStudio.Core.dll even after copying a patch. The next patch also carries
Remove-LegacyStudio.ps1 and an updated Start-Server.ps1. After stopping the
deployment, copying the complete patch and starting via Start-Server.cmd, the
launcher removes only the obsolete Studio.Core DLL/PDB/dependency manifest. It
first checks the new Event dependency manifest and refuses removal while the
Event process is running. CharacterStore and live runtime state are untouched.
For custom launchers, run Remove-LegacyStudio.ps1 after installing the new Event
runtime and before starting services.

Validation includes the standalone Event host's real HTTP operations, Bot
regressions, Studio regressions, secure packaging tests and dependency/type
checks that reject Studio references or publishing/editor types in Event.Runtime.
Build outputs are tested without publishing over the live Dev deployment.
