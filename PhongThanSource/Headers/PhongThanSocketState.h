#ifndef PHONGTHAN_SOCKET_STATE_H
#define PHONGTHAN_SOCKET_STATE_H
#include "PhongThanWorldProtocol.h"
#include <map>
#include <stdio.h>
#include <string.h>
// Immutable socket definitions are referenced by the existing ScriptParam.
// Canonical character, mailbox and auction item layouts remain unchanged.
struct PT_SOCKET_ATTRIBUTE {int Type,Value[3];};
struct PT_SOCKET_STATE {int BaseParam,Holes[4],Gems[4],Bonus[4];PT_SOCKET_ATTRIBUTE Attributes[4];};
inline bool PtSocketParam(int p){return ((unsigned)p&0xff000000U)==0xc2000000U;}
inline bool PtSocketValid(const PT_SOCKET_STATE& s){
 if(PtSocketParam(s.BaseParam))return false;
 for(int i=0;i<4;++i){if(s.Holes[i]<0||s.Holes[i]>100||s.Gems[i]<0||s.Gems[i]>53||(!s.Holes[i]&&s.Gems[i]))return false;
  const PT_SOCKET_ATTRIBUTE& a=s.Attributes[i];if(a.Type<0||a.Type>=357||(!s.Gems[i]&&a.Type)|| (s.Gems[i]&&!a.Type))return false;
  if(s.Bonus[i]<0||s.Bonus[i]>1000000||s.Bonus[i]>a.Value[0])return false;
  for(int k=0;k<3;++k)if(a.Value[k]<-1000000||a.Value[k]>1000000)return false;
 }return true;
}
inline unsigned PtSocketHash(const void* data,unsigned size){unsigned h=2166136261U;const unsigned char* p=(const unsigned char*)data;while(size--){h^=*p++;h*=16777619U;}return h;}
std::map<int,PT_SOCKET_STATE>& PtSocketCache();
bool PtSocketRead(int param,PT_SOCKET_STATE& state);
bool PtSocketStore(const PT_SOCKET_STATE& state,int& param);
bool PtSocketRegister(int param,const PT_SOCKET_STATE& state);
int& PtSocketComposeMode();
int PtSocketPower(int param);
// Local query v2 separates the hole animation from its gem. A new query ID
// makes mixed old/new Game/CoreClient pairs fail without overrunning v1 buffers.
// Persistent and network socket state is unchanged.
#define PT_SOCKET_VISUAL_QUERY 0x505448
struct PT_SOCKET_VISUALS {int Count,X[4],Y[4],Slot[4];char Image[4][128],MenuImage[4][128],HoleImage[4][128];};
class KItem;
bool PtSocketVisuals(KItem& item,PT_SOCKET_VISUALS& visuals);
#ifndef _SERVER
void PtSocketRequest(int param);
#endif
#define PT_SOCKET_QUERY 0x5112
#define PT_SOCKET_REPLY 0x5113
#pragma pack(push,1)
struct PT_SOCKET_MESSAGE {PHONGTHAN_WIRE_HEADER Header;int Param;PT_SOCKET_STATE State;};
#pragma pack(pop)
#endif
