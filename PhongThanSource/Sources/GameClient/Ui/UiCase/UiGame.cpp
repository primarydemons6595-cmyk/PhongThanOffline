/*****************************************************************************************
//	�[�򴰿ڵĽӿ�
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-9-18
------------------------------------------------------------------------------------------
	�������������ҡ��¾�������9.18ȫ����������
*****************************************************************************************/
#include "KWin32.h"

#include "Windows.h"
#include "../Elem/WndMessage.h"
#include "../../../core/Src/CoreShell.h"
#include "../../../core/Src/GameDataDef.h"
#include "UiGame.h"
#include "UiMiniMap.h"
#include "memory.h"
#include "../UiBase.h"
#include "UiPlayerShop.h"
#include "UiChatCentre.h"
#include "UiFaceSelector.h"
#include "UiPhongThanNation.h"

#include "UiPlayerBar.h"
#include "../UiChatPhrase.h"
#include "../Elem/PopupMenu.h"
#include "../../../Engine/src/Text.h"
#include "../../../Engine/src/KDebug.h"

#include "../../../core/Src/PhongThanTradeInvite.h"

#include "../Elem/Wnds.h"
#include "../ShortcutKey.h"
#include "UiPhongThanNation.inl"

#include "CommCtrl.h"

extern iCoreShell *g_pCoreShell;
extern KUiChatPhrase g_UiChatPhrase;

KUiGameSpace g_WndGameSpace;

#define SEL_ACTION_MENU 1
#define SEL_EMOTE_MENU 2
#define PREFIX_SUBWORLD 16
#include "UiPlayerTarget.inl"
#include "UiPtPlayerServices.inl"
#include "UiRemotePeople.inl"
#include "UiPlayerContextCompat.inl"
static int s_PtPeopleAction=-1;
void PhongThanArmPeopleAction(int action){
    s_PtPeopleAction=action;
    Wnd_SwitchCursor(action<0?MOUSE_CURSOR_NORMAL:MOUSE_CURSOR_DIALOG);
}
static bool PhongThanHandleArmedAction(unsigned int msg,int coordinates){
    if(s_PtPeopleAction<0)return false;
    if(msg==WM_RBUTTONDOWN){PhongThanArmPeopleAction(-1);return true;}
    if(msg!=WM_LBUTTONDOWN)return false;
    KUiPlayerItem player;memset(&player,0,sizeof(player));int kind=-1;
    if(!g_pCoreShell||g_UiBase.GetStatus()!=UIS_S_IDLE||g_pCoreShell->CheckMouse())return true;
    if(!g_pCoreShell->FindSelectNPC((short)LOWORD(coordinates),(short)HIWORD(coordinates),
        relation_none|relation_ally|relation_enemy,false,&player,kind,true)||kind!=kind_player)return true;
    int action=s_PtPeopleAction;PhongThanArmPeopleAction(-1);
    if(action==14){if(player.nIndex>0&&g_pCoreShell->GetTradeState(player.nIndex,0))KUiPlayerShop::OpenWindow(&player);}
    else PhongThanProcessPlayerMenuAction(&player,action);
    return true;
}


//--------------------------------------------------------------------------
//	���ܣ����봦����Ϣ�Լ�һЩ�ض�������Ϣ����Ӧ
//--------------------------------------------------------------------------
int KUiGameSpace::WndProc(unsigned int uMsg, unsigned int uParam, int nParam) {
    if(uMsg==WM_LBUTTONDOWN||uMsg==WM_LBUTTONDBLCLK)KUiMiniMap::CancelCrossRoute();
    if(PhongThanHandleArmedAction(uMsg,nParam))return 0;
    if (PhongThanHandlePlayerContextChord(uMsg, uParam, nParam)) return 0;
    if(uMsg==WM_LBUTTONDOWN && g_pCoreShell && g_UiBase.GetStatus()==UIS_S_IDLE && !g_pCoreShell->CheckMouse()) {
        KUiPlayerItem selected;ZeroMemory(&selected,sizeof(selected));int selectedKind=-1;
        if(g_pCoreShell->FindSelectNPC((short)LOWORD(nParam),(short)HIWORD(nParam),relation_all,false,&selected,selectedKind,true) &&
           selectedKind==kind_player && selected.uId && selected.nIndex>0 && !KUiPlayerBar::IsSelfName(selected.Name))
            g_pCoreShell->OperationRequest(PT_PLAYER_TARGET_SELECT,selected.nIndex,(int)selected.uId);
    }
    bool bDefault = true;
    int nKind = -1;

    switch (uMsg) {
        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN: {
            //TamLTM Dieu khien theo auto chuot - ko click mouse
            if (g_WndGameSpace.m_bHoldCursor)
                SwitchHoldCursor();

            int nModifier = 0;
            if (uParam & MK_CONTROL)
                nModifier |= HOTKEYF_CONTROL;

            if (uParam & MK_SHIFT)
                nModifier |= HOTKEYF_SHIFT;

            if (GetKeyState(VK_MENU) & 0x8000)
                nModifier |= HOTKEYF_ALT;

            if (KShortcutKeyCentre::HandleMouseInput(uMsg == WM_LBUTTONDOWN ? VK_LBUTTON : VK_RBUTTON, nModifier,
                                                     LOWORD(nParam), HIWORD(nParam)))
                bDefault = false;
        }
            break;
        case WM_LBUTTONUP:
        case WM_RBUTTONUP:
            if (g_pCoreShell && g_pCoreShell->PAIOperation(GPI_ISACTIVE, NULL, NULL, NULL))
                g_pCoreShell->PAIOperation(GPI_PRIORITY_USE_MOUSE, FALSE, NULL, NULL);
            break;
        case WM_LBUTTONDBLCLK:
        case WM_RBUTTONDBLCLK: {
            int nModifier = 0;
            if (uParam & MK_CONTROL)
                nModifier |= HOTKEYF_CONTROL;

            if (uParam & MK_SHIFT)
                nModifier |= HOTKEYF_SHIFT;

            if (GetKeyState(VK_MENU) & 0x8000)
                nModifier |= HOTKEYF_ALT;

            if (KShortcutKeyCentre::HandleMouseInput(uMsg == WM_LBUTTONDBLCLK ? VK_LDBUTTON : VK_RDBUTTON, nModifier,
                                                     LOWORD(nParam), HIWORD(nParam)))
                bDefault = false;
        }
            break;
        case WND_M_MENUITEM_SELECTED:
            if (uParam == (unsigned int) (KWndWindow *) this) {
                if (HIWORD(nParam) == SEL_ACTION_MENU && short(LOWORD(nParam)) >= 0) {
                    PhongThanProcessPlayerMenuAction(&m_SelectPlayer, LOWORD(nParam));
                    bDefault = false;
                }
                if (HIWORD(nParam) == SEL_EMOTE_MENU && short(LOWORD(nParam)) >= 0) {
                    ProcessEmote(m_SelectPlayer.Name, "", LOWORD(nParam));
                    bDefault = false;
                }
            }
            break;
        case WM_MOUSEMOVE:
        case WM_MOUSEHOVER:
            if (uParam & MK_LBUTTON || g_WndGameSpace.m_bHoldCursor) {
                if (g_pCoreShell)
                    g_pCoreShell->GotoWhere(LOWORD(nParam), HIWORD(nParam), 0);
            } else if (uParam & MK_RBUTTON) {
                KShortcutKeyCentre::HandleMouseInput(0, VK_RBUTTON, LOWORD(nParam), HIWORD(nParam));
//			g_DebugLog("HandleMouseInput x %d y %d", LOWORD(nParam), HIWORD(nParam));
            }

            OnMouseMoveCursor(LOWORD(nParam), HIWORD(nParam));
            bDefault = false;
            break;
        case WM_MOUSEWHEEL:
            if (g_pCoreShell) {
                int zDelta = short(HIWORD(uParam));
                int nPos = (-zDelta / WHEEL_DELTA);
                if (nPos > 0)
                    g_pCoreShell->Turn(0);
                else if (nPos < 0)
                    g_pCoreShell->Turn(1);
            }
            break;
        default:
            break;
    }

    if (bDefault && g_pCoreShell)
        g_pCoreShell->ProcessInput(uMsg, uParam, nParam);

    return 0;
}

//--------------------------------------------------------------------------
//	���ܣ�������Ϸ����
//--------------------------------------------------------------------------
void KUiGameSpace::PaintWindow() {
    g_pCoreShell->DrawGameSpace();
    PhongThanUpdatePlayerTarget();
}

void KUiGameSpace::OnMouseMoveCursor(int x, int y) {
    if (g_pCoreShell == NULL)
        return;

    KUiPlayerItem SelectPlayer;
    int nKind = -1;
    int nObjectIdx = -1;
    if (g_pCoreShell->FindSelectNPC(x, y, relation_all, true, &SelectPlayer, nKind)) {
        if (g_pCoreShell->GetTradeState(SelectPlayer.nIndex, 0)) {
            if (GetKeyState(VK_CONTROL) & 0x8000)
                Wnd_SwitchCursor(MOUSE_CURSOR_TRADE);
        } else if (nKind == kind_dialoger)
            Wnd_SwitchCursor(MOUSE_CURSOR_DIALOG);
        else if (g_pCoreShell->GetNPCRelation(SelectPlayer.nIndex) == relation_enemy)
            Wnd_SwitchCursor(MOUSE_CURSOR_FIGHT);
        else
            Wnd_SwitchCursor(MOUSE_CURSOR_NORMAL);
    } else if (g_pCoreShell->FindSelectObject(x, y, true, nObjectIdx, nKind)) {
        if (nKind == Obj_Kind_MapObj)
            Wnd_SwitchCursor(MOUSE_CURSOR_DIALOG);
        else if (nKind == Obj_Kind_Item || nKind == Obj_Kind_Money)
            Wnd_SwitchCursor(MOUSE_CURSOR_PICK);
        else if (nKind == Obj_Kind_Prop)
            Wnd_SwitchCursor(MOUSE_CURSOR_USE);
        else if (nKind == Obj_Kind_Box)
            Wnd_SwitchCursor(CURSOR_POINT_TO_OBJ_NPC);
    } else if (g_WndGameSpace.m_bHoldCursor)
        Wnd_SwitchCursor(MOUSE_CURSOR_FINGER);
    else
        Wnd_SwitchCursor(MOUSE_CURSOR_NORMAL);
}

void AddBlackList(const char *strName, const char *strGroup);

//TamLTM add vao nhom
void ProcessPeople(KUiPlayerItem *pDest, int nAction) {
    if (pDest == NULL || pDest->Name[0] == 0)
        return;
    switch (nAction) {
        case 15:PhongThanServicePeople(*pDest,PT_PS_SEND_MAIL);break;
        case 16:PhongThanServicePeople(*pDest,PT_PS_POSITION);break;
        case 17:PhongThanServicePeople(*pDest,PT_PS_INVITE_PUPIL);break;
        case 19:PhongThanServicePeople(*pDest,PT_PS_REPORT);break;
        case 18:{KUiChatCentre::OpenWindow(false);int unit=KUiChatCentre::AddAddinUnit("B�n th��ng",0);if(unit>=0)KUiChatCentre::AddFriendInfo(unit,pDest->Name,"");break;}

        case 14: // Clear the current target, including the portrait/HP panel.
            if(g_pCoreShell)g_pCoreShell->OperationRequest(PT_PLAYER_TARGET_SELECT,0,0);
            break;
        case ACTION_JOINTEAM:    //Xin Gia nhap nhom to doi Ҫ��nIndex
            if (g_pCoreShell && pDest->nData == PLAYER_MENU_STATE_TEAMOPEN && pDest->nIndex != -1)
                g_pCoreShell->ApplyAddTeam(pDest);
            break;
        case ACTION_TRADE:    //Ҫ��nIndex
            if (g_pCoreShell && PhongThanCanReceiveTradeInvite(pDest->nData) && pDest->nIndex > 0) {
                g_pCoreShell->TradeApplyStart(pDest);
            }
            break;
        case ACTION_TRADE_FOLKGAME:
            if (g_pCoreShell && PhongThanCanReceiveTradeInvite(pDest->nData) && pDest->nIndex > 0)
                g_pCoreShell->TradeApplyStart(pDest, true);
            break;
        case ACTION_CONTEXT_FACE:
            KUiPlayerBar::OpenContextFaceSelector(pDest->Name);
            break;
        case ACTION_CHAT:        //Ҫ��Name
            if (pDest->Name[0])
            {
                KUiMsgCentrePad::OpenWhisperConversation();
                int recent=KUiPlayerBar::AddRecentPlayer(pDest->Name);
                KUiPlayerBar::InputNameMsg(false,pDest->Name,true);
                if(recent>=0)KUiPlayerBar::SetCurrentChannel(recent);
            }
            break;
        case ACTION_MAKEFRIEND:    //Ҫ��Name
            if (g_pCoreShell && !KUiChatCentre::IsMyFriend(pDest->Name))
                g_pCoreShell->OperationRequest(GOI_CHAT_FRIEND_ADD, (unsigned int) pDest, 0);
            break;
        case ACTION_INVITETEAM:            //Moi vao to doi Ҫ��uId
            if (g_pCoreShell && pDest->uId != 0) {
                KUiPlayerTeam TeamInfo;
                TeamInfo.cNumMember = 0;
                g_pCoreShell->TeamOperation(TEAM_OI_GD_INFO, (unsigned int) &TeamInfo, 0);
                if (TeamInfo.cNumMember == 0)
                    g_pCoreShell->TeamOperation(TEAM_OI_CREATE, 0, 0);
                g_pCoreShell->TeamOperation(TEAM_OI_INVITE, (unsigned int) pDest, 0);
            }
            break;
        case ACTION_FOLLOW:    //Ҫ��nIndex
            if (g_pCoreShell && pDest->nIndex != -1)
                g_pCoreShell->OperationRequest(GOI_FOLLOW_SOMEONE, (unsigned int) pDest, 0);
            break;
        case ACTION_VIEWITEM:            //Ҫ��uId
            if (g_pCoreShell && pDest->uId != 0)
                g_pCoreShell->OperationRequest(GOI_VIEW_PLAYERITEM, (unsigned int) pDest->uId, 0);
            break;
        case ACTION_BLACKLIST:    //Ҫ��Name
            AddBlackList(pDest->Name, "");
            break;
        case ACTION_TONG:
        case ACTION_JOINTONG:
        case ACTION_JOINTONG_REPLY:
            PhongThanNationOpenGuide();
            break;
        case ACTION_REVENGE:            //Ҫ��uId
            if (g_pCoreShell && pDest->uId != 0)
                g_pCoreShell->OperationRequest(GOI_REVENGE_SOMEONE, (unsigned int) pDest, 0);
            break;
    }
}

void ProcessEmote(char *szDest, char *szDestChannel, int nEmoteIndex) {
    if (g_pCoreShell && szDest) {
        KUiPlayerBaseInfo MyInfo;
        char szBuff[256];
        int nStringLen, nChannelIndex, nDestChannel;

        nDestChannel = KUiMsgCentrePad::GetChannelIndex(szDestChannel);
        if (nDestChannel == -1) {
            nChannelIndex = KUiPlayerBar::GetCurChannel();
        } else {
            nChannelIndex = nDestChannel;
        }
        if (nChannelIndex != -1) {
            int nChannelCount = KUiMsgCentrePad::GetChannelCount();
//			int nChannelID    = KUiMsgCentrePad::GetChannelID(nChannelIndex);

            int nChannelID = -1;
            int nChannelDataCount = KUiMsgCentrePad::GetChannelCount();
            for (int n = 0; n < nChannelDataCount; n++) {
                if (KUiMsgCentrePad::IsChannelType(n, KUiMsgCentrePad::ch_Screen)) {
                    nChannelID = KUiMsgCentrePad::GetChannelID(n);
                    break;
                }
            }
            if (nChannelID != -1) {
                //��ʼ��ȡ��ת���ִ�
                g_pCoreShell->GetGameData(GDI_PLAYER_BASE_INFO, (unsigned int) &MyInfo, 0);
                if (szDest[0])
                    nStringLen = g_UiChatPhrase.GetEmote(nEmoteIndex, szBuff, sizeof(szBuff));
                else
                    nStringLen = g_UiChatPhrase.GetEmote(nEmoteIndex, szBuff, sizeof(szBuff), 1);
                //��ȡ��ת���ִ�����

                //����ȥ
                if (KUiPlayerBar::IsCanSendMessage(szBuff, nStringLen, szDest, nChannelID)) {
                    char szBuffConverted[256];
                    nStringLen = KUiFaceSelector::ConvertFaceText(szBuffConverted, szBuff, nStringLen);
                    nStringLen = g_UiChatPhrase.ConvertEmoteString(szBuffConverted, nStringLen, MyInfo.Name, szDest);
                    if (nChannelIndex >= nChannelCount) {
                        if (szDest[0]) {
                            KUiPlayerBar::OnSendSomeoneMessage(szDest, szBuffConverted, nStringLen);
                            int nIndex = KUiPlayerBar::AddRecentPlayer(szDest);
                            KUiPlayerBar::SetCurrentChannel(nIndex);
                        }
                    } else {
                        KUiPlayerBar::OnSendChannelMessage(nChannelID, szBuffConverted, nStringLen);
                    }
                }
            }
        }
    }
}

//TamLTM xem thong tin va tin tuc to doi nhap doi nguoi choi
char g_ActionName[][32] =
        {
                "T�n g�u",
                "H�o h�u",
                "Giao d�ch",
                "Nh�p ��i",
                "T� ��i",
                "Theo sau",
                "Tin t�c",
                "S� �en",
                "Bang h�i",
                "Xin nh�p bang",
                "M�i nh�p bang",
                "C�u s�t",
        };

bool IsInBlackName(char *strName);

extern IInlinePicEngineSink* g_pIInlinePicSink;
static void PhongThanContextLabel(KPopupMenuItem& row,int action) {
    const char* name=PhongThanPeopleActionName(action);int offset=0;
    int resource=action==ACTION_CHAT?2:action==ACTION_CONTEXT_FACE?4:action==ACTION_VIEWITEM?7:action==ACTION_INVITETEAM?0:action==ACTION_MAKEFRIEND?9:action==19?13:-1;
    if(resource>=0 && g_pIInlinePicSink){
        static WORD pictures[14];static IInlinePicEngineSink* owner=0;
        if(owner!=g_pIInlinePicSink){for(int i=0;i<14;++i)pictures[i]=(WORD)-1;owner=g_pIInlinePicSink;}
        WORD picture=pictures[resource];
        if(picture==(WORD)-1){char path[128];sprintf(path,"\\spr\\pt_chat\\context_%d.spr",resource);g_pIInlinePicSink->AddCustomInlinePic(picture,path);pictures[resource]=picture;}
        if(picture!=(WORD)-1){row.szData[0]=KTC_INLINE_PIC;memcpy(row.szData+1,&picture,2);offset=3;}
    }
    if(!offset){memcpy(row.szData,"   ",3);offset=3;}
    int length=strlen(name);if(length>sizeof(row.szData)-offset-1)length=sizeof(row.szData)-offset-1;
    memcpy(row.szData+offset,name,length);row.uDataLen=offset+length;row.szData[row.uDataLen]=0;row.uID=action;row.uBgColor=0x060c0d1c;
}
void PopUpContextPeopleMenu(const KUiPlayerItem &SelectPlayer, int x, int y) {
    if (!g_pCoreShell || SelectPlayer.Name[0] == 0)
        return;
    g_DebugLog("PopUpContextPeopleMenu %d", SelectPlayer.uId);
    if (KUiPlayerBar::IsSelfName((char *) SelectPlayer.Name))
        return;

    BYTE btCurCamp = g_pCoreShell->GetGameData(GDI_PLAYER_CURCAMP, 0, 0);
    BOOL bTongFlag = g_pCoreShell->TongOperation(GTOI_TONG_FLAG, (unsigned int) &SelectPlayer, 0);
    BYTE btFigure = g_pCoreShell->TongOperation(GTOI_TONG_FIGURE, (unsigned int) &SelectPlayer, 0);
    BOOL bSelfTongFlag = g_pCoreShell->TongOperation(GTOI_TONG_FLAG, 0, 0);
    BYTE btSelfFigure = g_pCoreShell->TongOperation(GTOI_TONG_FIGURE, 0, 0);
    BOOL bTrade = g_pCoreShell->GetTradeState();
    bool remote=SelectPlayer.nIndex==-2;
    int nActionDataCount = 20;

    KPopupMenuData *pMenuData = (KPopupMenuData *) malloc(MENU_DATA_SIZE(nActionDataCount));
    if (pMenuData == NULL)
        return;

    KPopupMenu::InitMenuData(pMenuData, nActionDataCount);
    pMenuData->usMenuFlag |= PM_F_AUTO_DEL_WHEN_HIDE;
    pMenuData->usMenuFlag &= ~(PM_F_HAVE_HEAD_TAIL_IMG|PM_F_HAVE_ITEM_SEPARATOR);
    pMenuData->usMenuFlag |= PM_F_PLAYER_CONTEXT;
    pMenuData->byFontSize=14;pMenuData->nItemWidth=235;pMenuData->nItemHeight=24;pMenuData->nItemTitleIndent=8;
    pMenuData->uTextColor=0xffaaa9f8;pMenuData->uTextBorderColor=0xff000000;pMenuData->uSelTextColor=0xffffff70;
    pMenuData->uSelTextBorderColor=0xff000000;pMenuData->uSelItemBgColor=0x041b2038;pMenuData->uBorderLineColor=0;

    KUiPlayerTeam TeamInfo;
    memset(&TeamInfo, 0, sizeof(TeamInfo));
    TeamInfo.nTeamServerID = -1;
    TeamInfo.nCaptainPower = 0;
    g_pCoreShell->TeamOperation(TEAM_OI_GD_INFO, (unsigned int) &TeamInfo, 0);
    int nMenuCount = 0;
    if(remote){
        pMenuData->usMenuFlag|=PM_F_ANCHOR_AT_CLICK;
        sprintf(pMenuData->Items[0].szData,"[%.31s]",SelectPlayer.Name);
        pMenuData->Items[0].uDataLen=strlen(pMenuData->Items[0].szData);pMenuData->Items[0].uID=0xffff;
        pMenuData->Items[0].uBgColor=0x061c2440;nMenuCount=1;
    }
    int i;
    static const int order[]={ACTION_CHAT,15,ACTION_CONTEXT_FACE,ACTION_TRADE,ACTION_VIEWITEM,ACTION_FOLLOW,ACTION_TRADE_FOLKGAME,16,14,ACTION_INVITETEAM,ACTION_JOINTEAM,ACTION_MAKEFRIEND,17,18,ACTION_BLACKLIST,19};
    for (int oi=0;oi<sizeof(order)/sizeof(order[0]);++oi) {i=order[oi];
        if(remote){
            if(i!=ACTION_TRADE && i!=ACTION_TRADE_FOLKGAME){PhongThanContextLabel(pMenuData->Items[nMenuCount],i);++nMenuCount;}
            continue;
        }
        if ((i == ACTION_JOINTEAM && SelectPlayer.nIndex != -1 && SelectPlayer.nData == PLAYER_MENU_STATE_TEAMOPEN &&
             (TeamInfo.nTeamServerID == -1 || (TeamInfo.nTeamServerID != SelectPlayer.nTeamID) && !bTrade)) ||
            // tham gia vao doi / nhap doi
            (i == ACTION_TRADE && SelectPlayer.nIndex > 0 && PhongThanCanReceiveTradeInvite(SelectPlayer.nData) &&
             !bTrade) ||    //"������Ʒ", �Է�δ�򿪽���ʱ���ܼ���
            (i == ACTION_MAKEFRIEND && !KUiChatCentre::IsMyFriend((char *) SelectPlayer.Name)) || //Ket ban / hao huu
            (i == ACTION_INVITETEAM && SelectPlayer.uId != 0 && TeamInfo.nCaptainPower > 0 && !bTrade) ||
            //Moi vao to doi
            (i == ACTION_FOLLOW && SelectPlayer.nIndex != -1 && !bTrade) || // Theo sau nguoi choi
            (i == ACTION_VIEWITEM && SelectPlayer.uId != 0) ||
            (i == ACTION_TONG && SelectPlayer.nIndex != -1 && bTongFlag && !bTrade) ||
            (i == ACTION_BLACKLIST && !IsInBlackName((char *) SelectPlayer.Name)) ||
            (i == ACTION_CHAT) ||
            (i == ACTION_CONTEXT_FACE) ||
            (i==15||i==16||i==17||i==18||i==19) ||
              (i == 14) ||
            (i == ACTION_TRADE_FOLKGAME && SelectPlayer.nIndex > 0 &&
             PhongThanCanReceiveTradeInvite(SelectPlayer.nData) && !bTrade) ||
            (i == ACTION_JOINTONG && SelectPlayer.nIndex != -1 && bTongFlag && !bSelfTongFlag &&
             btFigure > enumTONG_FIGURE_MEMBER && !bTrade) ||
            (i == ACTION_JOINTONG_REPLY && SelectPlayer.nIndex != -1 && !bTongFlag && bSelfTongFlag &&
             btFigure > enumTONG_FIGURE_MEMBER && !bTrade) ||
            (i == ACTION_REVENGE && SelectPlayer.uId != 0 && !bTrade)
                ) {
            PhongThanContextLabel(pMenuData->Items[nMenuCount],i);
            nMenuCount++;
        }
    }
    pMenuData->nNumItem = nMenuCount;

    pMenuData->nX = x;
    pMenuData->nY = y;
    g_WndGameSpace.m_SelectPlayer = SelectPlayer;
    KPopupMenu::Popup(pMenuData, &g_WndGameSpace, SEL_ACTION_MENU);
}

void PopUpContextEmoteMenu(char *szDest, int x, int y) {
    if (szDest == NULL || szDest[0] == 0)
        return;

    if (KUiPlayerBar::IsSelfName(szDest))
        return;

    int nEmoteCount = g_UiChatPhrase.GetEmoteCount((KUiChatPhrase::EMOTE_TYPE) (1));

    KPopupMenuData *pMenuData = (KPopupMenuData *) malloc(MENU_DATA_SIZE(nEmoteCount));
    if (pMenuData == NULL) {
        return;
    }

    KPopupMenu::InitMenuData(pMenuData, nEmoteCount);
    pMenuData->usMenuFlag |= PM_F_AUTO_DEL_WHEN_HIDE;
    pMenuData->usMenuFlag &= ~PM_F_HAVE_HEAD_TAIL_IMG;

    int i, nBuffLen;
    char szBuff[128];
    pMenuData->nItemHeight = 0;
    for (i = 0; i < nEmoteCount; i++) {
        nBuffLen = g_UiChatPhrase.GetMenuEmoteName(i, szBuff, sizeof(szBuff));
        memcpy(pMenuData->Items[i].szData, szBuff, nBuffLen);
        pMenuData->Items[i].uDataLen = nBuffLen;
        pMenuData->Items[i].uID = i;
    }
    pMenuData->nNumItem = nEmoteCount;

    pMenuData->nX = x;
    pMenuData->nY = y;
    strncpy(g_WndGameSpace.m_SelectPlayer.Name, szDest, 32);
    g_WndGameSpace.m_SelectPlayer.nIndex = -1;
    g_WndGameSpace.m_SelectPlayer.uId = 0;
    g_WndGameSpace.m_SelectPlayer.Name[31] = 0;
    KPopupMenu::Popup(pMenuData, &g_WndGameSpace, SEL_EMOTE_MENU);
}

//TamLTM Dieu khien theo auto chuot - ko click mouse
void SwitchHoldCursor() {
    g_WndGameSpace.m_bHoldCursor = !g_WndGameSpace.m_bHoldCursor;
    if (g_pCoreShell) {
        if (g_WndGameSpace.m_bHoldCursor) {
            if (g_pCoreShell->PAIOperation(GPI_ISACTIVE, NULL, NULL, NULL))
                g_pCoreShell->PAIOperation(GPI_PRIORITY_USE_MOUSE, TRUE, NULL, NULL);
        } else
            g_pCoreShell->PAIOperation(GPI_PRIORITY_USE_MOUSE, FALSE, NULL, NULL);
    }
}
