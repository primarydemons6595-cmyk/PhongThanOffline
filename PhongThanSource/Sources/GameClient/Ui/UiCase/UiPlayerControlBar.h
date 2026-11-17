#include "UiPtBuffIcon.h"
#include "../Elem/WndImage.h"
#include "../Elem/WndButton.h"
#include "../Elem/WndPureTextBtn.h"
#include "KTabFile.h"

#define MAX_BUFF_ROLE            96
#define PT_BUFF_BANK_SIZE         48
#define MAX_ITEMBUTTON            120
#define    HIDE_POS                -300,0
#define    SHOW_POS                15,-2

struct KStateControl;

class KUiPlayerControlBar : KWndImage {
public:

    static KUiPlayerControlBar *OpenWindow();

    static KUiPlayerControlBar *GetIfVisible();

    static void CloseWindow(bool bDestory = TRUE);

    static void LoadScheme(const char *pScheme);

    static void SetItemBtnInfo(int nBtnNo, const char *pItem);

    static void ClearItemBtn();

    static void SetItemBtnFont(int number,int font);
    static void SetItemBtnPos(int nBtnNo, int X, int Y);

    static int GetNumberItem(int num);

private:
    KUiPlayerControlBar();

    ~KUiPlayerControlBar();

private:
    static KUiPlayerControlBar *m_pSelf;

private:
    void Initialize();

    void Breathe();
    void LoadPowderScheme(const char* scheme);
    void BreathePowders();
    void ShowStatusIcon(int slot, const char* image, const char* title,
                        const char* desc, unsigned long seconds, bool permanent);

    void Add(int nSkillPos, KStateControl *pStateSkill);

    void Release();

private:
    KPtBuffIcon m_BuffImage[MAX_BUFF_ROLE];
    KWndText32 m_txtBuffTime[MAX_BUFF_ROLE];

    KIniFile Ini;
    KIniFile m_PtPowderIni;
    KIniFile m_PtTimedItemsIni;
    KIniFile m_PtSkillBuffIni;
    bool m_bPtPowderScheme;
    int m_BuffListCount;
    int m_nListCount;
    KStateControl *m_pStateSkill;

    KWndPureTextBtn m_ItemBtn[MAX_ITEMBUTTON];
};
