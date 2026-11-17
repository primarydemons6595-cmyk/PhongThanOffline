static bool CompanionBoss(int npc){
 if(npc<=0||npc>=MAX_NPC||Npc[npc].IsPlayer()||Npc[npc].m_SubWorldIndex<0||Npc[npc].m_SubWorldIndex>=MAX_SUBWORLD)return false;int map=SubWorld[Npc[npc].m_SubWorldIndex].m_SubWorldID;
 if(Npc[npc].m_Kind==kind_normal&&!Npc[npc].Owner[0]&&Npc[npc].m_btSpecial==npc_gold)return true;
 for(std::map<int,PtEvents::Definition>::const_iterator i=PtEvents::definitions.begin();i!=PtEvents::definitions.end();++i)if(i->second.adapter==2&&i->second.map==map&&i->second.tid==Npc[npc].m_NpcSettingIdx)return true;return false;
}
static bool CompanionTargetType(Runtime& r,int npc,int mode){
 if(npc<=0||npc>=MAX_NPC||Npc[npc].m_CurrentLife<=0||Npc[npc].m_RegionIndex<0||Npc[npc].m_SubWorldIndex<0||Npc[npc].m_SubWorldIndex>=MAX_SUBWORLD)return false;
 if(mode==PTBC_BOSS)return CompanionBoss(npc);
 if(mode==PTBC_PLAYER){int target=Npc[npc].IsPlayer()?Npc[npc].GetPlayerIdx():0;return target>0&&target<MAX_PLAYER&&target!=r.Player&&target!=companions[&r-bots].Owner&&Player[target].m_PlayerDBLoad;}
 if(mode==PTBC_RAID_SUPPLY||mode==PTBC_RAID_ESCORT){int owner=0,kind=PhongThanEscortCargoKind(npc,&owner);return kind==(mode==PTBC_RAID_SUPPLY?1:2)&&owner>0&&owner!=r.Player&&owner!=companions[&r-bots].Owner;}
 return Npc[npc].m_Kind==kind_normal&&!Npc[npc].Owner[0]&&Npc[npc].m_nMissionGroup<0&&!CompanionBoss(npc);
}
static int CompanionResolveTarget(Runtime& r,CompanionState& c){
 KPlayer& human=Player[c.Owner];int leader=human.m_nIndex,actor=Player[r.Player].m_nIndex,observed=Npc[leader].m_SkillParam1==-1?Npc[leader].m_SkillParam2:0;
 char query[256];PTBotFoldPartySpeech(c.TargetName,query,sizeof(query));int named=0,nearby=0,observedTarget=0,score=2147483647;
 for(int n=NpcSet.GetNextIdx(0);n;n=NpcSet.GetNextIdx(n)){if(Npc[n].m_SubWorldIndex<0||Npc[n].m_SubWorldIndex>=MAX_SUBWORLD||Npc[n].m_CurrentLife<=0)continue;char name[256],plain[256];const char* shown=Npc[n].IsPlayer()?0: ::PhongThanRestoredNpcDisplayName(Npc[n].Name);PTPlainItemName(plain,sizeof(plain),shown?shown:Npc[n].Name);PTBotFoldPartySpeech(plain,name,sizeof(name));bool match=name[0]&&strstr(query,name);if(!match&&n!=observed&&(Npc[n].m_SubWorldIndex!=Npc[leader].m_SubWorldIndex||NpcSet.GetDistance(leader,n)>1024))continue;if(!CompanionTargetType(r,n,c.Mode))continue;int map=SubWorld[Npc[n].m_SubWorldIndex].m_SubWorldID,current=SubWorld[Npc[actor].m_SubWorldIndex].m_SubWorldID;
  int distance=Npc[n].m_SubWorldIndex==Npc[leader].m_SubWorldIndex?NpcSet.GetDistance(leader,n):2147483647;
  if(match){int hops=BotRoute(r,current,map);if(hops<0)continue;int value=hops*100000+(distance==2147483647?0:distance);if(!named||value<score){named=n;score=value;}}
  if(n==observed)observedTarget=n;
  if(distance>=0&&distance<=1024&&(c.Mode==PTBC_BOSS||c.Mode==PTBC_FIGHT||c.Mode==PTBC_RAID_SUPPLY||c.Mode==PTBC_RAID_ESCORT)&&(!nearby||distance<NpcSet.GetDistance(leader,nearby)))nearby=n;
 }
 return named?named:c.TargetExplicit?0:observedTarget?observedTarget:nearby;
}
