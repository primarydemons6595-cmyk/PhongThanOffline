#include "../../../../Headers/PhongThanConsumableUi.h"
static int ptChatColor=-1;
static bool PTHasChatColor() {
    PTConsumableUiState state;memset(&state,0,sizeof(state));
    return g_pCoreShell && g_pCoreShell->GetGameData(PT_GDI_CONSUMABLE_BUFFS,(unsigned int)&state,sizeof(state)) &&
        (state.RemainingSeconds[PhongThanTimedItemSlot(233)]>0 || state.RemainingSeconds[PhongThanTimedItemSlot(390)]>0);
}
static bool PTChatRgb(int index,int* rgb) {
    if(index<0 || index>=12)return false;
    KIniFile ini;if(!ini.Load("\\Ui\\ui3\\\xD1\xA1\xD1\xD5\xC9\xAB.ini"))return false;
    char key[12],value[64];sprintf(key,"%d",index);ini.GetString("ColorList",key,"",value,sizeof(value));
    return sscanf(value,"%d,%d,%d",rgb,rgb+1,rgb+2)==3;
}
static void PTChatColorMenu(KWndWindow* owner,int x,int y) {
    KPopupMenuData* menu=(KPopupMenuData*)malloc(MENU_DATA_SIZE(13));if(!menu)return;
    KPopupMenu::InitMenuData(menu,13);menu->usMenuFlag|=PM_F_AUTO_DEL_WHEN_HIDE;
    strcpy(menu->Items[0].szData,"Mac dinh");menu->Items[0].uDataLen=8;
    for(int i=0;i<12;++i){int rgb[3];if(!PTChatRgb(i,rgb)){free(menu);return;}
        char* label=menu->Items[i+1].szData;label[0]=KTC_COLOR;label[1]=(char)rgb[0];label[2]=(char)rgb[1];label[3]=(char)rgb[2];
        sprintf(label+4,"Abc  %d",i+1);menu->Items[i+1].uDataLen=4+strlen(label+4);
    }
    menu->nX=(short)x;menu->nY=(short)y;KPopupMenu::Popup(menu,owner,0x7e21);
}
