#ifdef _SERVER
#include "../../../Headers/PhongThanVttStone.h"
#include "PhongThanEventTicketPayment.inl"
struct PhongThanVttMoveContext {int player,world,map,x,y,oldMap,oldX,oldY;KMission* mission;};
static bool PhongThanVttHasStone(int player,int id){
    PhongThanQuestItemTuple stone;if(!PhongThanDecodeQuestItemTuple(8,143+id,2,0,0,0,stone))return false;
    KItemList& bag=Player[player].m_ItemList;
    for(PlayerItem* p=bag.GetFirstItem();p;p=bag.GetNextItem()){
        int i=p->nIdx;bool carried=p->nPlace==pos_equiproom||p->nPlace==pos_equiproomex||p->nPlace==pos_ptbag2||p->nPlace==pos_ptbag3;
        if(carried&&i>0&&i<MAX_ITEM&&Item[i].GetID()&&Item[i].GetStackNum()>0&&
           !Item[i].GetLock()->IsLock()&&PhongThanQuestItemMatches(Item[i],stone)&&
           PhongThanVttStoneValid(Item[i].GetParam(),(int)time(NULL))&&
           (!Item[i].m_CommonAttrib.nExpireTime||Item[i].m_CommonAttrib.nExpireTime>KSG_GetCurSec()))return true;
    }return false;
}
static bool PhongThanVttMove(void* opaque){
    PhongThanVttMoveContext& c=*(PhongThanVttMoveContext*)opaque;KNpc& actor=Npc[Player[c.player].m_nIndex];
    if(!actor.ChangeWorld(c.map,c.x,c.y)||actor.m_SubWorldIndex!=c.world)return false;
    if(c.mission->AddPlayer(c.player,Player[c.player].m_dwID,1)>0)return true;
    actor.ChangeWorld(c.oldMap,c.oldX,c.oldY);return false;
}
#endif
int LuaEventEnterVanTien(Lua_State* L){
    int result=0;
#ifdef _SERVER
    const int player=GetPlayerIndex(L);
    if(player>0&&player<MAX_PLAYER&&(Lua_GetTopIndex(L)==3||Lua_GetTopIndex(L)==4)&&Player[player].m_dwID&&Player[player].m_nIndex>0&&Player[player].m_nIndex<MAX_NPC){
        int id=(int)Lua_ValueToNumber(L,1),x=(int)Lua_ValueToNumber(L,2),y=(int)Lua_ValueToNumber(L,3);
        if(id>=1&&id<=4&&x>=0&&x<300000&&y>=0&&y<300000){
            int world=g_SubWorldSet.SearchWorld(1066+id),npc=Player[player].m_nIndex;
            int level=Npc[npc].m_Level,minLevel=id==1?30:id*20+11,maxLevel=id==4?120:id*20+30;
            KMission* mission=world>=0?SubWorld[world].m_MissionArray.FindByMissionId(id):NULL;
            if(mission&&Npc[npc].IsPlayer()&&level>=minLevel&&level<=maxLevel){
                int generation=mission->GetMissionValue(1),entryTask=4789+id,previous=Player[player].m_cTask.GetSaveVal(entryTask),phase=g_TaskGlobalValue[id];
                bool reentry=previous==generation&&generation>0;
                if(generation>0&&(phase==1||phase==2)&&(!PhongThanEventOwns(id)||PhongThanEventAccepting(id)||phase==2||reentry)&&(reentry||mission->m_MissionPlayer.GetFreeCount()>0)){
                    // Linh Thach is an alternative admission ticket, not an extra requirement.
                    int premium=Lua_GetTopIndex(L)==4?(int)Lua_ValueToNumber(L,4):0;
                    int normalCount=mission->GetMissionValue(35),premiumCount=mission->GetMissionValue(36);
                    if(premium<0||premium>1||(!reentry&&normalCount+premiumCount>=200)){Lua_PushNumber(L,-4);return 1;}
                    if(!reentry&&!premium&&normalCount>=50)premium=1;
                    PhongThanQuestExchangeRow needs[2];int needCount=1;
                    int genre=3,detail=61+id,particular=0,itemLevel=0,series=0,count=1;
                    if(PhongThanEventOwns(id)){genre=PhongThanEventValue(id,3);detail=PhongThanEventValue(id,4);particular=PhongThanEventValue(id,5);itemLevel=PhongThanEventValue(id,6);series=PhongThanEventValue(id,7);count=PhongThanEventValue(id,8);}
                    needs[0].count=count;
                    if(PhongThanDecodeQuestItemTuple(genre,detail,particular,itemLevel,series,0,needs[0].item)){
                        if(premium&&!reentry){
                            if(!PhongThanDecodeQuestItemTuple(8,143+id,2,0,0,0,needs[0].item)){Lua_PushNumber(L,-2);return 1;}
                            if(!PhongThanVttHasStone(player,id)){Lua_PushNumber(L,-1);return 1;}
                            needs[0].count=0;
                        }
                        PhongThanVttMoveContext context;context.player=player;context.world=world;context.map=1066+id;context.x=x*32;context.y=y*32;context.mission=mission;
                        int oldWorld=Npc[npc].m_SubWorldIndex;context.oldMap=oldWorld>=0&&oldWorld<MAX_SUBWORLD?SubWorld[oldWorld].m_SubWorldID:0;Npc[npc].GetMpsPos(&context.oldX,&context.oldY);
                        result=PhongThanEventTicketsPayment(player,needs,needCount,entryTask,previous,generation,PhongThanVttMove,&context);
                        // A missing ordinary ticket has made no changes, so try the matching stone alone.
                        if(result==-1&&!premium&&!reentry){
                            premium=1;needs[0].count=0;
                            if(PhongThanVttHasStone(player,id)&&PhongThanDecodeQuestItemTuple(8,143+id,2,0,0,0,needs[0].item))
                                result=PhongThanEventTicketsPayment(player,needs,1,entryTask,previous,generation,PhongThanVttMove,&context);
                        }
                        if(result==1&&!reentry){char value[32];sprintf(value,"%d",premium?premiumCount+1:normalCount+1);mission->SetMission(premium?36:35,value);}
                    }
                }
            }
        }
    }
#endif
    Lua_PushNumber(L,result);return 1;
}
