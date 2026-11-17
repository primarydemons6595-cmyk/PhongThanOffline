// Test handbook actions are server-owned and require the real reusable book.
// An unrelated NPC callback or fabricated client item ID cannot grant rewards.
#ifdef _SERVER
#include "PhongThanEventHandbookSkills.inl"
#include "PhongThanEventHandbookItems.inl"
#include "PhongThanHandbookSkills.h"
#include "../../../Headers/PhongThanRealmUi.h"
static bool PtHandbookAllowed(int player) {
    if(player<=0 || player>=MAX_PLAYER || Player[player].m_nIndex<=0 ||
       Npc[Player[player].m_nIndex].m_CurrentLife<=0 ||
       Player[player].GetLockState() || Player[player].CheckTrading())return false;
    KItemList& bag=Player[player].m_ItemList;
    for(PlayerItem* p=bag.GetFirstItem();p;p=bag.GetNextItem()){
        int i=p->nIdx;
        if((p->nPlace==pos_equiproom || p->nPlace==pos_equiproomex || p->nPlace==pos_ptbag2 || p->nPlace==pos_ptbag3) &&
           i>0 && i<MAX_ITEM && Item[i].GetID() && Item[i].GetGenre()==6 &&
           Item[i].GetDetailType()==5572 && Item[i].GetParticular()==0 && !Item[i].GetLock()->IsLock())return true;
    }
    return false;
}
#include "PhongThanQuickQuest.inl"
struct PtHandbookSkillActor {
    int player;KSkillList& list;
    PtHandbookSkillActor(int p):player(p),list(Npc[Player[p].m_nIndex].m_SkillList){}
    int Profession(){return Player[player].m_cProfession.GetProfession();}
    int Learned(int id){return list.GetLevel(id);}
    int FreeSlots(){return MAX_NPCSKILL-1-list.GetCount();}
    bool Ready(int id,int cap){
        if(id<=0 || id>MAX_SKILL || cap<=0 || cap>=MAX_SKILLLEVEL || cap>(int)g_SkillManager.GetSkillMaxLevel(id))return false;
        int index=list.FindSame(id),bonus=index?list.GetAddLevel(id):list.m_nAllSkillV;
        if(bonus<=-cap || bonus>=MAX_SKILLLEVEL-cap)return false;
        return g_SkillManager.GetSkill(id,cap)!=0 && g_SkillManager.GetSkill(id,cap+bonus)!=0;
    }
    bool Learn(int id,int cap){
        int index=list.FindSame(id);
        if(index)list.m_Skills[index].TempSkill=FALSE;
        return list.Add(id,cap)!=0 && list.GetLevel(id)==cap;
    }
    void Sync(){Player[player].SendSyncData_Skill();}
};
static int PtHandbookMaxSkills(int player) {
    if(!PtHandbookAllowed(player))return 0;
    PtHandbookSkillActor actor(player);
    return PhongThanHandbookMaxSkills(kPtHandbookSkills,sizeof(kPtHandbookSkills)/sizeof(kPtHandbookSkills[0]),actor);
}
#include "PhongThanHandbookRealm.inl"
static int PtHandbookGive(int player,int id,int quantity) {
    if(!PtHandbookAllowed(player))return -1;
    if(id<=0 || id>=sizeof(kPtHandbookItems)/sizeof(kPtHandbookItems[0]) || quantity<1 || quantity>100)return -2;
    const PtHandbookItem& e=kPtHandbookItems[id];
    if(e.genre==item_ibitem && e.detail==418 && e.part==0)return PhongThanPetHopeAdjust(player,quantity)?1:-8;
    if(e.genre==item_equip && quantity!=1)return -2;
    PhongThanQuestItemTuple t;
    if(!PhongThanDecodeQuestItemTuple(e.genre,e.detail,e.part,e.level,0,0,t))return -2;
    int requestedSeries=0;
    if(e.genre==item_equip){
        const KBASICPROP_EQUIPMENT* source=ItemGen.CatalogEquipment(e.detail,e.row);if(!source)return -3;
        requestedSeries=Player[player].m_cProfession.GetProfession();
        for(int r=0;r<6;++r)if(source->m_aryPropReq[r].nType==magic_requireseries &&
            source->m_aryPropReq[r].nPara>=0 && source->m_aryPropReq[r].nPara<PHONGTHAN_PROFESSION_COUNT)
            requestedSeries=source->m_aryPropReq[r].nPara;
    }
    int levels[MAX_ITEM_MAGICLEVEL];ZeroMemory(levels,sizeof(levels));
    int temp=ItemSet.Add(t.genre,requestedSeries,t.level,0,t.detail,t.particular,levels,g_SubWorldSet.GetGameVersion(),0,e.genre==item_equip?e.row:-1);
    if(temp<=0){g_DebugLog("[Handbook] generation failed id=%d genre=%d detail=%d part=%d level=%d row=%d",id,e.genre,e.detail,e.part,e.level,e.row);return -4;}
    if(e.recipe && !Item[temp].ApplyForgeRecipe(e.recipe)){g_DebugLog("[Handbook] recipe failed id=%d row=%d recipe=%d level=%d",id,e.row,e.recipe,e.level);ItemSet.Remove(temp);return -5;}
    KItem prototype=Item[temp];ItemSet.Remove(temp);
    ItemPos available;if(!Player[player].m_ItemList.SearchPosition(prototype.GetWidth(),prototype.GetHeight(),&available,true))return -6;
    PhongThanQuestExchangeRow reward;
    reward.item.genre=prototype.GetGenre();reward.item.detail=prototype.GetDetailType();reward.item.particular=prototype.GetParticular();
    reward.item.level=prototype.GetLevel();reward.item.series=prototype.GetSeries();reward.item.luck=0;reward.count=quantity;
    int task=TASKVALUE_PT_HANDBOOK_RECEIPT,value=Player[player].m_cTask.GetSaveVal(task);
    return PhongThanQuestExchange(player,task,value,value==1?0:1,0,0,&reward,1,true,&prototype)?1:-7;
}
static int PtHandbookCurrency(int player,int detail) {
    if(!PtHandbookAllowed(player) || (detail!=46 && detail!=47))return 0;
    PhongThanQuestExchangeRow reward;
    if(!PhongThanDecodeQuestItemTuple(4,detail,0,0,0,0,reward.item))return 0;
    reward.count=detail==47?100:1;
    int task=TASKVALUE_PT_HANDBOOK_RECEIPT,value=Player[player].m_cTask.GetSaveVal(task);
    return PhongThanQuestExchange(player,task,value,value==1?0:1,0,0,&reward,1,true);
}
static int PtHandbookVuSi(int player) {
    if(!PtHandbookAllowed(player) || Player[player].m_cProfession.GetProfession()!=PHONGTHAN_PROFESSION_GIAP_SI)return 0;
    KPlayer& owner=Player[player];KNpc& actor=Npc[owner.m_nIndex];
    // The player removes equipment explicitly: conversion never destroys,
    // drops or silently moves an item when F4 has insufficient room.
    for(int slot=0;slot<itempart_num;++slot)if(owner.m_ItemList.GetEquipment(slot))return 0;
    for(PlayerItem* gear=owner.m_ItemList.GetFirstItem();gear;gear=owner.m_ItemList.GetNextItem())if(gear->nIdx>0&&(gear->nPlace==pos_ptinactiveequip||gear->nPlace==pos_ptrealmescrow))return 0;
    if(!g_SkillManager.GetSkill(1691,1))return 0;
    int removed=0;
    for(int pre=0;pre<sizeof(kPtHandbookSkills)/sizeof(kPtHandbookSkills[0]);++pre)
        if(kPtHandbookSkills[pre].profession==0 && actor.m_SkillList.GetLevel(kPtHandbookSkills[pre].id)>0)++removed;
    if(actor.m_SkillList.GetLevel(1691)<=0 && actor.m_SkillList.GetCount()-removed>=MAX_NPCSKILL-1)return 0;
    if(!owner.SetProfession(PHONGTHAN_PROFESSION_VU_SI))return 0;
    owner.m_cTask.SetSaveVal(TASKVALUE_PT_VUSI_CONVERSION_LEVEL,actor.m_Level);
    actor.m_nProfession=PHONGTHAN_PROFESSION_VU_SI;
    for(int i=0;i<sizeof(kPtHandbookSkills)/sizeof(kPtHandbookSkills[0]);++i){
        const PtHandbookSkill& s=kPtHandbookSkills[i];if(s.profession!=0)continue;
        int learned=actor.m_SkillList.GetLevel(s.id);
        if(learned>0){actor.SetStateSkillEffect(owner.m_nIndex,s.id,learned,0,0,1,TRUE);actor.m_SkillList.Remove(s.id);}
    }
    actor.m_SkillList.Add(1691,1);
    owner.SetFirstDamage();owner.SetBaseAttackRating();owner.SetBaseDefence();
    owner.SetBaseResistData();owner.UpdataCurData();
    owner.m_uMustSave=SAVE_REQUEST;
    owner.SendCurNormalSyncData();owner.SendSyncData_Skill();return 1;
}
static int PtHandbookLevel(int player,int target) {
    if(!PtHandbookAllowed(player) || (target!=50 && target!=70 && target!=90 && target!=150 && target!=200))return 0;
    KPlayer& owner=Player[player];KNpc& actor=Npc[owner.m_nIndex];
    if(actor.m_Level>=target)return 1;
    while(actor.m_Level<target){int previous=actor.m_Level;owner.LevelUp();if(actor.m_Level<=previous)return 0;}
    PHONGTHAN_LEVEL_EVENT event;ZeroMemory(&event,sizeof(event));
    PhongThanInitializeWireHeader(&event.Header,PHONGTHAN_MSG_GAMEPLAY_LEVEL,sizeof(event),PHONGTHAN_WIRE_FLAG_RESPONSE,0);
    event.MapId=SubWorld[actor.m_SubWorldIndex].m_SubWorldID;event.EntityId=actor.m_dwID;
    event.Level=actor.m_Level;event.Experience=0;event.AttributePoints=owner.m_nAttributePoint;event.SkillPoints=owner.m_nSkillPoint;
    event.BaseMaxLife=actor.m_LifeMax;event.BaseMaxStamina=actor.m_StaminaMax;event.BaseMaxMana=actor.m_ManaMax;
    // Normal level growth already sent the four attribute updates. Never
    // reset to a fabricated starter profile when converting to Vu Si.
    event.ResetAttributes=FALSE;
    g_pServer->PackDataToClient(owner.m_nNetConnectIdx,(BYTE*)&event,sizeof(event));
    owner.SendCurNormalSyncData();return actor.m_Level==target;
}
#endif
int LuaEventHandbookAction(Lua_State* L) {
    int result=0;
#ifdef _SERVER
    int player=GetPlayerIndex(L);
    if(PtHandbookAllowed(player) && Lua_GetTopIndex(L)>=1 && Lua_IsNumber(L,1)){
        double op=Lua_ValueToNumber(L,1);
        if(op==1 && Lua_GetTopIndex(L)==1)result=PtHandbookMaxSkills(player);
        else if(op==2){
            if(Lua_GetTopIndex(L)==1)result=PtHandbookLevel(player,200);
            else if(Lua_GetTopIndex(L)==2 && Lua_IsNumber(L,2)){
                double target=Lua_ValueToNumber(L,2);
                if(target==50 || target==70 || target==90 || target==150 || target==200)result=PtHandbookLevel(player,(int)target);
            }
        }
        else if((op==46 || op==47) && Lua_GetTopIndex(L)==1)result=PtHandbookCurrency(player,(int)op);
        else if((op==6||op==7)&&Lua_GetTopIndex(L)==2&&Lua_IsNumber(L,2)){
            double amount=Lua_ValueToNumber(L,2);
            if(amount==100||amount==1000){
                if(op==7){int bonus=Player[player].m_cTask.GetSaveVal(TASKVALUE_PT_CARRY_BONUS);if(bonus>=0&&bonus<=1000000000-(int)amount){Player[player].m_cTask.SetSaveVal(TASKVALUE_PT_CARRY_BONUS,bonus+(int)amount,TRUE);Player[player].Save();result=1;}}
                else {int value=Player[player].m_cTask.GetSaveVal(TASKVALUE_PT_CREDIT);
                    if(value>=0&&value<=1000000000-(int)amount){Player[player].m_cTask.SetSaveVal(TASKVALUE_PT_CREDIT,value+(int)amount,TRUE);Player[player].m_uMustSave=SAVE_REQUEST;result=1;}}
            }
        }
        else if((op==8||op==9||op==10)&&Lua_GetTopIndex(L)==2&&Lua_IsNumber(L,2)){double id=Lua_ValueToNumber(L,2);if(id>=1&&id<=74&&id==(int)id)result=PtQuickQuest(player,(int)op,(int)id);}
        else if(op>=20&&op<=24&&op==(int)op){
            if(op==24&&Lua_GetTopIndex(L)==1)result=PtHandbookRealm(player,24,0);
            else if(op!=24&&Lua_GetTopIndex(L)==2&&Lua_IsNumber(L,2)){
                double value=Lua_ValueToNumber(L,2);
                if(value>=0&&value<=1000000000&&value==(int)value)result=PtHandbookRealm(player,(int)op,(int)value);
            }
        }
        else if(op==5 && Lua_GetTopIndex(L)==1)result=PtHandbookVuSi(player);
        else if(op==3 && Lua_GetTopIndex(L)==3 && Lua_IsNumber(L,2) && Lua_IsNumber(L,3)){
            double id=Lua_ValueToNumber(L,2),qty=Lua_ValueToNumber(L,3);
            if(id>=1 && id<sizeof(kPtHandbookItems)/sizeof(kPtHandbookItems[0]) && id==(int)id && qty>=1 && qty<=100 && qty==(int)qty)result=PtHandbookGive(player,(int)id,(int)qty);
        }
        else if(op==4 && Lua_GetTopIndex(L)==2 && Lua_IsNumber(L,2)){
            double units=Lua_ValueToNumber(L,2);
            if(units>=1 && units<=1000 && units==(int)units && Player[player].m_ItemList.GetEquipmentMoney()<=INT_MAX-(int)units*10000)
                result=Player[player].Earn((int)units*10000)?1:0;
        }
    }
#endif
    Lua_PushNumber(L,result);return 1;
}
