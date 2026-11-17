// Original PC layout/art; never reads or stores a password in account history.
void KUiLogin::InitRestoredLogin(KIniFile& ini,const char* scheme){
    AddChild(&m_AccountList);m_AccountList.Init(&ini,"AccountList");
    m_Invisible.Hide();m_Invisible.Enable(false);m_Text.Hide();m_Cancel.Hide();m_Cancel.Enable(false);
    int sw,sh,left,top;Wnd_GetScreenSize(sw,sh);GetPosition(&left,&top);
    KIniFile extra;char file[320];sprintf(file,"%s\\UiLoginExtra.ini",scheme);
    if(extra.Load(file)){
        const char* keys[]={"BtnRegister","BtnChangePassword","BtnOption","BtnBack"};
        for(int i=0;i<4;++i){AddChild(&m_ExtraButtons[i]);m_ExtraButtons[i].Init(&extra,keys[i]);
            int x,y;m_ExtraButtons[i].GetPosition(&x,&y);m_ExtraButtons[i].SetPosition(sw-214-left+x,sh-250-top+y);}
    }
    KIniFile keyboard;sprintf(file,"%s\\UiLoginKeyboard.ini",scheme);
    if(keyboard.Load(file)){
        for(int k=0;k<51;++k){char section[32];
            if(k<48)sprintf(section,"CharKey_%d",k);else if(k<50)sprintf(section,"FuncKey_%d",k-48);else strcpy(section,"ShiftKey");
            AddChild(&m_KeyboardButtons[k]);m_KeyboardButtons[k].Init(&keyboard,section);
            int x,y;m_KeyboardButtons[k].GetPosition(&x,&y);m_KeyboardButtons[k].SetPosition(10-left+x,sh-240-top+y);
            if(k<48){char value[8];keyboard.GetString(section,"Char"," ",value,sizeof(value));m_KeyboardChars[k][0]=value[0]?value[0]:' ';
                keyboard.GetString(section,"ShiftChar"," ",value,sizeof(value));m_KeyboardChars[k][1]=value[0]?value[0]:' ';
                keyboard.WriteInteger("KeyLabel","Left",7);keyboard.WriteInteger("KeyLabel","Top",4);
                keyboard.WriteInteger("KeyLabel","Width",18);keyboard.WriteInteger("KeyLabel","Height",17);
                keyboard.WriteInteger("KeyLabel","Font",14);keyboard.WriteInteger("KeyLabel","DummyWnd",1);
                keyboard.WriteString("KeyLabel","Color","255,255,255");keyboard.WriteString("KeyLabel","BorderColor","0,0,0");
                m_KeyboardButtons[k].AddChild(&m_KeyboardLetters[k]);m_KeyboardLetters[k].Init(&keyboard,"KeyLabel");}
        }
        UpdateKeyboardLetters();
    }
    // Keyboard/extra buttons are outside the small account panel rectangle.
    SetStyle(GetStyle()|WND_S_SIZE_WITH_ALL_CHILD);
    m_Account.BringToTop();m_PassWord.BringToTop();
}
void KUiLogin::UpdateKeyboardLetters(){
    for(int k=0;k<48;++k){char text[2]={m_KeyboardChars[k][m_KeyboardShift?1:0],0};m_KeyboardLetters[k].SetText(text);}
}
void KUiLogin::ShowAccountHistory(){
    m_RecentCount=0;ZeroMemory(m_RecentAccounts,sizeof(m_RecentAccounts));
    KIniFile* settings=g_UiBase.GetCommSettingFile();
    if(settings){for(int i=0;i<10;++i){char key[32],account[32];ZeroMemory(account,sizeof(account));sprintf(key,"RecentAccount%d",i);
        settings->GetStruct("AccountList",key,account,sizeof(account));account[31]=0;
        int length=strlen(account);if(!length)continue;EDOneTimePad_Decipher(account,length);account[31]=0;
        bool valid=account[0]!=0;for(int n=0;account[n];++n)if((unsigned char)account[n]<33)valid=false;
        bool duplicate=false;for(int j=0;j<m_RecentCount;++j)if(!strcmp(account,m_RecentAccounts[j]))duplicate=true;
        if(valid&&!duplicate)lstrcpyn(m_RecentAccounts[m_RecentCount++],account,32);
    }g_UiBase.CloseCommSettingFile(false);}
    if(!m_RecentCount){char account[32];if(g_LoginLogic.GetLoginAccount(account)&&account[0]){lstrcpyn(m_RecentAccounts[0],account,32);m_RecentCount=1;}}
    if(!m_RecentCount)return;
    KPopupMenuData* menu=(KPopupMenuData*)malloc(MENU_DATA_SIZE(m_RecentCount));if(!menu)return;
    KPopupMenu::InitMenuData(menu,m_RecentCount);menu->usMenuFlag|=PM_F_AUTO_DEL_WHEN_HIDE;
    int x,y;m_AccountList.GetAbsolutePos(&x,&y);menu->nX=x;menu->nY=y+20;
    for(int row=0;row<m_RecentCount;++row){strcpy(menu->Items[row].szData,m_RecentAccounts[row]);menu->Items[row].uDataLen=strlen(menu->Items[row].szData);}
    KPopupMenu::Popup(menu,this,0x7d31);
}
static void PtLoginServiceLink(const char* key,const char* fallback){
    KIniFile ini;char url[512];url[0]=0;
    if(ini.Load("\\Ui\\LoginServices.ini"))ini.GetString("Services",key,"",url,sizeof(url));
    if(!strncmp(url,"https://",8)||!strncmp(url,"http://",7))ShellExecuteA(NULL,"open",url,NULL,NULL,SW_SHOWNORMAL);
    else UIMessageBox(fallback);
}
bool KUiLogin::RestoredLoginEvent(unsigned int msg,unsigned int param,int data){
    if(msg==WND_N_EDIT_LCLICK||msg==WND_N_EDIT_KILL_FOCUS){
        if(param==(unsigned int)(KWndWindow*)&m_Account)m_KeyboardTarget=&m_Account;
        if(param==(unsigned int)(KWndWindow*)&m_PassWord)m_KeyboardTarget=&m_PassWord;
    }
    if(msg==WND_M_MENUITEM_SELECTED&&HIWORD(data)==0x7d31){int selected=(short)LOWORD(data);
        if(selected>=0&&selected<m_RecentCount){m_Account.SetText(m_RecentAccounts[selected]);m_PassWord.Clear(true);m_KeyboardTarget=&m_PassWord;Wnd_SetFocusWnd(&m_PassWord);}return true;}
    if(msg!=WND_N_BUTTON_CLICK)return false;
    if(param==(unsigned int)(KWndWindow*)&m_AccountList){ShowAccountHistory();return true;}
    for(int i=0;i<4;++i)if(param==(unsigned int)(KWndWindow*)&m_ExtraButtons[i]){
        if(i==0)PtLoginServiceLink("RegisterUrl","Vui lßng liªn hÖ qu¶n trÞ m¸y chñ ®Ó ®¨ng ký tµi kho¶n.");
        else if(i==1)PtLoginServiceLink("ChangePasswordUrl","Vui lßng liªn hÖ qu¶n trÞ m¸y chñ ®Ó ®æi mËt khÈu.");
        else if(i==2)KUiOptions::OpenWindow(this);else OnCancel();return true;
    }
    for(i=0;i<51;++i)if(param==(unsigned int)(KWndWindow*)&m_KeyboardButtons[i]){
        if(i==50){m_KeyboardShift=m_KeyboardButtons[i].IsButtonChecked()!=0;UpdateKeyboardLetters();return true;}
        if(!m_KeyboardTarget)m_KeyboardTarget=&m_PassWord;Wnd_SetFocusWnd(m_KeyboardTarget);
        if(i<48){char value=m_KeyboardChars[i][m_KeyboardShift?1:0];m_KeyboardTarget->WndProc(WM_CHAR,(unsigned char)value,0);}
        else if(i==48)m_KeyboardTarget->WndProc(WM_KEYDOWN,VK_BACK,0);
        else if(m_KeyboardTarget==&m_Account){m_KeyboardTarget=&m_PassWord;Wnd_SetFocusWnd(&m_PassWord);}else OnLogin();
        return true;
    }
    return false;
}
