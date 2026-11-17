// Host lifecycle for Phong Than offline mode: decides whether the mode is
// on, starts the embedded server (character store + KSOServer) lazily on
// the first login, pumps it once per client frame, and releases it when
// the client exits.
//
// Path discipline: Engine.dll keeps ONE process-wide root/file path, and
// parts of the server read plain relative paths (ServerCfg.ini,
// MapSleep.ini, settings\phongthan\ServerPerformance.ini) from the CWD.
// While server code runs, the guard below points CWD + Engine paths at
// the ServerData tree; the client's own values are restored immediately
// afterwards. Everything runs on the client main thread, and both sides
// set their file path right before use, so the interleave is safe.
#include "KWin32.h"
#include "PtEmbedded.h"

#include "KFilePath.h"
#include "KSOServer.h"
#include "../../MultiServer/Goddess/PhongThanCharacterStore.h"

#include <stdio.h>

extern KSwordOnLineSever g_SOServer;

// Globals that GameServer.cpp (the standalone entry point, not part of
// this build) normally provides for KSOServer.cpp.
int g_nPort = 0;
bool g_PhongThanShutdownSaved = true;

static bool g_bPtEnabledChecked = false;
static bool g_bPtEnabled = false;
static bool g_bPtHostStarted = false;
static bool g_bPtHostFailed = false;
static char g_szServerDataDir[MAX_PATH] = "";
static char g_szExeDir[MAX_PATH] = "";

static void PtResolveDirs()
{
	if (g_szExeDir[0])
		return;
	GetModuleFileName(NULL, g_szExeDir, MAX_PATH);
	char* pSlash = strrchr(g_szExeDir, '\\');
	if (pSlash)
		*pSlash = 0;
	_snprintf(g_szServerDataDir, MAX_PATH - 1, "%s\\ServerData", g_szExeDir);
	g_szServerDataDir[MAX_PATH - 1] = 0;
}

BOOL PtEmbedded_IsEnabled()
{
	if (!g_bPtEnabledChecked)
	{
		g_bPtEnabledChecked = true;
		char szEnv[8] = "";
		if (GetEnvironmentVariableA("PHONGTHAN_OFFLINE", szEnv, sizeof(szEnv)) &&
			szEnv[0] == '1')
		{
			g_bPtEnabled = true;
		}
		else
		{
			PtResolveDirs();
			char szConfig[MAX_PATH];
			_snprintf(szConfig, MAX_PATH - 1, "%s\\Config.ini", g_szExeDir);
			szConfig[MAX_PATH - 1] = 0;
			g_bPtEnabled = GetPrivateProfileIntA("Offline", "Enabled", 0,
				szConfig) == 1;
		}
	}
	return g_bPtEnabled;
}

const char* PtEmbedded_GetServerDataDir()
{
	PtResolveDirs();
	return g_szServerDataDir;
}

// ---------------------------------------------------------------------------
// RAII guard swapping CWD + Engine root/file path to the ServerData tree.
// ---------------------------------------------------------------------------
struct CPtServerPathGuard
{
	char szOldCwd[MAX_PATH];
	char szOldRoot[MAX_PATH];
	char szOldFile[MAX_PATH];

	void Apply()
	{
		GetCurrentDirectoryA(MAX_PATH, szOldCwd);
		g_GetRootPath(szOldRoot);
		g_GetFilePath(szOldFile);
		SetCurrentDirectoryA(PtEmbedded_GetServerDataDir());
		g_SetRootPath(NULL); // root := CWD = ServerData
		g_SetFilePath("\\");
	}
	void Restore()
	{
		SetCurrentDirectoryA(szOldCwd);
		g_SetRootPath(szOldRoot);
		g_SetFilePath(szOldFile);
	}
};

BOOL PtEmbedded_EnsureHostStarted()
{
	if (g_bPtHostStarted)
		return TRUE;
	if (g_bPtHostFailed)
		return FALSE;

	PtResolveDirs();

	char szStoreRoot[MAX_PATH];
	_snprintf(szStoreRoot, MAX_PATH - 1, "%s\\CharacterStore",
		g_szServerDataDir);
	szStoreRoot[MAX_PATH - 1] = 0;
	if (!g_PhongThanCharacterStore.Open(szStoreRoot))
	{
		g_bPtHostFailed = true;
		return FALSE;
	}

	g_bPtEmbeddedMode = TRUE;

	CPtServerPathGuard guard;
	guard.Apply();
	const BOOL bOk = g_SOServer.Init();
	guard.Restore();

	if (!bOk)
	{
		g_bPtHostFailed = true;
		return FALSE;
	}
	g_bPtHostStarted = true;
	return TRUE;
}

void PtEmbedded_Pump()
{
	if (!PtEmbedded_IsEnabled())
		return;

	// 1) Session requests first: a select/create may inject an attach
	//    packet that the server must see in this same frame.
	PtEmbedded_GetGateway()->PumpIn();

	// 2) One server breath. KSOServer::Breathe() throttles itself to the
	//    game FPS through its own KTimer, like the standalone main loop.
	if (g_bPtHostStarted)
	{
		CPtServerPathGuard guard;
		guard.Apply();
		g_SOServer.Breathe();
		guard.Restore();
	}

	// 3) Attach permits produced by the breath become ENTER_WORLD replies.
	PtEmbedded_GetGateway()->PumpOut();
}

void PtEmbedded_Shutdown()
{
	if (!g_bPtHostStarted)
		return;
	CPtServerPathGuard guard;
	guard.Apply();
	g_SOServer.Release(); // saves every playing character on the way out
	guard.Restore();
	g_bPtHostStarted = false;
}
