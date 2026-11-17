#pragma once
#include "../Elem/WndImage.h"
#include "../Elem/WndButton.h"
#include "../Elem/WndText.h"
#include "../Elem/WndScrollBar.h"
#include "../Elem/WndEdit.h"
#include "../../../core/src/gamedatadef.h"

// Existing callers use this handle for the Phong Than native auto panel.
class KUiAutoPlay : public KWndImage {
public:
    static KUiAutoPlay* OpenWindow(bool show=true);
    static KUiAutoPlay* GetIfVisible();
    static void CloseWindow(bool destroy);
    static int SavePrivateSetting();
    static BOOL LoadPrivateSetting();
    static bool SelectPetProfile(int slot);static int CurrentPetProfile();
    static bool SavePetProfile();static void LoadPetProfiles();
private:
    KUiAutoPlay(){Hide();}
    static KUiAutoPlay* m_pSelf;
};
