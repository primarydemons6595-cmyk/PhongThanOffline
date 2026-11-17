/*****************************************************************************************
//	½çÃæ--×´Ì¬½çÃæ
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-9-2
*****************************************************************************************/
#include "KWin32.h"
#include "KIniFile.h"
#include "../Elem/WndMessage.h"
#include "../elem/wnds.h"
#include "UiParadeItem.h"
#include "../../../../Headers/PhongThanStatusPages.h"
#include "../ShortcutKey.h"
#include "../UiSoundSetting.h"
#include "../../../core/src/coreshell.h"
#include "../UiBase.h"
#include "../../../Represent/iRepresent/iRepresentShell.h"
#include "../../Engine/Src/KDebug.h"

extern iRepresentShell *g_pRepresentShell;
extern iCoreShell *g_pCoreShell;

#define    SCHEME_INI        "\xBF\xB4\xB6\xD4\xB7\xBD\xCD\xE6\xBC\xD2\xD7\xB0\xB1\xB8.ini"

KUiParadeItem *KUiParadeItem::m_pSelf = NULL;

// -------------------------------------------------------------------------
// ---> ½¨Á¢¿Ø¼þÓëUIEP_*ÒÔ¼°¿É½ÓÄÉÎïÆ·µÄÀàÐÍµÄ¶ÔÓ¦¹ØÏµ
static struct UE_CTRL_MAP {
    int nPosition;
    const char *pIniSection;
} LegacyCtrlItemMap[_ITEM_COUNT] =
        {
                {UIEP_HEAD,        "Cap"},    //×°±¸-Ã±×Ó
                {UIEP_HAND,        "Weapon"},    //×°±¸-ÎäÆ÷
                {UIEP_NECK,        "Necklace"},    //×°±¸-ÏîÁ´
                {UIEP_FINESSE,     "Bangle"},    //×°±¸-ÊÖïí
                {UIEP_BODY,        "Cloth"},    //×°±¸-ÒÂ·þ
                {UIEP_WAIST,       "Sash"},    //×°±¸-Ñü´ø
                {UIEP_FINGER1,     "Ring1"},    //×°±¸-½äÖ¸
                {UIEP_FINGER2,     "Ring2"},    //×°±¸-½äÖ¸
                {UIEP_WAIST_DECOR, "Pendant"},    //×°±¸-Ñü×º
                {UIEP_FOOT,        "Shoes"},    //×°±¸-Ð¬×Ó
                {UIEP_HORSE,       "Horse"},    //×°±¸-Âí
                {UIEP_SIGNET,      "Signet"},    //×°±¸-Âí
                {UIEP_SHIPIN,      "Shipin"},    //×°±¸-Âí
        };


//--------------------------------------------------------------------------
//	¹¦ÄÜ£ºÈç¹û´°¿ÚÕý±»ÏÔÊ¾£¬Ôò·µ»ØÊµÀýÖ¸Õë
//--------------------------------------------------------------------------
#include "UiParadePtSlots.inl"
KUiParadeItem *KUiParadeItem::GetIfVisible() {
    if (m_pSelf && m_pSelf->IsVisible())
        return m_pSelf;
    return NULL;
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£º´ò¿ª´°¿Ú£¬·µ»ØÎ¨Ò»µÄÒ»¸öÀà¶ÔÏóÊµÀý
//--------------------------------------------------------------------------
KUiParadeItem *
KUiParadeItem::OpenWindow(KUiPlayerItem *pDest)//pdest la splayer do à , gio lay cai teamid do ra la id cua avatar thoi
{
    if (!pDest || !g_pCoreShell) return NULL;
    if (m_pSelf == NULL) {
        m_pSelf = new KUiParadeItem;
        if (m_pSelf)
            m_pSelf->Initialize();
    }
    if (m_pSelf) {

        char Scheme[256];
        g_UiBase.GetCurSchemePath(Scheme, 256);
        LoadScheme(Scheme);
        m_pSelf->m_Dest = *pDest;
        m_pSelf->UpdateData(pDest);
        m_pSelf->BringToTop();
        UiSoundPlay(UI_SI_WND_OPENCLOSE);
        m_pSelf->Show();
    }
    return m_pSelf;
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£º¹Ø±Õ´°¿Ú£¬Í¬Ê±¿ÉÒÔÑ¡ÔòÊÇ·ñÉ¾³ý¶ÔÏóÊµÀý
//--------------------------------------------------------------------------
void KUiParadeItem::CloseWindow(bool bDestroy) {
    if (m_pSelf) {
        if (bDestroy == false)
            m_pSelf->Hide();
        else {
            m_pSelf->Destroy();
            m_pSelf = NULL;
        }

        if (g_pCoreShell)
            g_pCoreShell->OperationRequest(GOI_VIEW_PLAYERITEM_END, 0, 0);
    }
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£º³õÊ¼»¯
//--------------------------------------------------------------------------
void KUiParadeItem::Initialize() {
    AddChild(&m_Background);
    AddChild(&m_EquipImage);
    for(int profileField=0;profileField<10;++profileField)AddChild(&m_PublicProfile[profileField]);
    AddChild(&m_Name);
    AddChild(&m_Title);
    AddChild(&m_Tong);
    AddChild(&m_WorldRank);
    AddChild(&m_Avt);
    AddChild(&m_PKValue);
    AddChild(&m_FuYuan);
    AddChild(&m_Repute);
    AddChild(&m_TransLife);
    AddChild(&m_MateName);
    AddChild(&m_Close);
    for (int i = 0; i < PT_PARADE_SLOT_COUNT; i++) {
        AddChild(&m_EquipSlotImage[i]);
        m_EquipBox[i].SetObjectGenre(CGOG_ITEM);
        AddChild(&m_EquipBox[i]);
        m_EquipBox[i].SetContainerId((int) UOC_EQUIPTMENT);
    }
    AddChild(&m_EquipExpandBtn);
    SwitchExpand(FALSE);

    Wnd_AddWindow(this);
}

void KUiParadeItem::SwitchExpand(BOOL bShow) {
    m_EquipExpandBtn.CheckButton(FALSE);
    m_EquipExpandImg.Hide();
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£ºÔØÈë´°¿ÚµÄ½çÃæ·½°¸
//--------------------------------------------------------------------------
void KUiParadeItem::LoadScheme(const char *pScheme) {
    if (m_pSelf) {
        char Buff[128];
        KIniFile Ini;
        sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI);
        if (Ini.Load(Buff))
            m_pSelf->LoadScheme(&Ini);
    }
}

//ÔØÈë½çÃæ·½°¸
#include "UiParadePtCompat.inl"

//--------------------------------------------------------------------------
//	¹¦ÄÜ£º´°¿Úº¯Êý
//--------------------------------------------------------------------------
int KUiParadeItem::WndProc(unsigned int uMsg, unsigned int uParam, int nParam) {
    int nRet = 0;
    switch (uMsg) {
        case WND_N_BUTTON_CLICK:
            if (uParam == (unsigned int) (KWndWindow *) &m_Close)
                CloseWindow(false);
            else if (uParam == (unsigned int) (KWndWindow *) &m_EquipExpandBtn)
                SwitchExpand(m_EquipExpandBtn.IsButtonChecked());
            break;
        default:
            nRet = KWndShowAnimate::WndProc(uMsg, uParam, nParam);
    }
    return nRet;
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£º¸üÐÂ»ù±¾Êý¾Ý£¨ÈËÃûµÈ²»Ò×±äÊý¾Ý£©
//--------------------------------------------------------------------------
void KUiParadeItem::UpdateBaseData(KUiPlayerItem *pDest) {
    if (!g_pCoreShell)
        return;

    KUiPlayerBaseInfo Info;
    memset(&Info, 0, sizeof(KUiPlayerBaseInfo));
    g_pCoreShell->GetGameData(GDI_PLAYER_BASE_INFO, (int) &Info, pDest->uId);
    m_Name.SetText(Info.Name);
    m_Title.SetText(Info.Title);
    if (Info.szTongName[0])
        m_Tong.SetText(Info.szTongName);
    else
        m_Tong.SetText("Ch­a vµo bang");
    m_WorldRank.SetIntText(Info.nRankInWorld, true);
    m_PKValue.SetIntText(Info.nPKValue);
    m_FuYuan.SetIntText(Info.nFuYuan);
    m_Repute.SetIntText(Info.nRepute);
    m_TransLife.Set4IntText(Info.nTranslife);
    if (Info.szMateName[0])
        m_MateName.SetText(Info.szMateName);
    else
        m_MateName.SetText(NORMAL_UNCLEAR_WORD);
    int variant = pDest->nTeamID > 0 ? (pDest->nTeamID - 1) % 3 : 0;
    int portrait = variant * 2 +
        (g_pCoreShell->GetGameData(GDI_PLAYER_IS_MALE, 0, pDest->uId) ? 1 : 2);
    char image[128];
    sprintf(image, "\\Spr\\Ui4\\\xD0\xA4\xCF\xF1\\%03d.spr", portrait);
    m_Avt.SetImage(ISI_T_SPR, image, false);
    PT_PERSONAL_PROFILE profile;ZeroMemory(&profile,sizeof(profile));
    if(g_pCoreShell->GetGameData(PT_VIEW_PROFILE_GET,(unsigned int)&profile,pDest->uId)){
        m_Name.SetToolTipInfo(profile.Memo,strlen(profile.Memo));
        char path[256];g_UiBase.GetCurSchemePath(path,sizeof(path));strcat(path,"\\UiStatus.ini");KIniFile choices;choices.Load(path);
        const char* sections[5]={"Sign_ComboxSex","Sign_ComboxAge","Sign_ComboxShengXiao","Sign_ComboxActivity","Sign_ComboxActTime"};
        const int ids[5]={profile.Sex,profile.Age,atoi(profile.Relationship),profile.Activity,profile.Hour};
        const int fields[5]={0,1,2,7,8};
        for(int f=0;f<5;++f){char key[32],value[128];sprintf(key,"SelText_%d",ids[f]);choices.GetString(sections[f],key,"--",value,sizeof(value));m_PublicProfile[fields[f]].SetText(value);}
        m_PublicProfile[3].SetIntText(profile.BirthMonth);m_PublicProfile[4].SetIntText(profile.BirthDay);
        m_PublicProfile[5].SetText("--");m_PublicProfile[6].SetText(profile.Province);m_PublicProfile[9].SetText(profile.Memo);

    }
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£º¸üÐÂÊý¾Ý
//--------------------------------------------------------------------------
void KUiParadeItem::UpdateData(KUiPlayerItem *pDest) {
    if (pDest == NULL)
        return;

    UpdateBaseData(pDest);

    UpdateAllEquips(pDest);

    m_Dest = *pDest;
}

void KUiParadeItem::UpdateAllEquips(KUiPlayerItem *pDest) {
    if (!g_pCoreShell)
        return;

    KUiObjAtRegion Equips[_ITEM_COUNT];
    int nCount = g_pCoreShell->GetGameData(GDI_PARADE_EQUIPMENT, (unsigned int) &Equips, 0);
    int i;
    for (i = 0; i < _ITEM_COUNT; i++)
        m_EquipBox[i].Celar();
    for (i = 0; i < nCount && i < _ITEM_COUNT; i++) {
        if (Equips[i].Obj.uGenre != CGOG_NOTHING)
            UpdateEquip(&Equips[i], true);
    }
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£º×°±¸±ä»¯¸üÐÂ
//--------------------------------------------------------------------------
void KUiParadeItem::UpdateEquip(KUiObjAtRegion *pEquip, int bAdd) {
    if (pEquip) {
        for (int i = 0; i < PT_PARADE_SLOT_COUNT; i++) {
            if (CtrlItemMap[i].nPosition == pEquip->Region.v) {
                if (bAdd)
                    m_EquipBox[i].HoldObject(pEquip->Obj.uGenre, pEquip->Obj.uId,
                                             pEquip->Region.Width, pEquip->Region.Height);
                else
                    m_EquipBox[i].HoldObject(CGOG_NOTHING, 0, 0, 0);
                break;
            }
        }
    }
}
