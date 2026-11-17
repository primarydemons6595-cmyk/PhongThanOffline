/*****************************************************************************************
//	界面--储物箱界面
//	Copyright : Kingsoft 2003
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2003-4-21
------------------------------------------------------------------------------------------
*****************************************************************************************/
#pragma once

#include "../elem/WndButton.h"
#include "../elem/WndLabeledButton.h"
#include "../elem/WndText.h"
#include "../elem/WndObjContainer.h"
#include "../Elem/WndShowAnimate.h"
#include "../../../core/src/GameDataDef.h"

struct PTBankSecurityState;
struct KUiObjAtRegion;

class KUiStoreBox : protected KWndShowAnimate {
public:
    static void BankPasswordResponse(const PTBankSecurityState&);
    static KUiStoreBox *OpenWindow(int nToPage);        //打开窗口，返回唯一的一个类对象实例
    static KUiStoreBox *GetIfVisible();        //如果窗口正被显示，则返回实例指针
    static void CloseWindow();        //关闭窗口
    static void LoadScheme(const char *pScheme);//载入界面方案
    bool PutInventoryItem(KUiDraggedObject* item, UIOBJECT_CONTAINER container);
    void UpdateItem(int nToPage, KUiObjAtRegion *pItem, int bAdd);//物品变化更新
private:
    KUiStoreBox() {}

    ~KUiStoreBox() {}

    void Initialize();                            //初始化
    int WndProc(unsigned int uMsg, unsigned int uParam, int nParam);//窗口函数
    void UpdateData(int nCurPage);

    void OnItemPickDrop(ITEM_PICKDROP_PLACE *pPickPos, ITEM_PICKDROP_PLACE *pDropPos);

    void OnClickItem(KUiDraggedObject *pItem);

    void OnGetMoney(int nMoney);

    void PaintWindow();
    bool PageAccessible() const;
    UIOBJECT_CONTAINER GetPageContainer() const;

private:
    static KUiStoreBox *m_pSelf;
private:
    int m_nRowOffset;
    KWndLabeledButton m_RowUp,m_RowDown;
    KWndButton m_LockBtn;
    int m_nMoney;
    KWndText32 m_Money;
    KWndButton m_GetMoneyBtn,m_SaveMoneyBtn,m_SortBtn;
    KWndButton m_RentBtn;
    KWndText256 m_RentInfo;
    KWndText32 m_TimeLeft;
    KWndImage m_RentImage;
    KWndButton m_CloseBtn;
    KWndObjectMatrix m_ItemBox;
    KWndText32 m_CurPage;
    KWndButton m_PreBtn;
    KWndButton m_NextBtn;
    KWndButton m_PageBtn[5];

    int m_nExpandBoxNum;
    int m_nCurPage;
};
