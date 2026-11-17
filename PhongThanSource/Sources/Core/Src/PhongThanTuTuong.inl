// Tuo Tuong: authoritative entry payment and one-shot partial-floor reward.
// Time/fee/drop defaults are local policy; public VNG rules do not publish them.
namespace PtTuTuong {
enum {Phase=4412,Day=4413,Rounds=4414,Deadline=4415,Kills=4416,Floors=4417,Family=4418,Origin=4419,Level=5336,Bonus=5337};
static int Get(int p,int t){return Player[p].m_cTask.GetSaveVal(t);}
static void Set(int p,int t,int v){Player[p].m_cTask.SetSaveVal(t,v,TRUE);}
static bool Near(int p){
 if(!PtDaily::Live(p)||Player[p].GetLockState()||Player[p].CheckTrading())return false;
 int a=Player[p].m_nIndex,n=Player[p].m_nLastNpcIndex,w=Npc[a].m_SubWorldIndex;
 if(w<0||w>=MAX_SUBWORLD||n<=0||n>=MAX_NPC||Npc[n].m_SubWorldIndex!=w||Npc[n].m_Kind!=kind_dialoger||Npc[a].m_CurrentLife<=0)return false;
 int map=SubWorld[w].m_SubWorldID;
 if(!((map==1020&&Npc[n].m_NpcSettingIdx==202)||(map==1021&&Npc[n].m_NpcSettingIdx==160)))return false;
 if(Npc[a].m_ActionScriptID!=Npc[n].m_ActionScriptID)return false;
 int x,y,nx,ny;Npc[a].GetMpsPos(&x,&y);Npc[n].GetMpsPos(&nx,&ny);
 return (__int64)(x-nx)*(x-nx)+(__int64)(y-ny)*(y-ny)<=256*256;
}
static void Row(PhongThanQuestExchangeRow& r,int g,int d,int part,int level,int count){PhongThanDecodeQuestItemTuple(g,d,part,level,0,0,r.item);r.count=count;}
static bool HasKey(int p){
 PhongThanQuestItemTuple key;PhongThanDecodeQuestItemTuple(8,329,2,0,0,0,key);
 for(PlayerItem* b=Player[p].m_ItemList.GetFirstItem();b;b=Player[p].m_ItemList.GetNextItem()){
  int n=b->nIdx;bool carried=b->nPlace==pos_equiproom||b->nPlace==pos_equiproomex||b->nPlace==pos_ptbag2||b->nPlace==pos_ptbag3;
  if(carried&&n>0&&n<MAX_ITEM&&Item[n].GetID()&&Item[n].GetStackNum()>0&&!Item[n].GetLock()->IsLock()&&PhongThanQuestItemMatches(Item[n],key)&&
     (!Item[n].m_CommonAttrib.nExpireTime||Item[n].m_CommonAttrib.nExpireTime>KSG_GetCurSec()))return true;
 }return false;
}
static int Enter(int p,int /*legacyMethod*/){
 if(!Near(p)||Get(p,Phase)!=0)return -1;
 int level=Npc[Player[p].m_nIndex].m_Level,day=(int)((time(NULL)+25200)/86400),rounds=Get(p,Day)==day?Get(p,Rounds):0;
 if(level<65||level>200||rounds<0||rounds>=6)return -2;
 int fee=50000+(level-65)/10*50000;
 PhongThanQuestExchangeRow needs[2];Row(needs[0],3,82,0,0,1);int count=1;
 if(rounds){if(HasKey(p))Row(needs[count++],8,329,2,0,1);else Row(needs[count++],4,47,0,0,4);}
 if(!Player[p].Pay(fee))return -4;
 if(PhongThanQuestExchange(p,Phase,0,1,needs,count,0,0,true,0,true)!=1){Player[p].Earn(fee);return -5;}
 Set(p,Day,day);Set(p,Rounds,rounds+1);Set(p,Level,level);Set(p,Deadline,(int)time(NULL)+1800);
 Set(p,Kills,0);Set(p,Floors,0);Set(p,Family,1+g_Random(4));Set(p,Bonus,g_Random(100)<10?1:0);
 Set(p,Origin,SubWorld[Npc[Player[p].m_nIndex].m_SubWorldIndex].m_SubWorldID);
 Player[p].m_uMustSave=SAVE_REQUEST;return 1;
}
static int Claim(int p){
 if(!Near(p))return -1;int phase=Get(p,Phase),floors=Get(p,Floors),level=Get(p,Level);
 if((phase!=1&&phase!=2)||floors<1||floors>5)return -2;
 if(level<65||level>200)level=Npc[Player[p].m_nIndex].m_Level;
 const int cumulative[6]={0,1000,3000,6000,10000,16000};
 time_t local=time(NULL)+25200;tm* today=gmtime(&local);int xp=level*cumulative[floors]*(today&&today->tm_wday==3?2:1);
 PhongThanQuestExchangeRow reward;int nr=0;
 if(floors==5&&Get(p,Bonus)==1){Row(reward,8,330,0,0,1);nr=1;}
 if(PhongThanQuestExchange(p,Phase,phase,0,0,0,&reward,nr,true)!=1)return -3;
 Player[p].DirectAddExp(xp);Player[p].m_uMustSave=SAVE_REQUEST;return xp;
}
}
int LuaTuTuongService(Lua_State* L){
 int result=0,p=GetPlayerIndex(L);
 if(Lua_GetTopIndex(L)==2&&Lua_IsNumber(L,1)&&Lua_IsNumber(L,2)){
  double op=Lua_ValueToNumber(L,1),method=Lua_ValueToNumber(L,2);
  if(op==1&&(method==0||method==1||method==2))result=PtTuTuong::Enter(p,(int)method);
  else if(op==2&&method==0)result=PtTuTuong::Claim(p);
 }
 Lua_PushNumber(L,result);return 1;
}
