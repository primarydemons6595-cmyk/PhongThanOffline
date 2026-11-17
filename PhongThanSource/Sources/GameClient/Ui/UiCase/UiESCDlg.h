// -------------------------------------------------------------------------
//	�ļ���		��	UiESCDlg.h
//	������		��	������
//	����ʱ��	��	2002-9-16 10:32:22
//	��������	��
//
// -------------------------------------------------------------------------
#ifndef __UIESCDLG_H__
#define __UIESCDLG_H__

#include "../Elem/WndLabeledButton.h"
#include "../Elem/WndShowAnimate.h"

class KUiESCDlg : protected KWndShowAnimate {
public:
    //----�������ͳһ�Ľӿں���----
    static KUiESCDlg *OpenWindow();        //�򿪴��ڣ�����Ψһ��һ�������ʵ��
    static KUiESCDlg *GetIfVisible();
    static void OpenExitWindow();
    static void OfflineResult(int result,unsigned int sequence);

    static void CloseWindow(bool bDestroy);        //�رմ���
private:
    KUiESCDlg() {}

    ~KUiESCDlg();

    void PaintWindow();
    void Show();

    void Hide();

    int Initialize();                        //��ʼ��
    void LoadScheme(const char *pScheme);    //������淽��
    int WndProc(unsigned int uMsg, unsigned int uParam, int nParam);//���ں���
    void OnClickButton(KWndWindow *pWnd);    //��Ӧ�����ť
    int OnKeyDown(unsigned int uKey);

    KWndButton *GetActiveBtn();

private:
    KWndLabeledButton m_ExitGameBtn;            // exit game
    KWndLabeledButton m_HelpBtn;
    KWndLabeledButton m_SelectPlayerBtn;
    KWndLabeledButton m_OptionsBtn;
    KWndLabeledButton m_ContinueGameBtn;        // continue game
    KWndLabeledButton m_SaveGameBtn;            // offline: manual save
    KWndImage m_ESCBg;

    static KUiESCDlg *m_pSelf;
};

#endif // __UIESCDLG_H__
