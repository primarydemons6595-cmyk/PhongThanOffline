#include "../../../Headers/PhongThanOriginalItemBuffData.h"
static int PtOriginalItemBuffMask(KPlayer& p)
{
    int mask=0;const DWORD now=KSG_GetCurSec();
    for(int slot=0;slot<48;++slot){
        const DWORD packed=(DWORD)p.m_cTask.GetSaveVal(TASKVALUE_PT_IBBUFF_BEGIN+slot*2);
        const DWORD end=(DWORD)p.m_cTask.GetSaveVal(TASKVALUE_PT_IBBUFF_BEGIN+slot*2+1);
        if(!(packed>>22) || (end && end<=now))continue;
        for(int i=0;i<2;++i)if((packed&0x3fff)==ptOriginalItemBuffs[i].id)mask|=1<<i;
    }
    return mask;
}
static void PtOriginalItemBuffApply(KPlayer& p)
{
    if(p.m_nIndex<=0 || p.m_nIndex>=MAX_NPC)return;
    const int mask=PtOriginalItemBuffMask(p);
    // The two versions are alternatives, never additive copies of one item.
    for(int i=0;i<2;++i)if(mask&(1<<i)){
        for(int a=0;a<6;++a)if(ptOriginalItemBuffs[i].attrs[a][0]){
            KMagicAttrib attr;ZeroMemory(&attr,sizeof(attr));
            attr.nAttribType=ptOriginalItemBuffs[i].attrs[a][0];attr.nValue[0]=ptOriginalItemBuffs[i].attrs[a][1];
            Npc[p.m_nIndex].ModifyAttrib(p.m_nIndex,&attr);
        }
        break;
    }
}
static void PtOriginalItemBuffTick(KPlayer& p)
{
    static DWORD owner[MAX_PLAYER],actor[MAX_PLAYER];static int masks[MAX_PLAYER];
    const int player=(int)(&p-Player);
    if(player<0 || player>=MAX_PLAYER || p.m_nIndex<=0 || p.m_nIndex>=MAX_NPC)return;
    const int mask=PtOriginalItemBuffMask(p);
    const bool replaced=owner[player]!=p.m_dwID || actor[player]!=Npc[p.m_nIndex].m_dwID;
    const bool changed=replaced?mask!=0:masks[player]!=mask;
    owner[player]=p.m_dwID;actor[player]=Npc[p.m_nIndex].m_dwID;masks[player]=mask;
    if(changed)p.UpdataCurData();
}

// Original Hoa Than Toa: positive character experience is blocked for seven real days.
static bool PtOriginalExperienceLocked(KPlayer& p)
{
    const DWORD now=KSG_GetCurSec();
    for(int slot=0;slot<48;++slot){
        const DWORD packed=(DWORD)p.m_cTask.GetSaveVal(TASKVALUE_PT_IBBUFF_BEGIN+slot*2);
        const DWORD end=(DWORD)p.m_cTask.GetSaveVal(TASKVALUE_PT_IBBUFF_BEGIN+slot*2+1);
        if((packed&0x3fff)==9445 && (packed>>22)>0 && end>now)return true;
    }
    return false;
}
