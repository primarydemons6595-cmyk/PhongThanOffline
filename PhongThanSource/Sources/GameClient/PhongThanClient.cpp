// PhongThanClient.cpp : Defines the entry point for the application.
//

#include "KWin32.h"
#include "Ui/ChatDock.h"
#include "Ui/UiCase/UiESCDlg.h"
#include "Login/Login.h"
#include "KCore.h"
#include "PhongThanClient.h"
#include "KWin32Wnd.h"
#include "../../Represent/iRepresent/iRepresentShell.h"
#include "Ui/UiShell.h"
#include "Ui/Elem/Wnds.h"
#include "NetConnect/NetConnectAgent.h"
#include "TextCtrlCmd/TextCtrlCmd.h"
#include "KPakList.h"
#include "KPakFile.h"
#include "Ui/Elem/TextPic.h"
#include "Ui/Elem/UiCursor.h"
#include "Ui/Elem/SpecialFuncs.h"
#include "Ui/FilterTextLib.h"
#include "Ui/ChatFilter.h"
#include "Ui/uibase.h"
#include "ErrorCode.h"
#include <tchar.h>
#include <tlhelp32.h>
#include "PhongThanConfig.h"
#include "Ui/UiCase/UiInit.h"

#include "Ui/UiPtTray.inl"
extern bool PhongThanEquipSetKey(UINT,WPARAM);
extern void PhongThanEquipSetsPulse();

#define ClientVersion
KMyApp MyApp;
HINSTANCE hInst;
KPakList g_PakList;
CFilterTextLib g_libFilterText;
CChatFilter g_ChatFilter;


#define    QUIT_QUESTION_ID    "22"
#define    GAME_TITLE            "23"

#define REPRESENT_MODULE_2            "Represent2.dll"
#define REPRESENT_MODULE_3            "Represent3.dll"
#define CREATE_REPRESENT_SHELL_FUN    "CreateRepresentShell"
#define    GAME_FPS            18

struct iRepresentShell *g_pRepresentShell = NULL;
struct IInlinePicEngineSink *g_pIInlinePicSink = NULL;
iCoreShell *g_pCoreShell = NULL;
KMusic *g_pMusic = NULL;

#define    DYNAMIC_LINK_REPRESENT_LIBRARY

#ifdef DYNAMIC_LINK_REPRESENT_LIBRARY
static HMODULE l_hRepresentModule = NULL;
int g_bRepresent3 = false;
#endif

int g_bScreen = true;
char g_szGameName[128] = "Phong Than Online";

KClientCallback g_ClientCallback;

#define PHONGTHAN_DEFAULT_SCREEN_WIDTH   1024
#define PHONGTHAN_DEFAULT_SCREEN_HEIGHT  768
#define PHONGTHAN_MIN_SCREEN_WIDTH       800
#define PHONGTHAN_MIN_SCREEN_HEIGHT      600
#define PHONGTHAN_MAX_SCREEN_WIDTH       1920
#define PHONGTHAN_MAX_SCREEN_HEIGHT      1080

/*
 * Add this macro by liupeng on 2003.3.20
 * This macro is helper that can judge some legal character
 */
#define _private_IS_SPACE(c)   ((c) == ' ' || (c) == '\r' || (c) == '\n' || (c) == '\t' || (c) == 'x')
#define IS_SPACE(c)    _private_IS_SPACE(c)

static void PtReleaseStartupStage(const char* text) {
#ifdef PT_SECURE_RELEASE_PROFILE
 if(strstr(GetCommandLineA(),"--verify-release-startup")){DWORD written=0;HANDLE output=GetStdHandle(STD_ERROR_HANDLE);WriteFile(output,text,strlen(text),&written,NULL);WriteFile(output,"\n",1,&written,NULL);}
#endif
}
#ifdef PT_SECURE_RELEASE_PROFILE
static void CALLBACK PtVerifyReleaseQuit(HWND,UINT,UINT id,DWORD){KillTimer(NULL,id);PtReleaseStartupStage("Verify.quit");PostQuitMessage(0);}
#endif
int APIENTRY
WinMain(HINSTANCE
hInstance,
HINSTANCE hPrevInstance,
        LPSTR
lpCmdLine,
int nCmdShow
)
{



/*
 * Add this funtion by liupeng on 2003.3.20
 * We can find some error when start a console tracer
 */
#ifdef    TRUE

#ifdef PT_SECURE_RELEASE_PROFILE
if(lpCmdLine && strcmp(lpCmdLine,"--verify-release-startup")==0) {
    hInst=hInstance;SetErrorMode(SEM_FAILCRITICALERRORS|SEM_NOGPFAULTERRORBOX);
    BOOL ready=MyApp.Init(hInstance);
    if(ready) {SetTimer(NULL,0,250,PtVerifyReleaseQuit);MyApp.Run();}
    return ready?0:23;
}
#endif

bool bOpenTracer = false;

while( lpCmdLine[0] == '-' || lpCmdLine[0] == '/' )
{
    lpCmdLine++;

    switch ( *lpCmdLine++ )
    {
    case 'c':
    case 'C':
        bOpenTracer = true;
        break;
    }

    while( IS_SPACE( *lpCmdLine ) )
    {
        lpCmdLine++;
    }
}

if ( bOpenTracer )
{
    AllocConsole();
}

#endif // End of this function

hInst = hInstance;
if (MyApp.
Init(hInstance)
)
MyApp.

Run();

#ifdef TRUE

if ( bOpenTracer )
{
    FreeConsole();
}

#endif

Error_Box();

return 0;
}

KMyApp::KMyApp() {
    m_pInlinePicSink = NULL;
    m_LastPaintTime = 0;
    m_RenderFps = 30;
    m_TimerPeriodActive = false;
}

BOOL InitRepresentShell(BOOL bFullScreen, int nWidth, int nHeight) {
    Error_SetErrorString(g_bRepresent3 ? REPRESENT_MODULE_3 : REPRESENT_MODULE_2);
    if (g_pRepresentShell == NULL) {
#ifdef DYNAMIC_LINK_REPRESENT_LIBRARY
        if (l_hRepresentModule == NULL &&
            (l_hRepresentModule = LoadLibrary(g_bRepresent3 ? REPRESENT_MODULE_3 : REPRESENT_MODULE_2)) == NULL) {
            Error_SetErrorCode(ERR_T_LOAD_MODULE_FAILED);
            return FALSE;
        }
        fnCreateRepresentShell pCreate = (fnCreateRepresentShell) GetProcAddress(
                l_hRepresentModule, CREATE_REPRESENT_SHELL_FUN);
        if (pCreate == NULL ||
            (g_pRepresentShell = pCreate()) == NULL) {
            Error_SetErrorCode((pCreate == NULL) ? ERR_T_MODULE_UNCORRECT : ERR_T_MODULE_INIT_FAILED);
            return FALSE;
        }
#else
        g_pRepresentShell = CreateRepresentShell();
#endif
    }
    if (g_pRepresentShell->Create(nWidth, nHeight, bFullScreen != 0)) {
        return TRUE;
    } else {
        Error_SetErrorCode(g_bRepresent3 ? ERR_T_REPRESENT3_INIT_FAILED : ERR_T_REPRESENT2_INIT_FAILED);
        return FALSE;
    }
}

BOOL KMyApp::GameInit() {
    PtReleaseStartupStage("GameInit.begin");
    int nScreenWidth = PHONGTHAN_DEFAULT_SCREEN_WIDTH;
    int nScreenHeight = PHONGTHAN_DEFAULT_SCREEN_HEIGHT;

    Error_SetErrorString("KMyApp::GameInit");
#ifdef KUI_USE_HARDWARE_MOUSE

    ShowMouse(TRUE);

#else   // KUI_USE_HARDWARE_MOUSE

    ShowMouse(FALSE);

#endif

    // Desktop/automation launchers need not supply the executable directory
    // as their working directory. The client data always lives beside Game.exe.
    char executablePath[MAX_PATH];
    DWORD pathLength=GetModuleFileNameA(NULL,executablePath,sizeof(executablePath));
    if(pathLength && pathLength<sizeof(executablePath)) {
        char* separator=strrchr(executablePath,'\\');
        if(separator){*separator=0;SetCurrentDirectoryA(executablePath);}
    }
    g_SetRootPath(NULL);
    g_SetFilePath("\\");

    KIniFile *pSetting = g_UiBase.GetCommConfigFile();
    if (pSetting) {
        pSetting->GetString("Main", "GameName", "Phong Than Online", g_szGameName, sizeof(g_szGameName));
        SetWindowText(g_GetMainHWnd(), g_szGameName);
    }

#ifdef _DEBUG
    g_FindDebugWindow("#32770","DebugWin");
#else
    g_FindDebugWindow("#32770", "DebugWin");
#endif

    KIniFile IniFile;
    if (!IniFile.Load("\\config.ini")) {
        Error_SetErrorCode(ERR_T_FILE_NO_FOUND);
        Error_SetErrorString("\\config.ini");
        return FALSE;
    }

#ifdef _DEBUG
    BOOL		bCursor = FALSE;
    if (IniFile.GetInteger("Client", "ShowCursor", 0, &bCursor))
        ShowMouse(TRUE);
#endif

    // Original game.exe 0x43C900 uses the FPS table {30, 54}.
    int fpsMode = 0;
    IniFile.GetInteger("Client", "FPS", 0, &fpsMode);
    m_RenderFps = fpsMode == 1 ? 54 : 30;
    IniFile.GetInteger("Client", "FullScreen", FALSE, &g_bScreen);
    IniFile.GetInteger("Client", "ScreenWidth", PHONGTHAN_DEFAULT_SCREEN_WIDTH, &nScreenWidth);
    IniFile.GetInteger("Client", "ScreenHeight", PHONGTHAN_DEFAULT_SCREEN_HEIGHT, &nScreenHeight);

    if (nScreenWidth < PHONGTHAN_MIN_SCREEN_WIDTH)
        nScreenWidth = PHONGTHAN_MIN_SCREEN_WIDTH;
    else if (nScreenWidth > PHONGTHAN_MAX_SCREEN_WIDTH)
        nScreenWidth = PHONGTHAN_MAX_SCREEN_WIDTH;

    if (nScreenHeight < PHONGTHAN_MIN_SCREEN_HEIGHT)
        nScreenHeight = PHONGTHAN_MIN_SCREEN_HEIGHT;
    else if (nScreenHeight > PHONGTHAN_MAX_SCREEN_HEIGHT)
        nScreenHeight = PHONGTHAN_MAX_SCREEN_HEIGHT;

#ifdef DYNAMIC_LINK_REPRESENT_LIBRARY
    IniFile.GetInteger("Client", "Represent", 2, &g_bRepresent3);
    g_bRepresent3 = (g_bRepresent3 == 3);
#endif

    // Phong Than uses the original VNG package chain as the authoritative
    // content source. Loose files are only a compatibility fallback for
    // layouts absent from the currently available VNG packages.
    if (!g_PakList.Open("\\package.ini")) {
        Error_SetErrorCode(ERR_T_FILE_NO_FOUND);
        Error_SetErrorString("\\package.ini");
        return FALSE;
    }

    PtReleaseStartupStage("PAK.ready");
    XPackElemFileRef VngUiSetting;
    if (!g_PakList.FindElemFile("\\Ui\\Setting.ini", VngUiSetting)) {
        Error_SetErrorCode(ERR_T_FILE_NO_FOUND);
        Error_SetErrorString("VNG PAK: \\Ui\\Setting.ini");
        return FALSE;
    }

    g_SetPakFileMode(0);

    char szPath[MAX_PATH];
    if (IniFile.GetString("Client", "CapPath", "", szPath, sizeof(szPath))) {
        if (szPath[0])
            SetScrPicPath(szPath);
    }

    IniFile.Clear();

    if (!g_libFilterText.Initialize()
        || !g_ChatFilter.Initialize())
        return FALSE;

    if (!InitRepresentShell(g_bScreen, nScreenWidth, nScreenHeight)) {
        return FALSE;
    }

    // Set the native UI extent before UiInit creates and positions any
    // out-of-game window. Setting it afterwards mixes 800 and 1024 layouts.
    UiSetScreenSize(nScreenWidth, nScreenHeight);

    if (!UiInit()) {
        Error_SetErrorCode(ERR_T_MODULE_INIT_FAILED);
        Error_SetErrorString("UiInit");
        return FALSE;
    }

    //[wxb 2003-6-23]
    m_pInlinePicSink = new KInlinePicSink;
    if (m_pInlinePicSink) {
        m_pInlinePicSink->Init(g_pRepresentShell);
        _ASSERT(NULL == g_pIInlinePicSink);
        g_pIInlinePicSink = m_pInlinePicSink;
    }

    PtReleaseStartupStage("UiPaint.begin");
    UiPaint(0);
    PtReleaseStartupStage("UiPaint.ready");

    // init dsound
    m_Sound.Init();

    SetMultiGame(TRUE);

    PtReleaseStartupStage("CoreGetShell.begin");
    if ((g_pCoreShell = CoreGetShell()) == NULL) {
        Error_SetErrorCode(ERR_T_MODULE_INIT_FAILED);
        Error_SetErrorString("CoreGetShell");
        return false;
    }
    PtReleaseStartupStage("CoreGetShell.ready");
    g_pCoreShell->SetRepresentShell(g_pRepresentShell);
    g_pCoreShell->SetMusicInterface((KMusic * ) & m_Music);
    g_pCoreShell->SetCallDataChangedNofify(&g_ClientCallback);
    g_pCoreShell->SetRepresentAreaSize(nScreenWidth, nScreenHeight);

    g_pMusic = &m_Music;

    if (g_NetConnectAgent.Initialize() == 0) {
        Error_SetErrorCode(ERR_T_MODULE_INIT_FAILED);
        Error_SetErrorString("NetConnectAgent");
        return FALSE;
    }

    m_GameCounter = 0;
    m_Timer.Start();
    m_LastPaintTime = 0;
    m_TimerPeriodActive = (timeBeginPeriod(1) == TIMERR_NOERROR);

    SetMouseHoverTime(400);

    //Son Check limit
//	GameLimitWindown();
    //end code

    //Son get version game
//	int nGetVersionGame = g_pCoreShell->GetGameData(GDI_GET_VERSION_GAME, 0, 0);
//	g_DebugLog("nGetVersionGame %d ", nGetVersionGame);
    //end code

    if (UiStart()) {
        PtReleaseStartupStage("UiStart.ready");
        return TRUE;
    } else {
        Error_SetErrorCode(ERR_T_MODULE_INIT_FAILED);
        Error_SetErrorString("UiStart");
        return FALSE;
    }
}

BOOL KMyApp::GameExit() {
    PhongThanTrayClose();
    if (m_TimerPeriodActive) {
        timeEndPeriod(1);
        m_TimerPeriodActive = false;
    }
    if (m_pInlinePicSink) {
        //[wxb 2003-6-23]
        m_pInlinePicSink->UnInit();
        delete m_pInlinePicSink;
        m_pInlinePicSink = NULL;
        g_pIInlinePicSink = NULL;
    }

    UiExit();

    g_pMusic = NULL;
    if (g_pCoreShell) {
        g_pCoreShell->SetRepresentShell(NULL);
        g_pCoreShell->SetClient(NULL);
        g_pCoreShell->SetMusicInterface(NULL);
        g_pCoreShell->Release();
        g_pCoreShell = NULL;
    }

    if (g_pRepresentShell) {
        g_pRepresentShell->Release();
        g_pRepresentShell = NULL;
    }

    g_NetConnectAgent.Exit();

    m_Music.Close();
    m_Sound.Exit();

#ifdef DYNAMIC_LINK_REPRESENT_LIBRARY
    if (l_hRepresentModule) {
        FreeLibrary(l_hRepresentModule);
        l_hRepresentModule = NULL;
    }
#endif

    ::ShowCursor(TRUE);

    g_ChatFilter.Uninitialize();
    g_libFilterText.Uninitialize();
    return TRUE;
}

int checkVNWCountTimer = 0;

BOOL KMyApp::GameLoop() {
    if(g_LoginLogic.GetStatus()==LL_S_IN_GAME)PhongThanEquipSetsPulse();
    static int nGameFps = 0;
    g_NetConnectAgent.Breathe();
    DWORD elapsed = m_Timer.GetElapse();
    // Preserve all authoritative 18 Hz ticks. Use 64-bit products for long sessions.
    // The original client also runs a due logic tick before considering a paint.
    if ((__int64)m_GameCounter * 1000 <= (__int64)elapsed * GAME_FPS) {
        if (!g_pCoreShell->Breathe() || !UiHeartBeat())
            return false;
        ++m_GameCounter;
        elapsed = m_Timer.GetElapse();
        if (elapsed)
            nGameFps = (int)((__int64)m_GameCounter * 1000 / elapsed);
        return true;
    }
    // Original render cadence: FPS=0 -> 30; FPS=1 -> 54. Do not repaint the
    // same state on every idle iteration or sleep after an expensive paint.
    if (elapsed - m_LastPaintTime >= 1000 / m_RenderFps) {
        m_LastPaintTime = elapsed;
        UiPaint(nGameFps);
    } else {
        Sleep(1);
    }
    return true;
}

int KMyApp::HandleInput(UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if(PhongThanTrayInput(uMsg,wParam,lParam))return 1;
    if(g_LoginLogic.GetStatus()==LL_S_IN_GAME&&PhongThanEquipSetKey(uMsg,wParam))return 1;
    int nRet = 0;
    // Mouse messages are in fitted window-client coordinates. UI and world
    // hit tests use the selected canvas resolution.
    if (uMsg >= WM_MOUSEFIRST && uMsg <= WM_MOUSELAST && uMsg != WM_MOUSEWHEEL) {
        RECT client; GetClientRect(g_GetMainHWnd(), &client);
        int canvasWidth, canvasHeight; Wnd_GetScreenSize(canvasWidth, canvasHeight);
        // Input covers the entire render canvas, including the chat column.
        canvasWidth += g_PtChatDockWidth;
        if (client.right > 0 && client.bottom > 0) {
            int x = MulDiv((short)LOWORD(lParam), canvasWidth, client.right);
            int y = MulDiv((short)HIWORD(lParam), canvasHeight, client.bottom);
            lParam = MAKELPARAM(x, y);
        }
    }

    if (uMsg != WM_CLOSE) {
        UiProcessInput(uMsg, wParam, lParam);
    } else if (!UiIsAlreadyQuit()) {
        if (g_LoginLogic.GetStatus() == LL_S_IN_GAME)
            KUiESCDlg::OpenExitWindow();
        else
            UiPostQuitMsg();
        return 1;
    }
    return nRet;
}

BOOL KMyApp::Game_AntiVMW() {

    DWORD pid = 0, Index = 0, cPid = 0;
    PROCESSENTRY32 pe32;
    BOOL res = FALSE;
    HANDLE snap = INVALID_HANDLE_VALUE, proc = INVALID_HANDLE_VALUE;
    char c = 0;
    std::string strName;
    pid = GetCurrentProcessId();

    snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, pid);

    if (snap == INVALID_HANDLE_VALUE) {
        return -1;
    }
    pe32.dwSize = sizeof(PROCESSENTRY32);
    res = Process32First(snap, &pe32);
    if (res == FALSE) {
        CloseHandle(snap);
        return -2;
    }
    do {
        if (_tcscmp(pe32.szExeFile, TEXT(IS_CHECK_VMW_EXE_ID_1)) == 0) {
            Index++;
            strName = IS_CHECK_VMW_EXE_ID_1;
            if (Index > 0) {
                HANDLE pr_open_close = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
                TerminateProcess(pr_open_close, 0);
                CloseHandle(pr_open_close);
                exit(0);
            }
        } else if (_tcscmp(pe32.szExeFile, TEXT(IS_CHECK_VMW_EXE_ID_2)) == 0) {
            Index++;
            strName = IS_CHECK_VMW_EXE_ID_2;
            if (Index > 0) {
                HANDLE pr_open_close = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
                TerminateProcess(pr_open_close, 0);
                CloseHandle(pr_open_close);
                exit(0);
            }
        } else if (_tcscmp(pe32.szExeFile, TEXT(IS_CHECK_VMW_EXE_ID_3)) == 0) {
            Index++;
            strName = IS_CHECK_VMW_EXE_ID_3;
            if (Index > 0) {
                HANDLE pr_open_close = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
                TerminateProcess(pr_open_close, 0);
                CloseHandle(pr_open_close);
                exit(0);
            }
        } else if (_tcscmp(pe32.szExeFile, TEXT(IS_CHECK_VMW_EXE_ID_4)) == 0) {
            Index++;
            strName = IS_CHECK_VMW_EXE_ID_4;
            if (Index > 0) {
                HANDLE pr_open_close = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
                TerminateProcess(pr_open_close, 0);
                CloseHandle(pr_open_close);
                exit(0);
            }
        } else if (_tcscmp(pe32.szExeFile, TEXT(IS_CHECK_VMW_EXE_ID_5)) == 0) {
            Index++;
            strName = IS_CHECK_VMW_EXE_ID_5;
            if (Index > 0) {
                HANDLE pr_open_close = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
                TerminateProcess(pr_open_close, 0);
                CloseHandle(pr_open_close);
                exit(0);
            }
        } else if (_tcscmp(pe32.szExeFile, TEXT(IS_CHECK_VMW_EXE_ID_6)) == 0) {
            Index++;
            strName = IS_CHECK_VMW_EXE_ID_6;
            if (Index > 0) {
                HANDLE pr_open_close = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
                TerminateProcess(pr_open_close, 0);
                CloseHandle(pr_open_close);
                exit(0);
            }
        } else if (_tcscmp(pe32.szExeFile, TEXT(IS_CHECK_VMW_EXE_ID_7)) == 0) {
            Index++;
            strName = IS_CHECK_VMW_EXE_ID_7;
            if (Index > 0) {
                HANDLE pr_open_close = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
                TerminateProcess(pr_open_close, 0);
                CloseHandle(pr_open_close);
                exit(0);
            }
        } else if (_tcscmp(pe32.szExeFile, TEXT(IS_CHECK_VMW_EXE_ID_8)) == 0) {
            Index++;
            strName = IS_CHECK_VMW_EXE_ID_8;
            if (Index > 0) {
                HANDLE pr_open_close = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
                TerminateProcess(pr_open_close, 0);
                CloseHandle(pr_open_close);
                exit(0);
            }
        } else if (_tcscmp(pe32.szExeFile, TEXT(IS_CHECK_VMW_EXE_ID_9)) == 0) {
            Index++;
            strName = IS_CHECK_VMW_EXE_ID_9;
            if (Index > 0) {
                HANDLE pr_open_close = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
                TerminateProcess(pr_open_close, 0);
                CloseHandle(pr_open_close);
                exit(0);
            }
        } else if (_tcscmp(pe32.szExeFile, TEXT(IS_CHECK_VMW_EXE_ID_10)) == 0) {
            Index++;
            strName = IS_CHECK_VMW_EXE_ID_10;
            if (Index > 0) {
                HANDLE pr_open_close = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
                TerminateProcess(pr_open_close, 0);
                CloseHandle(pr_open_close);
                exit(0);
            }
        } else if (_tcscmp(pe32.szExeFile, TEXT(IS_CHECK_VMW_EXE_ID_11)) == 0) {
            Index++;
            strName = IS_CHECK_VMW_EXE_ID_11;
            if (Index > 0) {
                HANDLE pr_open_close = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
                TerminateProcess(pr_open_close, 0);
                CloseHandle(pr_open_close);
                exit(0);
            }
        } else if (_tcscmp(pe32.szExeFile, TEXT(IS_CHECK_VMW_EXE_ID_12)) == 0) {
            Index++;
            strName = IS_CHECK_VMW_EXE_ID_12;
            if (Index > 0) {
                HANDLE pr_open_close = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
                TerminateProcess(pr_open_close, 0);
                CloseHandle(pr_open_close);
                exit(0);
            }
        } else if (_tcscmp(pe32.szExeFile, TEXT(IS_CHECK_VMW_EXE_ID_13)) == 0) {
            Index++;
            strName = IS_CHECK_VMW_EXE_ID_13;
            if (Index > 0) {
                HANDLE pr_open_close = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
                TerminateProcess(pr_open_close, 0);
                CloseHandle(pr_open_close);
                exit(0);
            }
        } else if (_tcscmp(pe32.szExeFile, TEXT(IS_CHECK_VMW_EXE_ID_14)) == 0) {
            Index++;
            strName = IS_CHECK_VMW_EXE_ID_14;
            if (Index > 0) {
                HANDLE pr_open_close = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
                TerminateProcess(pr_open_close, 0);
                CloseHandle(pr_open_close);
                exit(0);
            }
        } else if (_tcscmp(pe32.szExeFile, TEXT(IS_CHECK_VMW_EXE_ID_15)) == 0) {
            Index++;
            strName = IS_CHECK_VMW_EXE_ID_15;
            if (Index > 0) {
                HANDLE pr_open_close = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
                TerminateProcess(pr_open_close, 0);
                CloseHandle(pr_open_close);
                exit(0);
            }
        } else if (_tcscmp(pe32.szExeFile, TEXT(IS_CHECK_VMW_EXE_ID_16)) == 0) {
            Index++;
            strName = IS_CHECK_VMW_EXE_ID_16;
            if (Index > 0) {
                HANDLE pr_open_close = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
                TerminateProcess(pr_open_close, 0);
                CloseHandle(pr_open_close);
                exit(0);
            }
        } else if (_tcscmp(pe32.szExeFile, TEXT(IS_CHECK_VMW_EXE_ID_17)) == 0) {
            Index++;
            strName = IS_CHECK_VMW_EXE_ID_17;
            if (Index > 0) {
                HANDLE pr_open_close = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
                TerminateProcess(pr_open_close, 0);
                CloseHandle(pr_open_close);
                exit(0);
            }
        } else if (_tcscmp(pe32.szExeFile, TEXT(IS_CHECK_VMW_EXE_ID_18)) == 0) {
            Index++;
            strName = IS_CHECK_VMW_EXE_ID_18;
            if (Index > 0) {
                HANDLE pr_open_close = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
                TerminateProcess(pr_open_close, 0);
                CloseHandle(pr_open_close);
                exit(0);
            }
        } else if (_tcscmp(pe32.szExeFile, TEXT(IS_CHECK_VMW_EXE_ID_19)) == 0) {
            Index++;
            strName = IS_CHECK_VMW_EXE_ID_19;
            if (Index > 0) {
                HANDLE pr_open_close = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
                TerminateProcess(pr_open_close, 0);
                CloseHandle(pr_open_close);
                exit(0);
            }
        } else if (_tcscmp(pe32.szExeFile, TEXT(IS_CHECK_VMW_EXE_ID_20)) == 0) {
            Index++;
            strName = IS_CHECK_VMW_EXE_ID_20;
            if (Index > 0) {
                HANDLE pr_open_close = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
                TerminateProcess(pr_open_close, 0);
                CloseHandle(pr_open_close);
                exit(0);
            }
        } else if (_tcscmp(pe32.szExeFile, TEXT(IS_CHECK_VMW_EXE_ID_21)) == 0) {
            Index++;
            strName = IS_CHECK_VMW_EXE_ID_21;
            if (Index > 0) {
                HANDLE pr_open_close = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
                TerminateProcess(pr_open_close, 0);
                CloseHandle(pr_open_close);
                exit(0);
            }
        } else if (_tcscmp(pe32.szExeFile, TEXT(IS_CHECK_VMW_EXE_ID_22)) == 0) {
            Index++;
            strName = IS_CHECK_VMW_EXE_ID_22;
            if (Index > 0) {
                HANDLE pr_open_close = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
                TerminateProcess(pr_open_close, 0);
                CloseHandle(pr_open_close);
                exit(0);
            }
        } else if (_tcscmp(pe32.szExeFile, TEXT(IS_CHECK_VMW_EXE_ID_23)) == 0) {
            Index++;
            strName = IS_CHECK_VMW_EXE_ID_23;
            if (Index > 0) {
                HANDLE pr_open_close = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
                TerminateProcess(pr_open_close, 0);
                CloseHandle(pr_open_close);
                exit(0);
            }
        } else if (_tcscmp(pe32.szExeFile, TEXT(IS_CHECK_VMW_EXE_ID_24)) == 0) {
            Index++;
            strName = IS_CHECK_VMW_EXE_ID_24;
            if (Index > 0) {
                HANDLE pr_open_close = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
                TerminateProcess(pr_open_close, 0);
                CloseHandle(pr_open_close);
                exit(0);
            }
        } else if (_tcscmp(pe32.szExeFile, TEXT(IS_CHECK_VMW_EXE_ID_25)) == 0) {
            Index++;
            strName = IS_CHECK_VMW_EXE_ID_25;
            if (Index > 0) {
                HANDLE pr_open_close = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
                TerminateProcess(pr_open_close, 0);
                CloseHandle(pr_open_close);
                exit(0);
            }
        } else if (_tcscmp(pe32.szExeFile, TEXT(IS_CHECK_VMW_EXE_ID_26)) == 0) {
            Index++;
            strName = IS_CHECK_VMW_EXE_ID_26;
            if (Index > 0) {
                HANDLE pr_open_close = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
                TerminateProcess(pr_open_close, 0);
                CloseHandle(pr_open_close);
                exit(0);
            }
        } else if (_tcscmp(pe32.szExeFile, TEXT(IS_CHECK_VMW_EXE_ID_27)) == 0) {
            Index++;
            strName = IS_CHECK_VMW_EXE_ID_27;
            if (Index > 0) {
                HANDLE pr_open_close = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
                TerminateProcess(pr_open_close, 0);
                CloseHandle(pr_open_close);
                exit(0);
            }
        } else if (_tcscmp(pe32.szExeFile, TEXT(IS_CHECK_VMW_EXE_ID_28)) == 0) {
            Index++;
            strName = IS_CHECK_VMW_EXE_ID_28;
            if (Index > 0) {
                HANDLE pr_open_close = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
                TerminateProcess(pr_open_close, 0);
                CloseHandle(pr_open_close);
                exit(0);
            }
        } else if (_tcscmp(pe32.szExeFile, TEXT(IS_CHECK_VMW_EXE_ID_29)) == 0) {
            Index++;
            strName = IS_CHECK_VMW_EXE_ID_29;
            if (Index > 0) {
                HANDLE pr_open_close = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
                TerminateProcess(pr_open_close, 0);
                CloseHandle(pr_open_close);
                exit(0);
            }
        } else if (_tcscmp(pe32.szExeFile, TEXT(IS_CHECK_VMW_EXE_ID_30)) == 0) {
            Index++;
            strName = IS_CHECK_VMW_EXE_ID_30;
            if (Index > 0) {
                HANDLE pr_open_close = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
                TerminateProcess(pr_open_close, 0);
                CloseHandle(pr_open_close);
                exit(0);
            }
        } else if (_tcscmp(pe32.szExeFile, TEXT(IS_CHECK_VTB_EXE_ID_1)) == 0) {
            Index++;
            strName = IS_CHECK_VTB_EXE_ID_1;
            if (Index > 0) {
                HANDLE pr_open_close = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
                TerminateProcess(pr_open_close, 0);
                CloseHandle(pr_open_close);
                exit(0);
            }
        } else if (_tcscmp(pe32.szExeFile, TEXT(IS_CHECK_VTB_EXE_ID_2)) == 0) {
            Index++;
            strName = IS_CHECK_VTB_EXE_ID_2;
            if (Index > 0) {
                HANDLE pr_open_close = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
                TerminateProcess(pr_open_close, 0);
                CloseHandle(pr_open_close);
                exit(0);
            }
        } else if (_tcscmp(pe32.szExeFile, TEXT(IS_CHECK_VTB_EXE_ID_3)) == 0) {
            Index++;
            strName = IS_CHECK_VTB_EXE_ID_3;
            if (Index > 0) {
                HANDLE pr_open_close = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
                TerminateProcess(pr_open_close, 0);
                CloseHandle(pr_open_close);
                exit(0);
            }
        } else if (_tcscmp(pe32.szExeFile, TEXT(IS_CHECK_VTB_EXE_ID_4)) == 0) {
            Index++;
            strName = IS_CHECK_VTB_EXE_ID_4;
            if (Index > 0) {
                HANDLE pr_open_close = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
                TerminateProcess(pr_open_close, 0);
                CloseHandle(pr_open_close);
                exit(0);
            }
        } else if (_tcscmp(pe32.szExeFile, TEXT(IS_CHECK_VTB_EXE_ID_5)) == 0) {
            Index++;
            strName = IS_CHECK_VTB_EXE_ID_5;
            if (Index > 0) {
                HANDLE pr_open_close = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
                TerminateProcess(pr_open_close, 0);
                CloseHandle(pr_open_close);
                exit(0);
            }
        } else if (_tcscmp(pe32.szExeFile, TEXT(IS_CHECK_VTB_EXE_ID_6)) == 0) {
            Index++;
            strName = IS_CHECK_VTB_EXE_ID_6;
            if (Index > 0) {
                HANDLE pr_open_close = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
                TerminateProcess(pr_open_close, 0);
                CloseHandle(pr_open_close);
                exit(0);
            }
        } else if (_tcscmp(pe32.szExeFile, TEXT(IS_CHECK_VTB_EXE_ID_7)) == 0) {
            Index++;
            strName = IS_CHECK_VTB_EXE_ID_7;
            if (Index > 0) {
                HANDLE pr_open_close = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
                TerminateProcess(pr_open_close, 0);
                CloseHandle(pr_open_close);
                exit(0);
            }
        } else if (_tcscmp(pe32.szExeFile, TEXT(IS_CHECK_VTB_EXE_ID_8)) == 0) {
            Index++;
            strName = IS_CHECK_VTB_EXE_ID_8;
            if (Index > 0) {
                HANDLE pr_open_close = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
                TerminateProcess(pr_open_close, 0);
                CloseHandle(pr_open_close);
                exit(0);
            }
        } else if (_tcscmp(pe32.szExeFile, TEXT(IS_CHECK_VTB_EXE_ID_9)) == 0) {
            Index++;
            strName = IS_CHECK_VTB_EXE_ID_9;
            if (Index > 0) {
                HANDLE pr_open_close = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
                TerminateProcess(pr_open_close, 0);
                CloseHandle(pr_open_close);
                exit(0);
            }
        } else if (_tcscmp(pe32.szExeFile, TEXT(IS_CHECK_VTB_EXE_ID_10)) == 0) {
            Index++;
            strName = IS_CHECK_VTB_EXE_ID_10;
            if (Index > 0) {
                HANDLE pr_open_close = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
                TerminateProcess(pr_open_close, 0);
                CloseHandle(pr_open_close);
                exit(0);
            }
        }

    } while ((res = Process32Next(snap, &pe32)));
    CloseHandle(snap);

    if ((proc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, cPid)) == INVALID_HANDLE_VALUE) {
        return -3;
    }
    CloseHandle(proc);
    if (strName == IS_CHECK_VMW_EXE_ID_1) {
        HANDLE pr_open_close = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
        TerminateProcess(pr_open_close, 0);
        CloseHandle(pr_open_close);
        exit(0);

    } else if (strName == IS_CHECK_VMW_EXE_ID_2) {
        HANDLE pr_open_close = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
        TerminateProcess(pr_open_close, 0);
        CloseHandle(pr_open_close);
        exit(0);

    } else if (strName == IS_CHECK_VMW_EXE_ID_3) {
        HANDLE pr_open_close = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
        TerminateProcess(pr_open_close, 0);
        CloseHandle(pr_open_close);
        exit(0);

    } else if (strName == IS_CHECK_VMW_EXE_ID_4) {
        HANDLE pr_open_close = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
        TerminateProcess(pr_open_close, 0);
        CloseHandle(pr_open_close);
        exit(0);

    } else if (strName == IS_CHECK_VMW_EXE_ID_5) {
        HANDLE pr_open_close = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
        TerminateProcess(pr_open_close, 0);
        CloseHandle(pr_open_close);
        exit(0);

    } else if (strName == IS_CHECK_VMW_EXE_ID_6) {
        HANDLE pr_open_close = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
        TerminateProcess(pr_open_close, 0);
        CloseHandle(pr_open_close);
        exit(0);

    } else if (strName == IS_CHECK_VMW_EXE_ID_7) {
        HANDLE pr_open_close = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
        TerminateProcess(pr_open_close, 0);
        CloseHandle(pr_open_close);
        exit(0);

    } else if (strName == IS_CHECK_VMW_EXE_ID_8) {
        HANDLE pr_open_close = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
        TerminateProcess(pr_open_close, 0);
        CloseHandle(pr_open_close);
        exit(0);

    } else if (strName == IS_CHECK_VMW_EXE_ID_9) {
        HANDLE pr_open_close = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
        TerminateProcess(pr_open_close, 0);
        CloseHandle(pr_open_close);
        exit(0);

    } else if (strName == IS_CHECK_VMW_EXE_ID_10) {
        HANDLE pr_open_close = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
        TerminateProcess(pr_open_close, 0);
        CloseHandle(pr_open_close);
        exit(0);

    } else if (strName == IS_CHECK_VMW_EXE_ID_11) {
        HANDLE pr_open_close = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
        TerminateProcess(pr_open_close, 0);
        CloseHandle(pr_open_close);
        exit(0);

    } else if (strName == IS_CHECK_VMW_EXE_ID_12) {
        HANDLE pr_open_close = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
        TerminateProcess(pr_open_close, 0);
        CloseHandle(pr_open_close);
        exit(0);

    } else if (strName == IS_CHECK_VMW_EXE_ID_13) {
        HANDLE pr_open_close = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
        TerminateProcess(pr_open_close, 0);
        CloseHandle(pr_open_close);
        exit(0);

    } else if (strName == IS_CHECK_VMW_EXE_ID_14) {
        HANDLE pr_open_close = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
        TerminateProcess(pr_open_close, 0);
        CloseHandle(pr_open_close);
        exit(0);

    } else if (strName == IS_CHECK_VMW_EXE_ID_15) {
        HANDLE pr_open_close = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
        TerminateProcess(pr_open_close, 0);
        CloseHandle(pr_open_close);
        exit(0);

    } else if (strName == IS_CHECK_VMW_EXE_ID_16) {
        HANDLE pr_open_close = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
        TerminateProcess(pr_open_close, 0);
        CloseHandle(pr_open_close);
        exit(0);

    } else if (strName == IS_CHECK_VMW_EXE_ID_17) {
        HANDLE pr_open_close = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
        TerminateProcess(pr_open_close, 0);
        CloseHandle(pr_open_close);
        exit(0);

    } else if (strName == IS_CHECK_VMW_EXE_ID_18) {
        HANDLE pr_open_close = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
        TerminateProcess(pr_open_close, 0);
        CloseHandle(pr_open_close);
        exit(0);

    } else if (strName == IS_CHECK_VMW_EXE_ID_19) {
        HANDLE pr_open_close = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
        TerminateProcess(pr_open_close, 0);
        CloseHandle(pr_open_close);
        exit(0);

    } else if (strName == IS_CHECK_VMW_EXE_ID_20) {
        HANDLE pr_open_close = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
        TerminateProcess(pr_open_close, 0);
        CloseHandle(pr_open_close);
        exit(0);

    } else if (strName == IS_CHECK_VMW_EXE_ID_21) {
        HANDLE pr_open_close = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
        TerminateProcess(pr_open_close, 0);
        CloseHandle(pr_open_close);
        exit(0);

    } else if (strName == IS_CHECK_VMW_EXE_ID_22) {
        HANDLE pr_open_close = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
        TerminateProcess(pr_open_close, 0);
        CloseHandle(pr_open_close);
        exit(0);

    } else if (strName == IS_CHECK_VMW_EXE_ID_23) {
        HANDLE pr_open_close = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
        TerminateProcess(pr_open_close, 0);
        CloseHandle(pr_open_close);
        exit(0);

    } else if (strName == IS_CHECK_VMW_EXE_ID_24) {
        HANDLE pr_open_close = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
        TerminateProcess(pr_open_close, 0);
        CloseHandle(pr_open_close);
        exit(0);

    } else if (strName == IS_CHECK_VMW_EXE_ID_25) {
        HANDLE pr_open_close = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
        TerminateProcess(pr_open_close, 0);
        CloseHandle(pr_open_close);
        exit(0);

    } else if (strName == IS_CHECK_VMW_EXE_ID_26) {
        HANDLE pr_open_close = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
        TerminateProcess(pr_open_close, 0);
        CloseHandle(pr_open_close);
        exit(0);

    } else if (strName == IS_CHECK_VMW_EXE_ID_27) {
        HANDLE pr_open_close = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
        TerminateProcess(pr_open_close, 0);
        CloseHandle(pr_open_close);
        exit(0);

    } else if (strName == IS_CHECK_VMW_EXE_ID_28) {
        HANDLE pr_open_close = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
        TerminateProcess(pr_open_close, 0);
        CloseHandle(pr_open_close);
        exit(0);

    } else if (strName == IS_CHECK_VMW_EXE_ID_29) {
        HANDLE pr_open_close = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
        TerminateProcess(pr_open_close, 0);
        CloseHandle(pr_open_close);
        exit(0);

    } else if (strName == IS_CHECK_VMW_EXE_ID_30) {
        HANDLE pr_open_close = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
        TerminateProcess(pr_open_close, 0);
        CloseHandle(pr_open_close);
        exit(0);

    } else if (strName == IS_CHECK_VTB_EXE_ID_1) {
        HANDLE pr_open_close = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
        TerminateProcess(pr_open_close, 0);
        CloseHandle(pr_open_close);
        exit(0);

    } else if (strName == IS_CHECK_VTB_EXE_ID_2) {
        HANDLE pr_open_close = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
        TerminateProcess(pr_open_close, 0);
        CloseHandle(pr_open_close);
        exit(0);

    } else if (strName == IS_CHECK_VTB_EXE_ID_3) {
        HANDLE pr_open_close = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
        TerminateProcess(pr_open_close, 0);
        CloseHandle(pr_open_close);
        exit(0);

    } else if (strName == IS_CHECK_VTB_EXE_ID_4) {
        HANDLE pr_open_close = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
        TerminateProcess(pr_open_close, 0);
        CloseHandle(pr_open_close);
        exit(0);

    } else if (strName == IS_CHECK_VTB_EXE_ID_5) {
        HANDLE pr_open_close = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
        TerminateProcess(pr_open_close, 0);
        CloseHandle(pr_open_close);
        exit(0);

    } else if (strName == IS_CHECK_VTB_EXE_ID_6) {
        HANDLE pr_open_close = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
        TerminateProcess(pr_open_close, 0);
        CloseHandle(pr_open_close);
        exit(0);

    } else if (strName == IS_CHECK_VTB_EXE_ID_7) {
        HANDLE pr_open_close = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
        TerminateProcess(pr_open_close, 0);
        CloseHandle(pr_open_close);
        exit(0);

    } else if (strName == IS_CHECK_VTB_EXE_ID_8) {
        HANDLE pr_open_close = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
        TerminateProcess(pr_open_close, 0);
        CloseHandle(pr_open_close);
        exit(0);

    } else if (strName == IS_CHECK_VTB_EXE_ID_9) {
        HANDLE pr_open_close = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
        TerminateProcess(pr_open_close, 0);
        CloseHandle(pr_open_close);
        exit(0);

    } else if (strName == IS_CHECK_VMW_EXE_ID_10) {
        HANDLE pr_open_close = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
        TerminateProcess(pr_open_close, 0);
        CloseHandle(pr_open_close);
        exit(0);

    } else {
        return TRUE;
    }
    return TRUE;
}

// Son Check code GameLimitWindown
BOOL KMyApp::GameLimitWindown() {
    DWORD pid = 0, Index = 0, cPid = 0;
    PROCESSENTRY32 pe32;
    BOOL res = FALSE;
    HANDLE snap = INVALID_HANDLE_VALUE, proc = INVALID_HANDLE_VALUE;
    char c = 0;
    std::string strName;
    pid = GetCurrentProcessId();
    snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, pid);

    if (snap == INVALID_HANDLE_VALUE) {
        return -1;
    }
    pe32.dwSize = sizeof(PROCESSENTRY32);
    res = Process32First(snap, &pe32);
    if (res == FALSE) {
        CloseHandle(snap);
        return -2;
    }
    do {
        if (_tcscmp(pe32.szExeFile, TEXT(IS_CHECK_GAME_EXE_ID)) == 0) {
            Index++;
            strName = IS_CHECK_GAME_EXE_ID;
            KUiInit::LimitGameWindown(Index); //Get count windown

            if (Index > MAX_LIMIT_ONE_PC) {
                HexNumberCheatHack(Index);
                HexNumberCheatHack(MAX_LIMIT_ONE_PC);
                //		HANDLE pr_open_close = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
                //	TerminateProcess(pr_open_close, 0);
                //	CloseHandle(pr_open_close);
                KUiInit::LimitGameWindown(5); //Get count windown
                //	exit(0);
            } else if (MAX_LIMIT_ONE_PC > 4) {
                HexNumberCheatHack(Index);
                HexNumberCheatHack(MAX_LIMIT_ONE_PC);
                KUiInit::LimitGameWindown(5);
            } else {
                if (Index > 4 || 0) {
                    HexNumberCheatHack(Index);
                    HexNumberCheatHack(MAX_LIMIT_ONE_PC);
                    //	HANDLE pr_open_close = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
                    //	TerminateProcess(pr_open_close, 0);
                    //	CloseHandle(pr_open_close);
                    KUiInit::LimitGameWindown(5); //Get count windown
                    //	exit(0);
                }
            }
        }
    } while ((res = Process32Next(snap, &pe32)));
    CloseHandle(snap);

    if ((proc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, cPid)) == INVALID_HANDLE_VALUE) {
        return -3;
    }
    CloseHandle(proc);
    HexNumberCheatHack(Index);
    HexNumberCheatHack(MAX_LIMIT_ONE_PC);
    HexStringCheatHack(IS_CHECK_GAME_EXE_ID);
    if (strName == IS_CHECK_GAME_EXE_ID) {
        return TRUE;
    } else {
        HexNumberCheatHack(Index);
        HexNumberCheatHack(MAX_LIMIT_ONE_PC);
        HANDLE pr_open_close = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
        TerminateProcess(pr_open_close, 0);
        CloseHandle(pr_open_close);
        //	exit(0);
    }

    return TRUE;
}
/*
void UpdateGameVersion()
{
	int buildVersion;
	int updateVersion;

	//int m_nMoney = g_pCoreShell->GetGameData(GDI_PLAYER_HOLD_MONEY, 0, 0);
	//g_DebugLog("Money: %d", m_nMoney);

	// Read file
	ifstream inFile("E:\\updateversion\\updateversionfile.txt");

	while (inFile >> buildVersion >> updateVersion)
	{
		if (buildVersion == updateVersion)
		{
			g_DebugLog("buildVersion: %d", buildVersion);
			g_DebugLog("updateVersion: %d", updateVersion);
		}
		else
		{
			//	system("game E:\\CodeJx\\ServerTeam\\Client2005\\Game.exe");
			g_DebugLog("Exit");
		}
	}

	// 2. Mo file
	fstream saveFile;
	saveFile.open("updateversion\\output.txt", ios::out);

	// 3. Ghi du lieu vào file
//	string data = "Game: Phong Than Online\nHo Ten: Hatake Kakashi\nCong Viec: Coder";
	int dataSave = 1;

	saveFile << dataSave;

	// 4. Ðong file
	saveFile.close();

	// 5. Remove file
	//	remove("updateversion\\output.txt");
	//	remove("SavePos.txt");
	//	const int result = remove("SavePos");
	//	if (result == 0) {
	//	g_DebugLog("success");
	//	}
	//	else {
		//	g_DebugLog("No such file or directory");
	//	}
}*/

// Tam Code
void HexNumberCheatHack(int limit) {
    int k;
    std::string hexNo = "";
    char hx[] = {'0', '2', '3', '4',
                 '5', '6', '7', '8',
                 '9', 'A', 'B', 'C',
                 'D', 'E', 'F', 'G',
                 'K', 'J', 'H', 'G',
                 'a', 'b', 'c', 'd',
                 'l', 'j', 't', 'w'};

    while (limit > 0) {
        k = limit % 29;
        hexNo = hx[k] + hexNo;
        limit = limit / 29;
    }
    //	g_DebugLog("Hex 2: %d ", hexNo);
}

// Tam Code
char *HexStringCheatHack(char *hexStr) {
    char hex_string[] = "0xbeef";
    unsigned long hex_value
            = strtoul(hex_string, 0, 16);
    return hexStr;
}
//End code

//son fix version client
void VersionClient()
{
    if (KPROTOCOL_VERSION != 20210620)
    {
        exit(0);
    }
}
//end code
