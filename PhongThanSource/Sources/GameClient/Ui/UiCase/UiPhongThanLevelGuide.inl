// Original Phong Than level guide; owned by the system notification centre.
#include "../../../../Headers/PhongThanLevelGrowth.h"
#include "../Elem/WndMessageListBox.h"

class KUiPhongThanLevelGuide : public KWndImage {
    KWndButton closeButton, nextButton, lastButton, scrollUp, scrollDown;
    KWndMessageListBox descriptions;
    KWndScrollBar scroll;
    KWndText80 heading;
    KWndText32 before[7], after[7];
    PTLevelNotice notice;
    int page;
public:
    void Initialize() {
        AddChild(&closeButton); AddChild(&nextButton); AddChild(&lastButton);
        AddChild(&descriptions); AddChild(&scroll); AddChild(&scrollUp); AddChild(&scrollDown);
        AddChild(&heading);
        const char* labels[7][2]={{"TxtLevelBefore","TxtLevelAfter"},{"TxtLifeBefore","TxtLifeAfter"},
            {"TxtManaBefore","TxtManaAfter"},{"TxtStrBefore","TxtStrAfter"},
            {"TxtDexBefor","TxtDexAfter"},{"TxtConBefore","TxtConAfter"},{"TxtIntBefore","TxtIntAfter"}};
        char path[MAX_PATH]; g_UiBase.GetCurSchemePath(path,sizeof(path));
        strcat(path,"\\UiLevelGuide.ini"); KIniFile ini;
        if (!ini.Load(path)) return;
        Init(&ini,"Main"); closeButton.Init(&ini,"BtnClose");
        nextButton.Init(&ini,"BtnNextLevel"); lastButton.Init(&ini,"BtnLastLevel");
        descriptions.Init(&ini,"DescriptionList"); scroll.Init(&ini,"Scroll_Bar");
        scrollUp.Init(&ini,"Scroll_BtnUp"); scrollDown.Init(&ini,"Scroll_BtnDown");
        descriptions.SetScrollbar(&scroll); heading.Init(&ini,"TxtAdditionInfo");
        for(int i=0;i<7;++i) {
            AddChild(&before[i]); AddChild(&after[i]);
            before[i].Init(&ini,labels[i][0]); after[i].Init(&ini,labels[i][1]);
        }
        Wnd_AddWindow(this); Hide();
    }
    void Open(const PTLevelNotice& data) {
        notice=data; page=0; RefreshPage();
        int w,h; Wnd_GetScreenSize(w,h); SetPosition((w-m_Width)/2,(h-m_Height)/2);
        Show(); BringToTop();
    }
    void RefreshPage() {
        int level=notice.level+page;
        const PTLevelGrowth* row=PTGetLevelGrowth(notice.profession,notice.advanced,level);
        descriptions.Clear();
        if (row) {
            AddDescription(row->description);
            AddDescription(row->mission);
            if (!row->description[0] && !row->mission[0]) AddDescription(row->tip);
        }
        heading.SetText(page ? PT_LEVEL_NEXT : PT_LEVEL_CURRENT);
        int old[7], now[7];
        now[0]=level; now[1]=notice.life; now[2]=notice.mana;
        for(int a=0;a<4;++a) now[a+3]=notice.attr[a];
        int lifeDelta=row ? notice.lifePerLevel+row->attr[2]*notice.lifePerVitality : 0;
        int manaDelta=row ? notice.manaPerLevel+row->attr[3]*notice.manaPerEnergy : 0;
        for(int n=0;n<7;++n) old[n]=now[n];
        if (page) {
            old[0]=notice.level; now[1]+=lifeDelta; now[2]+=manaDelta;
            for(int a=0;a<4;++a) if(row)now[a+3]+=row->attr[a];
        } else {
            old[0]=level-1; old[1]-=lifeDelta; old[2]-=manaDelta;
            for(int a=0;a<4;++a) if(row)old[a+3]-=row->attr[a];
        }
        for(int i=0;i<7;++i) { before[i].SetIntText(old[i]); after[i].SetIntText(now[i]); }
        lastButton.Enable(page!=0);
        nextButton.Enable(!page && PTGetLevelGrowth(notice.profession,notice.advanced,level+1)!=0);
    }
    void AddDescription(const char* text) {
        if(!text || !text[0])return;
        char buffer[4096]; strncpy(buffer,text,sizeof(buffer)-1); buffer[sizeof(buffer)-1]=0;
        int length=TEncodeText(buffer,strlen(buffer)); descriptions.AddOneMessage(buffer,length);
    }
    virtual int WndProc(unsigned int msg,unsigned int param,int value) {
        if(msg==WND_N_BUTTON_CLICK) {
            if(param==(unsigned int)&closeButton) { Hide(); return 1; }
            if(param==(unsigned int)&nextButton) { page=1; RefreshPage(); return 1; }
            if(param==(unsigned int)&lastButton) { page=0; RefreshPage(); return 1; }
            if(param==(unsigned int)&scrollUp) { scroll.ScrollLine(true); return 1; }
            if(param==(unsigned int)&scrollDown) { scroll.ScrollLine(false); return 1; }
        }
        if(msg==WND_N_SCORLLBAR_POS_CHANGED && param==(unsigned int)&scroll) {
            descriptions.SetFirstShowLine(value); return 1;
        }
        return KWndImage::WndProc(msg,param,value);
    }
};
static KUiPhongThanLevelGuide* ptLevelGuide=0;
static void PTOpenLevelGuide(const PTLevelNotice& notice) {
    if(!ptLevelGuide) { ptLevelGuide=new KUiPhongThanLevelGuide; ptLevelGuide->Initialize(); }
    ptLevelGuide->Open(notice);
}
static void PTCloseLevelGuide() {
    if(ptLevelGuide) { ptLevelGuide->Destroy(); ptLevelGuide=0; }
}
