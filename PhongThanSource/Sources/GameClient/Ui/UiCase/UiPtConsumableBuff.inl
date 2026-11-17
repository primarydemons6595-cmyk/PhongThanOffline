#include "../../../../Headers/PhongThanLifeItemUseData.h"
#include "../../../../Headers/PhongThanStall.h"
#include "../../../../Headers/PhongThanIBBuffUi.h"
#include "../../../../Headers/PhongThanQuestSeals.h"
#include "../../../../Headers/PhongThanTooltipWrap.h"
void KPtBuffIcon::SetStatusTip(const char* text){
 PtWrapTooltip(text,m_StatusTip,sizeof(m_StatusTip),48);
 m_StatusTipLength=TEncodeText(m_StatusTip,strlen(m_StatusTip));
}
// Layout/image/title/description bytes are from the recovered PT bufferconfig.
#include "../../../../Headers/PhongThanStatusPages.h"
int KPtBuffIcon::WndProc(unsigned int msg,unsigned int param,int data){
 if(msg==WM_RBUTTONDOWN){FILE* f=fopen("pt_buff_cancel.log","a");if(f){fprintf(f,"right_click id=%d\n",m_CancelId);fclose(f);}}
 if(msg==WM_RBUTTONDOWN&&m_CancelId!=0&&g_pCoreShell){
  PT_STATUS_PAGES_REQUEST q;ZeroMemory(&q,sizeof(q));q.Operation=PT_STATUS_CANCEL_ITEM_BUFF;q.Value=m_CancelId;
  g_pCoreShell->TeamOperation(PT_STATUS_PAGES_ACTION,(unsigned int)&q,sizeof(q));return 1;
 }
 int result=KWndButton::WndProc(msg,param,data);
 if((msg==WND_M_MOUSE_ENTER||msg==WM_MOUSEMOVE||msg==WM_MOUSEHOVER)&&m_StatusTipLength>0){
  g_MouseOver.SetMouseHoverInfo((void*)(KWndWindow*)this,0,LOWORD(data),HIWORD(data),false,true,false);
  g_MouseOver.SetMouseHoverTitle(m_StatusTip,m_StatusTipLength,0xffffffff);
 }
 return result;
}
static bool PtQuestSealUiHex(KIniFile& ini,const char* section,const char* field,char* out,int size){
 char hex[4096];ini.GetString(section,field,"",hex,sizeof(hex));int n=strlen(hex);if(n%2||n/2>=size)return false;
 for(int i=0;i<n;i+=2){char pair[3]={hex[i],hex[i+1],0};char* end=0;int c=strtol(pair,&end,16);if(*end||c<=0||c>255)return false;out[i/2]=(char)c;}out[n/2]=0;return true;
}
void KUiPlayerControlBar::LoadPowderScheme(const char* scheme)
{
    char path[256];
    sprintf(path,"%s\\PtConsumableBuff.ini",scheme);
    m_bPtPowderScheme=m_PtPowderIni.Load(path)!=0;
    if(!m_bPtPowderScheme) return;
    sprintf(path,"%s\\PtNativeTimedItems.ini",scheme);
    m_PtTimedItemsIni.Load(path);
    sprintf(path,"%s\\PtSkillBuff.ini",scheme);
    m_PtSkillBuffIni.Load(path);
    KIniFile seals;if(seals.Load("\\settings\\phongthan\\QuestSeals.ini")){
        int version=0,total=0;seals.GetInteger("Main","Version",0,&version);seals.GetInteger("Main","Count",0,&total);
        if(version==1&&total>=0&&total<=PT_QUEST_SEAL_MAX)for(int seal=1;seal<=total;++seal){
            char section[32],key[32],image[256],title[256],desc[1201];sprintf(section,"Seal%d",seal);int id=0,kind=0;seals.GetInteger(section,"Id",0,&id);seals.GetInteger(section,"Kind",0,&kind);
            if(kind!=0||id<1||id>PT_QUEST_SEAL_MAX||!PtQuestSealUiHex(seals,section,"ImageHex",image,sizeof(image))||!PtQuestSealUiHex(seals,section,"NameHex",title,sizeof(title))||!PtQuestSealUiHex(seals,section,"TooltipHex",desc,sizeof(desc)))continue;
            sprintf(key,"BuffInfo_%d",PT_QUEST_SEAL_FIRST_BUFF+id-1);m_PtPowderIni.WriteString(key,"Image",image);m_PtPowderIni.WriteString(key,"Title",title);m_PtPowderIni.WriteString(key,"Desc",desc);
        }
    }
    KIniFile stalls;if(stalls.Load("\\settings\\phongthan\\StallPolicy.ini")){
        int version=0,total=0;stalls.GetInteger("Main","Version",0,&version);stalls.GetInteger("Main","Count",0,&total);
        if(version==1&&total>=0&&total<=49)for(int row=0;row<total;++row){
            char section[32],key[32],image[256],title[256],desc[1201];sprintf(section,"License%d",row);int id=0;stalls.GetInteger(section,"Buff",0,&id);
            if(!PTStallBuff(id)||!PtQuestSealUiHex(stalls,section,"ImageHex",image,sizeof(image))||!PtQuestSealUiHex(stalls,section,"NameHex",title,sizeof(title))||!PtQuestSealUiHex(stalls,section,"TooltipHex",desc,sizeof(desc)))continue;
            sprintf(key,"BuffInfo_%d",id);m_PtPowderIni.WriteString(key,"Image",image);m_PtPowderIni.WriteString(key,"Title",title);m_PtPowderIni.WriteString(key,"Desc",desc);
        }
    }
    for(int life=0;life<sizeof(ptLifeItemUses)/sizeof(ptLifeItemUses[0]);++life){const PTLifeItemUse& d=ptLifeItemUses[life];if(d.kind!=1)continue;char key[32];sprintf(key,"BuffInfo_%d",d.buff);m_PtPowderIni.WriteString(key,"Image",d.image);m_PtPowderIni.WriteString(key,"Title",d.name);m_PtPowderIni.WriteString(key,"Desc",d.description);}
    KWndImage::Init(&m_PtPowderIni,"Main");
    for(int i=0;i<MAX_BUFF_ROLE;++i) {
        m_BuffImage[i].Init(&m_PtPowderIni,"BuffWnd_Templ_Img");
        m_txtBuffTime[i].Init(&m_PtPowderIni,"BuffWnd_Templ_Txt");
        m_BuffImage[i].Hide();m_txtBuffTime[i].Hide();
    }
}
void KUiPlayerControlBar::ShowStatusIcon(int slot, const char* image, const char* title,
    const char* desc, unsigned long seconds, bool permanent)
{
    if(slot<0 || slot>=MAX_BUFF_ROLE || !image || !image[0]) return;
    int width,height,sw,sh,it,tt;
    GetSize(&width,&height);
    m_PtPowderIni.GetInteger("BuffWnd_Templ","Width",34,&sw);
    m_PtPowderIni.GetInteger("BuffWnd_Templ","Height",50,&sh);
    m_PtPowderIni.GetInteger("BuffWnd_Templ_Img","Top",0,&it);
    m_PtPowderIni.GetInteger("BuffWnd_Templ_Txt","Top",35,&tt);
    if(sw<=0 || sh<=0 || width<sw) return;
    int screenWidth,screenHeight; Wnd_GetScreenSize(screenWidth,screenHeight);
    bool skillBuff=slot>=PT_BUFF_BANK_SIZE;
    int index=skillBuff ? slot-PT_BUFF_BANK_SIZE : slot;
    int originX,originY,columns;
    if (skillBuff) {
        int left=38,top=73;
        m_PtSkillBuffIni.GetInteger("Main","BuffWndLeft",38,&left);
        m_PtSkillBuffIni.GetInteger("Main","BuffWndTop",73,&top);
        m_PtSkillBuffIni.GetInteger("BuffWnd_Templ","Width",18,&sw);
        m_PtSkillBuffIni.GetInteger("BuffWnd_Templ","Height",18,&sh);
        if(sw<16||sh<16){sw=18;sh=18;}
        originX=0;originY=0;
        KUiHeaderControlBar* header=KUiHeaderControlBar::GetSelf();
        if(header)header->GetAbsolutePos(&originX,&originY);
        originX+=left;originY+=top;it=0;
        columns=18;
        m_BuffImage[slot].SetSize(16,16);
    } else {
        // Authored item/event strip sits immediately left of the minimap.
        originX=max(0,screenWidth-196-8*sw); originY=5; columns=8;
    }
    char tip[800],time[32];
    if(permanent) strcpy(time,skillBuff?"\247\141\156\147\040\142\313\164":"--");
    else sprintf(time,"%02lu:%02lu:%02lu",seconds/3600,seconds/60%60,seconds%60);
    if(!title) title=""; if(!desc) desc="";
    if(*title=='$' || *title=='#') ++title;
    if(*desc=='$' || *desc=='#') ++desc;
    sprintf(tip,"%.255s\n%.480s\n%s",title,desc,time);
    if(!permanent) {
        if(seconds>=86400) sprintf(time,"%lud",seconds/86400);
        else if(seconds>=3600) sprintf(time,"%luh",seconds/3600);
        else if(seconds>=60) sprintf(time,"%lum",seconds/60);
        else sprintf(time,"%lus",seconds);
    }
    m_BuffImage[slot].SetCancelId(0);
    m_BuffImage[slot].SetCompact(skillBuff);
    m_BuffImage[slot].SetPosition(originX+index%columns*sw,originY+index/columns*sh+it);
    m_BuffImage[slot].SetImage(ISI_T_SPR,image,false);
    m_BuffImage[slot].SetStatusTip(tip);
    m_txtBuffTime[slot].SetPosition(originX+index%columns*sw,originY+index/columns*sh+tt);
    m_txtBuffTime[slot].SetText(time);
    m_BuffImage[slot].Show();
    if(skillBuff)m_txtBuffTime[slot].Hide();else m_txtBuffTime[slot].Show();
}
void KUiPlayerControlBar::BreathePowders()
{
    if(!m_bPtPowderScheme || !g_pCoreShell) return;
    PTConsumableUiState state;memset(&state,0,sizeof(state));
    int timedQuery=g_pCoreShell->GetGameData(PT_GDI_CONSUMABLE_BUFFS,(unsigned int)&state,sizeof(state));
    static unsigned long previousTimed=0;unsigned long signature=2166136261UL;int activeTimed=0;
    for(int timed=0;timed<PT_CONSUMABLE_SLOTS;++timed)if(state.RemainingSeconds[timed]){signature=(signature^timed)*16777619UL;++activeTimed;}
    if(signature!=previousTimed){previousTimed=signature;FILE* trace=fopen("pt_consumable_ui_trace.log","a");if(trace){fprintf(trace,"event=TIMED_BUFF_QUERY result=%d bytes=%d active=%d details=",timedQuery,sizeof(state),activeTimed);for(int t=0;t<PT_CONSUMABLE_SLOTS;++t)if(state.RemainingSeconds[t])fprintf(trace,"%d:%lu,",PhongThanTimedItemDetail(t),state.RemainingSeconds[t]);fprintf(trace,"\n");fclose(trace);}}

    int width,height,slotWidth,slotHeight,imageTop,textTop;
    GetSize(&width,&height);
    m_PtPowderIni.GetInteger("BuffWnd_Templ","Width",0,&slotWidth);
    m_PtPowderIni.GetInteger("BuffWnd_Templ","Height",0,&slotHeight);
    m_PtPowderIni.GetInteger("BuffWnd_Templ_Img","Top",0,&imageTop);
    m_PtPowderIni.GetInteger("BuffWnd_Templ_Txt","Top",0,&textTop);
    if(slotWidth<=0 || slotHeight<=0 || width<slotWidth) return;
    int screenWidth,screenHeight;Wnd_GetScreenSize(screenWidth,screenHeight);
    SetPosition(0,0); SetSize(screenWidth,screenHeight);
    PTIBBuffUiState ib;memset(&ib,0,sizeof(ib));g_pCoreShell->GetGameData(PT_GDI_IBBUFFS,(unsigned int)&ib,sizeof(ib));
    int columns=width/slotWidth,count=0;
    for(int i=0;i<PT_CONSUMABLE_SLOTS && count<PT_BUFF_BANK_SIZE;++i) if(state.RemainingSeconds[i] && !PTHasIBBuff(ib,PhongThanTimedItemDetail(i))) {
        char section[32],image[256],title[256],desc[512];
        sprintf(section,"BuffInfo_%d",PhongThanTimedItemDetail(i));
        m_PtPowderIni.GetString(section,"Image","",image,sizeof(image));
        if(!image[0]) m_PtTimedItemsIni.GetString(section,"Image","",image,sizeof(image));
        if(!image[0]) continue;
        m_PtPowderIni.GetString(section,"Title","",title,sizeof(title));
        m_PtPowderIni.GetString(section,"Desc","",desc,sizeof(desc));
        if(!title[0]) m_PtTimedItemsIni.GetString(section,"Title","",title,sizeof(title));
        if(!desc[0]) m_PtTimedItemsIni.GetString(section,"Desc","",desc,sizeof(desc));
        ShowStatusIcon(count,image,title,desc,state.RemainingSeconds[i],false);
        m_BuffImage[count++].SetCancelId(-i-1);
    }
    for(int buff=0;buff<ib.Count && count<PT_BUFF_BANK_SIZE;++buff) {
        PTIBBuffUiEntry& entry=ib.Entries[buff];char key[32],image[256],title[256],desc[512];
        sprintf(key,"BuffInfo_%d",entry.Id);m_PtPowderIni.GetString(key,"Image","",image,sizeof(image));
        if(!image[0])continue;
        m_PtPowderIni.GetString(key,"Title","",title,sizeof(title));m_PtPowderIni.GetString(key,"Desc","",desc,sizeof(desc));
        int slot=count++;ShowStatusIcon(slot,image,title,desc,entry.Seconds,entry.Seconds==0);
        char prop[256];m_PtPowderIni.GetString(key,"Prop","",prop,sizeof(prop));
        if(strcmp(prop,"$NhÊp chuét ph¶i sÏ hñy bá hiÖu qu¶ nµy")==0&&entry.Id!=267&&entry.Id!=418)m_BuffImage[slot].SetCancelId(entry.Id);
        if(PtLifeBuff(entry.Id)&&PtLifeBuff(entry.Id)->cancelable)m_BuffImage[slot].SetCancelId(entry.Id);
        if(entry.Id==267 || entry.Id==418 || entry.Stacks>1)m_txtBuffTime[slot].SetIntText(entry.Stacks);
    }
    for(int item=count;item<PT_BUFF_BANK_SIZE;++item){m_BuffImage[item].Hide();m_txtBuffTime[item].Hide();}
    int itemCount=count; count=PT_BUFF_BANK_SIZE;
    PTStatusSkillsUi skills;memset(&skills,0,sizeof(skills));
    if(g_pCoreShell->GetGameData(PT_GDI_STATUS_SKILLS,(unsigned int)&skills,sizeof(skills)))
    {
        static unsigned long lastSignature=0;
        unsigned long signature=2166136261UL;
        for(int trace=0;trace<skills.Count && trace<PT_STATUS_UI_MAX;++trace)
            signature=(signature^skills.Entries[trace].SkillId)*16777619UL;
        if(signature!=lastSignature){
            lastSignature=signature;FILE* log=fopen("pt_skill_buff_ui_trace.log","a");
            if(log){fprintf(log,"count=%d slot=18 artwork=16 ids=",skills.Count);
                for(int t=0;t<skills.Count && t<PT_STATUS_UI_MAX;++t)fprintf(log,"%s%d",t?",":"",skills.Entries[t].SkillId);
                fprintf(log,"\n");fclose(log);}
        }
        for(int s=0;s<skills.Count && s<PT_STATUS_UI_MAX && count<MAX_BUFF_ROLE;++s) {
            PTStatusSkillUi& entry=skills.Entries[s];
            // Skill IDs and IB buffer IDs are independent namespaces.
            char image[256],fallback[32];sprintf(fallback,"SkillFallback_%d",entry.SkillId);
            m_PtSkillBuffIni.GetString(fallback,"Image",entry.Image,image,sizeof(image));
            int configured=0;m_PtSkillBuffIni.GetInteger("Main","BuffInfoCount",0,&configured);
            for(int row=0;row<configured;++row){
                char key[32];sprintf(key,"SkillInfo_%d",row);int id=0;
                m_PtSkillBuffIni.GetInteger(key,"SkillID",0,&id);
                if(id==entry.SkillId){m_PtSkillBuffIni.GetString(key,"Image",entry.Image,image,sizeof(image));break;}
            }
            if(!image[0])continue;
            ShowStatusIcon(count++,image,entry.Name,entry.Description,
                entry.LeftTicks>0 ? (entry.LeftTicks+GAME_FPS-1)/GAME_FPS : 0,
                entry.LeftTicks<0);
        }
    }
    for(int j=count;j<MAX_BUFF_ROLE;++j){m_BuffImage[j].Hide();m_txtBuffTime[j].Hide();}
    count=itemCount+count-PT_BUFF_BANK_SIZE;
    if(count!=m_nListCount) {
        FILE* f=fopen("pt_consumable_ui_trace.log","a");
        if(f){fprintf(f,"tick=%lu event=POWDER_ICONS count=%d resource=PT_BUFFERCONFIG\n",GetTickCount(),count);fclose(f);}
    }
    m_nListCount=count;
}
