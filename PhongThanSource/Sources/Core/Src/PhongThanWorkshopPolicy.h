#ifndef PHONGTHAN_WORKSHOP_POLICY_H
#define PHONGTHAN_WORKSHOP_POLICY_H
#include "KIniFile.h"
#include <vector>
namespace PtWorkshopPolicy {
struct Charm {int genre,detail,part,level,series,bonus,first,last;};
struct Rate {int id,base;};
static KIniFile ini;static bool attempted=false,enabled=false,valid=true;
static std::vector<Charm> charms;static std::vector<Rate> rates;
static int Number(const char* section,const char* key,int fallback){int n=fallback;ini.GetInteger((char*)section,(char*)key,fallback,&n);return n;}
static bool Tuple(const char* section,const char* key,int* values,int count){char text[160],tail;ini.GetString((char*)section,(char*)key,"",text,sizeof(text));int n=0;
 if(count==5)n=sscanf(text,"%d,%d,%d,%d,%d%c",values,values+1,values+2,values+3,values+4,&tail);
 else if(count==6)n=sscanf(text,"%d,%d,%d,%d,%d,%d%c",values,values+1,values+2,values+3,values+4,values+5,&tail);
 if(n!=count)return false;for(int i=0;i<count;++i)if(values[i]<0)return false;return true;}
static unsigned long revision=0;static DWORD checked=0;static FILETIME stamp={0,0};
static void Load(){DWORD now=GetTickCount();if(!checked||now-checked>=2000){checked=now;WIN32_FILE_ATTRIBUTE_DATA data;FILETIME next={0,0};if(GetFileAttributesExA("settings\\phongthan\\EquipmentWorkshop.ini",GetFileExInfoStandard,&data))next=data.ftLastWriteTime;if(!attempted||CompareFileTime(&next,&stamp)){stamp=next;attempted=false;enabled=false;valid=true;ini.Clear();charms.clear();rates.clear();++revision;}}if(attempted)return;attempted=true;if(!ini.Load("\\settings\\phongthan\\EquipmentWorkshop.ini"))return;
 enabled=true;if(Number("Main","Version",0)!=1){valid=false;return;}
 int count=Number("Main","CharmCount",-1),rc=Number("Main","RateCount",-1);if(count<0||count>128||rc<0||rc>8192){valid=false;return;}
 for(int c=1;c<=count;++c){char section[32];sprintf(section,"Charm%d",c);int v[5];if(!Tuple(section,"Item",v,5)){valid=false;return;}Charm a;
  a.genre=v[0];a.detail=v[1];a.part=v[2];a.level=v[3];a.series=v[4];a.bonus=Number(section,"Bonus",-1);a.first=Number(section,"First",-1);a.last=Number(section,"Last",-1);
  if(a.bonus<0||a.bonus>100||a.first<1||a.last>12||a.first>a.last){valid=false;return;}charms.push_back(a);}
 for(int r=1;r<=rc;++r){char section[32];sprintf(section,"Rate%d",r);Rate a;a.id=Number(section,"Id",0);a.base=Number(section,"Base",-1);if(a.id<=0||a.base<0||a.base>100){valid=false;return;}rates.push_back(a);}
}
static bool Base(int id,int fallback,int& result){Load();if(!valid)return false;result=fallback;for(unsigned i=0;i<rates.size();++i)if(rates[i].id==id){result=rates[i].base;break;}return result>=0&&result<=100;}
static int Bonus(int genre,int detail,int part,int level,int series,int step){Load();int result=0;for(unsigned i=0;i<charms.size();++i){const Charm& c=charms[i];if(c.genre==genre&&c.detail==detail&&c.part==part&&step>=c.first&&step<=c.last)result+=c.bonus;}return result;}
}
#endif
