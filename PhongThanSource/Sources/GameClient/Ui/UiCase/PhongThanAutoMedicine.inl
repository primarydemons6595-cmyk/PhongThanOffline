#include "../../../../Headers/PhongThanAutoRecovery.h"
class PTAutoMedicine : public KWndImage {
    PTAutoRow row[6];
    KWndButton enabled[6],choose[6];
    KWndText80 name[6];KWndText32 pct[6];
    KWndImage inputBack[6],barBack[6];
    KWndEdit32 value[6];KWndScrollBar slider[6];
    KIniFile catalog;
    int details[6];bool initializing,master;
public:
    PTAutoMedicine():initializing(true),master(false){for(int i=0;i<6;++i)details[i]=i;}
    void Initialize(KIniFile* ini){
        Init(ini,"Page_AutoMana");SetSize(380,315);
        char path[320],scheme[256];g_UiBase.GetCurSchemePath(scheme,sizeof(scheme));sprintf(path,"%s\\PtAutoNative.ini",scheme);catalog.Load(path);
        for(int i=0;i<6;++i){
            char key[40];sprintf(key,"Blood_Ctrl_Set_%d",i);AddChild(&row[i]);row[i].Init(ini,key);row[i].RemoveImage();row[i].SetSize(368,25);
            row[i].AddChild(&enabled[i]);enabled[i].Init(ini,"Ctrl_BM_Btn");enabled[i].CheckButton(true);
            row[i].AddChild(&choose[i]);choose[i].Init(ini,"Ctrl_BM_Group_Btn");choose[i].SetPosition(42,1);
            row[i].AddChild(&name[i]);name[i].Init(ini,"Ctrl_BM_Group_Edit");name[i].SetPosition(43,1);name[i].SetSize(96,15);UpdateName(i);
            row[i].AddChild(&barBack[i]);barBack[i].Init(ini,i<3?"PTLifeBar":"PTManaBar");
            row[i].AddChild(&inputBack[i]);inputBack[i].Init(ini,"Ctrl_BM_Edt_Back");
            row[i].AddChild(&value[i]);value[i].Init(ini,"Ctrl_BM_Edt");value[i].Enable(TRUE);
            row[i].AddChild(&slider[i]);slider[i].Init(ini,"Ctrl_BM_ScrBar");slider[i].SetValueRange(1,100);slider[i].SetScrollPos(i<3?60-i*20:50-(i-3)*15);
            row[i].AddChild(&pct[i]);pct[i].Init(ini,"Ctrl_BM_Txt_Percent");pct[i].SetPosition(224,-8);
            SyncValue(i);
        }
        initializing=false;
    }
    int Maximum(int i){KUiPlayerRuntimeInfo info;memset(&info,0,sizeof(info));g_pCoreShell->GetGameData(GDI_PLAYER_RT_INFO,(unsigned int)&info,0);return i<3?info.nLifeFull:info.nManaFull;}
    void SyncValue(int i){int n=Maximum(i)*slider[i].GetScrollPos()/100;value[i].SetIntText(n>0?n:1);char s[20];sprintf(s,"%d%%",slider[i].GetScrollPos());pct[i].SetText(s);}
    void UpdateName(int i){char key[40],s[128];sprintf(key,"Name%d",details[i]);catalog.GetString("PotionCatalog",key,"--",s,sizeof(s));if(s[0]=='#'||s[0]=='$')memmove(s,s+1,strlen(s));name[i].SetText(s);}
    void Apply(bool active){master=active;for(int i=0;i<6;++i){int v=value[i].GetIntNumber();if(v<1)v=1;if(v>100000000)v=100000000;g_pCoreShell->PAIOperation(PT_AUTO_RECOVERY_ROW,i,details[i],active&&enabled[i].IsButtonChecked()?-v:0);}}
    void Popup(int slot){
        int count=0;catalog.GetInteger("PotionCatalog","Count",0,&count);if(count<1||count>500)return;
        KPopupMenuData* menu=(KPopupMenuData*)malloc(MENU_DATA_SIZE(count));if(!menu)return;
        KPopupMenu::InitMenuData(menu,count);menu->nNumItem=0;menu->usMenuFlag|=PM_F_AUTO_DEL_WHEN_HIDE;
        for(int i=0;i<count;++i){char key[40],text[128];int detail,kind;sprintf(key,"Detail%d",i);catalog.GetInteger("PotionCatalog",key,-1,&detail);sprintf(key,"Kind%d",detail);catalog.GetInteger("PotionCatalog",key,0,&kind);
            if(detail<0||!(kind&(slot<3?1:2)))continue;
            sprintf(key,"Name%d",detail);catalog.GetString("PotionCatalog",key,"--",text,sizeof(text));if(text[0]=='#'||text[0]=='$')memmove(text,text+1,strlen(text));
            int n=menu->nNumItem++;lstrcpyn(menu->Items[n].szData,text,sizeof(menu->Items[n].szData));menu->Items[n].uDataLen=strlen(menu->Items[n].szData);menu->Items[n].uID=detail;
        }
        int x,y;choose[slot].GetAbsolutePos(&x,&y);menu->nX=(short)x;menu->nY=(short)y;KPopupMenu::Popup(menu,this,200+slot);
    }
    void Save(KIniFile* ini){for(int i=0;i<6;++i){char k[40];sprintf(k,"Potion%d",i);ini->WriteInteger("PTNativeMedicine",k,details[i]);sprintf(k,"Enabled%d",i);ini->WriteInteger("PTNativeMedicine",k,enabled[i].IsButtonChecked());sprintf(k,"Value%d",i);ini->WriteInteger("PTNativeMedicine",k,value[i].GetIntNumber());}}
    void Load(KIniFile* ini){
        initializing=true;
        for(int i=0;i<6;++i){char k[40];int n;sprintf(k,"Potion%d",i);ini->GetInteger("PTNativeMedicine",k,i,&details[i]);int kind=0;sprintf(k,"Kind%d",details[i]);catalog.GetInteger("PotionCatalog",k,0,&kind);if(!(kind&(i<3?1:2)))details[i]=i;UpdateName(i);sprintf(k,"Enabled%d",i);ini->GetInteger("PTNativeMedicine",k,1,&n);enabled[i].CheckButton(n!=0);sprintf(k,"Value%d",i);ini->GetInteger("PTNativeMedicine",k,Maximum(i)*(i<3?60-i*20:50-(i-3)*15)/100,&n);if(n<1)n=1;value[i].SetIntText(n);int max=Maximum(i);slider[i].SetScrollPos(max>0?(int)((double)n*100/max):50);char s[20];sprintf(s,"%d%%",slider[i].GetScrollPos());pct[i].SetText(s);}
        initializing=false;
    }
    virtual int WndProc(unsigned int msg,unsigned int param,int data){
        if(initializing)return 0;
        for(int i=0;i<6;++i){
            if(msg==WND_N_BUTTON_CLICK&&param==(unsigned int)&choose[i]){Popup(i);return 0;}
            if(msg==WND_N_SCORLLBAR_POS_CHANGED&&param==(unsigned int)&slider[i]){initializing=true;SyncValue(i);initializing=false;Apply(master);return 0;}
            if(msg==WND_N_EDIT_CHANGE&&param==(unsigned int)&value[i]){int max=Maximum(i);initializing=true;slider[i].SetScrollPos(max>0?(int)((double)value[i].GetIntNumber()*100/max):50);char s[20];sprintf(s,"%d%%",slider[i].GetScrollPos());pct[i].SetText(s);initializing=false;Apply(master);return 0;}
        }
        if(msg==WND_M_MENUITEM_SELECTED&&param==(unsigned int)this){int slot=HIWORD(data)-200;if(slot>=0&&slot<6&&(unsigned short)LOWORD(data)!=0xffff){details[slot]=LOWORD(data);UpdateName(slot);Apply(master);}return 0;}
        if(msg==WND_N_BUTTON_CLICK){Apply(master);return 0;}
        return KWndImage::WndProc(msg,param,data);
    }
};
