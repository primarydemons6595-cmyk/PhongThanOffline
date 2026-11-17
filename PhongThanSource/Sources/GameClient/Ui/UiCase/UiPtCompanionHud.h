#pragma once
#include "../Elem/WndImage.h"
#include "../Elem/WndImagePart.h"
#include "../Elem/WndText.h"
#include "../Elem/WndButton.h"
#include "UiPortraitMask.h"
class KPtHudPanel:public KWndImage {
public:int WndProc(unsigned int msg,unsigned int param,int value){if(msg==WND_N_BUTTON_CLICK&&m_pParentWnd)return m_pParentWnd->WndProc(msg,param,value);return KWndImage::WndProc(msg,param,value);}
};
class KPtCompanionHud:public KWndImage {
 KPtHudPanel m_Panels[2];KWndButton m_Portrait[2];KWndText80 m_Name[2],m_Values[4],m_Level;
 KWndImagePart m_Bars[4];KWndButton m_AI;
 char m_AITips[2][1024];
public:
 void Initialize(const char* scheme);void Update();
 int WndProc(unsigned int,unsigned int,int);
};
