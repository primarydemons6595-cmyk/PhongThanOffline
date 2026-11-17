// Bishop.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Application.h"
#include "Macro.h"

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	CBishopApp app( hInstance );

	int nRet = app.Run();

	return nRet;
}
