// Included in KSubWorld.cpp after all native world/entity definitions.
namespace PTMapSleep {
static bool enabled=false;static int idle=60;static char pinned[512]="";static DWORD lastCheck=0;static int occupied[MAX_SUBWORLD];static DWORD occupancyTick=0;static bool anyBot=false;
static void Poll(){DWORD now=GetTickCount();if(lastCheck&&now-lastCheck<2000)return;lastCheck=now;
 char path[MAX_PATH];GetFullPathNameA("MapSleep.ini",sizeof(path),path,0);
 int version=GetPrivateProfileIntA("Main","Version",0,path),value=GetPrivateProfileIntA("Main","Enabled",0,path);enabled=version==1&&value==1;
 idle=GetPrivateProfileIntA("Main","IdleSeconds",60,path);if(idle<10)idle=10;if(idle>3600)idle=3600;
 GetPrivateProfileStringA("Main","PinnedMaps","",pinned,sizeof(pinned),path);
}
static bool Occupied(int world){DWORD now=GetTickCount();if(!occupancyTick||now-occupancyTick>=50){occupancyTick=now;ZeroMemory(occupied,sizeof(occupied));anyBot=false;
 for(int p=1;p<MAX_PLAYER;++p)if(Player[p].m_PlayerDBLoad&&Player[p].m_nIndex>0&&Player[p].m_nIndex<MAX_NPC){int w=Npc[Player[p].m_nIndex].m_SubWorldIndex;if(w>=0&&w<MAX_SUBWORLD)++occupied[w];if(Player[p].m_bIsBot)anyBot=true;}}
 return occupied[world]>0;
}
}
bool PhongThanMapSleepEnabled(){PTMapSleep::Poll();return PTMapSleep::enabled;}
// Passive gather respawns must not wake cold maps. Player travel and explicit
// world operations retain SearchWorld/EnsureContent as the activation boundary.
int PhongThanGatherSpawnWorld(int map){
 bool sleeping=PhongThanMapSleepEnabled();
 for(int w=0;w<MAX_SUBWORLD;++w)if(SubWorld[w].m_SubWorldID==map){
  if(sleeping&&(!SubWorld[w].m_ContentLoaded||SubWorld[w].m_Dormant))return -1;
  return w;
 }
 return -1;
}
int PhongThanMapSleepIdleSeconds(){PTMapSleep::Poll();return PTMapSleep::idle;}
bool PhongThanMapSleepPinned(int map){PTMapSleep::Poll();const char* p=PTMapSleep::pinned;while(*p){char* end=0;long id=strtol(p,&end,10);if(end==p){++p;continue;}if(id==map)return true;p=end;}return false;}
BOOL KSubWorld::EnsureContent(){
 if(m_ContentLoading)return TRUE;if(m_ContentLoaded){m_LastUsedTick=GetTickCount();m_Dormant=false;return TRUE;}if(!m_Region||!m_ContentPath[0])return FALSE;
 m_ContentLoading=true;char before[MAX_PATH];g_GetFilePath(before);
 for(int y=0;y<m_nWorldRegionHeight;++y)for(int x=0;x<m_nWorldRegionWidth;++x){
  g_SetFilePath(m_ContentPath);m_Region[y*m_nWorldRegionWidth+x].LoadObject(m_nIndex,x+m_nRegionBeginX,y+m_nRegionBeginY);
 }
 g_SetFilePath(before);m_ContentLoaded=true;m_ContentLoading=false;m_Dormant=false;m_LastUsedTick=GetTickCount();return TRUE;
}
bool KSubWorld::SleepIdle(){
 if(!PhongThanMapSleepEnabled()){EnsureContent();m_Dormant=false;return false;}
 DWORD now=GetTickCount();bool occupied=PTMapSleep::Occupied(m_nIndex);
 // The existing bot planner indexes monsters across maps. Preserve that
 // catalogue without changing bot logic: cold maps warm once if a bot exists.
 if(PTMapSleep::anyBot&&!m_ContentLoaded)EnsureContent();
 // Bots are real server players and pin their map too. Mission clocks and
 // non-region NPCs (revivals/scripts) must retain the existing scheduler.
 bool pinned=occupied||PhongThanMapSleepPinned(m_SubWorldID)||m_MissionArray.GetUsedCount()>0||m_NoneRegionNpcList.GetNodeCount()>0;
 if(!pinned&&m_ContentLoaded&&now-m_LastUsedTick<(DWORD)PhongThanMapSleepIdleSeconds()*1000)return false;
 if(!pinned&&m_ContentLoaded&&!m_Dormant){for(int r=0;r<m_nTotalRegion&&!pinned;++r){
  if(m_Region[r].m_MissleList.GetNodeCount()>0){pinned=true;break;}
  for(KIndexNode* node=(KIndexNode*)m_Region[r].m_ObjList.GetHead();node;node=(KIndexNode*)node->GetNext()){
   KObj& object=Object[node->m_nIndex];if(object.m_nKind==Obj_Kind_Item||object.m_nKind==Obj_Kind_Money){pinned=true;break;}
  }
 }}
 if(pinned){EnsureContent();m_LastUsedTick=now;m_Dormant=false;return false;}
 if(!m_ContentLoaded){m_Dormant=true;return true;}
 if(now-m_LastUsedTick<(DWORD)PhongThanMapSleepIdleSeconds()*1000)return false;
 if(!m_Dormant){for(int r=0;r<m_nTotalRegion;++r)m_Region[r].SleepCells();m_Dormant=true;}
 return true;
}
unsigned long KSubWorld::GeometryBytes()const{unsigned long bytes=0;for(int r=0;r<m_nTotalRegion;++r)bytes+=m_Region[r].CellBytes();return bytes;}
void PhongThanMapSleepStatus(){
 static DWORD last=0;DWORD now=GetTickCount();if(now-last<3000)return;last=now;
 FILE* f=fopen("map_sleep_status.json.tmp","wb");if(!f)return;
 fprintf(f,"{\"Version\":1,\"Pid\":%lu,\"UtcSeconds\":%lu,\"Enabled\":%s,\"Maps\":[",GetCurrentProcessId(),(unsigned long)time(0),PhongThanMapSleepEnabled()?"true":"false");
 bool first=true;for(int w=0;w<MAX_SUBWORLD;++w){KSubWorld& map=SubWorld[w];if(map.m_SubWorldID<0)continue;
  fprintf(f,"%s{\"Map\":%d,\"Loaded\":%s,\"Sleeping\":%s,\"GeometryBytes\":%lu}",first?"":",",map.m_SubWorldID,map.m_ContentLoaded?"true":"false",map.m_Dormant?"true":"false",map.GeometryBytes());first=false;
 }
 fprintf(f,"]}");bool ok=!ferror(f);if(fclose(f))ok=false;if(ok)MoveFileExA("map_sleep_status.json.tmp","map_sleep_status.json",MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH);
}
