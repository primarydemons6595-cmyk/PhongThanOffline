#include "../../../../Headers/PhongThanVttText.h"
class KUiVttCountdown : public KWndText80 {
    unsigned int started;
    int duration,lastSeconds;
public:
    KUiVttCountdown():started(0),duration(0),lastSeconds(-1){}
    void Initialize(){
        char path[256];g_UiBase.GetCurSchemePath(path,sizeof(path));strcat(path,"\\UiVttCountdown.ini");
        KIniFile ini;if(ini.Load(path))Init(&ini,"Main");
        Wnd_AddWindow(this,WL_TOPMOST);Hide();
    }
    void SetSeconds(int seconds){
        if(seconds<=0){duration=0;Hide();return;}
        duration=seconds;started=IR_GetCurrentTime();lastSeconds=-1;Show();Breathe();
    }
    virtual void Breathe(){
        if(duration<=0)return;
        unsigned int elapsed=IR_GetCurrentTime()-started;
        int left=duration-(int)(elapsed/1000);
        if(left<=0){duration=0;Hide();return;}
        if(left!=lastSeconds){
            char text[80];sprintf(text,PT_VTT_COUNTDOWN,left/60,left%60);SetText(text);lastSeconds=left;
            int w,h;Wnd_GetScreenSize(w,h);SetPosition((w-500)/2,100);
        }
    }
};
static KUiVttCountdown* ptVttCountdown=0;
static void PTSetVttCountdown(int seconds){
    if(!ptVttCountdown){ptVttCountdown=new KUiVttCountdown;ptVttCountdown->Initialize();}
    ptVttCountdown->SetSeconds(seconds);
}
static void PTCloseVttCountdown(){if(ptVttCountdown){ptVttCountdown->Destroy();ptVttCountdown=0;}}
