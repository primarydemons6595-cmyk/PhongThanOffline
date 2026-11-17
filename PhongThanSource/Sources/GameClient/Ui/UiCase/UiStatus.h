#pragma once
#include "../Elem/WndEdit.h"
#include "../Elem/WndList.h"
#include "../Elem/WndScrollBar.h"
#include "../Elem/WndMessage.h"
#include "../Elem/WndImage.h"
#include "../Elem/WndButton.h"
#include "../Elem/WndText.h"
#include "../Elem/WndShowAnimate.h"
#include "../elem/WndObjContainer.h"
#include "../Elem/WndPureTextBtn.h"
#include "../Elem/WndLabeledButton.h"
#include "UiPtStatusCombo.h"
#include "UiPtPetPreview.h"
#include "../../../Core/Src/GameDatadef.h"

enum UI_PLAYER_ATTRIBUTE;
struct KUiPlayerRuntimeInfo;
struct KUiObjAtRegion;
struct KUiPlayerAttribute;
class KIniFile;
class KPtStatusPageFrame : public KWndImage {
public:
 int WndProc(unsigned int msg,unsigned int param,int value){
  if((msg==WND_N_BUTTON_CLICK||msg==WND_N_SCORLLBAR_POS_CHANGED)&&m_pParentWnd)return m_pParentWnd->WndProc(msg,param,value);
  return KWndImage::WndProc(msg,param,value);
 }
};
class KPtStatusLongText : public KWndText {
 char m_Text[4096];
public: KPtStatusLongText(){SetTextPtr(m_Text,sizeof(m_Text));}
};
#define _ITEM_COUNT 17
#define STATUS_LABEL_COUNT 26


class KUiStatus : public KWndShowAnimate {
public:
 static KUiStatus *OpenWindow(bool bShow=true); static KUiStatus *GetIfVisible(); static void CloseWindow(bool bDestroy);
 static void LoadScheme(const char *pScheme);
 static KUiStatus* OpenPetPage();
 void UpdateBaseData(); void UpdateData(); void Breathe();
 void UpdateRuntimeInfo(KUiPlayerRuntimeInfo *pInfo); void UpdateAllEquips(); void UpdateRuntimeAttribute(KUiPlayerAttribute *pInfo);
 void UpdateEquip(KUiObjAtRegion *pEquip,int bAdd); void SwitchExpand(BOOL bShow);
private:
 struct PTStatusPageEntry {int Id;char Name[80];};
 void InitRealmPage(KIniFile*);void UpdateRealmPage();
 KPtStatusPageFrame m_RealmPage;KWndText256 m_RealmLines[19];char m_RealmStatusImages[2][256];
 void InitPtPages(KIniFile*);void SelectPtPage(int);void UpdatePtPages();bool PtPagesClick(unsigned int);
 void ApplyPtPageVisibility();void UpdateProfilePage();void UpdateTitlePage();
 int m_StatusPageMode,m_StatusPageCount,m_StatusPageSelected;bool m_StatusPageLoaded;
 KWndImageTextButton m_StatusPagesTabs[3],m_StatusProfileTab,m_StatusPageSave,m_StatusPageHideTitle;
 KPtStatusPageFrame m_StatusPageFrame;KWndText80 m_StatusPageHeading,m_StatusPageLabels[10];KPtStatusLongText m_StatusPageDescription;
 KWndEdit80 m_StatusPageEdits[10];KWndList m_StatusPageList;KWndScrollBar m_StatusPageScroll;
 PTStatusPageEntry m_StatusPageEntries[4096];
 KPtStatusPageFrame m_ProfilePage,m_TitlePage;KPtStatusCombo m_ProfileCombos[5],m_TitleCombos[2];
 KWndEdit80 m_ProfileFields[4];KWndImage m_ProfileFieldFrames[4],m_TitlePreview;
 KWndText80 m_ProfileStar,m_TitleLabels[5],m_TitleName,m_TitleTextPreview;
 KPtStatusLongText m_TitleInfo;KWndButton m_TitleButtons[4],m_BookArrows[4];
 KWndScrollBar m_StatusDescScroll;int m_LastTitleChoice;char m_StatusBookImage[256];
 void InitPetPage(KIniFile*);void UpdatePetPage();bool PetPageClick(unsigned int);void SelectPetFood(int);
 KPtStatusPageFrame m_PetPage;KPtPetPreview m_PetPicture;KWndImage m_PetSkillImages[4];KWndButton m_PetSkillIcons[4];
 KWndImage m_PetPictureFrame;KWndButton m_PetSlots[4],m_PetSelectButtons[2];KWndText80 m_PetAttributeLabels[4];
 KWndText80 m_PetName,m_PetValues[7],m_PetLabels[10];
 KWndButton m_PetFlags[10],m_PetFoodButton;KWndImageTextButton m_PetFeed,m_PetRename,m_PetLog;KWndImage m_PetFoodImage;
 KWndEdit80 m_PetNameEdit;int m_PetFoodIndex;unsigned int m_PetFoodSerial,m_PetChatTick;
 void InitPetOptions();void OpenPetOptions(int);bool PetOptionsClick(unsigned int);
 KPtStatusPageFrame m_PetOptions[2];KWndLabeledButton m_PetOptionChecks[9];KWndImage m_PetOptionMarks[9];
 KWndButton m_PetOptionClose[2],m_PetOptionSave[2],m_PetPickupLogs[2];
 KWndText80 m_PetSaleLabels[4];KPtStatusLongText m_PetSaleIntro;KWndImage m_PetSaleBorders[2];KWndEdit80 m_PetSaleFields[2];
 int m_PetOptionsMode,m_PetPickMask,m_PetSaleMask;
 void InitPetLog();void OpenPetLog();void UpdatePetLog();bool PetLogClick(unsigned int);
 KPtStatusPageFrame m_PetLogFrame;KPtStatusLongText m_PetLogText;KWndText80 m_PetLogTitle;
 KWndButton m_PetLogClose,m_PetLogUp,m_PetLogDown;KWndScrollBar m_PetLogScroll;
 unsigned int m_PetLogStamp;bool m_PetLogVisible;
private:
 KUiStatus():m_PtDetailMode(false),m_PtTab(0),m_StatusPageMode(0){}
 ~KUiStatus(){}
 void Initialize(); void UseRemainPoint(int ntype,int nPoint); int WndProc(unsigned int,unsigned int,int);
 void InitPtDetails(KIniFile* ini,int x,int y); void UpdatePtDetails(); void LoadScheme(KIniFile *pIni);
 void OnEquiptChanged(ITEM_PICKDROP_PLACE *pPickPos,ITEM_PICKDROP_PLACE *pDropPos); void wait(int seconds);
 static KUiStatus *m_pSelf;
 KWndText32 m_PtElement[4],m_PtCritical,m_PtEvasion; KWndButton m_PtTabs[3]; KWndButton m_OpenExtAttrPage; KWndButton m_PtDetailClose; KWndImage m_PtDetailBg; KWndImage m_PtTabBg[3]; KWndText80 m_PtDetailTitle,m_PtDetailLine[14]; bool m_PtDetailMode; int m_PtTab;
 int m_PtDetailX[3][14],m_PtDetailY[3][14],m_PtDetailWidth[3][14];
 KWndImage m_StatusPageMain,m_NormalStatusBg; KWndButton m_StatusTab,m_NormalStatusBtn,m_JEvilStatusBtn;
 KWndText32 m_Name,m_Title,m_Life,m_Mana,m_Experience,m_Stamina; int m_nRemainPoint; KWndText32 m_RemainPoint;
 KWndText32 m_Strength,m_Dexterity,m_Vitality,m_Energy; KWndButton m_AddStrength,m_AddDexterity,m_AddVitality,m_AddEnergy;
 KWndText32 m_LeftDamage,m_RightDamage,m_Attack,m_Defence,m_MoveSpeed,m_AttackSpeed,m_CastSpeed;
 KWndText32 m_PhyDef,m_CoolDef,m_LightDef,m_EarthDef,m_FireDef; KWndText32 m_PhyDefPlus,m_CoolDefPlus,m_LightDefPlus,m_EarthDefPlus,m_FireDefPlus;
 KWndText32 m_PtExploit; KWndText32 m_Level,m_StatusDesc,m_PKValue,m_Repute,m_FuYuan,m_TransLife,m_WorldRank; KWndText80 m_AttributeLabel[STATUS_LABEL_COUNT]; KWndImage m_Avt;
 KWndButton m_OpenItemPad,m_Close,m_EquipExpandBtn; KWndImage m_EquipExpandImg,m_EquipSkin; KWndButton m_MaskFeature,m_UnlockBtn,m_BtnBind,m_BtnUnBind; KWndText80 m_chooseavatar; KWndButton m_Avatar; int nNumIcon;
 KWndObjectBox m_EquipBox[_ITEM_COUNT];
};
