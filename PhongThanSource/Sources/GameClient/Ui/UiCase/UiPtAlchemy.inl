#include "../../../Engine/Src/Text.h"
class KPtAlchemy : public KWndImage {
    KWndButton close;KWndImage vessel[3];KWndText512 status,instruction;
    KWndText32 count,stored,remaining;char statusText[3][256],instructionText[3][1024];DWORD lastFrame;
    static KPtAlchemy* self;
    static void ExpandText(char* out,int capacity,const char* input) {
        int used=0;if(input[0]=='$' || input[0]=='#')++input;
        while(*input && used<capacity-1) {
            const char* replacement=0;int consumed=0;
            if(!strncmp(input,"<c=r>",5)){replacement="<color=red>";consumed=5;}
            else if(!strncmp(input,"<c=g>",5)){replacement="<color=green>";consumed=5;}
            else if(!strncmp(input,"<c>",3)){replacement="<color>";consumed=3;}
            if(replacement) {int n=strlen(replacement);if(n>capacity-1-used)break;memcpy(out+used,replacement,n);used+=n;input+=consumed;}
            else out[used++]=*input++;
        }
        out[used]=0;
    }
    bool Initialize() {
        char scheme[256],path[384];g_UiBase.GetCurSchemePath(scheme,sizeof(scheme));
        sprintf(path,"%s\\\xC1\xB6\xB5\xA4\xC3\xE6\xB0\xE5.ini",scheme);KIniFile ini;if(!ini.Load(path))return false;
        Init(&ini,"Main");close.Init(&ini,"BtnClose");AddChild(&close);
        status.Init(&ini,"TxtStatus");instruction.Init(&ini,"TxtInstruction");AddChild(&status);AddChild(&instruction);
        count.Init(&ini,"TxtNumTake");stored.Init(&ini,"TxtNumTake");remaining.Init(&ini,"TxtNumTake");
        stored.SetPosition(144,217);remaining.SetPosition(144,235);AddChild(&count);AddChild(&stored);AddChild(&remaining);
        for(int i=0;i<3;++i) {
            char key[40],raw[1024];sprintf(key,"ImgVessel_%d",i);vessel[i].Init(&ini,key);AddChild(&vessel[i]);
            sprintf(key,"StatusTxt_%d",i);ini.GetString("Information",key,"",raw,sizeof(raw));ExpandText(statusText[i],sizeof(statusText[i]),raw);
            sprintf(key,"Instruction_%d",i);ini.GetString("Information",key,"",raw,sizeof(raw));ExpandText(instructionText[i],sizeof(instructionText[i]),raw);
        }
        lastFrame=0;Wnd_AddWindow(this);return true;
    }
public:
    static void Open() {
        if(!self){self=new KPtAlchemy;if(!self->Initialize()){delete self;self=0;return;}}
        self->Breathe();self->Show();self->BringToTop();
    }
    virtual void Breathe() {
        PTRoleValues role;memset(&role,0,sizeof(role));
        if(!g_pCoreShell || !g_pCoreShell->GetGameData(PT_GDI_ROLE_VALUES,(unsigned int)&role,sizeof(role)))return;
        int state=role.PillBonus==50?1:role.PillBonus==75?2:0;
        for(int i=0;i<3;++i)if(i==state)vessel[i].Show();else vessel[i].Hide();
        char encodedStatus[256],encodedInstruction[1024];
        strcpy(encodedStatus,statusText[state]);strcpy(encodedInstruction,instructionText[state]);
        int statusLength=TEncodeText(encodedStatus,strlen(encodedStatus));
        int instructionLength=TEncodeText(encodedInstruction,strlen(encodedInstruction));
        status.SetText(encodedStatus,statusLength);instruction.SetText(encodedInstruction,instructionLength);
        count.SetIntText(role.PillCount);stored.SetIntText(role.PillNew);remaining.SetIntText(role.PillAvailable);
        DWORD now=GetTickCount();if(now-lastFrame>=150){lastFrame=now;vessel[state].NextFrame();}
    }
    virtual int WndProc(unsigned int message,unsigned int param,int data) {
        if((message==WND_N_BUTTON_CLICK && param==(unsigned int)(KWndWindow*)&close) || (message==WM_KEYDOWN && param==VK_ESCAPE)){Hide();return 1;}
        return KWndImage::WndProc(message,param,data);
    }
};
KPtAlchemy* KPtAlchemy::self=0;
