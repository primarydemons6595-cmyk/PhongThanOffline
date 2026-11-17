#include "KCore.h"
#define private public
#include "KItem.h"
#undef private
#include <assert.h>
#include <stdio.h>
KItem::KItem(){ZeroMemory(this,sizeof(*this));}
KItem::~KItem(){}
#include "../../Output/Tools/NpcCatalogItemOperators.inl"
int main(){
    KItem item;
    KBASICPROP_QUEST quest;ZeroMemory(&quest,sizeof(quest));
    quest.m_nItemGenre=item_task;quest.m_nDetailType=26;quest.m_nWidth=quest.m_nHeight=1;quest.m_nPrice=100;
    strcpy(quest.m_szName,"Original quest item");
    item.m_GeneratorParam.nLuck=37;item.SetID(901);item=quest;
    assert(item.GetGenre()==item_task && item.GetDetailType()==26 && item.GetWidth()==1 && item.GetHeight()==1);
    assert(!strcmp(item.GetName(),"Original quest item") && item.m_GeneratorParam.nLuck==37 && item.GetID()==901);
    KBASICPROP_TOWNPORTAL portal;ZeroMemory(&portal,sizeof(portal));
    portal.m_nItemGenre=item_townportal;portal.m_nDetailType=0;portal.m_nWidth=portal.m_nHeight=1;portal.m_nPrice=500;
    strcpy(portal.m_szName,"Original town portal");
    for(int i=0;i<20;++i){
        item=portal;
        assert(item.GetGenre()==item_townportal && item.GetDetailType()==0 && item.GetWidth()==1 && item.GetHeight()==1);
        assert(!strcmp(item.GetName(),"Original town portal") && item.m_GeneratorParam.nLuck==37 && item.GetID()==901);
        item=quest;assert(item.GetGenre()==item_task && item.GetDetailType()==26);
    }
    puts("PASS NPC_REWARD_ITEM_MEMORY actual_production_operators quest_identity=PASS portal_identity=PASS generator_and_item_ID_preserved=PASS repeated_assignment=20");return 0;
}
