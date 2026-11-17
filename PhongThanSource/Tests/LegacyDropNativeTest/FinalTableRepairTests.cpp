#include "KCore.h"
#include "KPakList.h"
#include "PhongThanConsumableData.h"
#include "LegacyDropNativeCatalog.h"
#include <malloc.h>
KPakList*g_pPakList=0;
int main(int argc,char**argv){
 if(argc!=3||!SetCurrentDirectoryA(argv[1]))return 2;g_SetRootPath(argv[1]);g_SetFilePath("\\");KPakList packs;if(!packs.Open("\\package.ini"))return 3;g_pPakList=&packs;g_SetPakFileMode(1);if(!ItemGen.Init())return 4;ItemSet.Init();LegacyDropItemAdapter adapter;if(!adapter.Init(ItemGen))return 5;LegacyDropNativeCatalog cat(adapter,ItemGen);
 FILE*f=fopen(argv[2],"rb");if(!f)return 6;std::string bytes;char buffer[8192];size_t n;while((n=fread(buffer,1,sizeof(buffer),f)))bytes.append(buffer,n);fclose(f);LegacyDrop::Table t=LegacyDrop::Parse(bytes);int failed=t.errors.size(),generated=0;
 for(size_t j=0;j<t.sections.size();++j){const LegacyDrop::Section&row=t.sections[j];if(row.name=="Main")continue;int g,d,p,lo,hi;LegacyDrop::Get(&row,"Genre",g);LegacyDrop::Get(&row,"Detail",d);LegacyDrop::Get(&row,"Particular",p);LegacyDrop::Get(LegacyDrop::Find(t,"Main"),"MinItemLevel",lo);LegacyDrop::Get(LegacyDrop::Find(t,"Main"),"MaxItemLevel",hi);int level;
  if(LegacyDrop::Get(&row,"Level",level))lo=hi=level;else if(g==2||g==3||g==4||g==5||g==8)lo=hi=0;else if(g==6||g==7)lo=hi=1;
  bool ok=false;std::string error;for(int lv=lo;lv<=hi&&hi-lo<=256;++lv){if(!cat.Has(g,d,p,lv))continue;PhongThanQuestItemTuple tuple;int item=0;if(adapter.Map(g,d,p,tuple)){tuple.level=lv;ok=adapter.GenerateMapped(tuple,item,error);}if(item)ItemSet.Remove(item);if(ok){++generated;printf("ROW_NATIVE section=%s tuple=%d/%d/%d level=%d CREATED\n",row.name.c_str(),g,d,p,lv);break;}}
  if(!ok){++failed;printf("DATA_BLOCKED section=%s tuple=%d/%d/%d reason=%s\n",row.name.c_str(),g,d,p,error.c_str());}
 }
 if(ItemSet.GetItemNumber()||_heapchk()!=_HEAPOK)++failed;printf("TABLE_REPAIR_RESULT count=%d generated=%d failed=%d spawn_death=0\n",t.count,generated,failed);g_pPakList=0;return failed?1:0;
}
