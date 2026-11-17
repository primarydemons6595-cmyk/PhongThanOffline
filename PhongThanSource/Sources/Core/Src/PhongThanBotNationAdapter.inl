// Included after PTNationExecute. Bots use the same store, rights, wallets and
// receipts as people; no direct write to membership, money or city state.
static void PTBotNationCopy(int nation,int duty,PTBotNationInfo& out){
 memset(&out,0,sizeof(out));if(nation<0||nation>=PT_NATION_CAPACITY)return;
 const PTNationRecord& n=s_PtNation.State.Nations[nation];const PTNationDevelopment& d=s_PtNation.State.Development[nation];
 out.id=n.Id;out.duty=duty;out.members=PTNationCount(n.Id);out.money=n.Money;out.cityMap=d.CityMapId;out.cityLevel=d.CityLevel;out.popularity=d.Popularity;
 out.bronze=d.Resources[0];
 lstrcpynA(out.name,n.Name,32);lstrcpynA(out.leader,n.Leader,32);
}
bool PhongThanBotNationInfo(int p,PTBotNationInfo* out){
 if(!out||p<=0||p>=MAX_PLAYER||!Player[p].m_bIsBot||!Player[p].m_PlayerDBLoad||!PTNationLive(p)||!PTNationReady())return false;
 memset(out,0,sizeof(*out));int member=PTNationSelf(p);if(member>=0){PTBotNationCopy(s_PtNation.Nation(s_PtNation.State.Members[member].NationId),s_PtNation.State.Members[member].Duty,*out);const PTTerritoryContribution& c=s_PtNation.State.Contributions[member];out->today=c.Day==(int)(time(0)/86400)?c.Today:0;out->pending=c.PendingAfter!=0;}
 PhongThanNationSyncPlayer(p);return true;
}
bool PhongThanBotNationAt(int index,PTBotNationInfo* out){
 if(!out||index<0||!PTNationReady())return false;for(int n=0;n<PT_NATION_CAPACITY;++n)if(s_PtNation.State.Nations[n].Id&&index--==0){PTBotNationCopy(n,0,*out);return true;}return false;
}
int PhongThanBotNationPendingAt(int p,int index,char* target,int size){
 if(!target||size<1||index<0||p<=0||p>=MAX_PLAYER||!Player[p].m_bIsBot||!PTNationLive(p)||!PTNationReady())return 0;
 int me=PTNationSelf(p);if(me<0||!PTNationRight(me,5))return 0;unsigned int nation=s_PtNation.State.Members[me].NationId;
 for(int m=0;m<PT_NATION_MEMBER_CAPACITY;++m)if(s_PtNation.State.Members[m].NationId==nation&&!s_PtNation.State.Members[m].Duty&&index--==0){lstrcpynA(target,s_PtNation.State.Members[m].Name,size);return 1;}return 0;
}
int PhongThanBotNationAction(int p,int op,unsigned int nation,int value,const char* target,const char* name){
 if(p<=0||p>=MAX_PLAYER||!Player[p].m_bIsBot||!Player[p].m_PlayerDBLoad||!PTNationLive(p))return 0;
 if(op!=PT_NATION_CREATE&&op!=PT_NATION_APPLY&&op!=PT_NATION_CANCEL&&op!=PT_NATION_ACCEPT&&op!=PT_NATION_DEPOSIT)return 0;
 PTNationRequest q;memset(&q,0,sizeof(q));q.Family=PT_NATION_FAMILY;q.Protocol=q.Version=1;q.Operation=op;q.RequestId=1;q.NationId=nation;q.Value=value;
 if(target)lstrcpynA(q.Target,target,sizeof(q.Target));if(name)lstrcpynA(q.Text,name,sizeof(q.Text));if(!PTNationRequestValid(&q,sizeof(q)))return 0;
 return PTNationExecute(p,q);
}
int PhongThanBotCityRequirement(int land,int field){
 PTTerritory::Poll();if(land<0||land>4||!PTTerritory::policy.enabled||!PTTerritory::Tables())return -1;
 return field==0?PTTerritory::Cell(PTTerritory::create,land+2,2):field==1?PTTerritory::Cell(PTTerritory::create,land+2,6):field==2?PTTerritory::policy.credit:field==3?PTTerritory::policy.popularity:field==4?PTTerritory::policy.fee:field==5?PTTerritory::policy.cap:-1;
}
