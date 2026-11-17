//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KWin32Wnd.h.cpp
// Date:	2000.08.08
// Code:	WangWei(Daphnis)
// Desc:	Window Functions
//---------------------------------------------------------------------------
#include "KWin32.h"
#include "KDebug.h"
#include "KDDraw.h"
#include "KWin32Wnd.h"
#include "KCanvas.h"
//---------------------------------------------------------------------------
static HWND	g_hWndMain = NULL;
static HWND	g_hWndDraw = NULL;
//---------------------------------------------------------------------------
// 函数:	GetMainHWnd
// 功能:	取得主窗口句柄
// 参数:	void
// 返回:	主窗口句柄(有可能为NULL)
//---------------------------------------------------------------------------
HWND g_GetMainHWnd(void)
{
	return g_hWndMain;
}
//---------------------------------------------------------------------------
// 函数:	SetMainHWnd
// 功能:	设置主窗口句柄
// 参数:	hWnd	主窗口句柄
// 返回:	void
//---------------------------------------------------------------------------
void g_SetMainHWnd(HWND hWnd)
{
	g_hWndMain = hWnd;
}
//---------------------------------------------------------------------------
// 函数:	GetDrawHWnd
// 功能:	取得绘图窗口句柄
// 参数:	void
// 返回:	窗口句柄(有可能为NULL)
//---------------------------------------------------------------------------
HWND g_GetDrawHWnd(void)
{
	return g_hWndDraw;
}
//---------------------------------------------------------------------------
// 函数:	SetDrawHWnd
// 功能:	设置绘图窗口句柄
// 参数:	hWnd	窗口句柄
// 返回:	void
//---------------------------------------------------------------------------
void g_SetDrawHWnd(HWND hWnd)
{
	g_hWndDraw = hWnd;
}
//---------------------------------------------------------------------------
// 函数:	Get Client Rect
// 功能:	取得窗口客户坐标矩形
// 参数:	lpRect	矩形区域
// 返回:	void
//---------------------------------------------------------------------------
void g_GetClientRect(LPRECT lpRect)
{
	if (g_pDirectDraw->GetScreenMode() == FULLSCREEN)
	{
		lpRect->left = 0;
		lpRect->top = 0;
		lpRect->right = g_pDirectDraw->GetScreenWidth();
		lpRect->bottom = g_pDirectDraw->GetScreenHeight();
	}
	else
	{
		GetClientRect(g_hWndDraw, lpRect);
	}
}
//---------------------------------------------------------------------------
// 函数:	Client To Screen
// 功能:	客户坐标－屏幕坐标
// 参数:	lpRect
// 返回:	void
//---------------------------------------------------------------------------
void g_ClientToScreen(LPRECT lpRect)
{
	if (g_pDirectDraw->GetScreenMode() == WINDOWMODE)
	{
        RECT client; GetClientRect(g_hWndDraw, &client);
        if (g_pCanvas && g_pCanvas->GetWidth() && g_pCanvas->GetHeight()) {
            lpRect->left = MulDiv(lpRect->left, client.right, g_pCanvas->GetWidth());
            lpRect->right = MulDiv(lpRect->right, client.right, g_pCanvas->GetWidth());
            lpRect->top = MulDiv(lpRect->top, client.bottom, g_pCanvas->GetHeight());
            lpRect->bottom = MulDiv(lpRect->bottom, client.bottom, g_pCanvas->GetHeight());
        }
        ClientToScreen(g_hWndDraw, (LPPOINT)lpRect);
		ClientToScreen(g_hWndDraw, (LPPOINT)lpRect + 1);
	}
}
//---------------------------------------------------------------------------
// 函数:	Screen To Client
// 功能:	屏幕坐标－客户坐标
// 参数:	lpRect
// 返回:	void
//---------------------------------------------------------------------------
void g_ScreenToClient(LPRECT lpRect)
{
	if (g_pDirectDraw->GetScreenMode() == WINDOWMODE)
	{
		ScreenToClient(g_hWndDraw, (LPPOINT)lpRect);
        ScreenToClient(g_hWndDraw, (LPPOINT)lpRect + 1);
        RECT client; GetClientRect(g_hWndDraw, &client);
        if (g_pCanvas && client.right && client.bottom) {
            lpRect->left = MulDiv(lpRect->left, g_pCanvas->GetWidth(), client.right);
            lpRect->right = MulDiv(lpRect->right, g_pCanvas->GetWidth(), client.right);
            lpRect->top = MulDiv(lpRect->top, g_pCanvas->GetHeight(), client.bottom);
            lpRect->bottom = MulDiv(lpRect->bottom, g_pCanvas->GetHeight(), client.bottom);
        }
	}
}
//---------------------------------------------------------------------------

void g_ScreenToClient(LPPOINT lpPoint)
{
    if (g_pDirectDraw->GetScreenMode() == WINDOWMODE) {
        ScreenToClient(g_hWndDraw, lpPoint);
        RECT client; GetClientRect(g_hWndDraw, &client);
        if (g_pCanvas && client.right && client.bottom) {
            lpPoint->x = MulDiv(lpPoint->x, g_pCanvas->GetWidth(), client.right);
            lpPoint->y = MulDiv(lpPoint->y, g_pCanvas->GetHeight(), client.bottom);
        }
    }
}
