#include "LegacyDropExecutionPolicy.h"
#include <stdio.h>
#include <windows.h>
using namespace LegacyDrop;using namespace LegacyDrop::Execution;
static int checks=0,failed=0;
void Check(bool v,const char*s){++checks;if(!v){++failed;printf("FAIL %s\n",s);}}
class Equipment:public Catalog {public:bool Has(int g,int d,int p,int lv)const{return g==0&&d==0&&p==0&&lv>=1&&lv<=10;}bool EquipmentRequirement(int d,int p,int lv,int& required)const{required=lv*10;return true;}};
int main(int argc,char**argv){
 Table t=Parse("[Main]\nCount=1\nRandRange=100\nMagicRate=1\nMoneyRate=0\nMoneyScale=0\nMinItemLevel=1\nMaxItemLevel=10\nMinItemLevelScale=20\nMaxItemLevelScale=10\n[1]\nGenre=0\nDetail=0\nParticular=0\nRandRate=100\n");
 NpcSnapshot n;n.templateId=18;n.treasure=1;n.treasure1=0;for(int k=0;k<4;++k)n.slots[k]="test.ini";
 Quarantine q;std::string e;FILE*f=fopen("final_policy_quarantine.tmp","w+b");if(!f)return 2;fputs("V1\n",f);rewind(f);bool loaded=q.Load(f,e);fclose(f);remove("final_policy_quarantine.tmp");Check(loaded,"test quarantine initialized");Equipment cat;Plan p;Config c;c.profile=PROFILE_A;c.treasure=SUCCESS_CAP;c.level=EXPERIMENTAL_MONSTER_EQUIPMENT_CATALOG;c.magic=EXPERIMENTAL_NATIVE_CATALOG_ONLY;c.money=SYNTHETIC_FLAT_BERNOULLI;c.maxItems=64;c.npcLevel=5;
 Check(!p.Prepare(n,q,c,t,t,cat,e)&&e=="EQUIPMENT_NO_MONSTER_VALID_CANDIDATE","strict default remains closed");
 c.equipmentRequirementAhead=10;Check(p.Prepare(n,q,c,t,t,cat,e),"explicit bounded ahead permits valid tier1 requirement10");
 Result result;Random rng(7);Check(p.Draw(rng,result,e)&&result.items.size()==1&&result.items[0].level==1,"npc5 never randomizes tiers2to10");
 int low=0,high=0;Check(MonsterEquipmentResolver::Bounds(20,1,10,20,10,false,0,low,high,e)&&low==1&&high==2,"level20 divisor bound");
 Check(MonsterEquipmentResolver::Bounds(40,1,10,20,10,false,0,low,high,e)&&low==2&&high==4,"same template different npc level changes bound");
 Check(!MonsterEquipmentResolver::Bounds(20,1,10,20,10,true,10,low,high,e),"ahead never bypasses explicit tier bounds");
 c.equipmentRequirementAhead=101;Check(!p.Prepare(n,q,c,t,t,cat,e)&&e=="INVALID_EQUIPMENT_REQUIREMENT_AHEAD","ahead upper boundary");
 c.equipmentRequirementAhead=-1;Check(!p.Prepare(n,q,c,t,t,cat,e)&&e=="INVALID_EQUIPMENT_REQUIREMENT_AHEAD","negative ahead rejected");
 c.equipmentRequirementAhead=10;c.profile=PROFILE_B;Check(p.Prepare(n,q,c,t,t,cat,e),"same implementation profile B");
 q.npcs[18]="DATA_BLOCKED";Check(!p.Prepare(n,q,c,t,t,cat,e),"ahead cannot bypass quarantine");q.npcs.clear();
 Table broken=Parse("[Main]\nCount=1\nRandRange=100\n[2]\nGenre=0\nDetail=0\nParticular=0\nRandRate=1\n");Check(!broken.errors.empty()&&!p.Prepare(n,q,c,broken,t,cat,e),"malformed Count not relaxed");
 Check(LevelGateAllows(true,10,30,20)&&!LevelGateAllows(true,10,31,20),"player overlevel exact boundary");
 Check(!LiveNormalEngineEligible(1)&&LiveNormalEngineEligible(0),"custom Lua retains normal branch ownership");
 if(argc==3){
  WIN32_FIND_DATAA fd;std::string dir=argv[1],pattern=dir+"\\*.ini";HANDLE find=FindFirstFileA(pattern.c_str(),&fd);FILE*out=fopen(argv[2],"w");if(!out)return 3;fprintf(out,"file,count,status,reason\n");int files=0,rejected=0;
  if(find!=INVALID_HANDLE_VALUE){do{FILE*in=fopen((dir+"\\"+fd.cFileName).c_str(),"rb");if(!in){++failed;continue;}std::string bytes;char block[8192];size_t size;while((size=fread(block,1,sizeof(block),in)))bytes.append(block,size);fclose(in);Table parsed=Parse(bytes);++files;if(!parsed.errors.empty())++rejected;fprintf(out,"%s,%d,%s,%s\n",fd.cFileName,parsed.count,parsed.errors.empty()?"VALID":"DATA_BLOCKED",parsed.errors.empty()?"":parsed.errors[0].c_str());}while(FindNextFileA(find,&fd));FindClose(find);}fclose(out);Check(files==478,"all original main tables parsed");printf("PARSER_RESULT files=%d valid=%d rejected=%d NO_DATA_EDITS\n",files,files-rejected,rejected);
 }
 printf("FINAL_POLICY_RESULT checks=%d failed=%d KINGSOFT_PARITY_UNVERIFIED\n",checks,failed);return failed?1:0;
}
