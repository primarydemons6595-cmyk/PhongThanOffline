// Repair uses existing server-authoritative per-item requests and prices.
enum {PT_REPAIR_CAPACITY=itempart_num+4*EQUIPMENT_ROOM_WIDTH*EQUIPMENT_ROOM_HEIGHT};
static void PtRepairNotice(const char* text){
    KSystemMessage msg;memset(&msg,0,sizeof(msg));
    msg.eType=SMT_NORMAL;msg.byConfirmType=SMCT_NONE;
    strncpy(msg.szMessage,text,sizeof(msg.szMessage)-1);
    KUiSysMsgCentre::AMessageArrival(&msg,NULL);
}
static int PtCollectRepairEquipment(KUiObjAtContRegion* objects,int& total){
    total=0;
    if(!g_pCoreShell||g_pCoreShell->GetTradeState()||g_pCoreShell->GetLockState())return 0;
    KUiObjAtRegion equips[itempart_num];memset(equips,0,sizeof(equips));
    int count=g_pCoreShell->GetGameData(GDI_EQUIPMENT,(unsigned int)equips,0);
    if(count<0||count>itempart_num)return 0;
    int selected=0;
    for(int i=0;i<count;++i){
        if(equips[i].Obj.uGenre!=CGOG_ITEM||!equips[i].Obj.uId||!g_pCoreShell->IsDamage(equips[i].Obj.uId))continue;
        bool duplicate=false;
        for(int j=0;j<selected;++j)if(objects[j].Obj.uId==equips[i].Obj.uId)duplicate=true;
        if(duplicate)continue;
        KUiObjAtContRegion obj;memset(&obj,0,sizeof(obj));
        obj.Obj=equips[i].Obj;obj.Region=equips[i].Region;obj.eContainer=UOC_EQUIPTMENT;
        KUiItemBuySelInfo price;memset(&price,0,sizeof(price));
        if(!g_pCoreShell->GetGameData(GDI_REPAIR_ITEM_PRICE,(unsigned int)&obj,(int)&price)||price.nCurPrice<=0)continue;
        if(total>2147483647-price.nCurPrice)return 0;
        total+=price.nCurPrice;objects[selected++]=obj;
    }
    for(int page=0;page<4;++page){
        int query=PT_GDI_BAG_FIRST+page,capacity=EQUIPMENT_ROOM_WIDTH*EQUIPMENT_ROOM_HEIGHT;
        KUiObjAtRegion bag[EQUIPMENT_ROOM_WIDTH*EQUIPMENT_ROOM_HEIGHT];memset(bag,0,sizeof(bag));
        int expected=g_pCoreShell->GetGameData(query,0,0);if(expected<0||expected>capacity)return 0;
        int count=expected?g_pCoreShell->GetGameData(query,(unsigned int)bag,capacity):0;if(count!=expected)return 0;
        const int containers[]={UOC_ITEM_TAKE_WITH,UOC_ITEM_TAKE_WITH_EX,UOC_PT_BAG2,UOC_PT_BAG3};
        for(int n=0;n<count&&selected<PT_REPAIR_CAPACITY;++n){
            if(bag[n].Obj.uGenre!=CGOG_ITEM||!bag[n].Obj.uId||!g_pCoreShell->IsDamage(bag[n].Obj.uId))continue;
            bool duplicate=false;for(int j=0;j<selected;++j)if(objects[j].Obj.uId==bag[n].Obj.uId)duplicate=true;if(duplicate)continue;
            KUiObjAtContRegion obj;memset(&obj,0,sizeof(obj));obj.Obj=bag[n].Obj;obj.Region=bag[n].Region;obj.eContainer=(UIOBJECT_CONTAINER)containers[page];
            KUiItemBuySelInfo price;memset(&price,0,sizeof(price));
            if(!g_pCoreShell->GetGameData(GDI_REPAIR_ITEM_PRICE,(unsigned int)&obj,(int)&price)||price.nCurPrice<=0)continue;
            if(total>2147483647-price.nCurPrice)return 0;total+=price.nCurPrice;objects[selected++]=obj;
        }
    }
    return selected;
}
void KUiItem::ConfirmRepairAll(){
    KUiObjAtContRegion objects[PT_REPAIR_CAPACITY];int total=0;
    int count=PtCollectRepairEquipment(objects,total);
    if(!count){PtRepairNotice("Kh«ng cã trang bÞ cÇn söa ch÷a, hoÆc nh©n vËt ®ang bÞ khãa/giao dÞch.");return;}
    KUiItemBuySelInfo price;memset(&price,0,sizeof(price));
    price.nCurPrice=total;price.nMoneyUnit=0;
    strcpy(price.szItemName,"Söa toµn bé trang bÞ");
    KUiObjAtContRegion obj;memset(&obj,0,sizeof(obj));
    KUiTradeConfirm::OpenWindow(&obj,&price,TCA_REPAIR_ALL);
}
void KUiItem::RepairAllEquipped(int quotedPrice){
    KUiObjAtContRegion objects[PT_REPAIR_CAPACITY];int total=0;
    int count=PtCollectRepairEquipment(objects,total);
    if(!count)return;
    // Changes during the confirmation require a fresh quote.
    if(total!=quotedPrice){ConfirmRepairAll();return;}
    if(g_pCoreShell->GetOwnValue(0)<total){PtRepairNotice("Kh«ng ®ñ b¹c ®Ó söa toµn bé trang bÞ.");return;}
    for(int i=0;i<count;++i)
        g_pCoreShell->OperationRequest(GOI_TRADE_NPC_REPAIR,(unsigned int)&objects[i],0);
}
