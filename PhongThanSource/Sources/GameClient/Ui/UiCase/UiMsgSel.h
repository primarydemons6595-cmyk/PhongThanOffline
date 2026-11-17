#ifndef __UiMsgSel_H__
#define __UiMsgSel_H__

#include "../Elem/WndMessageListBox.h"
#include "../Elem/WndShowAnimate.h"
#include "../Elem/WndScrollBar.h"
#include "../Elem/WndText.h"
#include "../Elem/WndButton.h"
#include "../Elem/WndLabeledButton.h"

struct KUiQuestionAndAnswer;
struct KUiImage;

// Only NPC choice buttons use this stretchable, fully labelled control.
class KWndNpcChoiceButton : public KWndLabeledButton {
public:
    char label[MAX_SENTENCE_LENGTH];
    KWndNpcChoiceButton(){label[0]=0;}
    void SetChoiceLabel(const char* text);
    virtual void PaintWindow();
};

class KWndNpcChoiceScroll : public KWndScrollBar {
public:virtual void PaintWindow();
};

class KUiMsgSel : protected KWndShowAnimate {
public:
    //----界面面板统一的接口函数----
    static KUiMsgSel *
    OpenWindow(KUiQuestionAndAnswer *pQAA, KUiImage *pPic = NULL);        //打开窗口，返回唯一的一个类对象实例
    static KUiMsgSel *GetIfVisible();

    static void LoadScheme(const char *pScheme);    //载入界面方案
    static void CloseWindow(bool bDestroy);        //关闭窗口
    static void OnClickMsg(int nMsg);            //响应点击消息
private:
    KUiMsgSel();

    ~KUiMsgSel();

    void Show(KUiQuestionAndAnswer *pQAA);

    int Initialize(KUiImage *pPic);                                //初始化
    int WndProc(unsigned int uMsg, unsigned int uParam, int nParam);

    void ChangeCurSel(bool bNext);

    virtual void Breathe();


private:
    static KUiMsgSel *m_pSelf;

    KScrollMessageListBox m_MsgScrollList;    //备选文字和滚动条
    KWndNpcChoiceScroll m_ChoiceScroll;
    KWndScrollBar m_MessageScroll;    //备选文字和滚动条
    KWndMessageListBox m_MessageList;    //说明文字
    KWndImage m_Image;
    KWndButton m_CloseBtn;

    //为自动滚动加的变量
    int m_AnswerCount;
    int m_AnswerMap[MAX_ANSWERNUM];
    int m_AnswerPage;
    bool m_ButtonPresentation;
    bool m_EventHandbook;
    bool m_LongAnswers;
    int m_AnswerColumns,m_AnswerRows,m_ChoiceHeight,m_ChoiceTop;
    int m_QuestionLength;
    char m_QuestionText[MAX_SENTENCE_LENGTH];
    char m_FullAnswers[MAX_ANSWERNUM][MAX_SENTENCE_LENGTH];
    KWndNpcChoiceButton m_AnswerButtons[MAX_ANSWERNUM];
    KWndLabeledButton m_AnswerPrev, m_AnswerNext, m_EndDialog;
    void RefreshAnswerButtons();
    unsigned int m_uLeftTime;
    bool m_bAutoUp;
    bool m_bAutoDown;
    unsigned int m_uLastScrollTime;
    KUiImage m_pPic;
    char m_szPic[MAX_ANSWERNUM][MAX_PATH];
};


#endif // __UiMsgSel_H__
