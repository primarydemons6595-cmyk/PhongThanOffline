#include "PhongThanVttArena.h"
#include "PhongThanVttLayout.h"
#include <map>
#include <vector>
#include "../../../Headers/PhongThanVttBarrierProtocol.h"
#include "../../../Headers/PhongThanVttSafeCells.h"
namespace PtArena {
struct Entity {int index;DWORD serial;bool broken;Entity():index(0),serial(0),broken(false){}};
struct Arena {int generation;std::vector<Entity> walls;Entity gate,gate2;Arena():generation(0){}};
static Arena arenas[4];
typedef std::pair<const KRegion*,int> Cell;
struct Block {int id,segment;Block():id(0),segment(-1){}Block(int a,int b):id(a),segment(b){}};
static std::map<Cell,Block> barrier;
static void Remove(Entity& e){if(e.index>0&&e.index<MAX_NPC&&Npc[e.index].m_dwID==e.serial)PhongThanRemoveNpc(e.index);e.index=0;e.serial=0;}
static bool Live(const Entity& e){return e.index>0&&e.index<MAX_NPC&&Npc[e.index].m_dwID==e.serial;}
static void Spawn(Entity& e,int world,int tid,int x,int y,const char* script,bool destructible=false){
    if(Live(e)||e.broken)return;e.index=NpcSet.Add(MAKELONG(1,tid),world,x,y,FALSE);if(e.index<=0){e.serial=0;return;}
    e.serial=Npc[e.index].m_dwID;Npc[e.index].m_Kind=kind_dialoger;Npc[e.index].m_AiMode=0;
    if(destructible){KNpc& n=Npc[e.index];n.m_Kind=kind_normal;n.m_Camp=n.m_CurrentCamp=camp_animal;n.m_CurrentLife=n.m_CurrentLifeMax=30000;n.m_Experience=0;n.m_DeathScriptID=0;}
    g_StrCpyLen(Npc[e.index].Name,script?"Cöa V¹n Tiªn TrËn":"Hµng rµo",sizeof(Npc[e.index].Name));
    if(script){if(!g_GetScript(g_FileName2Id((char*)script)))ReLoadScript((char*)script);g_StrCpyLen(Npc[e.index].ActionScript,script,sizeof(Npc[e.index].ActionScript));Npc[e.index].m_ActionScriptID=g_FileName2Id((char*)script);}
}
static void Put(KMission* mission,int slot,int value){char text[32];sprintf(text,"%d",value);mission->SetMission(slot,text);}
static bool Member(int p,KMission* mission){unsigned long c=0;while((c=mission->m_MissionPlayer.m_UseIdx.GetNext(c))!=0){TMissionPlayerInfo& m=mission->m_MissionPlayer.m_Data[c];if(m.m_ulPlayerIndex==p&&m.m_ulPlayerID==Player[p].m_dwID)return true;}return false;}
static void Grant(int p,int id){
    int task=4670+id,bits=Player[p].m_cTask.GetSaveVal(task);
    for(int slot=0;slot<4;++slot)if(bits&(1<<slot)){
        PhongThanQuestExchangeRow row;row.count=1;if(!PhongThanDecodeQuestItemTuple(3,66+slot,0,0,0,0,row.item))continue;
        int next=bits&~(1<<slot);if(PhongThanQuestPayout(p,task,bits,next,&row,1,0,0,0)==1)bits=next;
    }
}
static void GuardKilled(int p,int npc){
    if(!PtVtt::Live(p))return;int world=Npc[npc].m_SubWorldIndex,id=SubWorld[world].m_SubWorldID-1066,slot=Npc[npc].m_NpcSettingIdx-(129+(id-1)*5);
    KMission* mission=PtVtt::Mission(id);if(id<1||id>4||slot<0||slot>3||!mission||!Member(p,mission)||Npc[Player[p].m_nIndex].m_SubWorldIndex!=world)return;
    int generation=PtVtt::Generation(mission);if(Player[p].m_cTask.GetSaveVal(4660+id)!=generation){Player[p].m_cTask.SetSaveVal(4660+id,generation,TRUE);Player[p].m_cTask.SetSaveVal(4670+id,0,TRUE);}
    Player[p].m_cTask.SetSaveVal(4670+id,Player[p].m_cTask.GetSaveVal(4670+id)|(1<<slot),TRUE);Grant(p,id);
}
static bool Anchor(int p,int id,KMission* mission){
    if(!PtVtt::Live(p)||!mission||id<1||id>4||!Member(p,mission))return false;int npc=Player[p].m_nLastNpcIndex,actor=Player[p].m_nIndex;
    if(!((Live(arenas[id-1].gate)&&npc==arenas[id-1].gate.index)||(Live(arenas[id-1].gate2)&&npc==arenas[id-1].gate2.index))||Npc[actor].m_SubWorldIndex!=Npc[npc].m_SubWorldIndex)return false;
    int x,y,nx,ny;Npc[actor].GetMpsPos(&x,&y);Npc[npc].GetMpsPos(&nx,&ny);return PtEscortNear(x,y,nx,ny,512);
}
static int Open(int p,int id,int op){
    KMission* mission=PtVtt::Mission(id);if(!Anchor(p,id,mission))return -1;Grant(p,id);
    if(op==0)return mission->GetMissionValue(34)?1:0;
    if(g_TaskGlobalValue[id]!=2)return -2;
    if(op==1&&!mission->GetMissionValue(34)){
        for(int killSlot=11;killSlot<=14;++killSlot)if(!mission->GetMissionValue(killSlot))return -3;
        PhongThanQuestExchangeRow cost[4];for(int slot=0;slot<4;++slot){cost[slot].count=1;if(!PhongThanDecodeQuestItemTuple(3,66+slot,0,0,0,0,cost[slot].item))return -4;}
        int task=4680+id,old=Player[p].m_cTask.GetSaveVal(task),generation=PtVtt::Generation(mission);
        if(old==generation||PhongThanQuestExchange(p,task,old,generation,cost,4,NULL,0)!=1)return -4;
        mission->SetMission(34,"1");int world=g_SubWorldSet.SearchWorld(1066+id);char path[96];sprintf(path,"\\script\\phongthan\\van_tien\\mission%02d.lua",id);mission->ExecuteScript(path,"pt_progress",id);
    }
    if(op==1){if(mission->GetMissionValue(34)){Remove(arenas[id-1].gate);Remove(arenas[id-1].gate2);PhongThanVttBarrierNotify(0);}return mission->GetMissionValue(34)?1:-3;}
    if(op==2)return mission->GetMissionValue(34)?1:-3;
    return 0;
}
}
// A live segment blocks walking and jumping until it has actually been destroyed.
bool PhongThanVttBarrierAt(const KRegion* region,int x,int y){
    std::map<PtArena::Cell,PtArena::Block>::const_iterator it=PtArena::barrier.find(PtArena::Cell(region,y*16+x));
    if(it==PtArena::barrier.end())return false;const PtArena::Block& b=it->second;
    if(b.id<1||b.id>4)return false;
    KMission* mission=PtVtt::Mission(b.id);if(!mission||!g_TaskGlobalValue[b.id])return false;
    if(b.segment<0)return !mission->GetMissionValue(34);
    
    const PtArena::Arena& a=PtArena::arenas[b.id-1];
    if(b.segment>=(int)a.walls.size())return true;
    const PtArena::Entity& e=a.walls[b.segment];
    if(e.broken)return false;
    return !PtArena::Live(e)||Npc[e.index].m_CurrentLife>0;
}
static bool PtVttCellBlocked(int world,int gx,int gy){
    int r,x,y,ox,oy;SubWorld[world].Mps2Map(gx*32+16,gy*32+16,&r,&x,&y,&ox,&oy);
    return r>=0&&PhongThanVttBarrierAt(&SubWorld[world].m_Region[r],x,y);
}
bool PhongThanVttSegmentBlocked(int world,int x0,int y0,int x1,int y1){
    if(world<0||world>=MAX_SUBWORLD||SubWorld[world].m_SubWorldID<1067||SubWorld[world].m_SubWorldID>1070)return false;
    int dx=x1-x0,dy=y1-y0,steps=(max(abs(dx),abs(dy))+7)/8;
    if(steps<=0)return false;int px=x0/32,py=y0/32;
    for(int n=1;n<=steps;++n){int gx=(x0+(int)((double)dx*n/steps))/32,gy=(y0+(int)((double)dy*n/steps))/32;
        if(PtVttCellBlocked(world,gx,gy))return true;
        if(gx!=px&&gy!=py&&(PtVttCellBlocked(world,px,gy)||PtVttCellBlocked(world,gx,py)))return true;
        px=gx;py=gy;
    }
    return false;
}
bool PhongThanVttCanDamage(int target,int launcher){
    for(int id=1;id<=4;++id){PtArena::Arena& a=PtArena::arenas[id-1];
        for(unsigned int n=0;n<a.walls.size();++n){PtArena::Entity& e=a.walls[n];
            if(!PtArena::Live(e)||e.index!=target)continue;
            if(g_TaskGlobalValue[id]!=2)return false;
            if(launcher<=0||launcher>=MAX_NPC||Npc[launcher].m_SubWorldIndex!=Npc[target].m_SubWorldIndex)return false;
            int p=Npc[launcher].IsPlayer()?Npc[launcher].GetPlayerIdx():0;
            // Summons may hit only while their owning arena is in combat; no damage before RunMission.
            KMission* mission=PtVtt::Mission(id);if(!mission||p>0&&!PtArena::Member(p,mission))return false;
            return true;
        }
    }
    return true;
}
#ifndef PT_VTT_TEST_HOST
static void PtVttSendBarrierState(int id){
    int world=-1;KMission* mission=PtVtt::Mission(id,&world);if(!mission||world<0)return;
    const PT_VTT_LAYOUT& layout=PT_VTT_LAYOUTS[id-1];PtArena::Arena& a=PtArena::arenas[id-1];
    PT_VTT_BARRIERS packet;memset(&packet,0,sizeof(packet));PhongThanInitializeWireHeader(&packet.Header,PT_VTT_BARRIER_STATE,sizeof(packet),PHONGTHAN_WIRE_FLAG_RESPONSE,0);
    packet.MapId=1066+id;packet.Session=PtVtt::Generation(mission);packet.Phase=g_TaskGlobalValue[id];packet.GateClosed=packet.Phase&&!mission->GetMissionValue(34);packet.Sequence=GetTickCount();
    if(packet.Phase)for(int n=0;n<layout.visualCount;++n){if(n>=(int)a.walls.size()){packet.ClosedWalls|=((PT_VTT_MASK)1)<<n;continue;}const PtArena::Entity& e=a.walls[n];if(!e.broken&&(!PtArena::Live(e)||Npc[e.index].m_CurrentLife>0))packet.ClosedWalls|=((PT_VTT_MASK)1)<<n;}
    for(int p=1;p<MAX_PLAYER;++p)if(PtVtt::Live(p)&&Npc[Player[p].m_nIndex].m_SubWorldIndex==world)g_pServer->PackDataToClient(Player[p].m_nNetConnectIdx,(BYTE*)&packet,sizeof(packet));
}
#else
static void PtVttSendBarrierState(int){}
#endif
void PhongThanVttBarrierNotify(int npc){for(int id=1;id<=4;++id){if(!npc){PtVttSendBarrierState(id);continue;}PtArena::Arena& a=PtArena::arenas[id-1];for(unsigned int n=0;n<a.walls.size();++n)if(a.walls[n].index==npc&&PtArena::Live(a.walls[n])){PtVttSendBarrierState(id);break;}}}
bool PhongThanVttMonsterCanMove(int index,int x,int y){
    if(index<=0||index>=MAX_NPC||Npc[index].IsPlayer())return true;int world=Npc[index].m_SubWorldIndex;if(world<0||world>=MAX_SUBWORLD)return true;
    int id=SubWorld[world].m_SubWorldID-1066;if(id<1||id>4)return true;int tid=Npc[index].m_NpcSettingIdx;
    if(!((tid==51+id*2||tid==52+id*2)||(tid>=129+(id-1)*5&&tid<=133+(id-1)*5)))return true;
    return !PhongThanVttSafeCell(id,x/32,y/32);
}
void PhongThanVttArenaTick(){
    g_PtVttGuardCredit=PtArena::GuardKilled;PtArena::barrier.clear();
    for(int id=1;id<=4;++id){PtArena::Arena& a=PtArena::arenas[id-1];const PT_VTT_LAYOUT& layout=PT_VTT_LAYOUTS[id-1];int world=-1;KMission* mission=PtVtt::Mission(id,&world);int phase=g_TaskGlobalValue[id],gen=PtVtt::Generation(mission);
        if(!mission||!phase||a.generation!=gen){for(unsigned int i=0;i<a.walls.size();++i)PtArena::Remove(a.walls[i]);a.walls.clear();PtArena::Remove(a.gate);PtArena::Remove(a.gate2);a.gate.broken=false;a.gate2.broken=false;a.generation=gen;}
        if(!mission||!phase||world<0)continue;
        if(!mission->GetMissionValue(34)){
            PtArena::Spawn(a.gate,world,PT_VTT_GATE_TEMPLATE,layout.gateX,layout.gateY,"\\script\\phongthan\\van_tien\\inner_gate.lua");
            if(PtArena::Live(a.gate)){PtArena::Put(mission,32,a.gate.index);PtArena::Put(mission,33,a.gate.serial);}
            if(layout.entranceCount>1)PtArena::Spawn(a.gate2,world,PT_VTT_GATE_TEMPLATE,layout.entrances[1].x,layout.entrances[1].y,"\\script\\phongthan\\van_tien\\inner_gate.lua");
        }else{PtArena::Remove(a.gate);PtArena::Remove(a.gate2);}
        if(!mission->GetMissionValue(34))for(int d=0;d<layout.gateCount;++d){int r,x,y,ox,oy;SubWorld[world].Mps2Map(layout.door[d].x,layout.door[d].y,&r,&x,&y,&ox,&oy);if(r>=0)PtArena::barrier[PtArena::Cell(&SubWorld[world].m_Region[r],y*16+x)]=PtArena::Block(id,-1);}
        if(phase==1||phase==2){
            a.walls.resize(layout.visualCount);
            for(int i=0;i<layout.visualCount;++i){PtArena::Entity& e=a.walls[i];
                if(e.serial&&(!PtArena::Live(e)||Npc[e.index].m_CurrentLife<=0)){e.broken=true;PtArena::Remove(e);}
                PtArena::Spawn(e,world,layout.visual[i].tid,layout.visual[i].x,layout.visual[i].y,NULL,true);
            }
            for(int n=0;n<layout.count;++n){int r,x,y,ox,oy;SubWorld[world].Mps2Map(layout.fence[n].x,layout.fence[n].y,&r,&x,&y,&ox,&oy);if(r>=0)PtArena::barrier[PtArena::Cell(&SubWorld[world].m_Region[r],y*16+x)]=PtArena::Block(id,layout.groups[n]);}
        }else{for(unsigned int i=0;i<a.walls.size();++i)PtArena::Remove(a.walls[i]);a.walls.clear();}
        PtVttSendBarrierState(id);
    }
}
int LuaEventVttGate(Lua_State* L){int p=GetPlayerIndex(L),id=(int)Lua_ValueToNumber(L,1),op=(int)Lua_ValueToNumber(L,2);Lua_PushNumber(L,id>=1&&id<=4?PtArena::Open(p,id,op):-1);return 1;}
int LuaEventVttLeave(Lua_State* L){int p=GetPlayerIndex(L),id=(int)Lua_ValueToNumber(L,1);if(PtVtt::Live(p)&&id>=1&&id<=4){Player[p].m_cTask.SetSaveVal(4660+id,0,TRUE);Player[p].m_cTask.SetSaveVal(4670+id,0,TRUE);Player[p].m_cTask.SetSaveVal(4680+id,0,TRUE);}
    return 0;
}
