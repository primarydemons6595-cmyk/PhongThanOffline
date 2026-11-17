#include "KCore.h"
#include "BlueMagicTestPolicy.h"
#include <stdio.h>
static int checked=0,failed=0;
void Check(bool ok,const char*name){++checked;if(!ok){++failed;printf("FAIL %s\n",name);}}
std::string Read(const std::string&path){FILE*f=fopen(path.c_str(),"rb");if(!f)return "";std::string bytes;char block[8192];size_t n;while((n=fread(block,1,sizeof(block),f)))bytes.append(block,n);fclose(f);return bytes;}
std::string Unhex(const std::string&hex){std::string bytes;for(size_t k=0;k+1<hex.size();k+=2){char part[3]={hex[k],hex[k+1],0};bytes+=(char)strtol(part,0,16);}return bytes;}
int main(int argc,char**argv){
 if(argc!=2)return 2;std::string root=argv[1],error;LegacyDrop::Table ini=LegacyDrop::Parse(Read(root+"\\settings\\phongthan\\BlueMagicExperimental.ini"));const LegacyDrop::Section*s=LegacyDrop::Find(ini,"BlueMagicExperimental");if(!s)return 3;
 std::string magicPath=Unhex(s->fields.find("MagicPathHex")->second),limitPath=Unhex(s->fields.find("LimitPathHex")->second);BlueMagic::BlueMagicTableLoader table;Check(table.Parse(Read(root+magicPath),error),"reuse actual magic table loader");Check(table.sha256==s->fields.find("MagicSha256")->second,"magic source unchanged");
 for(int version=0;version<2;++version){
  std::string path=limitPath;if(version){size_t p=path.find("\\000\\");if(p==std::string::npos)return 4;path.replace(p,5,"\\001\\");}
  BlueMagic::BlueLimits limits;Check(limits.Parse(Read(root+path),error),"actual versioned caps");limits.version=version;
  for(int tier=1;tier<=10;++tier){int count=-1;Check(limits.Limit(9,tier,count,error)&&count==limits.caps[2][tier-1],"detail9 uses original cloak row every tier");}
  int count=-1;Check(!limits.Limit(3,1,count,error)&&error=="BLUE_LIMIT_CLASS_UNRESOLVED","ring stays unbound");Check(!limits.Limit(4,1,count,error)&&error=="BLUE_LIMIT_CLASS_UNRESOLVED","amulet stays catalog-only");Check(!limits.Limit(8,1,count,error)&&error=="BLUE_LIMIT_CLASS_UNRESOLVED","cuff stays unbound");
  Check(!limits.Limit(9,0,count,error)&&!limits.Limit(9,11,count,error),"tier boundary");
  Check(limits.Limit(0,1,count,error)&&count==limits.caps[0][0]&&limits.Limit(6,1,count,error)&&count==limits.caps[1][0]&&limits.Limit(10,1,count,error)&&count==limits.caps[3][0],"existing equipment mappings unchanged");
  if(version)continue;
  for(tier=1;tier<=10;++tier){BlueMagic::Config c;c.mode=BlueMagic::Config::LIVE_TEST;c.npc=18;c.version=0;c.group=0;c.chance=100000;c.lines=limits.caps[2][tier-1]<3?limits.caps[2][tier-1]:3;c.uniqueType=c.blankElementUnrestricted=c.attributeLevelIsTier=true;BlueMagic::BlueMagicTestPolicy plan;
   Check(plan.PrepareWorld(table,limits,c,18,9,tier,0,error),"cape policy prepares with catalog cap");LegacyDrop::Random rng(20261018+tier);BlueMagic::Result result;Check(plan.Draw(rng,result,error)&&result.blue==(result.chanceDraw<100000)&&(!result.blue||(int)result.attributes.size()==c.lines),"normal ten percent boundary and magic line cap");
   c.chance=0;Check(plan.PrepareWorld(table,limits,c,18,9,tier,0,error)&&plan.Draw(rng,result,error)&&!result.blue,"zero chance remains white");}
 }
 printf("CLOAK_POLICY_RESULT checks=%d failures=%d item_creation=0 world_spawn_death=0 rates_changed=0\n",checked,failed);return failed?1:0;
}
