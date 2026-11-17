// Included inside PtCustomActions. One purchased pot is consumed per batch.
// Cooking payment/timers remain character-owned; NPC indices are never identities.
struct Pot {std::string path,owner;int eventId,map,x,y,index,recipe;unsigned long started,ready,burn,placed;DWORD entity;Pot():eventId(0),map(0),x(0),y(0),index(0),recipe(0),started(0),ready(0),burn(0),placed(0),entity(0){}};
static std::vector<Pot> pots;static bool potsLoaded=false;
static bool PotPath(int p,int eventId,char* path){char job[400];if(!JobPath(p,eventId,0,job))return false;char* name=strrchr(job,'\\');if(!name)return false;CreateDirectoryA("EventService\\CustomEvents\\pots",0);char relative[400];sprintf(relative,"EventService\\CustomEvents\\pots%s",name);return GetFullPathNameA(relative,400,path,0)>0;}
static bool LivePot(const Pot& pot){return pot.index>0&&pot.index<MAX_NPC&&Npc[pot.index].m_dwID==pot.entity&&!Npc[pot.index].IsPlayer();}
static bool IdlePotExpired(const Pot& pot,const PTCustomEvent& e,unsigned long now){return !pot.recipe&&pot.placed&&now>=pot.placed&&now-pot.placed>=(unsigned long)e.toolIdleSeconds;}
static void RemovePot(Pot& pot){if(LivePot(pot)){KNpc& n=Npc[pot.index];int w=n.m_SubWorldIndex,r=n.m_RegionIndex,x=n.m_MapX,y=n.m_MapY;NpcSet.Remove(pot.index);if(w>=0&&w<MAX_SUBWORLD&&r>=0&&r<SubWorld[w].m_nTotalRegion){SubWorld[w].m_Region[r].RemoveNpc(pot.index);SubWorld[w].m_Region[r].DecRef(x,y,obj_npc);}}pot.index=0;pot.entity=0;}
static int PotHex(char c){return c>='0'&&c<='9'?c-'0':c>='a'&&c<='f'?c-'a'+10:c>='A'&&c<='F'?c-'A'+10:-1;}
static std::string PotOwner(const std::string& hex){std::string name;if(hex.size()>62||hex.size()%2)return name;for(unsigned int i=0;i<hex.size();i+=2){int a=PotHex(hex[i]),b=PotHex(hex[i+1]);if(a<0||b<0||a*16+b<32)return "";name+=(char)(a*16+b);}return name;}
static std::string PotName(const Pot& pot,const PTCustomEvent& e,unsigned int capacity){
 if(pot.owner.empty()||capacity<5)return e.toolName.substr(0,capacity?capacity-1:0);
 std::string owner=pot.owner.substr(0,capacity-4<31?capacity-4:31);unsigned int base=capacity>owner.size()+4?capacity-owner.size()-4:0;
 return e.toolName.substr(0,base)+" ("+owner+")";
}
static bool SpawnPot(Pot& pot,const PTCustomEvent& e){int w=g_SubWorldSet.SearchWorld(pot.map);if(w<0||SubWorld[w].GetBarrier(pot.x,pot.y)!=0)return false;
 int n=NpcSet.Add(MAKELONG(1,e.toolNpc),w,pot.x,pot.y,TRUE);if(n<=0||n>=MAX_NPC)return false;KNpc& npc=Npc[n];npc.m_Kind=kind_dialoger;npc.SetFightMode(0);npc.SetCurrentCamp(camp_free);npc.m_DeathScriptID=npc.m_TimerScriptID=npc.m_DropScriptID=0;
 lstrcpynA(npc.Name,PotName(pot,e,sizeof(npc.Name)).c_str(),sizeof(npc.Name));lstrcpynA(npc.ActionScript,e.toolScript.c_str(),sizeof(npc.ActionScript));npc.m_ActionScriptID=g_FileName2Id(npc.ActionScript);pot.index=n;pot.entity=npc.m_dwID;return true;
}
static Pot* OwnedPot(int p,int eventId){char path[400];if(!PotPath(p,eventId,path))return 0;for(unsigned int i=0;i<pots.size();++i)if(pots[i].path==path&&pots[i].eventId==eventId)return &pots[i];return 0;}
static void FinishPot(int p,int eventId,int recipe){Pot* found=OwnedPot(p,eventId);if(!found||found->recipe!=recipe)return;for(unsigned int i=0;i<pots.size();++i)if(&pots[i]==found){RemovePot(pots[i]);DeleteFileA(pots[i].path.c_str());pots.erase(pots.begin()+i);return;}}
static bool ToolAccess(int p,const PTCustomEvent& e){if(!Ready(p)||!e.enabled)return false;char path[400];if(!PotPath(p,e.id,path))return false;int actor=Player[p].m_nIndex;
 for(unsigned int i=0;i<pots.size();++i){Pot& pot=pots[i];if(pot.path!=path||pot.eventId!=e.id||!LivePot(pot))continue;
  return !IdlePotExpired(pot,e,(unsigned long)time(NULL))&&Player[p].m_nLastNpcIndex==pot.index&&Npc[actor].m_SubWorldIndex==Npc[pot.index].m_SubWorldIndex&&NpcSet.GetDistance(actor,pot.index)<=384;
 }return false;
}
static bool SavePot(const Pot& pot){char next[420],data[512],owner[65];unsigned int len=0;for(unsigned int i=0;i<pot.owner.size()&&i<31;++i)len+=sprintf(owner+len,"%02x",(unsigned char)pot.owner[i]);owner[len]=0;sprintf(next,"%s.next",pot.path.c_str());sprintf(data,"[Pot]\nEvent=%d\nMap=%d\nX=%d\nY=%d\nRecipe=%d\nBurn=%lu\nOwnerHex=%s\nStarted=%lu\nReady=%lu\nPlaced=%lu\n",pot.eventId,pot.map,pot.x,pot.y,pot.recipe,pot.burn,owner,pot.started,pot.ready,pot.placed);HANDLE f=CreateFileA(next,GENERIC_WRITE,0,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);if(f==INVALID_HANDLE_VALUE)return false;DWORD n=0;bool ok=WriteFile(f,data,strlen(data),&n,0)&&n==strlen(data)&&FlushFileBuffers(f);CloseHandle(f);if(ok)ok=MoveFileExA(next,pot.path.c_str(),MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH)!=0;if(!ok)DeleteFileA(next);WritePrivateProfileStringA(NULL,NULL,NULL,pot.path.c_str());return ok;}
static void LoadPots(){if(potsLoaded)return;potsLoaded=true;WIN32_FIND_DATAA found;HANDLE h=FindFirstFileA("EventService\\CustomEvents\\pots\\*.ini",&found);if(h==INVALID_HANDLE_VALUE)return;
 do{if((found.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)||pots.size()>=1024)continue;char relative[400],absolute[400];sprintf(relative,"EventService\\CustomEvents\\pots\\%s",found.cFileName);if(!GetFullPathNameA(relative,400,absolute,0))continue;Pot pot;pot.path=absolute;char owner[65];GetPrivateProfileStringA("Pot","OwnerHex","",owner,sizeof(owner),absolute);pot.owner=PotOwner(owner);
 if(pot.owner.empty()){const char* start=strchr(found.cFileName,'-');const char* end=start?strchr(start+1,'-'):0;if(start&&end)pot.owner=PotOwner(std::string(start+1,end-start-1));}
 pot.eventId=GetPrivateProfileIntA("Pot","Event",0,absolute);pot.map=GetPrivateProfileIntA("Pot","Map",0,absolute);pot.x=GetPrivateProfileIntA("Pot","X",0,absolute);pot.y=GetPrivateProfileIntA("Pot","Y",0,absolute);pot.recipe=GetPrivateProfileIntA("Pot","Recipe",0,absolute);pot.burn=GetPrivateProfileIntA("Pot","Burn",0,absolute);pot.started=GetPrivateProfileIntA("Pot","Started",0,absolute);pot.ready=GetPrivateProfileIntA("Pot","Ready",0,absolute);pot.placed=GetPrivateProfileIntA("Pot","Placed",0,absolute);if(!pot.recipe&&!pot.placed){pot.placed=(unsigned long)time(NULL);SavePot(pot);}
 if(pot.recipe&&!pot.ready){std::string job=pot.path;unsigned int suffix=job.rfind("-0.ini");unsigned int folder=job.find("\\pots\\");if(suffix!=std::string::npos&&folder!=std::string::npos){char ending[40];sprintf(ending,"-%d.ini",pot.recipe);job.replace(suffix,6,ending);job.replace(folder,6,"\\jobs\\");pot.ready=GetPrivateProfileIntA("Cook","Ready",0,job.c_str());const PTCustomRecipe* r=PhongThanCustomRecipe(pot.recipe);pot.started=r&&pot.ready>(unsigned int)r->cook?pot.ready-r->cook:pot.ready?pot.ready-1:0;}}
if(pot.eventId>0&&pot.map>0&&pot.x>0&&pot.y>0&&pot.x<1000000&&pot.y<1000000)pots.push_back(pot);
 }while(FindNextFileA(h,&found));FindClose(h);
}
static void SendPotProgress(Pot& pot){
 if(!LivePot(pot)||!pot.recipe||!pot.started||!pot.ready)return;
 const PTCustomRecipe* recipe=PhongThanCustomRecipe(pot.recipe);if(!recipe)return;
 PTPotProgress msg;memset(&msg,0,sizeof(msg));
 PhongThanInitializeWireHeader(&msg.Header,PT_POT_PROGRESS,sizeof(msg),PHONGTHAN_WIRE_FLAG_RESPONSE,0);
 msg.MapId=pot.map;msg.EntityId=pot.entity;msg.Now=(unsigned int)time(NULL);msg.Started=pot.started;msg.Ready=pot.ready;msg.Burn=pot.burn;
 if(msg.Now>=msg.Ready){std::string text=Format(recipe->readyText,*recipe,msg.Burn>msg.Now?msg.Burn-msg.Now:0);lstrcpynA(msg.Text,text.c_str(),sizeof(msg.Text));}
 Npc[pot.index].SendDataToNearRegion((BYTE*)&msg,sizeof(msg));
}
static void TickPots(){LoadPots();for(unsigned int i=0;i<pots.size();){Pot& pot=pots[i];const PTCustomEvent* e=PhongThanCustomEvent(pot.eventId);
 if(e&&IdlePotExpired(pot,*e,(unsigned long)time(NULL))){RemovePot(pot);DeleteFileA(pot.path.c_str());pots.erase(pots.begin()+i);continue;}
 if(pot.burn&&(unsigned long)time(NULL)>=pot.burn){for(int p=1;p<MAX_PLAYER;++p)if(Ready(p)&&OwnedPot(p,pot.eventId)==&pot){const PTCustomRecipe* recipe=PhongThanCustomRecipe(pot.recipe);if(recipe){std::string text=Format(recipe->burntText,*recipe,0);if(!text.empty())Tell(p,text.c_str());}break;}RemovePot(pot);DeleteFileA(pot.path.c_str());pots.erase(pots.begin()+i);continue;}
 if(!e||e->toolScript.empty()){RemovePot(pot);DeleteFileA(pot.path.c_str());pots.erase(pots.begin()+i);continue;}
 if(!e->enabled){RemovePot(pot);++i;continue;}
 if(LivePot(pot)&&Npc[pot.index].m_NpcSettingIdx!=e->toolNpc)RemovePot(pot);
 if(!LivePot(pot))SpawnPot(pot,*e);else{lstrcpynA(Npc[pot.index].Name,PotName(pot,*e,sizeof(Npc[pot.index].Name)).c_str(),sizeof(Npc[pot.index].Name));lstrcpynA(Npc[pot.index].ActionScript,e->toolScript.c_str(),sizeof(Npc[pot.index].ActionScript));Npc[pot.index].m_ActionScriptID=g_FileName2Id(Npc[pot.index].ActionScript);}SendPotProgress(pot);++i;
 }}
static bool PlacePot(int p,const PTCustomEvent& e){LoadPots();Pot* existing=OwnedPot(p,e.id);if(existing&&existing->recipe)return false;char path[400];if(!PotPath(p,e.id,path))return false;int actor=Player[p].m_nIndex,w=Npc[actor].m_SubWorldIndex;if(w<0||w>=MAX_SUBWORLD)return false;
 int x=0,y=0;Npc[actor].GetMpsPos(&x,&y);Pot pot;pot.path=path;pot.owner=Player[p].Name;pot.placed=(unsigned long)time(NULL);pot.eventId=e.id;pot.map=SubWorld[w].m_SubWorldID;
 // Find an open tile beside the character, never underneath an obstacle.
 static int dx[]={64,-64,0,0,64,-64,64,-64};static int dy[]={0,0,64,-64,64,64,-64,-64};bool placed=false;
 for(int step=0;step<8;++step){pot.x=x+dx[step];pot.y=y+dy[step];if(pot.x>0&&pot.y>0&&SpawnPot(pot,e)){placed=true;break;}}if(!placed)return false;
 unsigned int at;for(at=0;at<pots.size();++at)if(pots[at].path==path)break;
 if((at==pots.size()&&pots.size()>=1024)||!SavePot(pot)){RemovePot(pot);return false;}
 if(at<pots.size()){RemovePot(pots[at]);pots[at]=pot;}else pots.push_back(pot);
 // Do not open an inventory cooking menu. The player must click the placed NPC.
 return true;
}
