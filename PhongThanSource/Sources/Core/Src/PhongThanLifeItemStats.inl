#include "../../../Headers/PhongThanLifeItemUseData.h"
// Internal life-item identifiers are data buffs, never Skills.txt IDs.
static void PtLifeItemBuffApply(KPlayer& p){
 if(p.m_nIndex<=0||p.m_nIndex>=MAX_NPC)return;DWORD now=KSG_GetCurSec();
 for(int slot=0;slot<48;++slot){DWORD packed=(DWORD)p.m_cTask.GetSaveVal(TASKVALUE_PT_IBBUFF_BEGIN+slot*2),end=(DWORD)p.m_cTask.GetSaveVal(TASKVALUE_PT_IBBUFF_BEGIN+slot*2+1);
  const PTLifeItemUse* d=PtLifeBuff(packed&0x3fff);if(!d||!(packed>>22)||end<=now)continue;
  for(int a=0;a<12;++a)if(d->attrs[a][0]){KMagicAttrib v;ZeroMemory(&v,sizeof(v));v.nAttribType=d->attrs[a][0];v.nValue[0]=d->attrs[a][1];Npc[p.m_nIndex].ModifyAttrib(p.m_nIndex,&v);}
 }
}
static void PtLifeItemBuffTick(KPlayer& p){
 static DWORD owners[MAX_PLAYER],actors[MAX_PLAYER],signatures[MAX_PLAYER];int index=(int)(&p-Player);if(index<0||index>=MAX_PLAYER||p.m_nIndex<=0||p.m_nIndex>=MAX_NPC)return;
 DWORD signature=0,now=KSG_GetCurSec();for(int i=0;i<48;++i){DWORD packed=(DWORD)p.m_cTask.GetSaveVal(TASKVALUE_PT_IBBUFF_BEGIN+i*2),end=(DWORD)p.m_cTask.GetSaveVal(TASKVALUE_PT_IBBUFF_BEGIN+i*2+1);if((packed>>22)&&end>now&&PtLifeBuff(packed&0x3fff))signature=signature*33+(packed&0x3fff);}
 bool replaced=owners[index]!=p.m_dwID||actors[index]!=Npc[p.m_nIndex].m_dwID;bool changed=replaced?signature!=0:signature!=signatures[index];owners[index]=p.m_dwID;actors[index]=Npc[p.m_nIndex].m_dwID;signatures[index]=signature;if(changed)p.UpdataCurData();
}
