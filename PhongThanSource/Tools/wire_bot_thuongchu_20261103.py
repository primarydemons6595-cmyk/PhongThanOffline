from pathlib import Path
r=Path(__file__).resolve().parents[1]/'Sources/Core/Src'
def edit(name,a,b):
 p=r/name;d=p.read_bytes();a=a.encode();b=b.encode();assert d.count(a)==1,(name,a,d.count(a));p.write_bytes(d.replace(a,b))
edit('ScriptFuns.cpp','int PhongThanThuongChuCertificate(int player)','/* TGPT seal/reward transactions use the existing inventory rollback path. */\n#include "PhongThanThuongChuEconomy.inl"\nint PhongThanThuongChuCertificate(int player)')
edit('PhongThanThuongChu.inl','extern int PhongThanThuongChuEconomy(int,int,int);','extern int PhongThanThuongChuEconomy(int,int,int);\nextern void PhongThanThuongChuBuffTick(int,unsigned long);')
edit('PhongThanThuongChu.inl','for(int p=1;p<MAX_PLAYER;++p)if(Valid(p)){','for(int p=1;p<MAX_PLAYER;++p)if(Valid(p)){\n  PhongThanThuongChuBuffTick(p,now);')
edit('PhongThanBotTraining.inl','#include "PhongThanBotActivities.inl"','#include "PhongThanBotActivities.inl"\n#include "PhongThanBotThuongChu.inl"')
edit('PhongThanBotTraining.inl','if(CompanionTick(r,t,now))return;','if(CompanionTick(r,t,now))return;\n if(BotThuongChu(r,t,now))return;')
edit('PhongThanBotTraining.inl','if(npc.m_CurrentLife<=0||npc.m_Doing==do_death){','if(npc.m_CurrentLife<=0||npc.m_Doing==do_death){\n  if(PtThuongChu::Own(r.Player)&&PtThuongChu::match.phase==2&&PtThuongChu::World(r.Player)==PtThuongChu::Map){\n   if(!t.DeadAt)t.DeadAt=now;lstrcpyA(r.State,"thuong_chu_dead");if(now-t.DeadAt>=8000){Player[r.Player].Revive(REMOTE_REVIVE_TYPE);if(npc.m_CurrentLife>0){t.DeadAt=0;t.Path.clear();t.Target=0;t.Identity=0;t.CooldownUntil=0;Player[r.Player].Save();}}return;\n  }')
print('Wired reward engine, buff lifecycle and autonomous bot registration')
