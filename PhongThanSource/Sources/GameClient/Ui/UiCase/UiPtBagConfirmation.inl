#include "../../../Engine/Src/Text.h"
// Exact original general-message frame and its image buttons.
class KPtBagConfirmation : public KWndImage {
    KWndButton yes,no,close;KWndText512 text;KWndWindow* caller;DWORD opened;
    static KPtBagConfirmation* self;
    void Finish(int choice) {
        KWndWindow* target=caller;caller=0;Hide();Wnd_ReleaseExclusive(this);
        if(target)target->WndProc(WND_M_OTHER_WORK_RESULT,0x4634,choice);
    }
    bool Initialize() {
        char scheme[256],path[384];g_UiBase.GetCurSchemePath(scheme,sizeof(scheme));
        sprintf(path,"%s\\\xCD\xA8\xD3\xC3\xCF\xFB\xCF\xA2\xBF\xF2.ini",scheme);
        KIniFile ini;if(!ini.Load(path))return false;
        Init(&ini,"Main");text.Init(&ini,"Information");
        yes.Init(&ini,"BtnYES");no.Init(&ini,"BtnNO");close.Init(&ini,"BtnClose");
        AddChild(&text);AddChild(&yes);AddChild(&no);AddChild(&close);caller=0;
        Wnd_AddWindow(this,WL_TOPMOST);Hide();return true;
    }
public:
    static bool Open(const char* message,KWndWindow* target=0) {
        if(!self){self=new KPtBagConfirmation;if(!self->Initialize()){delete self;self=0;return false;}}
        if(self->IsVisible())return false;
        char encoded[512];strncpy(encoded,message,sizeof(encoded)-1);encoded[sizeof(encoded)-1]=0;
        int length=TEncodeText(encoded,strlen(encoded));self->text.SetText(encoded,length);
        int width,height;Wnd_GetScreenSize(width,height);
        self->SetPosition((width-self->m_Width)/2,(height-self->m_Height)/2);
        self->caller=target;self->opened=GetTickCount();self->Show();self->BringToTop();Wnd_SetExclusive(self);return true;
    }
    virtual void Breathe(){if(IsVisible() && GetTickCount()-opened>=30000)Finish(1);}
    virtual int WndProc(unsigned int message,unsigned int param,int data) {
        if(message==WND_N_BUTTON_CLICK){
            if(param==(unsigned int)(KWndWindow*)&yes){Finish(0);return 1;}
            if(param==(unsigned int)(KWndWindow*)&no || param==(unsigned int)(KWndWindow*)&close){Finish(1);return 1;}
        }
        if(message==WM_KEYDOWN && param==VK_ESCAPE){Finish(1);return 1;}
        return KWndImage::WndProc(message,param,data);
    }
};
KPtBagConfirmation* KPtBagConfirmation::self=0;
