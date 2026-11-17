
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define FALSE 0
#define GOI_INFORMATION_CONFIRM_NOTIFY 1
struct KUiInformationParam {char sInformation[80];int nInforLen,bNeedConfirmNotify;};
struct KWndShowAnimate {bool visible;void Hide(){visible=false;}};
struct Text {int lines,top;int GetLineCount(){return lines;}void SetTopLine(int n){top=n;}};
struct Core {int confirms;void OperationRequest(int,int,int){++confirms;}} core;
Core* g_pCoreShell=&core;
void Wnd_ReleaseExclusive(void*){}
struct KWndWindow {};
struct KUiInformation2:KWndShowAnimate {
 KUiInformationParam* m_pWordDataList;int m_nNumWordData,m_nCurrentWord,m_ViewTop,m_bNeedConfirmNotify;
 Text m_Information;char current[80];
 KUiInformation2(){memset(this,0,sizeof(*this));}
 bool IsVisible(){return visible;}
 void Show(const char* s,int,int confirm){strcpy(current,s);visible=true;m_ViewTop=0;m_Information.top=0;m_Information.lines=1;m_bNeedConfirmNotify=confirm;}
 void SpeakWords(KUiInformationParam*,int);void Hide();
};
void KUiInformation2::SpeakWords(KUiInformationParam *pWordDataList, int nCount) {
    if (pWordDataList == NULL || nCount <= 0)
        return;
    KUiInformationParam *pNewList = (KUiInformationParam *) malloc(
            sizeof(KUiInformationParam) * (nCount + m_nNumWordData - m_nCurrentWord));
    if (pNewList == NULL)
        return;
    const int pending=m_nNumWordData-m_nCurrentWord;
    if(pending>0)memcpy(pNewList,m_pWordDataList+m_nCurrentWord,sizeof(KUiInformationParam)*pending);
    memcpy(pNewList+pending,pWordDataList,sizeof(KUiInformationParam)*nCount);
    m_nNumWordData=pending+nCount;m_nCurrentWord=0;

    if (m_pWordDataList)
        free(m_pWordDataList);
    m_pWordDataList = pNewList;

    if (IsVisible() == false) {
        Show(m_pWordDataList->sInformation, m_pWordDataList->nInforLen, m_pWordDataList->bNeedConfirmNotify);
        m_nCurrentWord++;
    }
}
void KUiInformation2::Hide() {
    // Continue inside the existing frame before acknowledging the server page.
    if(m_ViewTop+8<m_Information.GetLineCount()){m_ViewTop+=8;m_Information.SetTopLine(m_ViewTop);return;}
    if (m_bNeedConfirmNotify) {
        g_pCoreShell->OperationRequest(GOI_INFORMATION_CONFIRM_NOTIFY, 0, 0);
        m_bNeedConfirmNotify = FALSE;
    }
    Wnd_ReleaseExclusive((KWndWindow *) this);
    if (m_nCurrentWord < m_nNumWordData) {
        Show(m_pWordDataList[m_nCurrentWord].sInformation,
             m_pWordDataList[m_nCurrentWord].nInforLen, m_pWordDataList[m_nCurrentWord].bNeedConfirmNotify);
        m_nCurrentWord++;
    } else {
        KWndShowAnimate::Hide();
    }
    if (m_pWordDataList && m_nCurrentWord >= m_nNumWordData) {
        m_nNumWordData = 0;
        m_nCurrentWord = 0;
        free(m_pWordDataList);
        m_pWordDataList = NULL;
    }
}
#define CHECK(c) do{if(!(c)){printf("FAIL dialog queue line %d: %s\n",__LINE__,#c);return 1;}}while(0)
int main(){
 KUiInformation2 ui;KUiInformationParam batch[3];memset(batch,0,sizeof(batch));
 strcpy(batch[0].sInformation,"A");strcpy(batch[1].sInformation,"B");strcpy(batch[2].sInformation,"C");
 ui.SpeakWords(batch,3);CHECK(!strcmp(ui.current,"A"));
 KUiInformationParam next;memset(&next,0,sizeof(next));strcpy(next.sInformation,"D");ui.SpeakWords(&next,1);
 CHECK(!strcmp(ui.current,"A"));ui.Hide();CHECK(!strcmp(ui.current,"B"));ui.Hide();CHECK(!strcmp(ui.current,"C"));ui.Hide();CHECK(!strcmp(ui.current,"D"));
 CHECK(ui.m_pWordDataList==0);ui.Hide();CHECK(!ui.visible);
 strcpy(next.sInformation,"Long page");next.bNeedConfirmNotify=1;ui.SpeakWords(&next,1);ui.m_Information.lines=20;
 ui.Hide();CHECK(ui.visible&&ui.m_ViewTop==8&&core.confirms==0);
 ui.Hide();CHECK(ui.visible&&ui.m_ViewTop==16&&core.confirms==0);
 ui.Hide();CHECK(!ui.visible&&core.confirms==1);
 ui.Hide();CHECK(core.confirms==1);
 puts("PASS production dialog queue append order, retained pending pages, pagination and single confirmation");return 0;
}
