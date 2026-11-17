from pathlib import Path
ROOT=Path(__file__).resolve().parents[2]
source=(ROOT/'PhongThanSource/Tools/publish_starter_story_live.py').read_text('utf8')
source=source.replace('StarterStory20261029','MasterAlias20261030').replace('starter_story_20261029','master_alias_20261030')
source=source.replace("assert e['role']=='Server' and","assert e['role'] in ('Client','Server') and")
source=source.replace("src=PACKAGE/'compiled/Server'/e['relative'];dst=RUNTIME/e['relative']","src=PACKAGE/'compiled'/e['role']/e['relative'];dst=ROOT/'PhongThanRuntime-Loose'/e['role']/e['relative']")
source=source.replace('for src,dst,e in plans:\n    current=',"for src,dst,e in plans:\n    if e['role']!='Server':continue\n    current=")
exec(compile(source,__file__,'exec'))
