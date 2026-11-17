#include "../Elem/PopupMenu.h"
#include "../Elem/WndPureTextBtn.h"
// Presets contain stable item serials in this character's private folder.
// Each equip goes through the ordinary server-validated movement request;
// the next request waits for its authoritative equipment snapshot.
class PtEquipSets : public KWndImage {
    enum {Sets=10,Slots=11};
    DWORD saved[Sets][Slots],wanted[Slots],owner,waitingSerial,waitingSince;
    int selected,step,waitingSlot;bool running;
    char privateFile[320],names[Sets][48];
    KIniFile scheme;
    KWndButton save,apply,clear,close;
    KWndButton selector;
    KWndText80 selectedName;
    KWndText256 note,hint;
    KWndImage slotArt[Slots];KWndObjectBox boxes[Slots];
    static PtEquipSets* self;
    static int Slot(int n){static const int slots[]={UIEP_HEAD,UIEP_BODY,UIEP_WAIST,UIEP_FOOT,UIEP_WAIST_DECOR,UIEP_HAND,UIEP_HORSE,UIEP_NECK,UIEP_FINGER1,UIEP_SHIPIN,UIEP_FINESSE};return n>=0&&n<Slots?slots[n]:-1;}
    static DWORD Serial(unsigned int item){char value[64];value[0]=0;unsigned long serial=0;
        if(g_pCoreShell&&item&&g_pCoreShell->GetGameData(GDI_GET_ITEM_PARAM,(unsigned int)value,item)&&sscanf(value,"PTI:%lu",&serial)==1)return serial;return 0;}
    bool Ready(){KUiDraggedObject drag;memset(&drag,0,sizeof(drag));Wnd_GetDragObj(&drag);
        return g_pCoreShell&&owner==g_pCoreShell->GetdwIDPlayer(0)&&g_UiBase.GetStatus()==UIS_S_IDLE&&!g_pCoreShell->GetTradeState()&&(!drag.uId||drag.uGenre==CGOG_NOTHING);}
    static void OriginalText(KWndText256& target,const char* text){
        char out[512];int n=0;
        while(*text&&n<490){const char* replacement=0;int count=0;
            if(!strncmp(text,"<c=r>",5)){replacement="<color=red>";count=5;}
            else if(!strncmp(text,"<c=g>",5)){replacement="<color=green>";count=5;}
            else if(!strncmp(text,"<c>",3)){replacement="<color>";count=3;}
            if(replacement){int len=strlen(replacement);memcpy(out+n,replacement,len);n+=len;text+=count;}else out[n++]=*text++;
        }out[n]=0;target.SetText(out);
    }
    void UpdateHint(){bool any=false;for(int s=0;s<Slots;++s)if(saved[selected][s])any=true;
        char text[256],out[256];scheme.GetString("Scheme_STR",any?"ShortcutGreen":"ShortcutRed","",text,sizeof(text));
        const char* p=text[0]=='$'?text+1:text;sprintf(out,p,(selected+1)%10);OriginalText(hint,out);
    }
    void Message(const char* key){char text[256],out[256];scheme.GetString("Scheme_STR",key,"",text,sizeof(text));const char* p=text[0]=='$'?text+1:text;
        if(strstr(p,"%d")){sprintf(out,p,selected+1);OriginalText(note,out);}else OriginalText(note,p);}
    int Equipment(KUiObjAtRegion* equipped){memset(equipped,0,17*sizeof(*equipped));return g_pCoreShell?g_pCoreShell->GetGameData(GDI_EQUIPMENT,(unsigned int)equipped,0):0;}
    DWORD Equipped(int slot){KUiObjAtRegion e[17];int n=Equipment(e);if(n<0||n>17)return 0;for(int i=0;i<n;++i)if(e[i].Obj.uGenre==CGOG_ITEM&&e[i].Region.v==Slot(slot))return Serial(e[i].Obj.uId);return 0;}
    bool BagItem(DWORD serial,KUiObjAtContRegion& item){
        static const UIOBJECT_CONTAINER containers[]={UOC_ITEM_TAKE_WITH,UOC_ITEM_TAKE_WITH_EX,UOC_PT_BAG2,UOC_PT_BAG3};
        for(int page=0;page<4;++page){KUiObjAtRegion list[35];memset(list,0,sizeof(list));int expected=g_pCoreShell->GetGameData(PT_GDI_BAG_FIRST+page,0,0);
            if(expected<0||expected>35)return false;int count=expected?g_pCoreShell->GetGameData(PT_GDI_BAG_FIRST+page,(unsigned int)list,35):0;
            if(count!=expected)return false;
            for(int i=0;i<count;++i)if(list[i].Obj.uGenre==CGOG_ITEM&&Serial(list[i].Obj.uId)==serial){
                memset(&item,0,sizeof(item));item.Obj=list[i].Obj;item.Region=list[i].Region;item.eContainer=containers[page];return true;}
        }return false;
    }
    void Preview(){selectedName.SetText(names[selected]);UpdateHint();
        for(int s=0;s<Slots;++s){boxes[s].HoldObject(CGOG_NOTHING,0,0,0);if(!saved[selected][s])continue;
            KUiObjAtRegion e[17];int n=Equipment(e);unsigned int item=0;int w=1,h=1;
            for(int i=0;i<n&&i<17;++i)if(e[i].Obj.uGenre==CGOG_ITEM&&Serial(e[i].Obj.uId)==saved[selected][s]){item=e[i].Obj.uId;w=e[i].Region.Width;h=e[i].Region.Height;break;}
            if(!item){KUiObjAtContRegion bag;if(BagItem(saved[selected][s],bag)){item=bag.Obj.uId;w=bag.Region.Width;h=bag.Region.Height;}}
            if(item)boxes[s].HoldObject(CGOG_ITEM,item,w,h);
        }
    }
    void Read(){memset(saved,0,sizeof(saved));KIniFile ini;if(!ini.Load(privateFile))return;
        for(int set=0;set<Sets;++set)for(int slot=0;slot<Slots;++slot){char section[32],key[24],value[32];sprintf(section,"Set_%d",set);sprintf(key,"Slot_%d",slot);ini.GetString(section,key,"0",value,sizeof(value));saved[set][slot]=strtoul(value,0,10);}}
    bool Write(){KIniFile ini;for(int set=0;set<Sets;++set)for(int slot=0;slot<Slots;++slot){char section[32],key[24],value[32];sprintf(section,"Set_%d",set);sprintf(key,"Slot_%d",slot);sprintf(value,"%lu",saved[set][slot]);ini.WriteString(section,key,value);}return ini.Save(privateFile)!=0;}
    void Save(){if(!Ready()||running)return;DWORD before[Slots];memcpy(before,saved[selected],sizeof(before));bool any=false;
        for(int s=0;s<Slots;++s){saved[selected][s]=Equipped(s);if(saved[selected][s])any=true;}
        if(!any){memcpy(saved[selected],before,sizeof(before));Message("CannotNullSet");return;}
        if(!Write()){memcpy(saved[selected],before,sizeof(before));return;}Message("SetCurEquipment");Preview();}
    void Clear(){if(!Ready()||running)return;DWORD before[Slots];memcpy(before,saved[selected],sizeof(before));memset(saved[selected],0,sizeof(before));
        if(!Write()){memcpy(saved[selected],before,sizeof(before));return;}Message("ClearCurEquipment");Preview();}
    void Start(){if(!Ready()||running)return;bool any=false,changed=false;
        for(int s=0;s<Slots;++s)if(saved[selected][s]){any=true;if(Equipped(s)==saved[selected][s])continue;changed=true;KUiObjAtContRegion item;
            if(!BagItem(saved[selected][s],item)){Message("SomeHaveNOt");return;}}
        if(!any){Message("NoSetEquipment");return;}if(!changed){Message("NoOperator");return;}
        memcpy(wanted,saved[selected],sizeof(wanted));step=0;waitingSlot=-1;running=true;Tick();}
    bool Initialize(){char path[320],base[256];g_UiBase.GetCurSchemePath(base,sizeof(base));sprintf(path,"%s\\PtEquipSets.ini",base);if(!scheme.Load(path))return false;
        Init(&scheme,"Main");SetSize(284,372);AddChild(&save);save.Init(&scheme,"BtnSaveEquipSet");AddChild(&apply);apply.Init(&scheme,"BtnChangeEquip");AddChild(&clear);clear.Init(&scheme,"BtnClearEquipSet");AddChild(&close);close.Init(&scheme,"BtnClose");
        AddChild(&selector);selector.Init(&scheme,"ComboxEquipSetSel_Btn");int x,y;scheme.GetInteger("ComboxEquipSetSel","Left",80,&x);scheme.GetInteger("ComboxEquipSetSel","Top",220,&y);selector.SetPosition(x,y);selector.SetSize(120,14);selector.AddChild(&selectedName);selectedName.Init(&scheme,"ComboxEquipSetSel_Edit");selectedName.Enable(false);AddChild(&hint);hint.Init(&scheme,"TxtHintInfo");hint.SetPosition(20,203);hint.SetSize(235,15);AddChild(&note);note.Init(&scheme,"TxtDetailHintInfo");note.SetPosition(20,320);note.SetSize(235,32);
        for(int i=0;i<Sets;++i){char key[32];sprintf(key,"SelText_%d",i);scheme.GetString("ComboxEquipSetSel",key,"",names[i],sizeof(names[i]));if(names[i][0]=='$')memmove(names[i],names[i]+1,strlen(names[i]));}
        for(int s=0;s<Slots;++s){char key[64];int x,y;AddChild(&slotArt[s]);slotArt[s].Init(&scheme,"ObjCommonBox");AddChild(&boxes[s]);boxes[s].Init(&scheme,"ObjCommonBox");boxes[s].SetObjectGenre(CGOG_ITEM);
            sprintf(key,"ObjCommonBox_%d_Left",s);scheme.GetInteger("ObjCommonBox",key,0,&x);sprintf(key,"ObjCommonBox_%d_Top",s);scheme.GetInteger("ObjCommonBox",key,0,&y);slotArt[s].SetPosition(x,y);boxes[s].SetPosition(x,y);boxes[s].Enable(false);}
        Wnd_AddWindow(this,WL_TOPMOST);Hide();return true;}
    void Attach(){running=false;waitingSlot=-1;owner=g_pCoreShell->GetdwIDPlayer(0);char folder[256];g_UiBase.GetUserPrivateDataFolder(folder,sizeof(folder));
        sprintf(privateFile,"%s%sEquipSets.ini",folder,strlen(folder)&&folder[strlen(folder)-1]!='\\'?"\\":"");Read();Preview();Message("CurViewEquipment");
        int w,h;Wnd_GetScreenSize(w,h);SetPosition((w-284)/2,(h-372)/2);}
public:
    PtEquipSets():owner(0),waitingSerial(0),selected(0),step(0),waitingSlot(-1),running(false){memset(saved,0,sizeof(saved));}
    static void Open(){if(!g_pCoreShell)return;if(!self){self=new PtEquipSets;if(!self->Initialize()){delete self;self=0;return;}}self->Attach();self->Show();self->BringToTop();}
    static bool Quick(int set){if(set<0||set>=Sets||!g_pCoreShell)return false;
        if(!self){self=new PtEquipSets;if(!self->Initialize()){delete self;self=0;return false;}}
        if(self->running)return true;self->Attach();self->selected=set;self->Preview();self->Start();return true;
    }
    static void Close(){if(self){self->running=false;self->Hide();}}
    void Tick(){if(!running)return;if(!Ready()){running=false;Message("SomeHaveNOt");return;}
        if(waitingSlot>=0){if(Equipped(waitingSlot)==waitingSerial){waitingSlot=-1;++step;Preview();}else{if(GetTickCount()-waitingSince>4000){running=false;Message("SomeHaveNOt");}return;}}
        while(step<Slots){if(!wanted[step]||Equipped(step)==wanted[step]){++step;continue;}
            KUiObjAtContRegion from,to;if(!BagItem(wanted[step],from)){running=false;Message("SomeHaveNOt");return;}
            to=from;to.eContainer=UOC_EQUIPTMENT;to.Region.h=0;to.Region.v=Slot(step);
            waitingSlot=step;waitingSerial=wanted[step];waitingSince=GetTickCount();
            g_pCoreShell->OperationRequest(GOI_SWITCH_OBJECT,(unsigned int)&from,(int)&to);return;
        }running=false;Message("SuccessEquipment");Preview();}
    static void Breathe(){if(self)self->Tick();}
    virtual int WndProc(unsigned int msg,unsigned int param,int data){
        if(msg==WND_N_BUTTON_CLICK){if(param==(unsigned int)&close){Close();return 1;}if(param==(unsigned int)&save){Save();return 1;}if(param==(unsigned int)&apply){Start();return 1;}if(param==(unsigned int)&clear){Clear();return 1;}
            if(param==(unsigned int)&selector&&!running){KPopupMenuData* menu=(KPopupMenuData*)malloc(MENU_DATA_SIZE(Sets));if(!menu)return 1;KPopupMenu::InitMenuData(menu,Sets);menu->nNumItem=Sets;menu->usMenuFlag|=PM_F_AUTO_DEL_WHEN_HIDE;menu->byFontSize=14;menu->nSelectedItem=selected;int x,y;selector.GetAbsolutePos(&x,&y);menu->nX=x;menu->nY=y+14;menu->nItemWidth=120;
                for(int i=0;i<Sets;++i){strcpy(menu->Items[i].szData,names[i]);menu->Items[i].uDataLen=strlen(names[i]);}KPopupMenu::Popup(menu,this,0);return 1;}}
        if(msg==WND_M_MENUITEM_SELECTED&&param==(unsigned int)this&&!running){int row=(short)LOWORD(data);if(row>=0&&row<Sets){selected=row;Preview();Message("CurViewEquipment");}return 1;}
        if(msg==WM_KEYDOWN&&param==VK_ESCAPE){Close();return 1;}return KWndImage::WndProc(msg,param,data);
    }
};
PtEquipSets* PtEquipSets::self=0;

bool PhongThanEquipSetKey(UINT message,WPARAM key){
    if(message!=WM_KEYDOWN||key<'0'||key>'9'||!(GetKeyState(VK_CONTROL)&0x8000)||(GetKeyState(VK_MENU)&0x8000)||(GetKeyState(VK_SHIFT)&0x8000))return false;
    return PtEquipSets::Quick(key=='0'?9:(int)key-'1');
}
void PhongThanEquipSetsPulse(){PtEquipSets::Breathe();}
