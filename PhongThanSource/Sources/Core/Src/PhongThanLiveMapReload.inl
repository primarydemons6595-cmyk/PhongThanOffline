struct Geometry {long obstacle[REGION_GRID_WIDTH][REGION_GRID_HEIGHT];DWORD traps[REGION_GRID_WIDTH][REGION_GRID_HEIGHT];};
// Preserve NPCs, objects, missions and region indices. Stage all geometry first.
BOOL KSubWorld::ReloadLiveGeometry(char* error,int capacity){
 g_StrCpyLen(error,"map_reload_failed",capacity);
 for(int p=1;p<MAX_PLAYER;++p)if(Player[p].m_nIndex>0&&Player[p].m_nIndex<MAX_NPC&&
  Npc[Player[p].m_nIndex].m_SubWorldIndex==m_nIndex&&Player[p].m_nNetConnectIdx>=0){g_StrCpyLen(error,"waiting_for_players_to_leave",capacity);return FALSE;}
 if(m_SubWorldID>=1067&&m_SubWorldID<=1070){g_StrCpyLen(error,"protected_event_map_geometry",capacity);return FALSE;}
 if(!m_Region||m_nTotalRegion<=0||m_nTotalRegion>20000)return FALSE;
 char previous[MAX_PATH];g_GetFilePath(previous);g_SetFilePath("\\");
 struct RestorePath {char* path;RestorePath(char* p):path(p){}~RestorePath(){g_SetFilePath(path);}}restore(previous);
 KIniFile worlds;if(!worlds.Load("\\settings\\WorldSet.ini"))return FALSE;
 char id[20],resource[128];sprintf(id,"%d",m_SubWorldID);worlds.GetString("List",id,"",resource,sizeof(resource));if(!resource[0])return FALSE;

 std::vector<Geometry> staged(m_nTotalRegion);
 for(int r=0;r<m_nTotalRegion;++r){
  Geometry& next=staged[r];memset(&next,0,sizeof(next));KRegion& old=m_Region[r];int x=LOWORD(old.m_RegionID),y=HIWORD(old.m_RegionID);
  KPakFile file;char path[300];sprintf(path,"\\maps\\%s_S\\v_%03d\\%03d_Region_S.dat",resource,y,x);
  BOOL opened=file.Open(path);if(!opened){sprintf(path,"\\maps\\%s\\v_%03d\\%03d_Region_S.dat",resource,y,x);opened=file.Open(path);}
  if(!opened){
   bool nonempty=false;for(int a=0;a<REGION_GRID_WIDTH;++a)for(int b=0;b<REGION_GRID_HEIGHT;++b)if(old.m_Cells.Get()->obstacle[a][b]||old.m_Cells.Get()->trap[a][b])nonempty=true;
   if(nonempty){g_StrCpyLen(error,"missing_existing_region_data",capacity);return FALSE;}continue;
  }
  DWORD count=0;if(file.Read(&count,4)!=4||count<3||count>64)return FALSE;
  std::vector<KCombinFileSection> sections(count);if(file.Read(&sections[0],count*sizeof(KCombinFileSection))!=count*sizeof(KCombinFileSection))return FALSE;
  DWORD header=4+count*sizeof(KCombinFileSection),size=file.Size();
  for(int k=0;k<2;++k){int section=k==0?REGION_OBSTACLE_FILE_INDEX:REGION_TRAP_FILE_INDEX;
   if(section>=(int)count)return FALSE;KCombinFileSection& s=sections[section];
   if(header>size||s.uOffset>size-header||s.uLength>size-header-s.uOffset)return FALSE;
   if(!s.uLength)continue;file.Seek(header+s.uOffset,FILE_BEGIN);
   if(k==0){if(s.uLength!=sizeof(next.obstacle)||file.Read(next.obstacle,sizeof(next.obstacle))!=sizeof(next.obstacle))return FALSE;}
   else{KTrapFileHead head;if(s.uLength<sizeof(head)||file.Read(&head,sizeof(head))!=sizeof(head)||head.uNumTrap>(s.uLength-sizeof(head))/sizeof(KSPTrap)||sizeof(head)+head.uNumTrap*sizeof(KSPTrap)!=s.uLength)return FALSE;
    for(unsigned ti=0;ti<head.uNumTrap;++ti){KSPTrap cell;if(file.Read(&cell,sizeof(cell))!=sizeof(cell)||(unsigned)cell.cY>=REGION_GRID_HEIGHT||(unsigned)cell.cX>=REGION_GRID_WIDTH||cell.cNumCell<=0||(unsigned)cell.cX+(unsigned)cell.cNumCell>REGION_GRID_WIDTH)return FALSE;for(int dx=0;dx<cell.cNumCell;++dx)next.traps[cell.cX+dx][cell.cY]=cell.uTrapId;}}
  }
 }
 for(int z=0;z<m_nTotalRegion;++z){memcpy(m_Region[z].m_Cells.Get()->obstacle,staged[z].obstacle,sizeof(staged[z].obstacle));memcpy(m_Region[z].m_Cells.Get()->trap,staged[z].traps,sizeof(staged[z].traps));}
 error[0]=0;return TRUE;
}
