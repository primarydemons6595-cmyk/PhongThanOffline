#include "PhongThanRealm.h"
#include "PhongThanGiapGather.h"
#include "PhongThanTwoFaction.h"
#include "PhongThanDailyQuest.h"
#include "PhongThanTranNguyen.h"
// Executed only on the authoritative world thread. The external service owns
// scheduling/configuration; it never owns player inventory or quest storage.
#include "PhongThanEventControl.h"
#include "PhongThanVttGates.inl"
#include "PhongThanVttArena.h"
#include "KMission.h"
#include "PhongThanScriptWire.h"
#include <map>
#include <set>
#include <vector>
#include <stdio.h>
#include <time.h>
#include <errno.h>

namespace PtEvents {
struct Rule {
    int id,enabled,registration,duration,genre,detail,particular,level,series,count,mode;
    char slots[4][96];
    int quest[15];
    Rule(){memset(this,0,sizeof(*this));}
};
struct Definition {
    int id,adapter,map,content,mission,tid,flag;
    char start[260],label[128];
    Definition(){memset(this,0,sizeof(*this));flag=-1;}
};
struct State {
    Rule active;unsigned long session;long activeRevision;int phase;long acceptUntil;bool closed;char error[80];
    State():session(0),activeRevision(0),phase(0),acceptUntil(0),closed(false){error[0]=0;}
};
struct Owned {int index,eventId,mode;unsigned long entity,session;char slots[4][96];};
static std::map<int,Definition> definitions;
static std::map<int,Rule> rules;
static std::map<int,State> states;
static std::map<unsigned long,Owned> owned;
static unsigned long indexEntity[MAX_NPC];
static bool managed=false;
static long revision=0,lease=0,boot=0;
static int dispatchEvent=0;
static unsigned long nextSession=0;

static bool Read(const char* path,std::string& bytes,unsigned int max) {
    bytes="";HANDLE f=CreateFileA(path,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
    if(f==INVALID_HANDLE_VALUE)return false;
    DWORD high=0,size=GetFileSize(f,&high),read=0;bool ok=!high&&size>0&&size<=max;
    if(ok){std::vector<char> data(size);ok=ReadFile(f,&data[0],size,&read,NULL)&&read==size;if(ok)bytes.assign(&data[0],size);}
    CloseHandle(f);return ok;
}
static bool Write(const char* path,const std::string& bytes) {
    char temporary[400];if(strlen(path)>350)return false;sprintf(temporary,"%s.tmp",path);
    HANDLE f=CreateFileA(temporary,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);if(f==INVALID_HANDLE_VALUE)return false;
    DWORD written=0;bool ok=WriteFile(f,bytes.data(),bytes.size(),&written,NULL)&&written==bytes.size()&&FlushFileBuffers(f);CloseHandle(f);
    if(ok)ok=MoveFileExA(temporary,path,MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH)!=0;
    if(!ok)DeleteFileA(temporary);return ok;
}
static bool Number(const std::string& s,long& n,long low,long high) {
    if(s.empty()||s.size()>10)return false;char* tail=0;errno=0;n=strtol(s.c_str(),&tail,10);
    return errno!=ERANGE&&tail&&!*tail&&n>=low&&n<=high;
}
static std::vector<std::string> Split(const std::string& line,char delimiter) {
    std::vector<std::string> parts;size_t start=0;
    for(size_t at=0;at<=line.size();++at)if(at==line.size()||line[at]==delimiter){parts.push_back(line.substr(start,at-start));start=at+1;}
    return parts;
}
static int HexDigit(char c){if(c>='0'&&c<='9')return c-'0';if(c>='a'&&c<='f')return c-'a'+10;if(c>='A'&&c<='F')return c-'A'+10;return -1;}
static bool Hex(const std::string& raw,char* out,unsigned int capacity) {
    if(raw.size()%2||raw.size()/2>=capacity)return false;
    for(unsigned int i=0;i<raw.size();i+=2){int a=HexDigit(raw[i]),b=HexDigit(raw[i+1]);if(a<0||b<0||(a==0&&b==0))return false;out[i/2]=(char)((a<<4)|b);}
    out[raw.size()/2]=0;return true;
}
static bool TableName(const char* s) {
    unsigned int n=strlen(s);if(n<5||n>90||_stricmp(s+n-4,".ini"))return false;
    for(unsigned int i=0;i<n-4;++i)if(!((s[i]>='a'&&s[i]<='z')||(s[i]>='A'&&s[i]<='Z')||(s[i]>='0'&&s[i]<='9')||s[i]=='_'||s[i]=='-'))return false;
    return true;
}
static bool LoadCatalog() {
    std::string bytes;if(!Read("EventService\\catalog.tsv",bytes,65536))return false;
    std::map<int,Definition> candidate;std::vector<std::string> lines=Split(bytes,'\n');
    for(unsigned int i=0;i<lines.size();++i){
        std::string line=lines[i];if(!line.empty()&&line[line.size()-1]=='\r')line.erase(line.size()-1);if(line.empty())continue;
        std::vector<std::string> row=Split(line,'\t');if(row.size()!=10)return false;long n[8];
        for(int j=0;j<8;++j)if(!Number(row[j],n[j],j==7?-1:0,j==0?4095:1000000))return false;
        Definition d;d.id=n[0];d.adapter=n[1];d.map=n[2];d.content=n[3];d.mission=n[4];d.tid=n[5];d.flag=n[7];
        if(d.id<1||d.adapter<1||d.adapter>4||candidate.find(d.id)!=candidate.end()||!Hex(row[8],d.start,sizeof(d.start))||!Hex(row[9],d.label,sizeof(d.label)))return false;
        if(d.adapter!=3&&d.adapter!=4&&(_strnicmp(d.start,"\\script\\",8)||strstr(d.start,"..")))return false;
        if(d.adapter!=3&&d.adapter!=4&&!g_GetScript(d.start)&&(!ReLoadScript(d.start)||!g_GetScript(d.start)))return false;
        if(d.adapter==1&&(d.map<1067||d.map>1070||d.mission!=d.map-1066))return false;
        if(d.adapter==2&&(d.tid<0||d.tid>=MAX_NPCSTYLE||d.flag<0||d.flag>=TASKGLOBALVALUENUM))return false;
        candidate[d.id]=d;if(candidate.size()>256)return false;
    }
    if(candidate.empty())return false;definitions.swap(candidate);return true;
}
static bool LoadSnapshot(long utc) {
    std::string bytes;if(!Read("EventService\\snapshot.tsv",bytes,65536))return false;
    if(bytes.find('\0')!=std::string::npos)return false;
    std::vector<std::string> lines=Split(bytes,'\n');if(lines.empty())return false;
    std::vector<std::string> head=Split(lines[0],'\t');long version=0,rev=0,until=0,count=0;
    if(head.size()!=5||head[0]!="PT_EVENT_SNAPSHOT"||!Number(head[1],version,2,2)||!Number(head[2],rev,1,1000000000)||
       !Number(head[3],until,0,2147483647)||!Number(head[4],count,1,256)||count!=definitions.size()||until>utc+30)return false;
    if(rev<revision)return false;
    std::map<int,Rule> candidate;
    for(unsigned int i=1;i<lines.size();++i){
        if(lines[i].empty())continue;std::vector<std::string> row=Split(lines[i],'\t');if(row.size()!=30)return false;long n[11];
        for(int j=0;j<11;++j)if(!Number(row[j],n[j],0,1000000000))return false;
        Rule r;r.id=n[0];r.enabled=n[1];r.registration=n[2];r.duration=n[3];r.genre=n[4];r.detail=n[5];r.particular=n[6];r.level=n[7];r.series=n[8];r.count=n[9];r.mode=n[10];
        if(definitions.find(r.id)==definitions.end()||candidate.find(r.id)!=candidate.end()||r.enabled>1||r.registration<1||r.registration>3600||
           r.duration<60||r.duration>86400||r.genre>8||r.level>1000||r.series>5||r.count>1000||r.mode>1)return false;
        for(int k=0;k<4;++k){if(row[11+k]=="-")r.slots[k][0]=0;else{if(row[11+k].size()>=sizeof(r.slots[k])||!TableName(row[11+k].c_str()))return false;strcpy(r.slots[k],row[11+k].c_str());}}
        for(int q=0;q<15;++q){long v=0;if(!Number(row[15+q],v,0,1000000000))return false;r.quest[q]=(int)v;}
        if(r.quest[0]<1||r.quest[0]>1000||r.quest[1]<1||r.quest[1]>100||r.quest[2]>r.quest[1]||r.quest[3]>8||
           r.quest[6]>1000||r.quest[7]>5||r.quest[8]>64||r.quest[11]>1000000||r.quest[12]>1||r.quest[13]>1000000||r.quest[14]>1)return false;
        candidate[r.id]=r;
    }
    if(candidate.size()!=definitions.size())return false;
    rules.swap(candidate);revision=rev;lease=until;return true;
}
static int World(const Definition& d){return g_SubWorldSet.SearchWorld(d.map);}
static int Boss(const Definition& d) {
    for(int n=NpcSet.GetNextIdx(0);n;n=NpcSet.GetNextIdx(n)){int world=Npc[n].m_SubWorldIndex;
        if(Npc[n].m_NpcSettingIdx==d.tid&&!Npc[n].IsPlayer()&&Npc[n].m_CurrentLife>0&&Npc[n].IsAlive()&&!PhongThanRealmIsHuntNpc(n)&&world>=0&&world<MAX_SUBWORLD&&SubWorld[world].m_SubWorldID==d.map)return n;}
    return 0;
}
static void RemoveDeadBosses(const Definition& d){
 for(int n=NpcSet.GetNextIdx(0);n;){int next=NpcSet.GetNextIdx(n),w=Npc[n].m_SubWorldIndex;
  if(Npc[n].m_NpcSettingIdx==d.tid&&!Npc[n].IsPlayer()&&!PhongThanRealmIsHuntNpc(n)&&w>=0&&w<MAX_SUBWORLD&&SubWorld[w].m_SubWorldID==d.map&&(Npc[n].m_CurrentLife<=0||!Npc[n].IsAlive())){
   int region=Npc[n].m_RegionIndex;if(region>=0&&region<SubWorld[w].m_nTotalRegion){SubWorld[w].m_Region[region].RemoveNpc(n);SubWorld[w].m_Region[region].DecRef(Npc[n].m_MapX,Npc[n].m_MapY,obj_npc);}NpcSet.Remove(n);
  }n=next;
 }
}
static bool Lua(const char* path,const char* callback,int parameter,int world) {
    KLuaScript* script=(KLuaScript*)g_GetScript(path);if(!script||!script->m_LuaState)return false;
    Lua_PushNumber(script->m_LuaState,world);script->SetGlobalName(SCRIPT_SUBWORLDINDEX);
    int top=0;script->SafeCallBegin(&top);BOOL ok=script->CallFunction((char*)callback,0,"d",parameter);script->SafeCallEnd(top);return ok!=FALSE;
}
static void Timer(KMission* mission,int id,int frames) {
    KTimerTaskFun key;key.SetTimer(1,id);KTimerTaskFun* old=mission->m_cTimerTaskSet.GetData(&key);
    if(old)mission->m_cTimerTaskSet.Remove(old);
    KTimerTaskFun* timer=mission->m_cTimerTaskSet.Add();if(timer){timer->SetOwner(mission);timer->SetTimer(frames,id);}
}
static void Announce(const Definition& d,const char* action,int seconds) {
    KPhongThanScriptAction message;ZeroMemory(&message,sizeof(message));message.View=UI_NEWSINFO;message.OptionCount=NEWSMESSAGE_NORMAL;
    message.Operation=PHONGTHAN_SCRIPT_SHOW;message.ServerOwned=1;
    const char* display=!strcmp(action,"sap mo")?"\x73\xBE\x70\x20\x6D\xEB":!strcmp(action,"da mo")?"\xAE\xB7\x20\x6D\xEB":"\xAE\xB7\x20\x6B\xD5\x74\x20\x74\x68\xF3\x63";
    if(seconds>0)sprintf(message.Content,"\x25\x73\x3A\x20\x25\x73\x20\x74\x72\x6F\x6E\x67\x20\x25\x64\x20\x70\x68\xF3\x74\x2E",d.label,display,(seconds+59)/60);else sprintf(message.Content,"%s: %s.",d.label,display);
    int n=strlen(message.Content),repeat=1;memcpy(message.Content+n,&repeat,4);message.ContentLength=n+4;
    PHONGTHAN_U8 packet[sizeof(PHONGTHAN_UI_ACTION_HEADER)+PHONGTHAN_UI_MAX_KEY+PHONGTHAN_UI_MAX_CONTENT];
    unsigned int size=PhongThanBuildScriptPacket(message,packet,sizeof(packet));if(size)PhongThanQueueUiBroadcast(packet,size);
}
static bool Open(int id,std::string& message) {
    Definition& d=definitions[id];State& state=states[id];Rule& current=rules[id];int world=World(d);
    if(!current.enabled){message="disabled";return false;}if(d.adapter!=3&&world<0){message="map_not_ready";return false;}
    if(d.adapter==4&&PhongThanThuongChuPhase()){message="already_active";return true;}
    if(d.adapter==1&&SubWorld[world].m_MissionArray.FindByMissionId(d.mission)){message="already_active";return true;}
    if(d.adapter==2&&Boss(d)){message="already_active";return true;}
    state.active=current;state.activeRevision=revision;state.session=++nextSession;state.phase=1;state.error[0]=0;
    if(d.adapter==3){state.closed=false;state.acceptUntil=(long)time(NULL)+current.duration;message="accepting_new_entries";return true;}
    if(d.adapter==4){bool ok=PhongThanThuongChuOpen(current.registration,current.duration);if(!ok){state.session=0;state.phase=0;message="battle_start_failed";return false;}Announce(d,"da mo bao danh",current.registration);message="battle_registration_open";return true;}
    // A stale global flag or corpse must not suppress the next scheduled live boss.
    if(d.adapter==2){RemoveDeadBosses(d);if(d.flag>=0)g_TaskGlobalValue[d.flag]=-1;}
    dispatchEvent=id;bool ok=Lua(d.start,"main",0,world);dispatchEvent=0;
    if(d.adapter==1){
        KMission* mission=SubWorld[world].m_MissionArray.FindByMissionId(d.mission);
        ok=ok&&mission&&g_TaskGlobalValue[d.mission]>0;
        if(ok)Timer(mission,2999+d.mission*2,current.registration*GAME_FPS);
    }else ok=ok&&Boss(d)>0;
    if(!ok){state.session=0;state.phase=0;strcpy(state.error,"native_start_failed");message=state.error;return false;}
    Announce(d,"da mo",0);message="native_started";return true;
}
static bool Close(int id,std::string& message) {
    Definition& d=definitions[id];State& state=states[id];int world=World(d);
    if(d.adapter==4){PhongThanThuongChuClose();message="battle_closing";return true;}
    if(d.adapter==1&&world>=0){
        KMission* mission=SubWorld[world].m_MissionArray.FindByMissionId(d.mission);
        if(mission){char path[100];sprintf(path,"\\script\\phongthan\\van_tien\\mission%02d.lua",d.mission);
            if(!Lua(path,"EndMission",d.mission,world)){message="native_end_failed";return false;}
            mission->StopMission();SubWorld[world].m_MissionArray.Remove(mission);}
    }else if(d.adapter==2){
        // Only entities spawned by this adapter may be removed. An already
        // existing/static boss is deliberately never adopted by an open call.
        std::map<unsigned long,Owned>::iterator it=owned.begin();
        for(;it!=owned.end();++it)if(it->second.eventId==id&&it->second.session==state.session){
            int n=it->second.index;if(n>0&&n<MAX_NPC&&Npc[n].m_dwID==it->second.entity&&!Npc[n].IsPlayer()){
                int w=Npc[n].m_SubWorldIndex,r=Npc[n].m_RegionIndex;
                if(w>=0&&w<MAX_SUBWORLD&&r>=0&&r<SubWorld[w].m_nTotalRegion){SubWorld[w].m_Region[r].RemoveNpc(n);SubWorld[w].m_Region[r].DecRef(Npc[n].m_MapX,Npc[n].m_MapY,obj_npc);}
                NpcSet.Remove(n);
            }
        }
        if(!Boss(d)&&d.flag>=0)g_TaskGlobalValue[d.flag]=-1;
    }
    state.session=0;state.phase=0;state.closed=true;Announce(d,"da ket thuc luot",0);message="native_closed";return true;
}
static bool Ack(const char* id,const char* state,const std::string& message,long utc) {
    char path[160],text[512];sprintf(path,"EventService\\acks\\%s.json",id);
    // All messages above are fixed ASCII codes, never user-supplied text.
    sprintf(text,"{\"Id\":\"%s\",\"State\":\"%s\",\"Message\":\"%s\",\"Pid\":%lu,\"BootUtc\":%ld,\"UtcSeconds\":%ld}",id,state,message.c_str(),GetCurrentProcessId(),boot,utc);return Write(path,text);
}
static void Commands(long utc) {
    WIN32_FIND_DATAA found;HANDLE search=FindFirstFileA("EventService\\pending\\*.cmd",&found);if(search==INVALID_HANDLE_VALUE)return;
    int count=0;DWORD started=GetTickCount();
    do{
        if(++count>4||(DWORD)(GetTickCount()-started)>10)break;
        const char* name=found.cFileName;if(strlen(name)!=36||strcmp(name+32,".cmd"))continue;bool idOk=true;
        for(int k=0;k<32;++k)if(HexDigit(name[k])<0)idOk=false;if(!idOk)continue;
        char id[33],path[160],ack[160];memcpy(id,name,32);id[32]=0;sprintf(path,"EventService\\pending\\%s",name);sprintf(ack,"EventService\\acks\\%s.json",id);
        if(GetFileAttributesA(ack)!=(DWORD)-1){DeleteFileA(path);continue;}
        std::string bytes;if(!Read(path,bytes,2048))continue;std::vector<std::string> row=Split(bytes,'\t');
        if(row.size()!=11||row[0]!="PT_EVENT_COMMAND"||row[1]!="1"||row[2]!=id){Ack(id,"rejected","bad_command",utc);DeleteFileA(path);continue;}
        if(!row[10].empty()&&row[10][row[10].size()-1]=='\n')row[10].erase(row[10].size()-1);
        long pid=0,born=0,issued=0,expires=0,event=0,rev=0,argument=0;
        if(!Number(row[3],pid,1,2147483647)||!Number(row[4],born,1,2147483647)||!Number(row[5],issued,1,2147483647)||
           !Number(row[6],expires,1,2147483647)||!Number(row[7],event,1,4095)||!Number(row[9],rev,1,1000000000)||!Number(row[10],argument,0,86400)){
            Ack(id,"rejected","bad_fields",utc);DeleteFileA(path);continue;}
        const char* reason=0;
        if(pid!=(long)GetCurrentProcessId()||born!=boot)reason="stale_server_boot";
        else if(issued>utc+5||expires<utc||expires-issued>60)reason="expired_command";
        else if(definitions.find(event)==definitions.end()||rules.find(event)==rules.end())reason="unknown_event";
        else if(rev!=revision)reason="revision_changed";
        else if(row[8]!="open"&&row[8]!="close"&&row[8]!="warning")reason="unknown_operation";
        else if(row[8]!="close"&&lease<utc)reason="service_lease_expired";
        if(reason){Ack(id,"rejected",reason,utc);DeleteFileA(path);continue;}
        if(!Ack(id,"executing","world_thread",utc))continue;
        std::string message;bool ok=false;
        if(row[8]=="open")ok=Open(event,message);else if(row[8]=="close")ok=Close(event,message);
        else {Announce(definitions[event],"sap mo",argument);ok=true;message="warning_sent";}
        Ack(id,ok?"applied":"rejected",message,utc);DeleteFileA(path);
    }while(FindNextFileA(search,&found));FindClose(search);
}
// Recreate each enabled golden boss from its own authored script once per world boot.
// Map-file placeholders must not suppress the actual event spawn or its level/position.
static void RestoreGoldenBosses(long utc){
 static std::map<int,bool> done;static std::map<int,long> attempts;
 if(lease<utc||rules.size()!=definitions.size())return;
 for(std::map<int,Definition>::iterator it=definitions.begin();it!=definitions.end();++it){
  Definition& d=it->second;if(d.adapter!=2||done[d.id])continue;
  if(!rules[d.id].enabled){done[d.id]=true;continue;}
  if(World(d)<0||attempts[d.id]&&utc-attempts[d.id]<30)continue;attempts[d.id]=utc;
  for(int n=NpcSet.GetNextIdx(0);n;){int next=NpcSet.GetNextIdx(n),w=Npc[n].m_SubWorldIndex;
   if(Npc[n].m_NpcSettingIdx==d.tid&&!Npc[n].IsPlayer()&&!PhongThanRealmIsHuntNpc(n)&&w>=0&&w<MAX_SUBWORLD&&SubWorld[w].m_SubWorldID==d.map){
    int region=Npc[n].m_RegionIndex;if(region>=0&&region<SubWorld[w].m_nTotalRegion){SubWorld[w].m_Region[region].RemoveNpc(n);SubWorld[w].m_Region[region].DecRef(Npc[n].m_MapX,Npc[n].m_MapY,obj_npc);}NpcSet.Remove(n);
   }n=next;
  }
  if(d.flag>=0)g_TaskGlobalValue[d.flag]=-1;std::string message;
  if(Open(d.id,message)&&Boss(d)>0)done[d.id]=true;
 }
}
static void Status(long utc) {
    char line[512];sprintf(line,"{\"Version\":1,\"Pid\":%lu,\"BootUtc\":%ld,\"UtcSeconds\":%ld,\"Revision\":%ld,\"LeaseValid\":%s,\"Events\":[",GetCurrentProcessId(),boot,utc,revision,lease>=utc?"true":"false");
    std::string output=line;bool first=true;
    std::map<int,Definition>::iterator it=definitions.begin();
    for(;it!=definitions.end();++it){Definition& d=it->second;State& s=states[d.id];int world=World(d),phase=0;
        if(d.adapter==1&&world>=0&&SubWorld[world].m_MissionArray.FindByMissionId(d.mission))phase=g_TaskGlobalValue[d.mission];
        else if(d.adapter==2&&Boss(d))phase=2;
        else if(d.adapter==4)phase=PhongThanThuongChuPhase();
        else if(d.adapter==3&&!s.closed&&rules.find(d.id)!=rules.end()&&rules[d.id].enabled&&lease>=utc&&(!rules[d.id].quest[14]||s.acceptUntil>=utc))phase=1;
        if(phase==0&&d.adapter!=3)s.session=0;s.phase=phase;
        sprintf(line,"%s{\"Id\":%d,\"Phase\":%d,\"ActiveRevision\":%ld,\"Session\":%lu,\"Error\":\"%s\"}",first?"":",",d.id,phase,s.session?s.activeRevision:0,s.session,s.error);first=false;output+=line;
    }
    output+="]}";Write("EventService\\status.json",output);
}
}

bool PhongThanEventOwns(int id){return PtEvents::managed&&PtEvents::definitions.find(id)!=PtEvents::definitions.end();}
bool PhongThanEventAccepting(int id){long now=(long)time(NULL);return PhongThanEventOwns(id)&&PtEvents::lease>=now&&PtEvents::rules.find(id)!=PtEvents::rules.end()&&PtEvents::rules[id].enabled&&
    (PtEvents::definitions[id].adapter!=3||(!PtEvents::states[id].closed&&(!PtEvents::rules[id].quest[14]||PtEvents::states[id].acceptUntil>=now)));}
bool PhongThanEventOwnsSchedule(const char* path){if(!PtEvents::managed||!path)return false;std::map<int,PtEvents::Definition>::iterator it=PtEvents::definitions.begin();for(;it!=PtEvents::definitions.end();++it)if(!_stricmp(it->second.start,path))return true;return false;}
int PhongThanEventValue(int id,int field){
    if(!PhongThanEventOwns(id)||PtEvents::rules.find(id)==PtEvents::rules.end())return -1;
    PtEvents::State& state=PtEvents::states[id];const PtEvents::Rule& r=state.session&&PtEvents::definitions[id].adapter!=3?state.active:PtEvents::rules[id];
    if(field>=10&&field<25)return r.quest[field-10];
    switch(field){case 1:return r.registration;case 2:return r.duration;case 3:return r.genre;case 4:return r.detail;case 5:return r.particular;case 6:return r.level;case 7:return r.series;case 8:return r.count;case 9:return r.mode;}return -1;
}
void PhongThanEventBindNpc(int index){
    if(!PtEvents::managed||index<=0||index>=MAX_NPC||!Npc[index].m_dwID||Npc[index].IsPlayer())return;int id=PtEvents::dispatchEvent;
    if(!id){int world=Npc[index].m_SubWorldIndex;if(world<0||world>=MAX_SUBWORLD)return;int map=SubWorld[world].m_SubWorldID;if(map>=1067&&map<=1070)id=map-1066;}
    if(!id||PtEvents::states.find(id)==PtEvents::states.end()||!PtEvents::states[id].session)return;
    PtEvents::Owned entry;entry.index=index;entry.eventId=id;entry.entity=Npc[index].m_dwID;entry.session=PtEvents::states[id].session;entry.mode=PtEvents::states[id].active.mode;memcpy(entry.slots,PtEvents::states[id].active.slots,sizeof(entry.slots));
    if(PtEvents::indexEntity[index])PtEvents::owned.erase(PtEvents::indexEntity[index]);PtEvents::indexEntity[index]=entry.entity;PtEvents::owned[entry.entity]=entry;
}
bool PhongThanEventApplyDropProfile(int index,unsigned long entity,std::string slots[4]){
    std::map<unsigned long,PtEvents::Owned>::iterator it=PtEvents::owned.find(entity);if(it==PtEvents::owned.end()||it->second.index!=index||it->second.mode!=1)return false;
    for(int i=0;i<4;++i)if(it->second.slots[i][0])slots[i]=it->second.slots[i];return true;
}
bool PhongThanEventLoadTable(const std::string& path,std::string& bytes,std::string& error,bool& handled){
    handled=false;const char* base=strrchr(path.c_str(),'\\');base=base?base+1:path.c_str();if(strncmp(base,"pt_event_",9))return false;handled=true;
    int id=0,rev=0,slot=0;char extra=0,canonical[100];if(sscanf(base,"pt_event_%d_r%d_s%d.ini%c",&id,&rev,&slot,&extra)!=3||id<1||id>4095||rev<1||slot<0||slot>3){error="EVENT_TABLE_NAME_INVALID";return false;}
    sprintf(canonical,"pt_event_%d_r%d_s%d.ini",id,rev,slot);if(strcmp(base,canonical)){error="EVENT_TABLE_NAME_INVALID";return false;}
    char physical[180];sprintf(physical,"EventService\\drop\\%s",canonical);if(!PtEvents::Read(physical,bytes,2*1024*1024)){error="EVENT_TABLE_MISSING";return false;}error="";return true;
}
void PhongThanLiveCampaignTick();
void PhongThanEventControlTick(){
    PhongThanLiveCampaignTick();
    static DWORD last=0;DWORD now=GetTickCount();if((DWORD)(now-last)<1000)return;last=now;
    PhongThanRealmQuestTick();
    PhongThanTwoFactionQuestTick();
    if(GetFileAttributesA("EventService\\managed.enabled")==((DWORD)-1))return;
    if(!PtEvents::managed){if(!PtEvents::LoadCatalog())return;PtEvents::managed=true;PtEvents::boot=(long)time(NULL);PtEvents::nextSession=PtEvents::boot;}
    long utc=(long)time(NULL);PtEvents::LoadSnapshot(utc);PtEvents::RestoreGoldenBosses(utc);PtEvents::Commands(utc);PtEvents::Status(utc);PtVttGates::Sync();PhongThanVttArenaTick();PhongThanDailyTick();PhongThanTranNguyenTick();PhongThanGiapGatherTick();
}
