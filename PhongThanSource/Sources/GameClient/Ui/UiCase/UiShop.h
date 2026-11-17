// -------------------------------------------------------------------------
//	�ļ���		��	UiTrade.h
//	������		��	Wooy(Wu yue)
//	����ʱ��	��	2002-12-21
//	��������	��	npc����ϵͳ����
// -------------------------------------------------------------------------
#pragma once

#include "../Elem/WndShowAnimate.h"
#include "../elem/wndbutton.h"
#include "../Elem/WndText.h"
#include "../elem/wndObjContainer.h"
#include "../../../Core/Src/GameDataDef.h"
#include "UiPtShopSlot.h"
#include "../../../../Headers/PhongThanEventShop.h"

struct KUiObjAtContRegion;

class KUiShop : protected KWndShowAnimate {
public:
    //----�������ͳһ�Ľӿں���----
    static KUiShop *OpenWindow();                //�򿪴��ڣ�����Ψһ��һ�������ʵ��
    static KUiShop *GetIfVisible();                //�������������ʾ���򷵻�ʵ��ָ��
    static void CloseWindow();                //�رմ��ڣ�ͬʱ����ѡ���Ƿ�ɾ������ʵ��
    static void LoadScheme(const char *pScheme);    //������淽��
    static void CancelTrade();

    static void OpenQuestShop();
    void UpdateData();

    void UpdateItem(KUiObjAtContRegion *pItem, int bAdd);

    int WndProc(unsigned int uMsg, unsigned int uParam, int nParam);//���ں���
private:
    KUiShop();

    ~KUiShop() {}

    void SetPage(int nIndex);

    void Clear();

    void OnClickButton(KWndButton *pWnd, int bCheck);

    void Breathe();

    void OnBuyItem(KUiDraggedObject *pItem, bool bDoImmed);

    void Initialize();                        //��ʼ��
private:
    static KUiShop *m_pSelf;
private:
    KUiObjAtContRegion *m_pObjsList;
    int m_nObjCount;
    int m_nPageCount;
    int m_nCurrentPage;
    KWndObjectMatrix m_ItemsBox;        //��Ʒ��
    bool m_bQuestShop;PTEventShopData m_QuestData;
    class QuestIcon : public KWndButton {public:unsigned goods;void SetOffer(const PTEventShopRow&);int WndProc(unsigned,unsigned,int);} m_QuestIcons[14];
    void InitQuestLayout(KIniFile*);void UpdateQuestShop(bool);void QuestBuy(unsigned);
    bool m_bPtLayout;
    KUiPtShopSlot m_PtSlots[14];
    KWndText80 m_PtNames[14];
    KWndText80 m_PtPrices[14];
    void InitPtLayout(KIniFile* ini);
    void SetPtPage(int page);
    KWndButton m_BuyBtn;
    KWndButton m_SellBtn;
    KWndButton m_RepairBtn;
    KWndButton m_RepairAllBtn;
    KWndButton m_PreBtn;
    KWndButton m_NextBtn;
    KWndButton m_CloseBtn;
};
