#ifndef PHONGTHAN_JADE_RULES_H
#define PHONGTHAN_JADE_RULES_H
#include "KTabFile.h"
// The existing 40 signed generator values travel with canonical inventory,
// mail, auction and character records. No pointer or new record layout.
namespace PtJade {
enum { Marker=0x4a440001, MarkerSlot=39, ValueSlot=20, AttributeSlot=6, Groups=5 };
static KTabFile catalogue,attributes;
static bool ready=false;
static bool Tables(){if(!ready)ready=catalogue.Load("\\settings\\item\\001\\jadependant.txt")&&attributes.Load("\\settings\\item\\001\\jade_magic.txt");return ready;}
static int Num(KTabFile& t,int r,int c){int n=0;t.GetInteger(r,c,0,&n);return n;}
static bool Shape(int row,int& capacity,int& quality){
 if(!Tables()||row<0||row+2>catalogue.GetHeight())return false;
 capacity=Num(catalogue,row+2,69);quality=Num(catalogue,row+2,70);
 return capacity>=1&&capacity<=5&&quality>=1&&quality<=5;
}
static int Count(const int* levels){int n=0;for(int i=0;i<5;++i)if(levels[i])++n;return n;}
static bool Valid(int row,const int* levels){
 int capacity=0,quality=0;if(!Shape(row,capacity,quality))return false;
 if(levels[MarkerSlot]!=Marker)return false;
 if(Count(levels)>capacity)return false;
 for(int g=0;g<5;++g){int r=levels[g],v=levels[ValueSlot+g];
  if(!r){if(v)return false;continue;}
  if(r<2||r>attributes.GetHeight()||Num(attributes,r,4)!=g+1||Num(attributes,r,6)<1||Num(attributes,r,7+quality-1)<=0||
     v<Num(attributes,r,2)||v>Num(attributes,r,3))return false;
 }
 for(int i=5;i<MAX_ITEM_MAGICLEVEL;++i)if(i!=MarkerSlot&&(i<ValueSlot||i>=ValueSlot+5)&&levels[i])return false;
 return true;
}
static bool Roll(int row,int group,const int* before,int* after,unsigned pick,unsigned value){
 int capacity=0,quality=0;if(!Shape(row,capacity,quality)||group<1||group>5)return false;
 if(before[MarkerSlot] && !Valid(row,before))return false;
 if(!before[MarkerSlot])for(int i=0;i<MAX_ITEM_MAGICLEVEL;++i)if(before[i])return false;
 if(!before[group-1]&&Count(before)>=capacity)return false;
 int total=0;for(int sumRow=2;sumRow<=attributes.GetHeight();++sumRow)
  if(Num(attributes,sumRow,4)==group&&Num(attributes,sumRow,6)>=1)total+=Num(attributes,sumRow,6+quality);
 if(total<=0)return false;int draw=pick%total;
 for(int r=2;r<=attributes.GetHeight();++r)if(Num(attributes,r,4)==group&&Num(attributes,r,6)>=1){
  int weight=Num(attributes,r,6+quality);if(weight<=0)continue;if(draw>=weight){draw-=weight;continue;}
  int lo=Num(attributes,r,2),hi=Num(attributes,r,3),type=Num(attributes,r,1);
  if(type<1||type>=magic_normal_end||hi<lo||hi-lo>1000000)return false;
  memcpy(after,before,sizeof(int)*MAX_ITEM_MAGICLEVEL);after[group-1]=r;
  after[ValueSlot+group-1]=lo+(int)(value%(hi-lo+1));after[MarkerSlot]=Marker;
  return Valid(row,after);
 }return false;
}
static bool Apply(KItem& item){
 if(item.GetGenre()!=item_equip||item.GetDetailType()!=equip_shipin)return true;
 const int* v=item.m_GeneratorParam.nGeneratorLevel;
 if(!v[MarkerSlot])return true; // Older unrefined records remain unmodified.
 if(!Valid(item.GetRow(),v))return false;
 for(int g=0;g<5;++g){KItemNormalAttrib& a=item.m_aryMagicAttrib[AttributeSlot+g];ZeroMemory(&a,sizeof(a));
  if(v[g]){a.nAttribType=Num(attributes,v[g],1);a.nValue[0]=v[ValueSlot+g];}}
 return true;
}
static bool Initialize(KItem& item){
 if(item.GetGenre()!=item_equip||item.GetDetailType()!=equip_shipin)return true;
 int* v=item.m_GeneratorParam.nGeneratorLevel;if(v[MarkerSlot])return Valid(item.GetRow(),v);
#ifdef _SERVER
 int after[MAX_ITEM_MAGICLEVEL];
 if(!Roll(item.GetRow(),1+g_Random(5),v,after,g_Random(1000000),g_Random(1000000)))return false;
 memcpy(v,after,sizeof(after));
#endif
 return true;
}
}
#endif
