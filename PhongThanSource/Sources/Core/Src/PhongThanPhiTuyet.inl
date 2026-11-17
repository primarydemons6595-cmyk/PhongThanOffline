// VNG 2007-07-30 Phi Tuyet recipes. Item identities are the shipped short
// names (Trac Ma ... Nghich Lan), not newly invented equipment templates.
#ifdef _SERVER
namespace PtPhiTuyet {
struct Recipe {int inputLevel,money,bronze,orders,jade,eye;};
static const Recipe recipes[10]={
    {1,400000,0,0,0,0},{2,800000,200,1,0,0},
    {3,1500000,500,5,0,43},{3,1500000,0,10,0,43},
    {4,5000000,800,3,0,47},{4,3000000,1200,3,0,47},
    {4,5000000,0,25,2,47},{5,5000000,1500,10,0,49},
    {5,8000000,1000,30,0,49},{5,10000000,0,120,5,49}
};
static bool Anchor(int p) {
    if(p<=0 || p>=MAX_PLAYER)return false;
    int actor=Player[p].m_nIndex,n=Player[p].m_nLastNpcIndex;
    if(actor<=0 || actor>=MAX_NPC || n<=0 || n>=MAX_NPC ||
       !Npc[n].m_dwID || Npc[n].m_Kind!=kind_dialoger || Npc[n].m_NpcSettingIdx!=777)return false;
    int w=Npc[n].m_SubWorldIndex;
    if(w<0 || w>=MAX_SUBWORLD || Npc[actor].m_SubWorldIndex!=w || SubWorld[w].m_SubWorldID!=1020)return false;
    int x,y,px,py;Npc[n].GetMpsPos(&x,&y);Npc[actor].GetMpsPos(&px,&py);
    return abs(px-x)<=512 && abs(py-y)<=512;
}
static void Material(PhongThanQuestExchangeRow& row,int genre,int detail,int quantity) {
    PhongThanDecodeQuestItemTuple(genre,detail,0,0,0,0,row.item);row.count=quantity;
}
static int Count(int player,const PhongThanQuestItemTuple& tuple) {
    int result=0;KItemList& bag=Player[player].m_ItemList;
    for(PlayerItem* p=bag.GetFirstItem();p;p=bag.GetNextItem()) {
        int i=p->nIdx;
        if(p->nPlace==pos_equiproom && i>0 && i<MAX_ITEM && Item[i].GetID() &&
           !Item[i].GetLock()->IsLock() && PhongThanQuestItemMatches(Item[i],tuple)) {
            int count=Item[i].GetStackNum();if(count>0 && result<=2147483647-count)result+=count;
        }
    }
    return result;
}
static int Upgrade(int p,int id) {
    if(id<1 || id>30 || !Anchor(p))return -1;
    if(Player[p].GetLockState() || Player[p].CheckTrading())return -2;
    int profession=(id-1)/10,step=(id-1)%10;
    Recipe r=recipes[step];
    // The linked VNG page explicitly gives the Di Nhan Trac Loi price as
    // 800 van + 1000 bronze + 30 orders (different from JS/DS).
    if(profession==2 && step==7){r.money=8000000;r.bronze=1000;r.orders=30;}
    PhongThanQuestExchangeRow needs[6],reward;int count=1;
    if(!PhongThanDecodeQuestItemTuple(0,equip_horse,21+profession,r.inputLevel,0,0,needs[0].item) ||
       !PhongThanDecodeQuestItemTuple(0,equip_horse,24+profession,step+1,0,0,reward.item))return -3;
    needs[0].count=reward.count=1;
    if(step==0)Material(needs[count++],4,39,1); // Lieu Moc, questkey.txt
    if(r.bronze)Material(needs[count++],3,6,r.bronze); // bronze (qing tong)
    if(r.orders)Material(needs[count++],3,100,r.orders);
    if(r.jade)Material(needs[count++],3,41,r.jade);
    if(r.eye)Material(needs[count++],3,r.eye,1);
    const int task=TASKVALUE_PT_PHI_TUYET_EXCHANGE;
    const int expected=Player[p].m_cTask.GetSaveVal(task),next=expected==1?0:1;
    if(!Player[p].Pay(r.money))return -4;
    if(!PhongThanQuestExchange(p,task,expected,next,needs,count,&reward,1)) {
        if(!Player[p].Earn(r.money))g_DebugLog("[PhiTuyet] money rollback invariant player=%d",p);
        return 0;
    }
    return 1;
}
}
int LuaPhiTuyetUpgrade(Lua_State* L) {
    int result=-1;
    if(Lua_GetTopIndex(L)==1 && Lua_IsNumber(L,1)) {
        double v=Lua_ValueToNumber(L,1);
        if(v>=1 && v<=30 && v==(int)v)result=PtPhiTuyet::Upgrade(GetPlayerIndex(L),(int)v);
    }
    Lua_PushNumber(L,result);return 1;
}
int LuaPhiTuyetInventory(Lua_State* L) {
    int values[7];ZeroMemory(values,sizeof(values));int p=GetPlayerIndex(L);
    if(p>0 && p<MAX_PLAYER && Lua_GetTopIndex(L)==1 && Lua_IsNumber(L,1)) {
        double id=Lua_ValueToNumber(L,1);
        if(id>=1 && id<=30 && id==(int)id) {
            int profession=((int)id-1)/10,step=((int)id-1)%10;
            PhongThanQuestItemTuple tuple;
            if(PhongThanDecodeQuestItemTuple(0,equip_horse,21+profession,PtPhiTuyet::recipes[step].inputLevel,0,0,tuple))values[0]=PtPhiTuyet::Count(p,tuple);
            values[1]=Player[p].m_ItemList.GetEquipmentMoney();
            int genres[5]={4,3,3,3,3},details[5]={39,6,100,41,PtPhiTuyet::recipes[step].eye};
            for(int i=0;i<5;++i)if(details[i] && PhongThanDecodeQuestItemTuple(genres[i],details[i],0,0,0,0,tuple))values[i+2]=PtPhiTuyet::Count(p,tuple);
        }
    }
    for(int i=0;i<7;++i)Lua_PushNumber(L,values[i]);return 7;
}
#endif
