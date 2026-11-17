// The original three-group exit UI. Keep the world live until confirmation.
extern iCoreShell* g_pCoreShell;
class KPhongThanExitDialog : public KWndImage {
    KWndButton m_Confirm, m_Cancel, m_Save;
    KWndLabeledButton m_Group[3];
    KWndText512 m_Info;
    int m_Choice[3];
    char m_Names[3][2][32];
    char m_Hints[3][2][1024];
    static KPhongThanExitDialog *s_Instance;
    bool m_Wait;unsigned int m_Request,m_Deadline;
public:
    KPhongThanExitDialog():m_Wait(false),m_Request(0),m_Deadline(0){}
    static void Open() {
        if (!s_Instance) { s_Instance = new KPhongThanExitDialog; s_Instance->Initialize(); }
        s_Instance->RestoreChoices();s_Instance->Show(); s_Instance->BringToTop(); Wnd_SetExclusive(s_Instance);
    }
    static void Result(int result,unsigned int sequence){
        if(!s_Instance||!s_Instance->IsVisible()||!s_Instance->m_Wait||s_Instance->m_Request!=sequence)return;
        s_Instance->m_Wait=false;s_Instance->m_Confirm.Enable(true);
        if(result==1){s_Instance->Close();UiPostQuitMsg();}
        else s_Instance->m_Info.SetText("\x43\x68\xad\x61\x20\xae\xc6\x74\x20\xae\xad\xee\x63\x20\xf1\x79\x20\x74\x68\xb8\x63\x2e\x20\x4b\x69\xd3\x6d\x20\x74\x72\x61\x20\x4e\x68\xcb\x74\x20\x4e\x67\x75\x79\xd6\x74\x20\xa7\xac\x6e\x20\xae\xb7\x20\x6b\xdd\x63\x68\x20\x68\x6f\xb9\x74\x20\x63\xdf\x6e\x20\x68\xb9\x6e\x2c\x20\x6b\xfc\x20\x6e\xa8\x6e\x67\x20\x63\x68\x75\xe9\x74\x20\x70\x68\xb6\x69\x20\x64\xad\xed\x69\x20\x63\xca\x70\x20\x31\x30\x20\x76\xb5\x20\x70\x68\xc7\x6e\x20\x74\x68\xad\xeb\x6e\x67\x20\xae\x61\x6e\x67\x20\x63\x68\xea\x20\x64\x6f\x20\x74\x68\x69\xd5\x75\x20\xae\xac\x6e\x20\x68\x6f\xc6\x63\x20\x74\x68\xad\x20\xae\xc7\x79\x2e\x20\x43\xe3\x20\x74\x68\xd3\x20\x63\x68\xe4\x6e\x20\x4b\x68\xab\x6e\x67\x20\xae\xd3\x20\x74\x68\x6f\xb8\x74\x20\x62\xd7\x6e\x68\x20\x74\x68\xad\xea\x6e\x67\x2e");
    }
    void Breathe(){if(m_Wait&&GetTickCount()-m_Deadline>=8000){m_Wait=false;m_Confirm.Enable(true);m_Info.SetText("\x43\x68\xad\x61\x20\x6e\x68\xcb\x6e\x20\xae\xad\xee\x63\x20\x78\xb8\x63\x20\x6e\x68\xcb\x6e\x20\x74\xf5\x20\x73\x65\x72\x76\x65\x72\x2e\x20\x48\xb7\x79\x20\x74\x68\xf6\x20\x6c\xb9\x69\x3b\x20\x63\xf6\x61\x20\x73\xe6\x20\x63\x68\xad\x61\x20\xae\xe3\x6e\x67\x20\x67\x61\x6d\x65\x2e");}}
    void RestoreChoices(){
        KIniFile* settings=g_UiBase.GetCommSettingFile();int remember=1;if(settings)settings->GetInteger("ExitGame","Remember",1,&remember);m_Save.CheckButton(remember!=0);
        for(int i=0;i<3;++i){int choice=0;char key[16];sprintf(key,"Group_%d",i);if(settings&&remember)settings->GetInteger("ExitGame",key,0,&choice);m_Choice[i]=choice==1?1:0;const char* text=m_Names[i][m_Choice[i]];m_Group[i].SetLabel(*text=='$'?text+1:text);}
        if(settings)g_UiBase.CloseCommSettingFile(false);m_Wait=false;m_Confirm.Enable(true);UpdateHint(0);
    }
    void Initialize() {
        KIniFile ini; char path[256];
        g_UiBase.GetCurSchemePath(path, sizeof(path)); strcat(path, "\\UiExitGame.ini");
        if (!ini.Load(path)) return;
        int w, h; Wnd_GetScreenSize(w, h);
        ini.WriteInteger("Main", "Left", (w - 326) / 2);
        ini.WriteInteger("Main", "Top", (h - 219) / 2);
        Init(&ini, "Main");
        m_Confirm.Init(&ini, "BtnConfirm"); m_Cancel.Init(&ini, "BtnCancel");
        m_Save.Init(&ini, "ChkSaveSetting"); m_Save.CheckButton(true);
        m_Info.Init(&ini, "TxtInfo");
        AddChild(&m_Confirm); AddChild(&m_Cancel); AddChild(&m_Save); AddChild(&m_Info);
        for (int i = 0; i < 3; ++i) {
            char section[32], button[32], key[32];
            sprintf(section, "Group_%d", i); sprintf(button, "%s_Btn", section);
            char image[128]; ini.GetString(button, "Image", "", image, sizeof(image));
            ini.WriteString(section, "Image", image); ini.WriteInteger(section, "Up", 0);
            ini.WriteInteger(section, "Down", 1); ini.WriteInteger(section, "Over", 1);
            ini.WriteInteger(section, "Font", 14); ini.WriteString(section, "Color", "218,255,165");
            ini.WriteInteger(section, "LabelXOffset", 3);ini.WriteString(section,"OverColor","235,255,195");ini.WriteString(section,"OverBorderColor","0,0,0");
            m_Group[i].Init(&ini, section); AddChild(&m_Group[i]); m_Choice[i] = 0;
            for (int n = 0; n < 2; ++n) {
                sprintf(key, "SelText_%d", n); ini.GetString(section, key, "", m_Names[i][n], 32);
                sprintf(key, "SelInfo_%d", n); ini.GetString(section, key, "", m_Hints[i][n], 1024);
            }
            m_Group[i].SetLabel(m_Names[i][0][0] == '$' ? m_Names[i][0] + 1 : m_Names[i][0]);
        }
        Wnd_AddWindow(this, WL_TOPMOST); UpdateHint(0);
    }
    void UpdateHint(int group) {
        char text[1024]; strcpy(text, m_Hints[group][m_Choice[group]]);
        char *content = text[0] == '$' ? text + 1 : text;
        int len = TEncodeText(content, strlen(content)); m_Info.SetText(content, len);
    }
    void Close() { m_Wait=false;KPopupMenu::Cancel(); Wnd_ReleaseExclusive(this); Hide(); }
    int WndProc(unsigned int msg, unsigned int param, int value) {
        if (msg == WM_KEYDOWN && param == VK_ESCAPE) { Close(); return 1; }
        if (msg == WND_N_BUTTON_CLICK) {
            if ((KWndWindow*)param == &m_Cancel) { Close(); return 1; }
            if ((KWndWindow*)param == &m_Confirm) {
                if(m_Wait)return 1;
                // Preserve original three-group selections, then wait for world ACK.
                KIniFile *settings = g_UiBase.GetCommSettingFile();
                if (settings) {
                    settings->WriteInteger("ExitGame","Remember",m_Save.IsButtonChecked()?1:0);
                    if (m_Save.IsButtonChecked())
                        for (int k = 0; k < 3; ++k) {
                            char key[16]; sprintf(key, "Group_%d", k);
                            settings->WriteInteger("ExitGame", key, m_Choice[k]);
                        }
                    g_UiBase.CloseCommSettingFile(true);
                }
                int mask=(m_Choice[0]?1:0)|(m_Choice[1]?2:0)|(m_Choice[2]?4:0);
                m_Request=g_pCoreShell?g_pCoreShell->OperationRequest(GOI_OFFLINE,mask,1):0;
                if(m_Request){m_Wait=true;m_Deadline=GetTickCount();m_Confirm.Enable(false);m_Info.SetText("\xa7\x61\x6e\x67\x20\x63\x68\xea\x20\x73\x65\x72\x76\x65\x72\x20\x78\xb8\x63\x20\x6e\x68\xcb\x6e\x20\x70\x68\xad\xac\x6e\x67\x20\x74\x68\xf8\x63\x20\x72\xea\x69\x20\x6d\xb9\x6e\x67\x2e\x2e\x2e");}return 1;
            }
            for (int i = 0; i < 3; ++i) if ((KWndWindow*)param == &m_Group[i]) {
                if(m_Wait)return 1;
                KPopupMenuData *menu = (KPopupMenuData*)malloc(MENU_DATA_SIZE(2));
                if (!menu) return 1;
                KPopupMenu::InitMenuData(menu, 2);
                menu->uTextColor = 0xffdaffa5; menu->uSelTextColor = 0xffffffff;
                menu->uTextBorderColor = menu->uSelTextBorderColor = 0xff000000;
                for (int n = 0; n < 2; ++n) {
                    const char *name = m_Names[i][n]; if (*name == '$') ++name;
                    strcpy(menu->Items[n].szData, name); menu->Items[n].uDataLen = strlen(name);
                }
                int x, y, width, height; m_Group[i].GetAbsolutePos(&x, &y); m_Group[i].GetSize(&width, &height);
                menu->nX = x; menu->nY = y + height; KPopupMenu::Popup(menu, this, 20 + i); return 1;
            }
        }
        if (msg == WND_M_MENUITEM_SELECTED && (KWndWindow*)param == this) {
            if(m_Wait){KPopupMenu::Cancel();return 1;}
            int group = HIWORD(value) - 20, selected = (short)LOWORD(value);
            if (group >= 0 && group < 3 && selected >= 0 && selected < 2) {
                m_Choice[group] = selected; const char *name = m_Names[group][selected];
                m_Group[group].SetLabel(*name == '$' ? name + 1 : name); UpdateHint(group);
            }
            KPopupMenu::Cancel(); return 1;
        }
        return KWndImage::WndProc(msg, param, value);
    }
};
KPhongThanExitDialog *KPhongThanExitDialog::s_Instance = NULL;
