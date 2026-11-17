// -------------------------------------------------------------------------
//	文件名		：	UiMsgSel.h
//	创建者		：	Wooy(Wu yue)
//	创建时间	：	2003-1-6
//	功能描述	：	带滚动条的消息选择
// -------------------------------------------------------------------------
#include "KWin32.h"
#include "KIniFile.h"
#include "../Elem/WndMessage.h"
#include "../Elem/WndMessageListBox.h"
#include "../Elem/Wnds.h"
#include "UiMsgSel.h"
#include "../../../../Headers/PhongThanUiLayoutRules.h"
#include "../../../Headers/PhongThanNpcDialogRules.h"
#include "../../../engine/src/KDebug.h"
#include "../../../core/src/coreshell.h"
#include "../../../core/src/GameDataDef.h"
#include "../../../engine/src/kstrbase.h"
#include "../UiBase.h"
#include "../../../Engine/Src/Text.h"
#include "../UiSoundSetting.h"
#include "../../../Represent/iRepresent/iRepresentShell.h"
#include "../../../Represent/iRepresent/KRepresentUnit.h"

extern iCoreShell *g_pCoreShell;
extern iRepresentShell *g_pRepresentShell;

#define    SCHEME_INI    "UiMsgSel.ini"
#define    SCHEME_INIP    "UiMsgSelPic.ini"

void KWndNpcChoiceButton::SetChoiceLabel(const char* text){g_StrCpyLen(label,text,sizeof(label));}
// Tile the original pill's middle; image stretch deliberately preserves aspect
// ratio for item previews and therefore cannot be used for a wider dialog button.
static void PaintNpcButtonSlice(const KUiImageRef& source,int x,int y,int left,int top,int right,int bottom){
    KRUImagePart part;memset(&part,0,sizeof(part));part.nType=ISI_T_SPR;
    part.Color.Color_dw=0xffffffff;part.bRenderStyle=IMAGE_RENDER_STYLE_ALPHA;
    part.bRenderFlag=RUIMAGE_RENDER_FLAG_FRAME_DRAW;part.nISPosition=IMAGE_IS_POSITION_INIT;part.nFrame=source.nFrame;
    g_StrCpyLen(part.szImage,source.szImage,sizeof(part.szImage));
    part.oPosition.nX=x;part.oPosition.nY=y;part.oImgLTPos.nX=left;part.oImgLTPos.nY=top;part.oImgRBPos.nX=right;part.oImgRBPos.nY=bottom;
    g_pRepresentShell->DrawPrimitives(1,&part,RU_T_IMAGE_PART,true);
}
void KWndNpcChoiceButton::PaintWindow(){
    if(!g_pRepresentShell)return;KRPosition2 offset,size;
    if(g_pRepresentShell->GetImageFrameParam(m_Image.szImage,m_Image.nFrame,&offset,&size,ISI_T_SPR)&&size.nX>24&&size.nY>6){
        int cap=12,edge=min((size.nY-1)/2,m_Height/2);
        // Nine slices preserve the original corners at any button size.
        for(int band=0;band<3;++band){int sy=band==0?0:band==1?edge:size.nY-edge;
            int sh=band==1?size.nY-2*edge:edge,dy=band==0?0:band==1?edge:m_Height-edge;
            int dh=band==1?m_Height-2*edge:edge;
            for(int yy=0;yy<dh;yy+=sh){int height=min(sh,dh-yy);
                PaintNpcButtonSlice(m_Image,m_nAbsoluteLeft,m_nAbsoluteTop+dy+yy,0,sy,cap,sy+height);
                for(int xx=cap;xx<m_Width-cap;xx+=size.nX-2*cap){int width=min(size.nX-2*cap,m_Width-cap-xx);
                    PaintNpcButtonSlice(m_Image,m_nAbsoluteLeft+xx,m_nAbsoluteTop+dy+yy,cap,sy,cap+width,sy+height);}
                PaintNpcButtonSlice(m_Image,m_nAbsoluteLeft+m_Width-cap,m_nAbsoluteTop+dy+yy,size.nX-cap,sy,size.nX,sy+height);
            }
        }
    }
    // Use the loaded 14px bitmap font at its native advance. The 11px
    // fallback changed pen advance without scaling the bitmap glyph.
    int length=strlen(label),font=14;
    int capacity=max(1,(m_Width-12)*2/font),starts[32],counts[32],lines=0;
    for(int pos=0;pos<length&&lines<32;){int count=min(capacity,length-pos);
        if(pos+count<length){int space=count;while(space>0&&label[pos+space]!=' ')--space;if(space>0)count=space;}
        starts[lines]=pos;counts[lines++]=count;pos+=count;while(label[pos]==' ')++pos;
    }
    // Labels may occupy two compact lines outside the fixed 18px skin.
    int visible=2,first=0;
    if(lines>visible&&(m_Flag&WNDBTN_F_OVER))first=(GetTickCount()/2500)%(lines-visible+1);
    int shown=min(lines,visible),y=m_nAbsoluteTop+(m_Height-shown*(font+1))/2;
    for(int line=first;line<first+shown;++line){int width=counts[line]*font/2;
        g_pRepresentShell->OutputText(font,label+starts[line],counts[line],m_nAbsoluteLeft+(m_Width-width)/2,y,0xfff1f199,0,TEXT_IN_SINGLE_PLANE_COORD,0xff000000);y+=font+1;}
}
void KWndNpcChoiceScroll::PaintWindow(){
    if(!g_pRepresentShell)return;
    KRUShadow parts[2];memset(parts,0,sizeof(parts));
    parts[0].oPosition.nX=m_nAbsoluteLeft+4;parts[0].oEndPos.nX=m_nAbsoluteLeft+6;
    parts[0].oPosition.nY=m_nAbsoluteTop;parts[0].oEndPos.nY=m_nAbsoluteTop+m_Height;
    parts[0].Color.Color_dw=0x0082712b;
    int top; m_SlideBtn.GetPosition(0,&top);
    parts[1].oPosition.nX=m_nAbsoluteLeft+1;parts[1].oEndPos.nX=m_nAbsoluteLeft+9;
    parts[1].oPosition.nY=m_nAbsoluteTop+top;parts[1].oEndPos.nY=parts[1].oPosition.nY+12;
    parts[1].Color.Color_dw=0x00d4bd79;
    g_pRepresentShell->DrawPrimitives(2,parts,RU_T_SHADOW,true);
}
KUiMsgSel *KUiMsgSel::m_pSelf = NULL;


KUiMsgSel::KUiMsgSel() {
    m_AnswerCount = 0;m_AnswerColumns=3;m_AnswerRows=2;m_ChoiceHeight=28;m_ChoiceTop=128;
    m_AnswerPage = 0;
    m_ButtonPresentation = false;m_EventHandbook=false;
    m_LongAnswers=false;m_QuestionLength=0;
    m_uLeftTime = 0;
    m_bAutoUp = false;
    m_bAutoDown = false;
    m_uLastScrollTime = 0;
    memset(m_szPic, 0, sizeof(m_szPic));
}

KUiMsgSel::~KUiMsgSel() {
}

//--------------------------------------------------------------------------
//	功能：打开窗口，返回唯一的一个类对象实例
//--------------------------------------------------------------------------
KUiMsgSel *KUiMsgSel::OpenWindow(KUiQuestionAndAnswer *pQAA, KUiImage *pPic) {
    if (!pQAA || pQAA->AnswerCount < 0 || pQAA->AnswerCount > MAX_ANSWERNUM)
        return NULL;
    const bool eventBook=pPic && !stricmp(pPic->Name,"\\spr\\pt_test\\book.spr");
    if(m_pSelf && m_pSelf->m_EventHandbook!=eventBook)CloseWindow(true);
    if (m_pSelf == NULL) {
        m_pSelf = new KUiMsgSel;
        if (m_pSelf && !m_pSelf->Initialize(pPic)) {
            delete m_pSelf;
            m_pSelf = NULL;
            return NULL;
        }
    }
    if (m_pSelf) {
        UiSoundPlay(UI_SI_WND_OPENCLOSE);
        m_pSelf->BringToTop();
        m_pSelf->Show(pQAA);
    }
    return m_pSelf;
}

//--------------------------------------------------------------------------
//	功能：关闭销毁窗口
//--------------------------------------------------------------------------
void KUiMsgSel::CloseWindow(bool bDestroy) {
    if (m_pSelf) {
        m_pSelf->m_bAutoUp = false;
        m_pSelf->m_bAutoDown = false;

        Wnd_ReleaseExclusive((KWndWindow *) m_pSelf);
        if (bDestroy) {
            m_pSelf->Destroy();
            m_pSelf = NULL;
        } else
            m_pSelf->Hide();
    }
}


KUiMsgSel *KUiMsgSel::GetIfVisible() {
    if (m_pSelf && m_pSelf->IsVisible())
        return m_pSelf;
    return NULL;
}

//--------------------------------------------------------------------------
//	功能：初始化
//--------------------------------------------------------------------------
int KUiMsgSel::Initialize(KUiImage *pPic) {
    if (pPic) {
        memcpy(&m_pPic, pPic, sizeof(KUiImage));
        m_EventHandbook=!stricmp(m_pPic.Name,"\\spr\\pt_test\\book.spr");
        if (m_pPic.Name[0])
            AddChild(&m_Image);
    } else
        memset(&m_pPic, 0, sizeof(KUiImage));
    AddChild(&m_CloseBtn);
    AddChild(&m_EndDialog);
    AddChild(&m_AnswerPrev); AddChild(&m_AnswerNext);
    for (int answer = 0; answer < MAX_ANSWERNUM; ++answer) {
        AddChild(&m_AnswerButtons[answer]);

    }
    AddChild(&m_MsgScrollList);
    AddChild(&m_MessageScroll);
    AddChild(&m_ChoiceScroll);
    AddChild(&m_MessageList);
    m_MessageList.SetScrollbar(&m_MessageScroll);
    char Scheme[256];
    g_UiBase.GetCurSchemePath(Scheme, 256);
    LoadScheme(Scheme);

    // Do not acquire exclusive input with an uninitialized dialog.
    if (m_Width <= 0 || m_Height <= 0 ||
        m_MsgScrollList.GetMessageListBox()->GetCapability() <= 0 ||
        m_MessageList.GetCapability() <= 0)
        return false;

    m_Style &= ~WND_S_VISIBLE;
    Wnd_AddWindow(this, WL_TOPMOST);
    return true;
}

//--------------------------------------------------------------------------
//	功能：载入窗口的界面方案
//--------------------------------------------------------------------------
void KUiMsgSel::LoadScheme(const char *pScheme) {
    if (m_pSelf == NULL)
        return;
    char Buff[128];
    KIniFile Ini;
    sprintf(Buff, "%s\\%s", pScheme, m_pSelf->m_pPic.Name[0] ? SCHEME_INIP : SCHEME_INI);
    if (Ini.Load(Buff)) {
        // NPC question panels in the reference have no slide transition.
        // Do not inherit stale StartPos from the old green list panel.
        int screenWidth, screenHeight;
        Wnd_GetScreenSize(screenWidth, screenHeight);
        const int left = (screenWidth - 344) / 2;
        const int top = (screenHeight - 230) / 2;
        Ini.WriteInteger("Main", "Left", left);
        Ini.WriteInteger("Main", "Top", top);
        Ini.WriteInteger2("Main", "StartPos", left, top);
        Ini.WriteInteger2("Main", "EndPos", left, top);
        m_pSelf->Init(&Ini, "Main");
        m_pSelf->m_CloseBtn.Init(&Ini, "CloseBtn");
        m_pSelf->m_EndDialog.Init(&Ini,"EndDialog");
        m_pSelf->m_MsgScrollList.Init(&Ini, "Select");
        m_pSelf->m_MessageScroll.Init(&Ini, "MessageScroll");
        m_pSelf->m_MessageList.Init(&Ini, "MessageList");
        Ini.WriteInteger("ChoiceScroll","Left",316);Ini.WriteInteger("ChoiceScroll","Top",90);
        Ini.WriteInteger("ChoiceScroll","Width",10);Ini.WriteInteger("ChoiceScroll","Height",90);
        Ini.WriteInteger("ChoiceScroll","Type",1);Ini.WriteInteger("ChoiceScroll","PageSize",1);
        Ini.WriteInteger("ChoiceScroll","SlideBegin",0);Ini.WriteInteger("ChoiceScroll","SlideEnd",78);
        Ini.WriteInteger("ChoiceScroll_Btn","Width",10);Ini.WriteInteger("ChoiceScroll_Btn","Height",12);
        Ini.WriteInteger("ChoiceScroll","Color",0x0082712b);Ini.WriteInteger("ChoiceScroll_Btn","Color",0x00c5ad63);
        m_pSelf->m_ChoiceScroll.Init(&Ini,"ChoiceScroll");m_pSelf->m_ChoiceScroll.Hide();

        m_pSelf->m_MessageList.SetFontSize(13);
        m_pSelf->m_MsgScrollList.GetMessageListBox()->SetFontSize(13);
        m_pSelf->m_AnswerPrev.Init(&Ini, "AnswerPrev");
        m_pSelf->m_AnswerNext.Init(&Ini, "AnswerNext");
        m_pSelf->m_AnswerPrev.SetSize(40,18);m_pSelf->m_AnswerPrev.SetPosition(21,195);
        m_pSelf->m_AnswerNext.SetSize(40,18);m_pSelf->m_AnswerNext.SetPosition(283,195);
        for (int answer = 0; answer < MAX_ANSWERNUM; ++answer) {
            m_pSelf->m_AnswerButtons[answer].Init(&Ini, "AnswerButton");

            m_pSelf->m_AnswerButtons[answer].Hide();
        }
        m_pSelf->m_MsgScrollList.Hide();
    }
}

//--------------------------------------------------------------------------
//	功能：窗口消息函数
//--------------------------------------------------------------------------
int KUiMsgSel::WndProc(unsigned int uMsg, unsigned int uParam, int nParam) {
    int nRet = 0;
    switch (uMsg) {
        case WND_N_BUTTON_CLICK: {
            if(uParam==(unsigned int)(KWndWindow*)&m_EndDialog){OnClickMsg(-1);return 1;}
            if (uParam == (unsigned int)(KWndWindow*)&m_AnswerPrev) {
                if (m_AnswerPage > 0) --m_AnswerPage;
                RefreshAnswerButtons(); return 1;
            }
            if (uParam == (unsigned int)(KWndWindow*)&m_AnswerNext) {
                if ((m_AnswerPage + 1) * (m_AnswerColumns*m_AnswerRows) < m_AnswerCount) ++m_AnswerPage;
                RefreshAnswerButtons(); return 1;
            }
            for (int answer = 0; answer < m_AnswerCount; ++answer)
                if (uParam == (unsigned int)(KWndWindow*)&m_AnswerButtons[answer]) {
                    OnClickMsg(answer); return 1;
                }
            if (uParam == (unsigned int)(KWndWindow*)&m_CloseBtn) { OnClickMsg(-1); return 1; }
            break;
        }
        case WM_MOUSEMOVE: {
            int x = LOWORD(nParam) - m_nAbsoluteLeft;
            int y = HIWORD(nParam) - m_nAbsoluteTop;
            if (x >= 0 && x <= m_Width &&
                y >= 0 && y <= m_Height) {
                int nMLeft = 0;
                int nMTop = 0;
                m_MsgScrollList.GetMessageListBox()->GetPosition(&nMLeft, &nMTop);
                int nMWidth = 0;
                int nMHeight = 0;
                m_MsgScrollList.GetMessageListBox()->GetSize(&nMWidth, &nMHeight);
                if (x >= nMLeft && x <= nMLeft + nMWidth) {
                    if (y <= nMTop && y >= nMTop - 50)
                        m_bAutoUp = true;
                    else
                        m_bAutoUp = false;

                    if (y >= nMTop + nMHeight && y <= nMTop + nMHeight + 50)
                        m_bAutoDown = true;
                    else
                        m_bAutoDown = false;
                }
            }
        }
            break;
        case WND_N_LIST_ITEM_HIGHLIGHT:
            m_bAutoUp = false;
            m_bAutoDown = false;
            if (nParam >= 0) {
                m_MsgScrollList.GetMessageListBox()->SetCurSel(nParam);
                if (m_pPic.Name[0]) {
                    int nMWidth, nMHeight;
                    if (nParam < MAX_ANSWERNUM && m_szPic[nParam][0]) {
                        m_Image.SetImage(ISI_T_SPR, m_szPic[nParam], true);
                    } else {
                        m_Image.SetImage(ISI_T_SPR, m_pPic.Name, true);
                        m_Image.SetMaxFrame(m_pPic.Frame);
                    }
                    m_Image.GetSize(&nMWidth, &nMHeight);
                    m_Image.SetPosition((m_EventHandbook?285:70) - nMWidth / 2, (m_EventHandbook?132:70) - nMHeight / 2);
                }
            } else {
                ChangeCurSel(true);    //经实际测试在nParam < 0 时是向下
                nRet = 1;
            }
            break;
        case WND_N_LIST_ITEM_ACTIVE:
            m_bAutoUp = false;
            m_bAutoDown = false;
            if (nParam >= 0)
                OnClickMsg(nParam);
            break;
        case WND_N_SCORLLBAR_POS_CHANGED:
            if(uParam==(unsigned int)(KWndWindow*)&m_ChoiceScroll){m_AnswerPage=nParam;RefreshAnswerButtons();return 1;}
            if (uParam == (unsigned int) (KWndWindow *) &m_MessageScroll)
                m_MessageList.SetFirstShowLine(nParam);
            break;
        case WM_MOUSEWHEEL:
            if(m_ButtonPresentation&&m_ChoiceScroll.IsVisible()){
                m_ChoiceScroll.ScrollLine((short)HIWORD(uParam)>0);return 1;
            }
            return KWndShowAnimate::WndProc(uMsg,uParam,nParam);
        case WM_KEYDOWN:
            m_bAutoUp = false;
            m_bAutoDown = false;
            if (uParam == VK_ESCAPE) {
                OnClickMsg(-1);
                return 1;
            } else if (uParam == VK_RETURN) {
                nRet = m_MsgScrollList.GetMessageListBox()->GetCurSel();
                if (nRet >= 0)
                    OnClickMsg(nRet);
                nRet = 1;
            } else if (uParam == VK_UP) {
                ChangeCurSel(false);
                nRet = 1;
            } else if (uParam == VK_DOWN) {
                ChangeCurSel(true);
                nRet = 1;
            }
            break;
        default:
            nRet = KWndShowAnimate::WndProc(uMsg, uParam, nParam);
            break;
    }
    return nRet;
}

//--------------------------------------------------------------------------
//	功能：响应点击按钮
//--------------------------------------------------------------------------
void KUiMsgSel::OnClickMsg(int nMsg) {
    if (m_pSelf) {
        // Visible rows keep the original server/Lua answer index.
        const int selection = nMsg >= 0 && nMsg < m_pSelf->m_AnswerCount ? m_pSelf->m_AnswerMap[nMsg] : -1;
        m_pSelf->CloseWindow(true);
        if (g_pCoreShell) g_pCoreShell->OperationRequest(GOI_QUESTION_CHOOSE, 0, selection);
    }
}

void KUiMsgSel::RefreshAnswerButtons() {
    m_AnswerPrev.Hide();m_AnswerNext.Hide();
    int totalRows=(m_AnswerCount+m_AnswerColumns-1)/m_AnswerColumns;
    int maxScroll=max(0,totalRows-m_AnswerRows);
    m_AnswerPage=max(0,min(m_AnswerPage,maxScroll));
    const int first=m_AnswerPage*m_AnswerColumns;
    const int visibleRows=min(totalRows,m_AnswerRows);
    const int top=186-visibleRows*(m_ChoiceHeight+6)+6;
    const int width=95;
    for(int answer=0;answer<MAX_ANSWERNUM;++answer){
        if(m_ButtonPresentation&&answer<m_AnswerCount&&answer>=first&&answer<first+visibleRows*m_AnswerColumns){
            int slot=answer-first;
            m_AnswerButtons[answer].SetPosition(21+(slot%m_AnswerColumns)*99,top+(slot/m_AnswerColumns)*(m_ChoiceHeight+6));
            m_AnswerButtons[answer].SetSize(width,m_ChoiceHeight);m_AnswerButtons[answer].Show();
        }else m_AnswerButtons[answer].Hide();
    }
    m_ChoiceScroll.SetPosition(319,m_ChoiceTop);m_ChoiceScroll.SetSize(10,186-m_ChoiceTop);
    m_ChoiceScroll.SetValueRange(0,maxScroll);
    m_ChoiceScroll.SetScrollPos(m_AnswerPage);
    if(m_ButtonPresentation&&maxScroll)m_ChoiceScroll.Show();else m_ChoiceScroll.Hide();
}

void KUiMsgSel::ChangeCurSel(bool bNext) {
    int nRet = m_MsgScrollList.GetMessageListBox()->GetCurSel();
    if (bNext) {
        if (nRet < m_MsgScrollList.GetMessageListBox()->GetMsgCount() - 1)
            nRet = m_MsgScrollList.GetMessageListBox()->SetCurSel(nRet + 1);
    } else {
        if (nRet > 0)
            nRet = m_MsgScrollList.GetMessageListBox()->SetCurSel(nRet - 1);
    }
    if(m_ButtonPresentation&&nRet>=0){
        int row=nRet/m_AnswerColumns;
        if(row<m_AnswerPage)m_AnswerPage=row;
        else if(row>=m_AnswerPage+m_AnswerRows)m_AnswerPage=row-m_AnswerRows+1;
        RefreshAnswerButtons();
    }
}

//--------------------------------------------------------------------------
//	功能：显示窗口
//--------------------------------------------------------------------------
void KUiMsgSel::Show(KUiQuestionAndAnswer *pQAA) {
    if (!pQAA || pQAA->AnswerCount < 0 || pQAA->AnswerCount > MAX_ANSWERNUM)
        return;
    {
        m_MsgScrollList.GetMessageListBox()->Clear();
        m_MessageList.Clear();
        // Honor the script presentation; never convert lists based on label length.
        m_ButtonPresentation = pQAA->ButtonPresentation != 0;m_LongAnswers=false;
        // Measure the wrapped prompt before choosing its viewport. Ordinary
        // NPC menus retain the compact layout; only long prompts expand.
        m_MessageList.SetSize(306,42);
        const int promptSkip = pQAA->Question[0] == '#' ? 1 : 0;
        m_QuestionLength=max(0,min((pQAA->QuestionLen<0?(int)strlen(pQAA->Question):pQAA->QuestionLen)-promptSkip,(int)sizeof(m_QuestionText)-1));
        memcpy(m_QuestionText,pQAA->Question+promptSkip,m_QuestionLength);m_QuestionText[m_QuestionLength]=0;
        if(strstr(m_QuestionText,"<color")||strstr(m_QuestionText,"<c="))m_QuestionLength=TEncodeText(m_QuestionText,m_QuestionLength);
        m_MessageList.AddOneMessage(m_QuestionText,m_QuestionLength);
        const int promptLines = m_MessageList.GetItemLineCount(m_MessageList.GetMsgCount());
        const bool expandedPrompt = !m_ButtonPresentation && promptLines > 2;
        int promptHeight=(m_ButtonPresentation || expandedPrompt)?100:42;
        int choiceTop=expandedPrompt?142:82,choiceHeight=expandedPrompt?70:100;
        // Only ordinary text-list dialogs use this adaptive viewport. Button
        // confirmations, portrait panels and the event handbook keep their layout.
        if(!m_ButtonPresentation&&!m_EventHandbook&&!m_pPic.Name[0]){
            int footerLeft,footerTop;m_EndDialog.GetPosition(&footerLeft,&footerTop);
            const PTNpcListLayout layout=PhongThanNpcListLayout(promptLines,footerTop);
            promptHeight=layout.PromptHeight;choiceTop=layout.ChoiceTop;choiceHeight=layout.ChoiceHeight;
            m_MessageScroll.SetPosition(318,40);
            m_MsgScrollList.GetScrollBar()->SetPosition(296,0);
            m_MsgScrollList.GetScrollBar()->SetSize(14,choiceHeight);
        }
        m_MessageList.SetSize(306,promptHeight);
        m_MessageScroll.SetSize(12,promptHeight);
        m_MsgScrollList.SetPosition(20,choiceTop);
        m_MsgScrollList.SetSize(310,choiceHeight);
        m_MsgScrollList.GetMessageListBox()->SetSize(292,choiceHeight);
        if(m_EventHandbook){
            m_MsgScrollList.SetSize(228,100);
            m_MsgScrollList.GetMessageListBox()->SetSize(210,100);
            m_MsgScrollList.GetScrollBar()->SetPosition(214,0);
        }
        m_AnswerCount = 0;m_AnswerColumns=3;m_AnswerRows=2;m_ChoiceHeight=28;m_ChoiceTop=128;
        m_AnswerPage = 0;
        for (int i = 0; i < pQAA->AnswerCount; ++i) {
            if (PhongThanIsDialogExit(pQAA->Answer[i].AnswerText, pQAA->Answer[i].AnswerLen)) continue;
            const int row = m_AnswerCount++;
            m_AnswerMap[row] = i;
            m_szPic[row][0] = 0;
            const char* link = strstr(pQAA->Answer[i].AnswerText, "LINK:");
            int length = pQAA->Answer[i].AnswerLen;
            if (link && m_pPic.Name[0]) {
                g_StrCpyLen(m_szPic[row], link + 5, sizeof(m_szPic[row]));
                length = link - pQAA->Answer[i].AnswerText;
            }
            // Encode raw Lua colour tags before adding a positive-length answer.
            char label[MAX_SENTENCE_LENGTH];
            int labelLength = length < 0 ? strlen(pQAA->Answer[i].AnswerText) : length;
            if (labelLength >= sizeof(label)) labelLength = sizeof(label)-1;
            memcpy(label, pQAA->Answer[i].AnswerText, labelLength);
            label[labelLength]=0;
            if(length<0||strstr(label,"<color")||strstr(label,"<c="))labelLength=TEncodeText(label,labelLength);
            m_MsgScrollList.GetMessageListBox()->AddOneMessage(label,labelLength);
            labelLength = TRemoveCtrlInEncodedText(label, labelLength);
            label[labelLength] = 0;
            g_StrCpyLen(m_FullAnswers[row],label,sizeof(m_FullAnswers[row]));
            if(m_ButtonPresentation){if(labelLength>13)m_AnswerColumns=min(m_AnswerColumns,2);if(labelLength>20)m_AnswerColumns=1;if(labelLength>46)m_LongAnswers=true;}
            // The full legacy sentence is rendered in the scrollable prompt,
            // so the 95px button never loses letters.
            m_AnswerButtons[row].SetChoiceLabel(label);
            m_AnswerButtons[row].SetToolTipInfo(pQAA->Answer[i].AnswerText, length);
        }
    }
    if(m_ButtonPresentation){
        int lines=m_MessageList.GetItemLineCount(m_MessageList.GetMsgCount());
        PTNpcButtonLayout layout=PhongThanNpcButtonLayout(lines,m_AnswerCount);
        int promptHeight=layout.PromptHeight;
        m_ChoiceTop=layout.ChoiceTop;m_ChoiceHeight=28;m_AnswerColumns=3;
        m_AnswerRows=layout.Rows;
        m_MessageList.SetPosition(21,40);m_MessageList.SetSize(306,promptHeight);
        m_MessageScroll.SetPosition(318,40);m_MessageScroll.SetSize(12,promptHeight);
    }
    RefreshAnswerButtons();
    m_EndDialog.Show();
    if (m_ButtonPresentation) m_MsgScrollList.Hide();
    else m_MsgScrollList.Show();
    KWndShowAnimate::Show();
    Wnd_SetExclusive((KWndWindow *) this);

    m_uLeftTime = pQAA->LeftTime;
    m_uLastScrollTime = IR_GetCurrentTime();
    m_bAutoUp = false;
    m_bAutoDown = false;

    if (m_pPic.Name[0]) {
        m_Image.SetImage(ISI_T_SPR, m_pPic.Name, true);
        int nMWidth, nMHeight;
        m_Image.GetSize(&nMWidth, &nMHeight);
        m_Image.SetPosition((m_EventHandbook?285:70) - nMWidth / 2, (m_EventHandbook?132:70) - nMHeight / 2);
        m_Image.SetMaxFrame(m_pPic.Frame);
    }
}

void KUiMsgSel::Breathe() {
    if (m_uLeftTime > 0) {
        m_uLeftTime--;
        if (m_uLeftTime == 0) {
            OnClickMsg(-1);
            return; // CloseWindow destroys this instance.
        }
    }

    if (m_Image.IsVisible())
        m_Image.NextFrame();

    if (m_bAutoUp) {
        if (IR_IsTimePassed(200, m_uLastScrollTime)) {
            ChangeCurSel(false);
            m_uLastScrollTime = IR_GetCurrentTime();
        }
    }

    if (m_bAutoDown) {
        if (IR_IsTimePassed(200, m_uLastScrollTime)) {
            ChangeCurSel(true);
            m_uLastScrollTime = IR_GetCurrentTime();
        }
    }
}
