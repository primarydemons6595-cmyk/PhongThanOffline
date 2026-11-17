#ifndef PHONGTHAN_SOCKET_RULES_H
#define PHONGTHAN_SOCKET_RULES_H
#include "../../../Headers/PhongThanSocketState.h"
#include "KTabFile.h"
#include "PhongThanGemRules.h"
#include <stdlib.h>
// Original tuple: quantity.genre.detail.particular.series.level.
// Lists, ranges and *<exclusions> retain their authored meaning.
inline bool PtSocketNumber(const char*& p,int& n){n=0;while(*p==' '||*p=='\t')++p;if(*p<'0'||*p>'9')return false;do{int d=*p++-'0';if(n>(2147483647-d)/10)return false;n=n*10+d;}while(*p>='0'&&*p<='9');while(*p==' '||*p=='\t')++p;return true;}
inline bool PtSocketSelector(const char* text,int value){
 if(!strcmp(text,"*"))return true;bool negate=text[0]=='*'&&text[1]=='<';const char* p=text+(negate?2:0);char end=negate?'>':*p=='['?']':0;if(!negate&&end)++p;
 bool found=false;do{int lo=0,hi=0;if(!PtSocketNumber(p,lo))return false;hi=lo;if(*p=='-'){++p;if(!PtSocketNumber(p,hi)||hi<lo)return false;}if(value>=lo&&value<=hi)found=true;if(!end)return !*p&&found;if(*p==end)return !p[1]&&(negate?!found:found);if(*p++!=',')return false;}while(*p);return false;
}
inline bool PtSocketTuple(const char* text,KItem& item){
 if(!strcmp(text,"*"))return true;int values[6]={item.GetStackNum(),item.GetGenre(),item.GetDetailType(),item.GetParticularType(),item.GetSeries(),item.GetLevel()};const char* p=text;
 for(int i=0;i<6;++i){char part[128];int n=0;while(*p&&*p!='.'){if(n>=127)return false;part[n++]=*p++;}part[n]=0;if(!PtSocketSelector(part,values[i]))return false;if(!*p)return i>=2;++p;}return !*p;
}
namespace PtSocketRules {
static KTabFile drill,limit,drillEquip,drillPosition,holes,gems,inlay,inlayHole,inlayEquip,inlayPosition,groups;
static bool ready=false;
static bool LoadOne(KTabFile& t,const char* name){char p[200];sprintf(p,"\\settings\\item\\da_kong_xiang_qian\\%s_sc.txt",name);return t.Load(p)!=0;}
static bool Tables(){if(!ready)ready=LoadOne(drill,"da_kong_xu_qiu_biao")&&LoadOne(limit,"zhuang_bei_da_kong_xian_zhi_biao")&&LoadOne(drillEquip,"da_kong_zhuang_bei_gui_ze_biao")&&LoadOne(drillPosition,"da_kong_kong_wei_gui_ze_biao")&&LoadOne(holes,"kong_shu_xing_biao")&&LoadOne(gems,"bao_shi_shu_xing_biao")&&LoadOne(inlay,"bao_shi_kong_xiang_qian_xu_qiu_biao")&&LoadOne(inlayHole,"bao_shi_kong_xiang_qian_gui_ze_biao")&&LoadOne(inlayEquip,"bao_shi_xiang_qian_zhuang_bei_gui_ze_biao")&&LoadOne(inlayPosition,"bao_shi_xiang_qian_kong_wei_gui_ze_biao")&&LoadOne(groups,"bao_shi_fen_zu_deng_ji_xin_xi_biao");return ready;}
static int Num(KTabFile& t,int r,int c,int def=0){int n=def;t.GetInteger(r,c,def,&n);return n;}
static int Row(KTabFile& t,int id){char s[24];sprintf(s,"%d",id);return t.FindRow(s);}
static bool Select(KTabFile& t,int r,int c,int n){char s[160];t.GetString(r,c,"",s,sizeof(s));return PtSocketSelector(s,n);}
static bool Equip(KTabFile& t,int r,int c,KItem& item){char s[160];t.GetString(r,c,"",s,sizeof(s));return PtSocketTuple(s,item);}
static int Group(int gem){for(int r=2;r<=groups.GetHeight();++r)for(int c=3;c<=6;++c)if(Num(groups,r,c)==gem)return Num(groups,r,2)?Num(groups,r,1):0;return 0;}
struct Plan {int Main,Position,Gem,GemParam,Recipe,Money,Chance,Used[9],HoleId[24],Threshold[24],Count;PT_SOCKET_STATE Before;};
static bool Materials(KTabFile& t,int row,int first,const int* indexes,const int* selected,int main,int gemSlot,bool exact,int* used){
 int left[9];ZeroMemory(used,sizeof(int)*9);for(int i=0;i<9;++i)left[i]=indexes[i]?selected[i]:0;
 if(left[main]<1)return false;--left[main];used[main]=1;
 if(gemSlot>=0){if(left[gemSlot]<1)return false;--left[gemSlot];++used[gemSlot];}
 for(int n=0;n<6;++n){char text[128];t.GetString(row,first+n,"",text,sizeof(text));if(!text[0])continue;const char* p=text;int quantity=0;if(!PtSocketNumber(p,quantity)||quantity<1||quantity>10000||*p!='.')return false;
  // Ignore the tuple quantity while matching the stack identity.
  char identity[128];sprintf(identity,"*%s",p);int amount=quantity;
  for(int s=0;s<9&&amount>0;++s)if(s!=main&&s!=gemSlot&&left[s]>0&&PtSocketTuple(identity,Item[indexes[s]])){int take=left[s]<amount?left[s]:amount;left[s]-=take;used[s]+=take;amount-=take;}if(amount)return false;
 }
 for(int e=0;e<9;++e)if(left[e]>0&&(exact||!used[e]))return false;return true;
}
static int Quote(const int* indexes,const int* selected,bool exact,Plan& out,int position=0){
 ZeroMemory(&out,sizeof(out));out.Main=-1;out.Position=-1;if(!Tables())return PT_COMPOSE_TABLE_ERROR;
 int gemSlot=-1,gemId=0;
 for(int s=0;s<9;++s)if(indexes[s]&&selected[s]>0){KItem& item=Item[indexes[s]];if(item.GetGenre()==item_equip){if(out.Main>=0||selected[s]!=1)return PT_COMPOSE_INVALID_ITEMS;out.Main=s;}
  else for(int r=2;r<=gems.GetHeight();++r)if(item.GetGenre()==Num(gems,r,2)&&item.GetDetailType()==Num(gems,r,3)&&item.GetParticularType()==Num(gems,r,4)&&item.GetLevel()==Num(gems,r,5)) {if(gemSlot>=0)return PT_COMPOSE_INVALID_ITEMS;gemSlot=s;gemId=Num(gems,r,1);break;}}
 if(out.Main<0)return PT_COMPOSE_NO_RECIPE;KItem& main=Item[indexes[out.Main]];
 if(PtSocketParam(main.GetParam())){if(!PtSocketRead(main.GetParam(),out.Before))return PT_COMPOSE_TABLE_ERROR;}else out.Before.BaseParam=main.GetParam();
 int count=0;for(int h=0;h<4;++h)if(out.Before.Holes[h])++count;
 if(gemSlot>=0){
  for(int pos=0;pos<4;++pos)if(out.Before.Holes[pos]&&(position?pos==position-1:!out.Before.Gems[pos])){
   int group=Group(gemId);bool duplicate=false;for(int old=0;old<4;++old)if(old!=pos&&group&&Group(out.Before.Gems[old])==group)duplicate=true;if(duplicate)continue;
   int hole=out.Before.Holes[pos];if(!Equip(inlayEquip,Row(inlayEquip,gemId),2,main)||!Select(inlayPosition,Row(inlayPosition,gemId),2,pos))continue;
   bool allowed=false;for(int a=2;a<=inlayHole.GetHeight();++a)if(Num(inlayHole,a,2)==gemId&&Select(inlayHole,a,1,hole)){allowed=true;break;}if(!allowed)continue;
   for(int r=2;r<=inlay.GetHeight();++r)if(Num(inlay,r,1)==hole&&Select(inlay,r,2,gemId)&&Equip(inlay,r,9,main)&&Select(inlay,r,11,pos)){
    char skill[128];inlay.GetString(r,13,"",skill,sizeof(skill));if(skill[0]||Num(inlay,r,12)!=100)continue;
    if(!Materials(inlay,r,3,indexes,selected,out.Main,gemSlot,exact,out.Used))continue;
   out.Position=pos;out.Gem=gemId;out.GemParam=Item[indexes[gemSlot]].GetParam();out.Recipe=10000+hole;out.Money=Num(inlay,r,10,-1);out.Chance=10000;return out.Money>=0?PT_COMPOSE_OK:PT_COMPOSE_TABLE_ERROR;
   }
  }return PT_COMPOSE_NO_RECIPE;
 }
 int maximum=0;for(int l=2;l<=limit.GetHeight();++l)if(Equip(limit,l,1,main))maximum=Num(limit,l,2);
 for(int r=2;r<=drill.GetHeight();++r){int id=Num(drill,r,1),pos=Num(drill,r,62,-1);if(pos<0||pos>=maximum||pos>=4)continue;
  // White ordinary armor/cloaks have two sockets. Authored expanded holy
  // families in rule 345 retain their extra socket entitlement.
  if((main.GetDetailType()==2||main.GetDetailType()==9)&&pos>=2&&main.GetPhongThanQuality()==0&&!Equip(drill,Row(drill,345),56,main))continue;
  if(!Equip(drill,r,56,main)||!Equip(drillEquip,Row(drillEquip,id),2,main)||!Select(drillPosition,Row(drillPosition,id),2,count)||!Select(drill,r,58,count))continue;
  bool replace=Num(drill,r,61)==1;if((out.Before.Holes[pos]!=0)!=replace)continue;
  char skill[128];drill.GetString(r,60,"",skill,sizeof(skill));if(skill[0]||Num(drill,r,59)!=100)continue;
  if(!Materials(drill,r,50,indexes,selected,out.Main,-1,exact,out.Used))continue;
  int previous=0;out.Count=0;for(int c=2;c<=48;c+=2){int hole=Num(drill,r,c),bound=Num(drill,r,c+1);if(!hole&&!bound)continue;
   // Original row 68 has 3300 after 6600, unlike the other eleven rows
   // of the same decoration family (3300/6600/9900/10000). Repair this
   // exact malformed cell only; the original table remains byte-identical.
   if(id==68&&hole==13&&bound==3300&&previous==6600)bound=9900;
   if(hole<1||hole>100||bound<=previous||bound>10000||Row(holes,hole)<2)return PT_COMPOSE_TABLE_ERROR;
   out.HoleId[out.Count]=hole;out.Threshold[out.Count++]=bound;previous=bound;}
  if(!out.Count)return PT_COMPOSE_TABLE_ERROR;
  out.Position=pos;out.Recipe=id;out.Money=Num(drill,r,57,-1);out.Chance=previous;return out.Money>=0?PT_COMPOSE_OK:PT_COMPOSE_TABLE_ERROR;
 }return PT_COMPOSE_NO_RECIPE;
}
static int DecorationBonus(int hole,int gem,int type){
 int r=Row(gems,gem);if(r<2)return 0;
 char active[512];holes.GetString(Row(holes,hole),6,"",active,sizeof(active));if(!active[0])return 0;
 const char* q=active;int vals[4]={1,Num(gems,r,2),Num(gems,r,3),Num(gems,r,4)};
 for(int f=0;f<4;++f){char component[512];int n=0;while(*q&&*q!='.'&&n<511)component[n++]=*q++;component[n]=0;if(!PtSocketSelector(component,vals[f]))return 0;if(*q=='.')++q;else if(f<3)return 0;}
 for(int a=0;a<10;++a)if(Num(gems,r,9+a*5)==type)return Num(gems,r,13+a*5)*(Num(holes,Row(holes,hole),7)?2:1);
 return 0;
}
static bool Resolve(const Plan& p,unsigned draw,unsigned attributeDraw,unsigned valueDraw,PT_SOCKET_STATE& after){
 after=p.Before;
 if(!p.Gem){for(int c=0;c<p.Count;++c)if((int)(draw%10000)<p.Threshold[c]){after.Holes[p.Position]=p.HoleId[c];
   if(after.Gems[p.Position]){int bonus=DecorationBonus(after.Holes[p.Position],after.Gems[p.Position],after.Attributes[p.Position].Type);after.Attributes[p.Position].Value[0]+=bonus-after.Bonus[p.Position];after.Bonus[p.Position]=bonus;}
   return PtSocketValid(after);}return false;}

 if(p.GemParam!=-1 && p.GemParam!=0){
  PT_SOCKET_ATTRIBUTE value;if(!PtGem::Decode(p.GemParam,p.Gem,value))return false;
  after.Attributes[p.Position]=value;after.Bonus[p.Position]=DecorationBonus(after.Holes[p.Position],p.Gem,value.Type);
  after.Attributes[p.Position].Value[0]+=after.Bonus[p.Position];after.Gems[p.Position]=p.Gem;return PtSocketValid(after);
 }
 int r=Row(gems,p.Gem);if(r<2||Num(gems,r,8)!=1)return false;
 int pick=attributeDraw%100,previous=0;
 for(int a=0;a<10;++a){int c=9+a*5,type=Num(gems,r,c),lo=Num(gems,r,c+1),hi=Num(gems,r,c+2),bound=Num(gems,r,c+3);
  if(!type&&!bound)continue;if(type<1||type>=magic_normal_end||lo<0||hi<lo||hi>1000000||bound<=previous||bound>100)return false;
  previous=bound;if(pick>=bound)continue;
  PT_SOCKET_ATTRIBUTE& v=after.Attributes[p.Position];ZeroMemory(&v,sizeof(v));v.Type=type;v.Value[0]=lo+valueDraw%(hi-lo+1);after.Bonus[p.Position]=0;
  after.Bonus[p.Position]=DecorationBonus(after.Holes[p.Position],p.Gem,type);v.Value[0]+=after.Bonus[p.Position];
  after.Gems[p.Position]=p.Gem;return PtSocketValid(after);
 }return false;
}
}
#endif
