#include "KWin32.h"
#include "KIniFile.h"
#include "../Elem/WndMessage.h"
#include "../Elem/PopupMenu.h"
#include "../Elem/Wnds.h"
#include "UiAutoPlay.h"
#include "../../../core/src/coreshell.h"
#include "../UiBase.h"
#include "../../../Represent/iRepresent/iRepresentShell.h"
extern iRepresentShell* g_pRepresentShell;
extern iCoreShell* g_pCoreShell;
#include "PhongThanAutoUi.inl"
#include "UiPtPetProfiles.inl"

KUiAutoPlay* KUiAutoPlay::m_pSelf=0;
KUiAutoPlay* KUiAutoPlay::OpenWindow(bool show){
    if(!m_pSelf)m_pSelf=new KUiAutoPlay;
    PtAutoGet();
    if(show)PtAutoSettings();else s_PtAutoPanel->Hide();
    return m_pSelf;
}
KUiAutoPlay* KUiAutoPlay::GetIfVisible(){
    if(!s_PtAutoPanel || !s_PtAutoPanel->IsVisible())return 0;
    if(!m_pSelf)m_pSelf=new KUiAutoPlay;
    return m_pSelf;
}
void KUiAutoPlay::CloseWindow(bool destroy){
    if(s_PtAutoPanel){
        if(destroy){s_PtAutoPanel->Destroy();s_PtAutoPanel=0;}
        else if(s_PtAutoPanel->IsVisible())s_PtAutoPanel->CancelEdit();
    }
    if(destroy && m_pSelf){delete m_pSelf;m_pSelf=0;}
}
int KUiAutoPlay::SavePrivateSetting(){
    SavePetProfile();
    if(s_PtAutoPanel && g_pCoreShell){
        KIniFile* file=g_UiBase.GetAutoSettingFile();
        if(file){file->Clear();s_PtAutoPanel->Save(file);g_UiBase.CloseAutoSettingFile(true);}
    }
    return TRUE;
}
BOOL KUiAutoPlay::LoadPrivateSetting(){
    if(g_pCoreShell){
        KIniFile* file=g_UiBase.GetAutoSettingFile();
        if(file){PtAutoGet()->Load(file);g_UiBase.CloseAutoSettingFile(false);}
        LoadPetProfiles();
    }
    return TRUE;
}
