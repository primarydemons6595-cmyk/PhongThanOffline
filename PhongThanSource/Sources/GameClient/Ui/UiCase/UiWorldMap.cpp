/*
 * File:     UiWorldMap.cpp
 * Desc:     世界地图
 * Author:   flying
 * Creation: 2003/7/22
 */
//-----------------------------------------------------------------------------
#include "KWin32.h"
#include "KIniFile.h"
#include "KStrBase.h"
#include "../elem/wnds.h"
#include "../Elem/WndMessage.h"
#include <crtdbg.h>
//#include "../ShortcutKey.h"
#include "UiWorldMap.h"
#include "../UiBase.h"
#include "../UiSoundSetting.h"
#include "Text.h"
#include "../../../Represent/iRepresent/iRepresentShell.h"
#include "../../../Represent/iRepresent/KRepresentUnit.h"
#include "../../../core/src/CoreShell.h"
#include "../../../core/src/GameDataDef.h"
#include "../../../Engine/src/KDebug.h"

extern iRepresentShell *g_pRepresentShell;
extern iCoreShell *g_pCoreShell;

KUiWorldmap *KUiWorldmap::m_pSelf = NULL;

#define        SCHEME_INI        "UiWorldMap.ini"

void MapToggleStatus();

KUiWorldmap *KUiWorldmap::OpenWindow() {
    if (m_pSelf == NULL) {
        m_pSelf = new KUiWorldmap;
        if (m_pSelf)
            m_pSelf->Initialize();
    }

    if (m_pSelf) {
        UiSoundPlay(UI_SI_WND_OPENCLOSE);
        m_pSelf->UpdateData();
        m_pSelf->Show();
        m_pSelf->BringToTop();
        Wnd_SetExclusive(m_pSelf);
    }
    return m_pSelf;
}

void KUiWorldmap::CloseWindow() {
    if (m_pSelf) {
        Wnd_ReleaseExclusive(m_pSelf);
        m_pSelf->Destroy();
        m_pSelf = NULL;
        MapToggleStatus();
    }
}

KUiWorldmap *KUiWorldmap::GetIfVisible() {
    if (m_pSelf && m_pSelf->IsVisible())
        return m_pSelf;
    else
        return NULL;
}

//初始化
void KUiWorldmap::Initialize() {
    AddChild(&m_Sign);
    AddChild(&m_City);
    AddChild(&m_Capital);
    AddChild(&m_Cave);
    AddChild(&m_Battlefield);
    AddChild(&m_Field);
    AddChild(&m_Others);
    AddChild(&m_Country);
    AddChild(&m_Tong);

    char Buff[128];
    g_UiBase.GetCurSchemePath(Buff, sizeof(Buff));
    strcat(Buff, "\\"SCHEME_INI);
    KIniFile Ini;
    if (Ini.Load(Buff)) {
        Init(&Ini, "Main");
        Ini.GetInteger("Destinations","Count",0,&m_DestinationCount);
        if(m_DestinationCount<0||m_DestinationCount>32)m_DestinationCount=0;
        for(int i=0;i<m_DestinationCount;++i){char section[40];sprintf(section,"Destination_%d",i);
            AddChild(&m_Destinations[i]);m_Destinations[i].Init(&Ini,section);
            Ini.GetString(section,"MapImage","",m_MapImages[i],sizeof(m_MapImages[i]));
            Ini.GetString(section,"MapTitle","",m_MapTitles[i],sizeof(m_MapTitles[i]));
            if(m_MapTitles[i][0]=='$')memmove(m_MapTitles[i],m_MapTitles[i]+1,strlen(m_MapTitles[i]));
        }
        AddChild(&m_Back);m_Back.Init(&Ini,"ReturnWorld");
        AddChild(&m_MapTitle);m_MapTitle.Init(&Ini,"RegionTitle");
        char scheme[128];g_UiBase.GetCurSchemePath(scheme,sizeof(scheme));
        strcat(scheme,"\\UiMiniMapBig.ini");KIniFile mini;
        if(mini.Load(scheme)){
            mini.GetString("MiniMap","Image","",m_RegionFrame,sizeof(m_RegionFrame));
            const char* sections[]={"BtnUp","BtnDown","BtnLeft","BtnRight"};
            for(int arrow=0;arrow<4;++arrow){
                AddChild(&m_RegionArrows[arrow]);m_RegionArrows[arrow].Init(&mini,sections[arrow]);
                int x,y;m_RegionArrows[arrow].GetPosition(&x,&y);m_RegionArrows[arrow].SetPosition(x+52,y+65);
            }
        }
        SelectMap(-1);

        m_Sign.Init(&Ini, "Sign");
        m_City.Init(&Ini, "City");
        m_Capital.Init(&Ini, "Capital");
        m_Cave.Init(&Ini, "Cave");
        m_Battlefield.Init(&Ini, "Battlefield");
        m_Field.Init(&Ini, "Field");
        m_Others.Init(&Ini, "Others");
        m_Country.Init(&Ini, "Country");
        m_Tong.Init(&Ini, "Tong");
    }

    Wnd_AddWindow(this, WL_TOPMOST);
    return;
}

void KUiWorldmap::SelectMap(int index){
    if(index>=m_DestinationCount||index< -1)return;
    if(index>=0){KImageParam image;memset(&image,0,sizeof(image));
        if(!g_pRepresentShell||!g_pRepresentShell->GetImageParam(m_MapImages[index],&image,ISI_T_SPR))return;}
    m_SelectedMap=index;m_ScrollX=m_ScrollY=0;
    if(index>=0){KImageParam info;memset(&info,0,sizeof(info));g_pRepresentShell->GetImageParam(m_MapImages[index],&info,ISI_T_SPR);m_ScrollX=(info.nWidth-640)/2;m_ScrollY=(info.nHeight-380)/2;ScrollRegion(0,0);}
    for(int arrow=0;arrow<4;++arrow){if(index<0)m_RegionArrows[arrow].Hide();else m_RegionArrows[arrow].Show();}
    m_Back.SetPosition(index<0?24:76,index<0?20:480);
    m_MapTitle.SetPosition(index<0?200:200,index<0?32:66);
    for(int i=0;i<m_DestinationCount;++i){if(index<0)m_Destinations[i].Show();else m_Destinations[i].Hide();}
    if(index<0){m_Back.Hide();m_MapTitle.Hide();}
    else {m_Back.Show();m_MapTitle.SetText(m_MapTitles[index]);m_MapTitle.Show();}
}
void KUiWorldmap::ScrollRegion(int dx,int dy){
    if(m_SelectedMap<0||!g_pRepresentShell)return;
    KImageParam info;memset(&info,0,sizeof(info));
    if(!g_pRepresentShell->GetImageParam(m_MapImages[m_SelectedMap],&info,ISI_T_SPR))return;
    m_ScrollX+=dx;m_ScrollY+=dy;
    int maxX=info.nWidth>640?info.nWidth-640:0,maxY=info.nHeight>380?info.nHeight-380:0;
    if(m_ScrollX<0)m_ScrollX=0;if(m_ScrollX>maxX)m_ScrollX=maxX;
    if(m_ScrollY<0)m_ScrollY=0;if(m_ScrollY>maxY)m_ScrollY=maxY;
}
int KUiWorldmap::WndProc(unsigned int msg,unsigned int param,int data){
    if(m_SelectedMap>=0){
        if(msg==WND_N_BUTTON_CLICK||msg==WND_N_BUTTON_HOLD){
            for(int a=0;a<4;++a)if(param==(unsigned int)&m_RegionArrows[a]){
                ScrollRegion(a==2?-24:a==3?24:0,a==0?-24:a==1?24:0);return 1;
            }
        }
        if(msg==WM_KEYDOWN){int dx=param==VK_LEFT?-24:param==VK_RIGHT?24:0,dy=param==VK_UP?-24:param==VK_DOWN?24:0;
            if(dx||dy){ScrollRegion(dx,dy);return 1;}}
    }
    if(msg==WND_N_BUTTON_CLICK){
        if(param==(unsigned int)&m_Back){SelectMap(-1);return 1;}
        for(int i=0;i<m_DestinationCount;++i)if(param==(unsigned int)&m_Destinations[i]){SelectMap(i);return 1;}
    }
    if(msg==WM_RBUTTONDOWN||(msg==WM_KEYDOWN&&param==VK_ESCAPE)){
        if(m_SelectedMap>=0)SelectMap(-1);else CloseWindow();return 1;
    }
    // A click on parchment is not a close command or a teleport request.
    if(msg==WM_LBUTTONDOWN)return 1;
    return KWndWindow::WndProc(msg,param,data);
}

void KUiWorldmap::UpdateData() {
    m_Sign.Hide();m_City.Hide();m_Capital.Hide();m_Cave.Hide();
    m_Battlefield.Hide();m_Field.Hide();m_Others.Hide();m_Country.Hide();m_Tong.Hide();
    // The original PT map already includes its labels. Legacy JX coordinates
    // and WorldSet's SwordOnline image must not overwrite this native artwork.
    int sw,sh,w,h;Wnd_GetScreenSize(sw,sh);GetSize(&w,&h);
    SetPosition((sw-w)/2,(sh-h)/2);
}

void KUiWorldmap::PaintWindow(){
    if(m_SelectedMap<0||m_SelectedMap>=m_DestinationCount){KWndImage::PaintWindow();return;}
    if(!g_pRepresentShell)return;
    KImageParam info;memset(&info,0,sizeof(info));
    if(!g_pRepresentShell->GetImageParam(m_MapImages[m_SelectedMap],&info,ISI_T_SPR))return;
    // Reuse the expanded minimap frame and its 640x380 viewport, at native scale.
    KRUImage frame;memset(&frame,0,sizeof(frame));frame.nType=ISI_T_SPR;
    frame.nISPosition=IMAGE_IS_POSITION_INIT;frame.bRenderStyle=IMAGE_RENDER_STYLE_ALPHA;frame.Color.Color_b.a=255;
    g_StrCpyLen(frame.szImage,m_RegionFrame,sizeof(frame.szImage));
    frame.oPosition.nX=m_nAbsoluteLeft+52;frame.oPosition.nY=m_nAbsoluteTop+65;
    g_pRepresentShell->DrawPrimitives(1,&frame,RU_T_IMAGE,true);
    KRUShadow background;memset(&background,0,sizeof(background));
    background.oPosition.nX=m_nAbsoluteLeft+82;background.oPosition.nY=m_nAbsoluteTop+95;
    background.oEndPos.nX=background.oPosition.nX+640;background.oEndPos.nY=background.oPosition.nY+380;
    // RU_T_SHADOW uses inverse opacity 0..32. Zero is opaque; 255 corrupts MMX blending.
    background.Color.Color_dw=0x00000000;
    g_pRepresentShell->DrawPrimitives(1,&background,RU_T_SHADOW,true);
    KRUImagePart image;memset(&image,0,sizeof(image));image.nType=ISI_T_SPR;image.nISPosition=IMAGE_IS_POSITION_INIT;
    image.bRenderStyle=IMAGE_RENDER_STYLE_ALPHA;image.Color.Color_b.a=255;
    g_StrCpyLen(image.szImage,m_MapImages[m_SelectedMap],sizeof(image.szImage));
    int width=info.nWidth<640?info.nWidth:640,height=info.nHeight<380?info.nHeight:380;
    image.oPosition.nX=m_nAbsoluteLeft+82+(640-width)/2;
    image.oPosition.nY=m_nAbsoluteTop+95+(380-height)/2;
    image.oImgLTPos.nX=m_ScrollX;image.oImgLTPos.nY=m_ScrollY;
    image.oImgRBPos.nX=m_ScrollX+width;image.oImgRBPos.nY=m_ScrollY+height;
    g_pRepresentShell->DrawPrimitives(1,&image,RU_T_IMAGE_PART,true);
}
