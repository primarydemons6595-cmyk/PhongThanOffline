// Preserve the authored placement; mission lifetime controls town gates.
#include "Scene/SceneDataDef.h"
namespace PtVttGates {
struct Gate {KSPNpc cell;int world,index;DWORD entity;bool known;Gate():world(-1),index(0),entity(0),known(false){}};
static Gate gates[5];
static int Identify(int map,const KSPNpc& cell){
    static const int maps[5]={0,1002,1004,1003,1001};
    static const char* scripts[5]={"","\\script\\phongthan\\npc_recovered_batch\\1002_f57647b5.lua","\\script\\phongthan\\npc_recovered_batch\\1004_281c1583.lua","\\script\\phongthan\\npc_recovered_batch\\1003_479d2a84.lua","\\script\\phongthan\\npc_recovered_batch\\1001_41e3ecb2.lua"};
    if(cell.nTemplateID!=247)return 0;
    for(int id=1;id<=4;++id)if(map==maps[id]&&!_stricmp(cell.szScript,scripts[id]))return id;
    return 0;
}
static void Sync(){
    for(int id=1;id<=4;++id){Gate& gate=gates[id];if(!gate.known)continue;
        bool live=gate.index>0&&gate.index<MAX_NPC&&Npc[gate.index].m_dwID==gate.entity;
        int phase=g_TaskGlobalValue[id];bool visible=phase==1||phase==2;
        if(visible&&!live){
            if(!g_GetScript(g_FileName2Id(gate.cell.szScript)))ReLoadScript(gate.cell.szScript);
            gate.index=NpcSet.Add(gate.world,&gate.cell);
            gate.entity=gate.index>0?Npc[gate.index].m_dwID:0;
        }else if(!visible&&live){
            int n=gate.index,w=Npc[n].m_SubWorldIndex,r=Npc[n].m_RegionIndex;
            if(w>=0&&w<MAX_SUBWORLD&&r>=0&&r<SubWorld[w].m_nTotalRegion){
                SubWorld[w].m_Region[r].RemoveNpc(n);
                SubWorld[w].m_Region[r].DecRef(Npc[n].m_MapX,Npc[n].m_MapY,obj_npc);
            }
            NpcSet.Remove(n);gate.index=0;gate.entity=0;
        }
    }
}
}
bool PhongThanVttDeferGate(int world,const KSPNpc* cell){
    if(!cell||world<0||world>=MAX_SUBWORLD)return false;
    int id=PtVttGates::Identify(SubWorld[world].m_SubWorldID,*cell);if(!id)return false;
    PtVttGates::Gate& gate=PtVttGates::gates[id];gate.cell=*cell;gate.world=world;gate.known=true;return true;
}
