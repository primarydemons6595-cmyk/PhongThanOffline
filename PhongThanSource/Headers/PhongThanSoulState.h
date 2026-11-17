#ifndef PHONGTHAN_SOUL_STATE_H
#define PHONGTHAN_SOUL_STATE_H
#include "PhongThanSocketState.h"
#include "PhongThanSoulFormulaData.h"
#include "PhongThanSoulMagicData.h"
// Immutable equipment extensions use the existing persisted ScriptParam.
// Socket -> soul -> original forge/script parameter; no character ABI change.
struct PT_SOUL_STATE {int BaseParam,Formula,Aura,MagicType,MagicValue;};
inline bool PtSoulParam(int p){return ((unsigned)p&0xff000000U)==0xc3000000U;}
inline bool PtSoulValid(const PT_SOUL_STATE& s){
 const PTSoulFormula* f=PtSoulFormula(s.Formula);if(!f||PtSoulParam(s.BaseParam)||PtSocketParam(s.BaseParam)||s.Aura<0||s.Aura>f->capacity)return false;
 if(!f->colored)return !s.MagicType&&!s.MagicValue;
 for(int i=0;i<sizeof(ptSoulMagic)/sizeof(ptSoulMagic[0]);++i){const PTSoulMagic& m=ptSoulMagic[i];if(s.MagicType==m.type&&s.MagicValue>=m.minimum&&s.MagicValue<=m.maximum)return true;}return false;
}
inline bool PtSoulRollMagic(PT_SOUL_STATE& s,unsigned roll,unsigned value){
 const PTSoulFormula* f=PtSoulFormula(s.Formula);if(!f)return false;s.MagicType=s.MagicValue=0;if(!f->colored)return true;
 int pick=roll%100;for(int i=0;i<sizeof(ptSoulMagic)/sizeof(ptSoulMagic[0]);++i){const PTSoulMagic& m=ptSoulMagic[i];pick-=m.weight;if(pick<0){s.MagicType=m.type;s.MagicValue=m.minimum+value%(m.maximum-m.minimum+1);return true;}}return false;
}
bool PtSoulRead(int param,PT_SOUL_STATE& state);
bool PtSoulRegister(int param,const PT_SOUL_STATE& state);
bool PtSoulStore(const PT_SOUL_STATE& state,int& param);
inline int PtSoulInnerParam(int param){PT_SOCKET_STATE sockets;return PtSocketRead(param,sockets)?sockets.BaseParam:param;}
inline bool PtSoulFromParam(int param,PT_SOUL_STATE& state){return PtSoulRead(PtSoulInnerParam(param),state);}
inline int PtEquipmentBaseParam(int param){param=PtSoulInnerParam(param);PT_SOUL_STATE s;return PtSoulRead(param,s)?s.BaseParam:param;}
bool PtSoulReplace(int original,const PT_SOUL_STATE& state,int& param);
#ifndef _SERVER
void PtSoulRequest(int param);
#endif
#define PT_SOUL_QUERY 0x5114
#define PT_SOUL_REPLY 0x5115
#define PT_SOUL_COMPOSE_REQUEST 0x5116
#define PT_SOUL_COMPOSE_REPLY 0x5117
int& PtSoulUiNonce();
int PhongThanSoulOpen(int player,int mode,unsigned long stone=0);
int PhongThanSoulUseStone(int player,int item);
#pragma pack(push,1)
struct PT_SOUL_MESSAGE {PHONGTHAN_WIRE_HEADER Header;int Param;PT_SOUL_STATE State;};
#pragma pack(pop)
#endif
