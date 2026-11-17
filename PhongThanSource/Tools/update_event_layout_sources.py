from pathlib import Path
ROOT=Path(__file__).resolve().parents[2]
for relative in ('PhongThanSource/Tests/EventService/Program.cs','PhongThanSource/Tests/EventServiceHttp/Program.cs'):
    p=ROOT/relative;s=p.read_text('utf8');s=s.replace('Path.Combine(root, "EventService",','Path.Combine(root, "Server", "EventService",');p.write_text(s,'utf8')
for relative in ('PhongThanSource/Tools/verify_event_publication.py','PhongThanSource/Tools/verify_live_event_commands.py'):
    p=ROOT/relative;s=p.read_text('utf8');s=s.replace("runtime/'EventService/", "runtime/'Server/EventService/");p.write_text(s,'utf8')
p=ROOT/'PhongThanSource/Tools/stage_escort_service.py';s=p.read_text('utf8').replace("put('EventService/catalog.json'", "put('Server/EventService/catalog.json'");p.write_text(s,'utf8')
p=ROOT/'PhongThanSource/Tools/Publish-EventPhongThan.ps1';s=p.read_text('utf8').replace('PhongThan.EventService','PhongThan.Event');p.write_text(s,'utf8')
p=ROOT/'PhongThanSource/Tools/prepare_event_publish_manifest.py';s=p.read_text('utf8').replace("('.exe','.dll','.json','.pdb')", "('.exe','.dll','.json','.pdb','.cfg')");p.write_text(s,'utf8')
print('Canonical source layout: Server/PhongThan.Event.exe + cfg + Server/EventService')
