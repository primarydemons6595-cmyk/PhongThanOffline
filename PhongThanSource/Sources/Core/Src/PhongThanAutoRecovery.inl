#include "../../../Headers/PhongThanAutoRecovery.h"
#include <stdio.h>
#include "../../../Headers/PhongThanRecoveryCatalog.h"
#ifndef PT_AUTO_RECOVERY_TEST
extern BOOL PhongThanAutoPoolUse(int item,ItemPos pos);
#endif
struct PTRecoveryRow { int detail,percent; };
static PTRecoveryRow s_PtRecovery[6];
static unsigned int s_PtRecoveryTime[2];
static int s_PtAutoMode=1,s_PtAutoRadius=800,s_PtAutoAnchorX=0,s_PtAutoAnchorY=0;
static bool s_PtAutoHasAnchor=false,s_PtAutoPreview=false;
void PhongThanSetAutoPreview(bool visible){s_PtAutoPreview=visible;}
bool PhongThanGetAutoRange(int& x,int& y,int& radius){
    int npc=Player[CLIENT_PLAYER_INDEX].m_nIndex;if(npc<=0)return false;
    bool active=Player[CLIENT_PLAYER_INDEX].m_cAI.m_bIsActive!=0;
    if(!s_PtAutoPreview&&(!active||!(s_PtAutoMode&8)))return false;
    radius=s_PtAutoRadius;
    if(active&&s_PtAutoHasAnchor){x=s_PtAutoAnchorX;y=s_PtAutoAnchorY;}
    else Npc[npc].GetMpsPos(&x,&y);
    return true;
}
static void PhongThanResetRecovery(){memset(s_PtRecovery,0,sizeof(s_PtRecovery));memset(s_PtRecoveryTime,0,sizeof(s_PtRecoveryTime));s_PtAutoMode=1;s_PtAutoHasAnchor=false;s_PtAutoPreview=false;}
void PhongThanSetAutoMode(int bits,int radius,bool anchor){
    if(bits<0||bits>31||radius<100||radius>1200)return;
    int old=s_PtAutoMode;s_PtAutoMode=bits;s_PtAutoRadius=radius;
    int npc=Player[CLIENT_PLAYER_INDEX].m_nIndex;
    if(npc>0&&(anchor||((bits&8)&&!(old&8)))){Npc[npc].GetMpsPos(&s_PtAutoAnchorX,&s_PtAutoAnchorY);s_PtAutoHasAnchor=true;}
}
static bool PhongThanAutoOutsideRange(int target){
    if(!(s_PtAutoMode&8)||!s_PtAutoHasAnchor)return false;
    int x,y;Npc[target].GetMpsPos(&x,&y);int dx=x-s_PtAutoAnchorX,dy=y-s_PtAutoAnchorY;
    if(abs(dx)>s_PtAutoRadius||abs(dy)>s_PtAutoRadius)return true;
    return dx*dx+dy*dy>s_PtAutoRadius*s_PtAutoRadius;
}
void PhongThanSetRecoveryRow(int slot,int detail,int percent){
    if(slot<0||slot>=6||detail<0||detail>65535||percent< -100000000||percent>100)return;
    s_PtRecovery[slot].detail=detail;s_PtRecovery[slot].percent=percent;
}
static void PhongThanRecoveryTick(){
    int npc=Player[CLIENT_PLAYER_INDEX].m_nIndex;if(npc<=0)return;
    unsigned int now=IR_GetCurrentTime();
    for(int kind=0;kind<2;++kind){
        int life=kind?Npc[npc].m_CurrentMana:Npc[npc].m_CurrentLife;
        int maximum=kind?Npc[npc].m_CurrentManaMax:Npc[npc].m_CurrentLifeMax;
        if(maximum<=0||life>=maximum||now-s_PtRecoveryTime[kind]<1000)continue;
        // Try the lower/urgent threshold first, then fall back if that potion
        // is absent. A small-potion rule must not mask the large-potion rule.
        for(int i=kind*3+2;i>=kind*3;--i){
            PTRecoveryRow row=s_PtRecovery[i];
            if(!row.percent)continue;
            if(row.percent>0){if((double)life*100>=((double)maximum*row.percent))continue;}
            else if(life>=-row.percent)continue;
            bool used=false;
            for(PlayerItem* p=Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();p;p=Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem()){
                if(p->nPlace!=pos_equiproom&&p->nPlace!=pos_immediacy)continue;
                KItem& item=Item[p->nIdx];
                const PTAutoPoolDef* pool=PhongThanAutoPool(row.detail);
                if(pool){if(item.GetGenre()!=8||item.GetDetailType()!=pool->detail||item.GetParticular()!=pool->part||pool->kind!=kind+1)continue;}
                else if(item.GetGenre()!=item_medicine||item.GetDetailType()!=row.detail||!item.HasMedicineEffect(kind?154:153))continue;
                ItemPos pos;pos.nPlace=p->nPlace;pos.nX=p->nX;pos.nY=p->nY;
                int detail=item.GetDetailType();
                int requested=pool?PhongThanAutoPoolUse(p->nIdx,pos):Player[CLIENT_PLAYER_INDEX].ApplyUseItem(p->nIdx,pos);
                #ifndef PT_AUTO_RECOVERY_TEST
                FILE* trace=fopen("auto_recovery_trace.log","a");
                if(trace){fprintf(trace,"request=%d kind=%d detail=%d current=%d maximum=%d threshold=%d\n",requested,kind,detail,life,maximum,row.percent);fclose(trace);}
                #endif
                s_PtRecoveryTime[kind]=now;used=true;break;
            }
            if(used)break;
        }
    }
}
