#include <shellapi.h>
#pragma comment(lib,"shell32.lib")
enum {PtTrayMessage=WM_APP+74,PtTrayId=74,PtTrayHotkey=0x5054};
static bool ptTrayVisible=false;
static WNDPROC ptTrayPrevious=0;
static bool PhongThanTrayInput(UINT,WPARAM,LPARAM);
static LRESULT CALLBACK PtTrayWindowProc(HWND window,UINT message,WPARAM key,LPARAM data){
    if(PhongThanTrayInput(message,key,data))return 0;
    return CallWindowProcA(ptTrayPrevious,window,message,key,data);
}
static NOTIFYICONDATAA PtTrayData(){
    NOTIFYICONDATAA n;ZeroMemory(&n,sizeof(n));n.cbSize=sizeof(n);
    n.hWnd=g_GetMainHWnd();n.uID=PtTrayId;n.uFlags=NIF_ICON|NIF_MESSAGE|NIF_TIP;
    n.uCallbackMessage=PtTrayMessage;n.hIcon=(HICON)GetClassLong(n.hWnd,GCL_HICON);
    if(!n.hIcon)n.hIcon=LoadIcon(NULL,IDI_APPLICATION);
    lstrcpynA(n.szTip,"Phong Than - Alt+B",sizeof(n.szTip));return n;
}
static void PhongThanRestoreClientFromTray(){
    if(!ptTrayVisible)return;NOTIFYICONDATAA n=PtTrayData();
    ShowWindow(n.hWnd,SW_RESTORE);SetForegroundWindow(n.hWnd);
    Shell_NotifyIconA(NIM_DELETE,&n);UnregisterHotKey(n.hWnd,PtTrayHotkey);ptTrayVisible=false;
}
void PhongThanHideClientToTray(){
    if(ptTrayVisible){PhongThanRestoreClientFromTray();return;}
    NOTIFYICONDATAA n=PtTrayData();
    // Keep the game reachable if Explorer cannot create a tray icon.
    if(!n.hWnd)return;
    if(!ptTrayPrevious){
        SetLastError(0);LONG previous=SetWindowLongA(n.hWnd,GWL_WNDPROC,(LONG)PtTrayWindowProc);
        if(!previous)return;ptTrayPrevious=(WNDPROC)previous;
    }
    if(!Shell_NotifyIconA(NIM_ADD,&n))return;
    ptTrayVisible=true;RegisterHotKey(n.hWnd,PtTrayHotkey,MOD_ALT,'B');ShowWindow(n.hWnd,SW_HIDE);
}
static bool PhongThanTrayInput(UINT message,WPARAM key,LPARAM data){
    if((message==PtTrayMessage&&(data==WM_LBUTTONUP||data==WM_LBUTTONDBLCLK||data==WM_RBUTTONUP)) ||
       (message==WM_HOTKEY&&key==PtTrayHotkey)) {PhongThanRestoreClientFromTray();return true;}
    if(message==WM_SYSKEYDOWN&&key=='B'&&(GetKeyState(VK_MENU)&0x8000)) {PhongThanHideClientToTray();return true;}
    return false;
}
static void PhongThanTrayClose(){
    if(ptTrayVisible){NOTIFYICONDATAA n=PtTrayData();Shell_NotifyIconA(NIM_DELETE,&n);UnregisterHotKey(n.hWnd,PtTrayHotkey);ptTrayVisible=false;}
    HWND window=g_GetMainHWnd();
    if(ptTrayPrevious&&IsWindow(window)){SetWindowLongA(window,GWL_WNDPROC,(LONG)ptTrayPrevious);ptTrayPrevious=0;}
}
