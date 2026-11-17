// Original PT art and layout, connected to the existing core AI operations.
class PTAutoRow : public KWndImage {
public:
    virtual int WndProc(unsigned int msg,unsigned int param,int data){
        if((msg==WND_N_BUTTON_CLICK||msg==WND_N_EDIT_CHANGE||msg==WND_N_SCORLLBAR_POS_CHANGED)&&GetParent())
            return GetParent()->WndProc(msg,param,data);
        return KWndImage::WndProc(msg,param,data);
    }
};
#include "PhongThanAutoMedicine.inl"
class PTAutoPanel : public KWndImage {
    PTAutoMedicine medicine;
    KIniFile snapshot;int wasActive;
    PTAutoRow fightOptions;
    KWndButton modes[5];KWndText80 modeText[5],headings[5];
    KWndScrollBar attackRange;
    bool initializingModes;
    KWndButton close,ok,cancel,defaults,tabs[2],recovery,combat,checks[6],select[4];
    PTAutoRow rows[6];
    KWndImage backs[2];
    KWndText80 labels[6],values[4],recoveryLabel,combatLabel,hints[2];
    KWndEdit32 percent[2];
    KWndButton nativeBuff[7];KWndText80 nativeBuffText[7];int nativeBuffId[7];
    KWndButton summonOn,petHeal,selfHeal,petBless;
    KWndButton summonSelect;
    KWndText80 summonName,summonText,petHealText,selfHealText,petHealSkill,selfHealSkill,blessText;
    KWndImage petHealBack,selfHealBack;
    KWndEdit32 petLife,selfLife;
    int summonSkill;
    int skills[4],page;
public:
    PTAutoPanel():page(0),wasActive(0),initializingModes(true),summonSkill(450){memset(skills,0,sizeof(skills));}
    int ModeBits(){int bits=0;for(int i=0;i<5;++i)if(modes[i].IsButtonChecked())bits|=1<<i;return bits;}
    int Radius(){return 100+attackRange.GetScrollPos()*9;}
    void BeginEdit(){g_pCoreShell->PAIOperation(PT_AUTO_RANGE_PREVIEW,page==1,0,0);snapshot.Clear();Save(&snapshot);wasActive=g_pCoreShell->PAIOperation(GPI_ISACTIVE,0,0,0);}
    void CancelEdit(){g_pCoreShell->PAIOperation(PT_AUTO_RANGE_PREVIEW,0,0,0);Load(&snapshot);g_pCoreShell->PAIOperation(GPI_SWITCH_ACTIVE,wasActive,0,0);combat.CheckButton(wasActive!=0);Hide();}
    void Initialize(){
        char scheme[256],file[320];g_UiBase.GetCurSchemePath(scheme,sizeof(scheme));
        sprintf(file,"%s\\PtAutoNative.ini",scheme);KIniFile ini;
        if(!ini.Load(file))return;
        Init(&ini,"AutoBloodMana_Main");SetSize(395,368);
        AddChild(&medicine);medicine.Initialize(&ini);
        AddChild(&close);close.Init(&ini,"BtnClose");
        AddChild(&ok);ok.Init(&ini,"BtnConfirm");
        AddChild(&cancel);cancel.Init(&ini,"BtnCancel");
        AddChild(&defaults);defaults.Init(&ini,"BtnSetting");
        AddChild(&recovery);recovery.Init(&ini,"ChkOpenBloodMana");
        AddChild(&recoveryLabel);recoveryLabel.Init(&ini,"TxtOpenBloodMana_Text");recoveryLabel.SetPosition(48,62);recoveryLabel.SetSize(300,17);
        AddChild(&combat);combat.Init(&ini,"ChkAutoKillLeft");combat.SetPosition(29,60);
        AddChild(&combatLabel);combatLabel.Init(&ini,"PTCombatLabel");
        for(int i=0;i<2;++i){
            AddChild(&tabs[i]);tabs[i].Init(&ini,i?"BtnAutoKillPage":"BtnBloodManaPage");
            AddChild(&hints[i]);hints[i].Init(&ini,i?"PTHintFight":"PTHintRecovery");
        }
        for(i=0;i<6;++i){
            char key[40];sprintf(key,"Blood_Ctrl_Set_%d",i);
            AddChild(&rows[i]);rows[i].Init(&ini,key);rows[i].RemoveImage();
            rows[i].SetSize(368,25);
            rows[i].SetPosition(6,84+(i<2?i:i-2)*34);
            rows[i].AddChild(&checks[i]);checks[i].Init(&ini,"Ctrl_BM_Btn");
            sprintf(key,"PTLabel%d",i);rows[i].AddChild(&labels[i]);labels[i].Init(&ini,key);
            if(i<2){
                rows[i].AddChild(&backs[i]);backs[i].Init(&ini,"Ctrl_BM_Edt_Back");
                rows[i].AddChild(&percent[i]);percent[i].Init(&ini,"Ctrl_BM_Edt");percent[i].Enable(TRUE);percent[i].SetIntText(i?50:60);
            }else{
                rows[i].AddChild(&select[i-2]);select[i-2].Init(&ini,"Ctrl_BM_Group_Btn");select[i-2].SetPosition(140,0);
                rows[i].AddChild(&values[i-2]);values[i-2].Init(&ini,"Ctrl_BM_Group_Edit");values[i-2].SetPosition(141,1);values[i-2].SetSize(98,16);
                values[i-2].SetText("--");
            }
        }
        AddChild(&fightOptions);fightOptions.Init(&ini,"Page_AutoKill");fightOptions.SetSize(380,315);
        const char* sections[]={"TxtAutoKillLeft","TxtAutoKillRight","TxtAutoKillRightLocal","ChkWait","ChkBlue"};
        for(i=0;i<5;++i){
            fightOptions.AddChild(&modes[i]);modes[i].Init(&ini,sections[i]);modes[i].SetSize(i<3?100:130,17);
            char key[64];sprintf(key,"%s_Text",sections[i]);modes[i].AddChild(&modeText[i]);modeText[i].Init(&ini,key);modeText[i].SetSize(i<3?100:130,17);
        }
        modes[0].CheckButton(true);
        const char* titles[]={"Mana_Title","Blood_Title","Skill_Title","Range_Small","Range_Big"};
        for(i=0;i<5;++i){fightOptions.AddChild(&headings[i]);headings[i].Init(&ini,titles[i]);}
        fightOptions.AddChild(&attackRange);attackRange.Init(&ini,"Ctrl_Kill_Range_Bar");attackRange.SetPosition(58,230);attackRange.SetValueRange(0,100);attackRange.SetScrollPos(50);
        // Preserve configurable learned buffs beside the original range controls.
        for(i=1;i<4;++i){
            fightOptions.AddChild(&checks[i+2]);checks[i+2].SetPosition(212,224+(i-1)*24);
            fightOptions.AddChild(&select[i]);select[i].SetPosition(232,224+(i-1)*24);
            fightOptions.AddChild(&values[i]);values[i].SetPosition(233,225+(i-1)*24);
            labels[i+2].Hide();
        }
        for(i=0;i<7;++i){char key[48];sprintf(key,i<5?"EnchanterSkill_%d":"KnightSkill_%d",i<5?i:i-5);fightOptions.AddChild(&nativeBuff[i]);nativeBuff[i].Init(&ini,key);ini.GetInteger(key,"SkillId",0,&nativeBuffId[i]);strcat(key,"_Text");nativeBuff[i].AddChild(&nativeBuffText[i]);nativeBuffText[i].Init(&ini,key);}
        fightOptions.AddChild(&summonOn);summonOn.Init(&ini,"ChkAutoConjure");
        summonOn.AddChild(&summonText);summonText.Init(&ini,"ChkAutoConjure_Text");summonText.SetSize(90,17);
        fightOptions.AddChild(&summonSelect);summonSelect.Init(&ini,"ComboxAutoConjure_Btn");summonSelect.SetPosition(264,224);
        fightOptions.AddChild(&summonName);summonName.Init(&ini,"ComboxAutoConjure_Edit");summonName.SetPosition(267,225);
        fightOptions.AddChild(&petHeal);petHeal.Init(&ini,"SummonRecover");
        petHeal.AddChild(&petHealText);petHealText.Init(&ini,"SummonRecover_Text");
        fightOptions.AddChild(&petHealBack);petHealBack.Init(&ini,"SummonRecover_Edt_Back");
        fightOptions.AddChild(&petLife);petLife.Init(&ini,"SummonRecover_Edit");petLife.Enable(TRUE);petLife.SetIntText(0);
        fightOptions.AddChild(&petHealSkill);petHealSkill.Init(&ini,"SummonRecover_Text_1");
        fightOptions.AddChild(&selfHeal);selfHeal.Init(&ini,"SelfRecover");
        selfHeal.AddChild(&selfHealText);selfHealText.Init(&ini,"SelfRecover_Text");
        fightOptions.AddChild(&selfHealBack);selfHealBack.Init(&ini,"SelfRecover_Edt_Back");
        fightOptions.AddChild(&selfLife);selfLife.Init(&ini,"SelfRecover_Edit");selfLife.Enable(TRUE);selfLife.SetIntText(0);
        fightOptions.AddChild(&selfHealSkill);selfHealSkill.Init(&ini,"SelfRecover_Text_1");
        fightOptions.AddChild(&petBless);petBless.Init(&ini,"MonstrousSkill_0");
        petBless.AddChild(&blessText);blessText.Init(&ini,"MonstrousSkill_0_Text");blessText.SetSize(175,17);
        initializingModes=false;
        Wnd_AddWindow(this);SetPage(0);Hide();
    }
    int Profession(){KUiPlayerBaseInfo info;memset(&info,0,sizeof(info));g_pCoreShell->GetGameData(GDI_PLAYER_BASE_INFO,(int)&info,0);return info.nProfession;}
    bool IsSummoner(){return Profession()==2;}
    void RefreshProfession(){
        bool pet=IsSummoner();int prof=Profession();
        for(int b=0;b<7;++b){if((prof==1&&b<5)||(prof==0&&b>=5))nativeBuff[b].Show();else nativeBuff[b].Hide();}
        KWndWindow* controls[]={&summonOn,&summonSelect,&summonName,&petHeal,&petHealBack,&petLife,&petHealSkill,&selfHeal,&selfHealBack,&selfLife,&selfHealSkill,&petBless};
        for(int i=0;i<12;++i){if(pet)controls[i]->Show();else controls[i]->Hide();}
        for(i=1;i<4;++i){checks[i+2].Hide();select[i].Hide();values[i].Hide();}
        char name[128];g_pCoreShell->GetSkillName(summonSkill,name);if(name[0]=='#'||name[0]=='$')memmove(name,name+1,strlen(name));summonName.SetText(name);
    }
    void SetPage(int n){
        g_pCoreShell->PAIOperation(PT_AUTO_RANGE_PREVIEW,n!=0,0,0);
        RefreshProfession();
        page=n;tabs[0].CheckButton(!n);tabs[1].CheckButton(n);
        if(n){recovery.Hide();recoveryLabel.Hide();combat.Show();combatLabel.Show();}
        else {recovery.Show();recoveryLabel.Show();combat.Hide();combatLabel.Hide();}
        for(int i=0;i<6;++i){if(i==2&&n)rows[i].Show();else rows[i].Hide();}
        if(n)fightOptions.Show();else fightOptions.Hide();
        if(!n)medicine.Show();else medicine.Hide();
        hints[0].Hide();hints[1].Hide();
    }
    void Apply(){
        if(!g_pCoreShell)return;
        int hp=percent[0].GetIntNumber(),mp=percent[1].GetIntNumber();
        hp=hp<1?1:(hp>100?100:hp);mp=mp<1?1:(mp>100?100:mp);
        percent[0].SetIntText(hp);percent[1].SetIntText(mp);
        g_pCoreShell->PAIOperation(GPI_EAT_LIFE,FALSE,0,0);
        g_pCoreShell->PAIOperation(GPI_EAT_MANA,FALSE,0,0);
        medicine.Apply(recovery.IsButtonChecked());
        g_pCoreShell->PAIOperation(PT_AUTO_COMBAT_MODE,ModeBits(),Radius(),0);
        g_pCoreShell->PAIOperation(GPI_RANGER_AUTO,Radius(),0,0);
        g_pCoreShell->PAIOperation(GPI_FIGHT_AUTO,Radius(),0,0);
        g_pCoreShell->PAIOperation(GPI_FIGHTBOSS,modes[4].IsButtonChecked()?1:0,0,0);
        g_pCoreShell->PAIOperation(GPI_LIFE_PERCENT,hp,0,0);
        g_pCoreShell->PAIOperation(GPI_MANA_PERCENT,mp,0,0);
        g_pCoreShell->PAIOperation(GPI_LIFE_TIME,1000,0,0);
        g_pCoreShell->PAIOperation(GPI_MANA_TIME,1000,0,0);
        g_pCoreShell->PAIOperation(GPI_FIGHT_SKILLS,checks[2].IsButtonChecked()?skills[0]:0,0,0);
        bool pet=IsSummoner();
        for(int i=0;i<3;++i)g_pCoreShell->PAIOperation(GPI_SUPPORT_SKILLS,i,0,0);
        int prof=Profession();for(i=0;i<5;++i){int id=0;
            if(prof==1&&nativeBuff[i].IsButtonChecked())id=nativeBuffId[i];
            if(prof==0&&i<2&&nativeBuff[i+5].IsButtonChecked())id=nativeBuffId[i+5];
            if(prof==2&&i==0&&petBless.IsButtonChecked())id=408;
            g_pCoreShell->PAIOperation(PT_AUTO_NATIVE_BUFF,i,id,0);
        }
        g_pCoreShell->PAIOperation(PT_AUTO_SUMMON,pet&&summonOn.IsButtonChecked()?summonSkill:0,pet&&petHeal.IsButtonChecked()?petLife.GetIntNumber():0,pet&&selfHeal.IsButtonChecked()?selfLife.GetIntNumber():0);
    }
    void ToggleRecovery(){recovery.CheckButton(!recovery.IsButtonChecked());Apply();}
    void ToggleCombat(){
        Apply();g_pCoreShell->PAIOperation(GPI_ATTACK_NPC_CHECK,TRUE,0,0);
        g_pCoreShell->PAIOperation(GPI_FIGHTNEAR_CHECK,TRUE,0,0);
        g_pCoreShell->PAIOperation(PT_AUTO_COMBAT_MODE,ModeBits(),Radius(),1);
        g_pCoreShell->PAIOperation(GPI_SWITCH_ACTIVE,0,1,0);
        combat.CheckButton(g_pCoreShell->PAIOperation(GPI_ISACTIVE,0,0,0)!=0);
    }
    void PopupSkill(int slot){
        int type=slot?0:1,count=g_pCoreShell->GetNextSkill(type,0);
        if(count<0||count>512)return;
        KPopupMenuData* menu=(KPopupMenuData*)malloc(MENU_DATA_SIZE(count+1));if(!menu)return;
        KPopupMenu::InitMenuData(menu,count+1);menu->nNumItem=count+1;menu->usMenuFlag|=PM_F_AUTO_DEL_WHEN_HIDE;
        for(int i=0;i<=count;++i){
            int id=i?g_pCoreShell->GetNextSkill(type,i):0;menu->Items[i].uID=id;
            if(id)g_pCoreShell->GetSkillName(id,menu->Items[i].szData);else strcpy(menu->Items[i].szData,"--");
            if(menu->Items[i].szData[0]=='#'||menu->Items[i].szData[0]=='$')memmove(menu->Items[i].szData,menu->Items[i].szData+1,strlen(menu->Items[i].szData));menu->Items[i].uDataLen=strlen(menu->Items[i].szData);
        }
        int x,y;select[slot].GetAbsolutePos(&x,&y);menu->nX=(short)x;menu->nY=(short)y;KPopupMenu::Popup(menu,this,100+slot);
    }
    void Save(KIniFile* ini){
        medicine.Save(ini);
        for(int b=0;b<7;++b){char key[32];sprintf(key,"NativeBuff%d",b);ini->WriteInteger("PTNativeAuto",key,nativeBuff[b].IsButtonChecked());}
        ini->WriteInteger("PTNativeAuto","SummonSkill",summonSkill);
        ini->WriteInteger("PTNativeAuto","SummonOn",summonOn.IsButtonChecked());
        ini->WriteInteger("PTNativeAuto","PetHeal",petHeal.IsButtonChecked());ini->WriteInteger("PTNativeAuto","SelfHeal",selfHeal.IsButtonChecked());
        ini->WriteInteger("PTNativeAuto","PetLife",petLife.GetIntNumber());ini->WriteInteger("PTNativeAuto","SelfLife",selfLife.GetIntNumber());ini->WriteInteger("PTNativeAuto","PetBless",petBless.IsButtonChecked());
        ini->WriteInteger("PTNativeAuto","ModeBits",ModeBits());ini->WriteInteger("PTNativeAuto","Range",attackRange.GetScrollPos());
        ini->WriteInteger("PTNativeAuto","Recovery",recovery.IsButtonChecked());
        for(int i=0;i<6;++i){char k[32];sprintf(k,"Enabled%d",i);ini->WriteInteger("PTNativeAuto",k,checks[i].IsButtonChecked());}
        for(i=0;i<2;++i){char k[32];sprintf(k,"Percent%d",i);ini->WriteInteger("PTNativeAuto",k,percent[i].GetIntNumber());}
        for(i=0;i<4;++i){char k[32];sprintf(k,"Skill%d",i);ini->WriteInteger("PTNativeAuto",k,skills[i]);}
    }
    void Load(KIniFile* ini){
        medicine.Load(ini);
        for(int b=0;b<7;++b){char key[32];int enabled;sprintf(key,"NativeBuff%d",b);ini->GetInteger("PTNativeAuto",key,0,&enabled);nativeBuff[b].CheckButton(enabled!=0);}
        int enabled,threshold;ini->GetInteger("PTNativeAuto","SummonSkill",450,&summonSkill);if(summonSkill<450||summonSkill>458)summonSkill=450;
        ini->GetInteger("PTNativeAuto","SummonOn",0,&enabled);summonOn.CheckButton(enabled!=0);
        ini->GetInteger("PTNativeAuto","PetHeal",0,&enabled);petHeal.CheckButton(enabled!=0);ini->GetInteger("PTNativeAuto","SelfHeal",0,&enabled);selfHeal.CheckButton(enabled!=0);
        ini->GetInteger("PTNativeAuto","PetLife",0,&threshold);petLife.SetIntText(threshold);ini->GetInteger("PTNativeAuto","SelfLife",0,&threshold);selfLife.SetIntText(threshold);
        ini->GetInteger("PTNativeAuto","PetBless",0,&enabled);petBless.CheckButton(enabled!=0);RefreshProfession();
        initializingModes=true;int bits,range;ini->GetInteger("PTNativeAuto","ModeBits",1,&bits);ini->GetInteger("PTNativeAuto","Range",50,&range);
        for(int j=0;j<5;++j)modes[j].CheckButton((bits&(1<<j))!=0);attackRange.SetScrollPos(range);initializingModes=false;
        int n;ini->GetInteger("PTNativeAuto","Recovery",0,&n);recovery.CheckButton(n!=0);
        combat.CheckButton(false);
        for(int i=0;i<6;++i){char k[32];sprintf(k,"Enabled%d",i);ini->GetInteger("PTNativeAuto",k,i<2,&n);checks[i].CheckButton(n!=0);}
        for(i=0;i<2;++i){char k[32];sprintf(k,"Percent%d",i);ini->GetInteger("PTNativeAuto",k,i?50:60,&n);percent[i].SetIntText(n);}
        for(i=0;i<4;++i){char k[32],name[128];sprintf(k,"Skill%d",i);ini->GetInteger("PTNativeAuto",k,0,&skills[i]);
            if(skills[i]>0)g_pCoreShell->GetSkillName(skills[i],name);else strcpy(name,"--");if(name[0]=='#'||name[0]=='$')memmove(name,name+1,strlen(name));values[i].SetText(name);}
        Apply();
    }
    virtual int WndProc(unsigned int msg,unsigned int param,int data){
        if(initializingModes)return 0;
        if(msg==WND_N_SCORLLBAR_POS_CHANGED&&param==(unsigned int)&attackRange){Apply();return 0;}
        if(msg==WND_N_BUTTON_CLICK){
            if(param==(unsigned int)&close||param==(unsigned int)&cancel){CancelEdit();return 0;}
            if(param==(unsigned int)&ok){g_pCoreShell->PAIOperation(PT_AUTO_RANGE_PREVIEW,0,0,0);Apply();KIniFile* f=g_UiBase.GetAutoSettingFile();if(f){Save(f);g_UiBase.CloseAutoSettingFile(true);}Hide();return 0;}
            if(param==(unsigned int)&tabs[0]){SetPage(0);return 0;}
            if(param==(unsigned int)&tabs[1]){SetPage(1);return 0;}
            if(param==(unsigned int)&defaults){KIniFile empty;Load(&empty);combat.CheckButton(g_pCoreShell->PAIOperation(GPI_ISACTIVE,0,0,0)!=0);return 0;}
            if(param==(unsigned int)&summonSelect){
                KPopupMenuData* menu=(KPopupMenuData*)malloc(MENU_DATA_SIZE(9));if(!menu)return 0;
                KPopupMenu::InitMenuData(menu,9);menu->nNumItem=9;menu->usMenuFlag|=PM_F_AUTO_DEL_WHEN_HIDE;
                for(int j=0;j<9;++j){menu->Items[j].uID=450+j;g_pCoreShell->GetSkillName(450+j,menu->Items[j].szData);char* name=menu->Items[j].szData;if(name[0]=='#'||name[0]=='$')memmove(name,name+1,strlen(name));menu->Items[j].uDataLen=strlen(name);}
                int x,y;summonSelect.GetAbsolutePos(&x,&y);menu->nX=(short)x;menu->nY=(short)y;KPopupMenu::Popup(menu,this,110);return 0;
            }
            if(param==(unsigned int)&combat){ToggleCombat();return 0;}
            for(int i=0;i<4;++i)if(param==(unsigned int)&select[i]){PopupSkill(i);return 0;}
            Apply();return 0;
        }
        if(msg==WND_M_MENUITEM_SELECTED&&param==(unsigned int)this){
            int slot=HIWORD(data)-100,id=LOWORD(data);
            if(slot==10 && id>=450&&id<=458){summonSkill=id;RefreshProfession();Apply();return 0;}
            if(slot>=0&&slot<4&&(short)id>=0){skills[slot]=id;char name[128];if(id)g_pCoreShell->GetSkillName(id,name);else strcpy(name,"--");if(name[0]=='#'||name[0]=='$')memmove(name,name+1,strlen(name));values[slot].SetText(name);checks[slot+2].CheckButton(id!=0);Apply();}return 0;
        }
        return KWndImage::WndProc(msg,param,data);
    }
};
static PTAutoPanel* s_PtAutoPanel=0;
static PTAutoPanel* PtAutoGet(){if(!s_PtAutoPanel){s_PtAutoPanel=new PTAutoPanel;s_PtAutoPanel->Initialize();}return s_PtAutoPanel;}
void PtAutoSettings(){PTAutoPanel* p=PtAutoGet();p->RefreshProfession();if(!p->IsVisible())p->BeginEdit();p->Show();p->BringToTop();}
void PtAutoToggle(){if(g_pCoreShell)PtAutoGet()->ToggleCombat();}
void PtAutoRecoveryToggle(){if(g_pCoreShell)PtAutoGet()->ToggleRecovery();}
