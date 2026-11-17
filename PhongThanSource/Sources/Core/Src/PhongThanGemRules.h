#ifndef PHONGTHAN_GEM_RULES_H
#define PHONGTHAN_GEM_RULES_H
#include "../../../Headers/PhongThanSocketState.h"
#include "KTabFile.h"
namespace PtGem {
static KTabFile table;
static bool ready=false;
static bool Tables(){if(!ready)ready=table.Load("\\settings\\item\\da_kong_xiang_qian\\bao_shi_shu_xing_biao_sc.txt")!=0;return ready;}
static int Num(int row,int col){int n=0;table.GetInteger(row,col,0,&n);return n;}
static bool Tagged(int p){return ((unsigned)p&0xc0000000U)==0x80000000U;}
static int Row(int id){if(!Tables())return 0;char key[24];sprintf(key,"%d",id);return table.FindRow(key);}
static int Identity(int genre,int detail,int part,int level){
 if(genre!=3||!Tables())return 0;
 for(int r=2;r<=table.GetHeight();++r)if(Num(r,2)==genre&&Num(r,3)==detail&&Num(r,4)==part&&Num(r,5)==level)return Num(r,1);
 return 0;
}
static bool Decode(int param,int gem,PT_SOCKET_ATTRIBUTE& result){
 if(!Tagged(param)||gem<1||gem>63||(((unsigned)param>>24)&63)!=(unsigned)gem)return false;
 int row=Row(gem),slot=((unsigned)param>>20)&15,value=(unsigned)param&0xfffff;
 if(row<2||slot>=10||Num(row,8)!=1)return false;
 int col=9+slot*5,type=Num(row,col),lo=Num(row,col+1),hi=Num(row,col+2),prob=Num(row,col+3);
 if(type<1||type>=magic_normal_end||lo<0||hi<lo||value<lo||value>hi||prob<1||prob>100)return false;
 ZeroMemory(&result,sizeof(result));result.Type=type;result.Value[0]=value;return true;
}
static bool Roll(int gem,unsigned attributeDraw,unsigned valueDraw,int& param){
 int row=Row(gem);if(row<2||gem>63||Num(row,8)!=1)return false;
 int previous=0,pick=attributeDraw%100;
 for(int slot=0;slot<10;++slot){int col=9+slot*5,type=Num(row,col),lo=Num(row,col+1),hi=Num(row,col+2),bound=Num(row,col+3);
  if(!type&&!bound)continue;
  if(type<1||type>=magic_normal_end||lo<0||hi<lo||hi>0xfffff||bound<=previous||bound>100)return false;
  previous=bound;if(pick>=bound)continue;
  int value=lo+valueDraw%(hi-lo+1);param=(int)(0x80000000U|((unsigned)gem<<24)|((unsigned)slot<<20)|(unsigned)value);
  PT_SOCKET_ATTRIBUTE check;return Decode(param,gem,check);
 }return false;
}
static bool Apply(KItem& item,int param){
 int gem=Identity(item.GetGenre(),item.GetDetailType(),item.GetParticularType(),item.GetLevel());
 if(!gem)return false;PT_SOCKET_ATTRIBUTE value;if(!Decode(param,gem,value))return false;
 ZeroMemory(item.m_aryMagicAttrib,sizeof(item.m_aryMagicAttrib));
 item.m_aryMagicAttrib[0].nAttribType=value.Type;
 memcpy(item.m_aryMagicAttrib[0].nValue,value.Value,sizeof(value.Value));item.m_CommonAttrib.nParam=param;return true;
}
static bool Maximum(KItem& item,int slot){
 if(slot!=0)return false;int gem=Identity(item.GetGenre(),item.GetDetailType(),item.GetParticularType(),item.GetLevel());
 PT_SOCKET_ATTRIBUTE value;if(!gem||!Decode(item.GetParam(),gem,value))return false;
 int row=Row(gem),col=9+((((unsigned)item.GetParam())>>20)&15)*5;
 return value.Value[0]==Num(row,col+2);
}
static bool MaximumValue(int gem,int type,int value){
 int row=Row(gem);if(row<2)return false;
 for(int slot=0;slot<10;++slot){int col=9+slot*5;
  if(Num(row,col)==type&&Num(row,col+3)>0&&value==Num(row,col+2))return true;
 }return false;
}
static bool Restore(KItem& item,int param){
 int gem=Identity(item.GetGenre(),item.GetDetailType(),item.GetParticularType(),item.GetLevel());if(!gem)return false;
 if(Tagged(param))return Apply(item,param);
#ifdef _SERVER
 if(param==-1||param==0){
  // Pre-fix records have no saved roll. Stable migration prevents rerolling
  // old auction/mail previews or changing the line on repeated reconnects.
  unsigned seed=(unsigned)item.GetDetailType()*2654435761U;int stable=0;
  return Roll(gem,seed,seed^(seed>>16),stable)&&Apply(item,stable);
 }
#endif
 item.m_CommonAttrib.nParam=param;ZeroMemory(item.m_aryMagicAttrib,sizeof(item.m_aryMagicAttrib));return true;
}
static bool Initialize(KItem& item){
 int gem=Identity(item.GetGenre(),item.GetDetailType(),item.GetParticularType(),item.GetLevel());if(!gem)return true;
#ifdef _SERVER
 int param=0;if(!Roll(gem,g_Random(100),g_Random(1000000),param)||!Apply(item,param))return false;
#endif
 return true;
}
static bool Unpolished(int detail){
 return (detail>=253&&detail<=280)||detail==374||detail==383||detail==392||detail==401||detail==5231||
  (detail>=5225&&detail<=5227)||(detail>=5524&&detail<=5537);
}
}
#endif
