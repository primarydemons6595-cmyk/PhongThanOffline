#include "../../../Headers/PhongThanF4Protocol.h"
#include "../../../Headers/PhongThanBank.h"
#include "PhongThanScriptAction.h"
static void PTF4Notice(int player,const char* text){KPhongThanScriptAction a;memset(&a,0,sizeof(a));a.Operation=PHONGTHAN_SCRIPT_SHOW;a.View=UI_TALKDIALOG;a.OptionCount=1;a.ServerOwned=1;lstrcpynA(a.Content,text,sizeof(a.Content));a.ContentLength=strlen(a.Content);Player[player].DoScriptAction(&a);}
static void PTF4SendLayout(int player,int page)
{
    PTF4Layout packet;memset(&packet,0,sizeof(packet));
    KPlayer& owner=Player[player];int actor=owner.m_nIndex;
    if(actor<=0 || actor>=MAX_NPC || page<0 || page>8)return;
    PhongThanInitializeWireHeader(&packet.Header,PT_F4_LAYOUT,sizeof(packet),PHONGTHAN_WIRE_FLAG_RESPONSE,0);
    packet.MapId=SubWorld[Npc[actor].m_SubWorldIndex].m_SubWorldID;packet.EntityId=Npc[actor].m_dwID;packet.Page=(BYTE)page;
    for(int p=1;p<=3;++p)packet.Rows[p-1]=(BYTE)owner.m_ItemList.GetBagRows(p);
    int place=page>=4?pos_repositoryroom+page-4:page==0?pos_equiproom:page==1?pos_equiproomex:page==2?pos_ptbag2:pos_ptbag3;
    PlayerItem* item=owner.m_ItemList.GetFirstItem();
    while(item) {
        if(item->nPlace==place && item->nIdx>0 && item->nIdx<MAX_ITEM) {
            if(packet.Count==(page>=4?60:35))return;
            PTF4Cell& cell=packet.Items[packet.Count++];cell.ItemId=Item[item->nIdx].GetID();cell.X=(BYTE)item->nX;cell.Y=(BYTE)item->nY;
        }
        item=owner.m_ItemList.GetNextItem();
    }
    g_pServer->PackDataToClient(owner.m_nNetConnectIdx,&packet,sizeof(packet));
}
#include "PhongThanBankSecurity.inl"
static void PTF4Process(int player,const PTF4Command& command)
{
    if(player<=0 || player>=MAX_PLAYER)return;
    KPlayer& owner=Player[player];int actor=owner.m_nIndex;
    if(actor<=0 || actor>=MAX_NPC || Npc[actor].m_SubWorldIndex<0 || Npc[actor].m_SubWorldIndex>=MAX_SUBWORLD)return;
    if(command.MapId!=(unsigned int)SubWorld[Npc[actor].m_SubWorldIndex].m_SubWorldID || command.EntityId!=Npc[actor].m_dwID)return;
    if(owner.CheckTrading() || owner.GetLockState() || owner.GetLockMove()->bLock || owner.m_ItemList.Hand() ||
       owner.m_PTrade.nTrade || owner.m_nPaceBarTime || Npc[actor].m_CurrentLife<=0){if(command.Action==PT_F4_UNLOCK||command.Action==PT_F4_BANK_MENU)PTF4Notice(player,"\102\271\156\040\256\141\156\147\040\142\313\156\054\040\150\267\171\040\153\325\164\040\164\150\363\143\040\164\150\141\157\040\164\270\143\040\150\151\326\156\040\164\271\151\040\162\345\151\040\164\150\366\040\154\271\151\056");return;}
    if(command.Action==PT_F4_BANK_MENU){
        if(!owner.ExecuteScript("\\script\\phongthan\\services\\bank_rental.lua","main",command.Page-3))PTF4Notice(player,"\103\150\255\141\040\155\353\040\256\255\356\143\040\144\336\143\150\040\166\364\040\164\150\165\252\040\153\150\157\054\040\166\165\151\040\154\337\156\147\040\164\150\366\040\154\271\151\056");return;
    }
    if(command.Page>=4 && (command.Action!=PT_F4_SORT || Npc[actor].m_FightMode ||
       !PhongThanBankAccess(player,command.Page-3)))return;
    if(command.Action==PT_F4_REALM){owner.m_ItemList.SwitchEquipmentRealm(command.Page);return;}
    if(command.Action==PT_F4_SORT)owner.m_ItemList.SortBag(command.Page);
    else if(command.Action==PT_F4_UNLOCK) {
        // One permanent 1x5 row, as the original F4 confirmation specifies.
        const int task=TASKVALUE_PT_BAG_ROWS_1+command.Page-1;
        int rows=owner.m_cTask.GetSaveVal(task);
        if(rows!=command.ExpectedRows || rows<0 || rows>=7){PTF4Notice(player,"\110\265\156\150\040\164\162\141\156\147\040\256\267\040\164\150\141\171\040\256\346\151\040\150\157\306\143\040\256\267\040\155\353\040\256\361\040\150\265\156\147\056\040\126\165\151\040\154\337\156\147\040\153\151\323\155\040\164\162\141\040\154\271\151\056");PTF4SendLayout(player,command.Page);return;}
        for(int prior=1;prior<command.Page;++prior)if(owner.m_cTask.GetSaveVal(TASKVALUE_PT_BAG_ROWS_1+prior-1)!=7){PTF4Notice(player,"\103\307\156\040\155\353\040\256\361\040\150\265\156\150\040\164\162\141\156\147\040\164\162\255\355\143\040\162\345\151\040\155\355\151\040\155\353\040\150\265\156\150\040\164\162\141\156\147\040\164\151\325\160\040\164\150\145\157\056");return;}
        extern int PhongThanPayBagUnlock(int,int,int);
        if(!PhongThanPayBagUnlock(player,command.Page,rows)){PTF4Notice(player,rows<4?"\113\150\253\156\147\040\256\361\040\115\266\156\150\040\166\266\151\040\150\157\306\143\040\070\040\130\165\040\164\162\157\156\147\040\150\265\156\150\040\164\162\141\156\147\040\256\323\040\155\353\040\150\265\156\147\040\156\265\171\056":"\113\150\253\156\147\040\256\361\040\126\266\151\040\144\307\171\040\150\157\306\143\040\061\062\070\040\130\165\040\164\162\157\156\147\040\150\265\156\150\040\164\162\141\156\147\040\256\323\040\155\353\040\150\265\156\147\040\156\265\171\056");return;}
        PTF4Notice(player,"\247\267\040\155\353\040\164\150\252\155\040\155\351\164\040\150\265\156\147\040\150\265\156\150\040\164\162\141\156\147\056");
    }
    PTF4SendLayout(player,command.Page);
}
