// Local Studio control plane. Requests are revision/PID/time bound; only the
// world thread commits state. Studio never edits a live native data file.
namespace PTTerritory {
static void Json(FILE* f,const char* value){fputc('"',f);for(const unsigned char* p=(const unsigned char*)value;*p;++p){if(*p<32||*p>=128)fprintf(f,"\\u%04x",*p);else if(*p=='"'||*p=='\\'){fputc('\\',f);fputc(*p,f);}else fputc(*p,f);}fputc('"',f);}
static void Publish(){CreateDirectoryA("Territory",0);FILE* f=fopen("Territory\\status.json.tmp","wb");if(!f)return;
 fprintf(f,"{\"Version\":1,\"NamesAreTcvn\":true,\"Pid\":%lu,\"Utc\":%lu,\"Revision\":%u,\"Nations\":[",GetCurrentProcessId(),(unsigned long)time(0),s_PtNation.State.Revision);bool first=true;
 for(int n=0;n<PT_NATION_CAPACITY;++n){const PTNationRecord& country=s_PtNation.State.Nations[n];if(!country.Id)continue;PTNationDevelopment& d=Development(n);fprintf(f,"%s{\"Id\":%u,\"Name\":",first?"":",",country.Id);first=false;Json(f,country.Name);fputs(",\"Leader\":",f);Json(f,country.Leader);
  fprintf(f,",\"Money\":%d,\"Members\":%d,\"Popularity\":%d,\"CityLevel\":%d,\"Land\":%d,\"Totem\":%d,\"Map\":%d,\"Prosperity\":%d,\"CityName\":",country.Money,PTNationCount(country.Id),d.Popularity,d.CityLevel,d.Land,d.Totem,d.CityMapId,d.Prosperity);Json(f,d.CityName);fputs(",\"Resources\":[",f);
  for(int i=0;i<10;++i)fprintf(f,"%s%d",i?",":"",d.Resources[i]);fprintf(f,"],\"BookFragments\":[%d,%d],\"People\":[",Books(n).Fragments[0],Books(n).Fragments[1]);bool firstMember=true;
  for(int m=0;m<PT_NATION_MEMBER_CAPACITY;++m){const PTNationMember& person=s_PtNation.State.Members[m];if(person.NationId!=country.Id||!person.Duty)continue;PTTerritoryContribution& c=Contribution(m);fprintf(f,"%s{\"Name\":",firstMember?"":",");firstMember=false;Json(f,person.Name);fprintf(f,",\"Duty\":%d,\"Contribution\":%d,\"Today\":%d}",person.Duty,c.Total,c.Day==(int)(time(0)/86400)?c.Today:0);}
  fputs("],\"Buildings\":[",f);for(int b=0;b<d.BuildingCount;++b){const PTTerritoryBuilding& x=d.Buildings[b];fprintf(f,"%s{\"Id\":%d,\"Type\":%d,\"X\":%d,\"Y\":%d,\"CompleteAt\":%u}",b?",":"",b+1,x.Type,x.X,x.Y,x.CompleteAt);}fputs("]}",f);
 }fputs("]}",f);bool ok=!ferror(f);if(fclose(f))ok=false;if(ok)MoveFileExA("Territory\\status.json.tmp","Territory\\status.json",MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH);
}
static bool Guid(const char* text){if(strlen(text)!=32)return false;for(const char* p=text;*p;++p)if(!((*p>='0'&&*p<='9')||(*p>='a'&&*p<='f')))return false;return true;}
static bool Int(const char* text,int low,int high,int& value){if(!text||!text[0])return false;unsigned int v=0;for(const char* p=text;*p;++p){if(*p<'0'||*p>'9'||(unsigned int)(*p-'0')>(unsigned int)high||v>((unsigned int)high-(*p-'0'))/10)return false;v=v*10+(*p-'0');}if(v<(unsigned int)low||v>(unsigned int)high)return false;value=(int)v;return true;}
static bool Hex(const char* text,char* result,int capacity){int length=strlen(text);if(length%2||length/2>=capacity)return false;for(int i=0;i<length;i+=2){int value=0;for(int n=0;n<2;++n){int c=text[i+n],digit=c>='0'&&c<='9'?c-'0':c>='A'&&c<='F'?c-'A'+10:c>='a'&&c<='f'?c-'a'+10:-1;if(digit<0)return false;value=value*16+digit;}if(value<32)return false;result[i/2]=(char)value;}result[length/2]=0;return true;}
static bool AdminMutation(int nation,const char* operation,int value,int second,int x,int y,const char* text){PTNationDevelopment& d=Development(nation);
 for(int pending=0;pending<PT_NATION_MEMBER_CAPACITY;++pending)if(s_PtNation.State.Contributions[pending].NationId==d.NationId&&s_PtNation.State.Contributions[pending].PendingAfter)return false;
 if(!strcmp(operation,"nation_set")){if(second<0)return false;if(value==0){if(second>2000000000)return false;s_PtNation.State.Nations[nation].Money=second;return true;}if(value==1){if(second>100000000)return false;d.Popularity=second;return true;}if(value>=2&&value<12){d.Resources[value-2]=second;return true;}if(value==12||value==13){if(second>2000000000)return false;Books(nation).Fragments[value-12]=second;return true;}return false;}
 if(!strcmp(operation,"city_create"))return Create(nation,value,second,text,true);
 if(!strcmp(operation,"city_upgrade"))return Upgrade(nation);
 if(!strcmp(operation,"building_add"))return AddBuilding(nation,value,x,y);
 if(!strcmp(operation,"building_move")){if(value<1||value>d.BuildingCount)return false;int world=PhongThanTerritoryWorld(d.CityMapId);if(world<0||!Position(world,x,y))return false;d.Buildings[value-1].X=x;d.Buildings[value-1].Y=y;return true;}
 if(!strcmp(operation,"building_remove")){if(value<1||value>d.BuildingCount)return false;int type=d.Buildings[value-1].Type;if(type==0||type==2||type==6||type==17||type==18)return false;memmove(d.Buildings+value-1,d.Buildings+value,(d.BuildingCount-value)*sizeof(PTTerritoryBuilding));memset(d.Buildings+--d.BuildingCount,0,sizeof(PTTerritoryBuilding));return true;}
 if(!strcmp(operation,"contribution_set")){int m=s_PtNation.Member(text);if(m<0||s_PtNation.State.Members[m].NationId!=d.NationId||value<0)return false;PTTerritoryContribution& c=Contribution(m);if(c.PendingAfter)return false;int used=c.Total-c.Usable;c.Total=value;c.Usable=value>used?value-used:0;return true;}return false;
}
static void Receipt(const char* id,bool ok,const char* code){char path[MAX_PATH],temp[MAX_PATH];sprintf(path,"Territory\\receipts\\%s.json",id);sprintf(temp,"%s.tmp",path);FILE* f=fopen(temp,"wb");if(!f)return;fprintf(f,"{\"Ok\":%s,\"Code\":",ok?"true":"false");Json(f,code);fputs(",\"Message\":",f);Json(f,code);fprintf(f,",\"Revision\":%u}",s_PtNation.State.Revision);bool written=!ferror(f);if(fclose(f))written=false;if(written)MoveFileExA(temp,path,MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH);}
static void Commands(){CreateDirectoryA("Territory",0);CreateDirectoryA("Territory\\commands",0);CreateDirectoryA("Territory\\receipts",0);WIN32_FIND_DATAA data;HANDLE list=FindFirstFileA("Territory\\commands\\*.command",&data);if(list==INVALID_HANDLE_VALUE)return;int processed=0;
 do{if(data.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)continue;char id[40],path[MAX_PATH],receipt[MAX_PATH];int length=strlen(data.cFileName);if(length!=40)continue;memcpy(id,data.cFileName,32);id[32]=0;if(!Guid(id))continue;sprintf(path,"Territory\\commands\\%s",data.cFileName);sprintf(receipt,"Territory\\receipts\\%s.json",id);if(GetFileAttributesA(receipt)!=(DWORD)-1){DeleteFileA(path);continue;}
  if(++processed>8)break;FILE* f=fopen(path,"rb");if(!f)continue;char line[1024];int read=fread(line,1,sizeof(line)-1,f);bool overflow=!feof(f);fclose(f);line[read]=0;
  char* field[11];int count=1;field[0]=line;for(char* p=line;*p;++p)if(*p=='\t'){*p=0;if(count<11)field[count++]=p+1;else count=99;}
  int pid=0,utc=0,revision=0,owner=0,value=0,second=0,x=0,y=0;char text[65];bool valid=!overflow&&count==11&&!strcmp(field[0],id)&&Int(field[1],1,INT_MAX,pid)&&pid==(int)GetCurrentProcessId()&&Int(field[2],1,INT_MAX,utc)&&abs((int)time(0)-utc)<=15&&Int(field[3],0,INT_MAX,revision)&&Int(field[4],1,INT_MAX,owner)&&Int(field[6],0,INT_MAX,value)&&Int(field[7],0,INT_MAX,second)&&Int(field[8],0,10000000,x)&&Int(field[9],0,10000000,y)&&Hex(field[10],text,sizeof(text));
  const char* code="INVALID_REQUEST";bool ok=false;int nation=valid?s_PtNation.Nation(owner):-1;
  if(valid&&!policy.admin)code="ADMIN_DISABLED";else if(valid&&revision!=(int)s_PtNation.State.Revision)code="STALE_REVISION";else if(valid&&nation>=0){
   PTNationState* old=new PTNationState;memcpy(old,&s_PtNation.State,sizeof(*old));ok=AdminMutation(nation,field[5],value,second,x,y,text);code=ok?"UPDATED":"REQUIREMENTS_NOT_MET";
   if(ok&&!Save()){ok=false;code="SAVE_FAILED";}if(!ok)memcpy(&s_PtNation.State,old,sizeof(*old));delete old;
  }
  if(ok)Publish();Receipt(id,ok,code);DeleteFileA(path);
 }while(FindNextFileA(list,&data));FindClose(list);
}
}
void PhongThanTerritoryTick(){DWORD now=GetTickCount();if(PTTerritory::tickAt&&now-PTTerritory::tickAt<1000)return;DWORD delta=PTTerritory::tickAt?(now-PTTerritory::tickAt)/1000:0;PTTerritory::tickAt=now;PTTerritory::Poll();if(!PTNationReady()||!PTTerritory::Tables())return;if(delta>5)delta=5;
 if(PTTerritory::policy.enabled){
  for(int p=1;p<MAX_PLAYER;++p)if(PTNationLive(p)&&Player[p].m_PlayerDBLoad&&!Player[p].m_pStatusLoadPlayerInfo){int n=PTTerritory::NationForPlayer(p);if(n<0)continue;int member=PTNationSelf(p);if(member>=0&&s_PtNation.State.Contributions[member].PendingAfter)PhongThanTerritoryFinishDonation(p,false);PTNationDevelopment& d=PTTerritory::Development(n);d.OnlineRemainder+=(int)delta;if(d.OnlineRemainder>=PTTerritory::policy.seconds){int points=d.OnlineRemainder/PTTerritory::policy.seconds;d.OnlineRemainder%=PTTerritory::policy.seconds;if(d.Popularity<=100000000-points)d.Popularity+=points;PTTerritory::dirty=true;}}
  for(int n=0;n<PT_NATION_CAPACITY;++n)if(s_PtNation.State.Nations[n].Id){PTNationDevelopment& d=PTTerritory::Development(n);unsigned int utc=(unsigned int)time(0),day=utc/86400;
   if(d.PopularityDay&&day>d.PopularityDay){__int64 amount=(__int64)(day-d.PopularityDay)*PTTerritory::policy.decay;d.Popularity=amount>d.Popularity?0:d.Popularity-(int)amount;PTTerritory::dirty=true;}d.PopularityDay=day;
   for(int b=0;b<d.BuildingCount;++b)if(d.Buildings[b].CompleteAt&&d.Buildings[b].CompleteAt<=utc){d.Buildings[b].CompleteAt=0;if(d.Prosperity<=INT_MAX-PTTerritory::policy.perBuilding)d.Prosperity+=PTTerritory::policy.perBuilding;PTTerritory::dirty=true;}
   if(d.CityLevel&&utc>=d.ProsperityAt+3600){int row=PTTerritory::Row(PTTerritory::upgrade,d.CityLevel+1);if(row>=2&&d.Prosperity>=PTTerritory::Cell(PTTerritory::upgrade,row,2)){if(d.SustainedChecks<INT_MAX)++d.SustainedChecks;}else d.SustainedChecks=0;d.ProsperityAt=utc;PTTerritory::dirty=true;}
  }
 }
 PTTerritory::Commands();if(PTTerritory::dirty&&(!PTTerritory::saveAt||now-PTTerritory::saveAt>=60000)){if(PTTerritory::Save())PTTerritory::saveAt=now;}
 if(!PTTerritory::snapshotAt||now-PTTerritory::snapshotAt>=2000){PTTerritory::snapshotAt=now;PTTerritory::Publish();}
 for(int w=0;w<MAX_SUBWORLD;++w){int n=PTTerritory::NationForMap(SubWorld[w].m_SubWorldID);if(n>=0&&SubWorld[w].m_ContentLoaded&&!SubWorld[w].m_Dormant)PTTerritory::Visuals(w,n);}
}
