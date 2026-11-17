#include "../../../../Headers/PhongThanStatusPages.h"
#include "KTabFile.h"
#include "UiInformation.h"
#include <stddef.h>
#include "../../../../Headers/PhongThanStatusText.h"
static void PtPageControl(KIniFile& ini,const char* section,int x,int y,int width,int height){
 ini.WriteInteger(section,"Left",x);ini.WriteInteger(section,"Top",y);ini.WriteInteger(section,"Width",width);ini.WriteInteger(section,"Height",height);
 ini.WriteInteger(section,"Font",14);ini.WriteString(section,"Color","218,255,165");ini.WriteInteger(section,"Trans",0);
}
KUiStatus* KUiStatus::OpenPetPage(){KUiStatus* window=OpenWindow(true);if(window)window->SelectPtPage(3);return window;}
#include "UiRealmStatus.inl"
void KUiStatus::InitPtPages(KIniFile* original){
 InitRealmPage(original);
 m_StatusPageMode=0;m_StatusPageLoaded=false;m_StatusPageSelected=-1;m_LastTitleChoice=-1;
 original->WriteInteger("Sign_BtnPersonalSign","Left",79);
 AddChild(&m_StatusProfileTab);m_StatusProfileTab.Init(original,"Sign_BtnPersonalSign");OffsetStatusPageControl(&m_StatusProfileTab,26,56);
 const char* tabs[3]={"CompoundBtn","PetPage_Btn","Title_Btn"};
 for(int i=0;i<3;++i){AddChild(&m_StatusPagesTabs[i]);m_StatusPagesTabs[i].Init(original,tabs[i]);m_StatusPagesTabs[i].Enable(true);}
 AddChild(&m_StatusPageFrame);m_StatusPageFrame.Init(original,"CompoundPage_Main");
 m_StatusPageFrame.AddChild(&m_StatusPageList);original->WriteString("CompoundPage_NameList","Color","255,227,160");
 original->WriteString("CompoundPage_NameList","BorderColor","0,0,0");
 original->WriteString("CompoundPage_NameList","SelColor","255,255,128");
 original->WriteString("CompoundPage_NameList","HighLightColor","255,255,255");
 m_StatusPageList.Init(original,"CompoundPage_NameList");
 m_StatusPageFrame.AddChild(&m_StatusPageDescription);m_StatusPageDescription.Init(original,"CompoundPage_DescList");
 m_StatusPageFrame.AddChild(&m_StatusPageScroll);m_StatusPageScroll.Init(original,"CompoundPage_ScrName_Bar");OffsetStatusPageControl(&m_StatusPageScroll,208,24);m_StatusPageList.SetScrollbar(&m_StatusPageScroll);
 m_StatusPageFrame.AddChild(&m_StatusDescScroll);m_StatusDescScroll.Init(original,"CompoundPage_ScrDesc_Bar");OffsetStatusPageControl(&m_StatusDescScroll,208,188);
 const char* arrows[4]={"CompoundPage_ScrName_BtnUp","CompoundPage_ScrName_BtnDown","CompoundPage_ScrDesc_BtnUp","CompoundPage_ScrDesc_BtnDown"};
 for(i=0;i<4;++i){m_StatusPageFrame.AddChild(&m_BookArrows[i]);m_BookArrows[i].Init(original,arrows[i]);OffsetStatusPageControl(&m_BookArrows[i],208,i<2?24:188);}
 AddChild(&m_ProfilePage);m_ProfilePage.Init(original,"Sign_ImgPersonalSignBG");OffsetStatusPageControl(&m_ProfilePage,26,56);m_ProfilePage.SetSize(235,245);
 const char* combos[5]={"Sign_ComboxSex","Sign_ComboxAge","Sign_ComboxShengXiao","Sign_ComboxActivity","Sign_ComboxActTime"};
 for(i=0;i<5;++i){m_ProfilePage.AddChild(&m_ProfileCombos[i]);m_ProfileCombos[i].Init(original,combos[i]);OffsetStatusPageControl(&m_ProfileCombos[i],0,-95);}
 const char* edits[4]={"Sign_EditBirthday_Month","Sign_EditBirthday_Day","Sign_EditProvince","Sign_EditSign"};
 for(i=0;i<4;++i){m_ProfilePage.AddChild(&m_ProfileFields[i]);m_ProfileFields[i].Init(original,edits[i]);OffsetStatusPageControl(&m_ProfileFields[i],0,-95);}
 m_ProfileFields[0].SetLimitText(2);m_ProfileFields[1].SetLimitText(2);m_ProfileFields[2].SetLimitText(8);m_ProfileFields[3].SetLimitText(64);
 const char* borders[4]={"Sign_BirthMonthKuang","Sign_BirthDayKuang","Sign_ProvinceKuang","Sign_StarKuang"};
 for(i=0;i<4;++i){m_ProfilePage.AddChild(&m_ProfileFieldFrames[i]);m_ProfileFieldFrames[i].Init(original,borders[i]);OffsetStatusPageControl(&m_ProfileFieldFrames[i],0,-95);}
 m_ProfilePage.AddChild(&m_ProfileStar);m_ProfileStar.Init(original,"Sign_Star");OffsetStatusPageControl(&m_ProfileStar,0,-95);
 m_ProfilePage.AddChild(&m_StatusPageSave);m_StatusPageSave.Init(original,"Sign_BtnConfirm");OffsetStatusPageControl(&m_StatusPageSave,0,-95);
 // Text and inputs render above their original border sprites.
 for(i=0;i<4;++i)m_ProfileFields[i].BringToTop();m_ProfileStar.BringToTop();
 AddChild(&m_TitlePage);m_TitlePage.Init(original,"Title_Main");
 const char* labels[5]={"Title_txtTitle","Title_txtInfo","Title_txtTitlePre","Title_txtBK","Title_txtTitleList"};
 for(i=0;i<5;++i){m_TitlePage.AddChild(&m_TitleLabels[i]);m_TitleLabels[i].Init(original,labels[i]);}
 m_TitlePage.AddChild(&m_TitleInfo);m_TitleInfo.Init(original,"Title_EdtInfo");
 m_TitlePage.AddChild(&m_TitlePreview);m_TitlePreview.Init(original,"Title_ImgPictureWnd");
 m_TitlePage.AddChild(&m_TitleName);m_TitleName.Init(original,"Title_txtTitle");
 m_TitlePage.AddChild(&m_TitleTextPreview);m_TitleTextPreview.Init(original,"Title_ImgPictureWnd");m_TitleTextPreview.SetHAlign(WNDTEXT_ES_HALIGN_CENTRE);
 m_TitlePage.AddChild(&m_TitleCombos[0]);m_TitleCombos[0].Init(original,"Title_comTitleList");
 m_TitlePage.AddChild(&m_TitleCombos[1]);m_TitleCombos[1].Init(original,"Title_comBkSel");
 const char* buttons[4]={"Title_BtnShow","Title_BtnHide","Title_BtnPreShow","Title_BtnSetBk"};
 for(i=0;i<4;++i){m_TitlePage.AddChild(&m_TitleButtons[i]);m_TitleButtons[i].Init(original,buttons[i]);}
 InitPetPage(original);m_StatusPageList.SetContent(0,0,0,0);ApplyPtPageVisibility();
}
void KUiStatus::ApplyPtPageVisibility(){
 PTRoleValues realm;memset(&realm,0,sizeof(realm));
 const bool unlocked=g_pCoreShell&&g_pCoreShell->GetGameData(PT_GDI_ROLE_VALUES,(unsigned int)&realm,sizeof(realm))&&PhongThanRealmUnlocked(realm.RealmStage,realm.RealmSide,realm.RealmLevel);
 if(!unlocked&&m_StatusPageMode==5)m_StatusPageMode=0;
 const bool top=m_StatusPageMode<2||m_StatusPageMode==5,normal=m_StatusPageMode==0;
 if(m_StatusPageMode==5)m_RealmPage.Show();else m_RealmPage.Hide();
 KWndWindow* upper[]={&m_StatusPageMain,&m_Avt,&m_Name,&m_Title,&m_StatusDesc,&m_PtExploit,&m_NormalStatusBtn,&m_JEvilStatusBtn,&m_StatusProfileTab};
 for(int i=0;i<sizeof(upper)/sizeof(upper[0]);++i){if(top)upper[i]->Show();else upper[i]->Hide();}
 KWndWindow* lower[]={&m_NormalStatusBg,&m_Level,&m_WorldRank,&m_PKValue,&m_Experience,&m_Repute,&m_RemainPoint,&m_LeftDamage,&m_RightDamage,&m_Attack,&m_Defence,&m_AttackSpeed,&m_CastSpeed,&m_CoolDef,&m_LightDef,&m_EarthDef,&m_FireDef,&m_Strength,&m_Dexterity,&m_Vitality,&m_Energy,&m_MoveSpeed,&m_Life,&m_Mana,&m_Stamina,&m_PhyDef,&m_CoolDefPlus,&m_LightDefPlus,&m_EarthDefPlus,&m_FireDefPlus,&m_PhyDefPlus,&m_OpenExtAttrPage,&m_PtCritical,&m_PtEvasion};
 for(i=0;i<sizeof(lower)/sizeof(lower[0]);++i){if(normal)lower[i]->Show();else lower[i]->Hide();}
 for(i=0;i<4;++i){if(normal)m_PtElement[i].Show();else m_PtElement[i].Hide();}
 for(i=0;i<STATUS_LABEL_COUNT;++i){if(normal)m_AttributeLabel[i].Show();else m_AttributeLabel[i].Hide();}
 if(m_StatusPageMode==1)m_ProfilePage.Show();else m_ProfilePage.Hide();
 if(m_StatusPageMode==2)m_StatusPageFrame.Show();else m_StatusPageFrame.Hide();
 if(m_StatusPageMode==3)m_PetPage.Show();else m_PetPage.Hide();
 if(m_StatusPageMode!=3){m_PetLogFrame.Hide();m_PetLogVisible=false;}
 if(m_StatusPageMode==4)m_TitlePage.Show();else m_TitlePage.Hide();
 m_JEvilStatusBtn.Enable(unlocked);
 if(unlocked&&top){m_JEvilStatusBtn.Show();m_JEvilStatusBtn.CheckButton(m_StatusPageMode==5);
    if(m_RealmStatusImages[realm.RealmSide-1][0]&&strcmp(m_JEvilStatusBtn.GetImage(),m_RealmStatusImages[realm.RealmSide-1]))m_JEvilStatusBtn.SetImage(ISI_T_SPR,m_RealmStatusImages[realm.RealmSide-1],true);
 }else m_JEvilStatusBtn.Hide();
 m_StatusProfileTab.SetPosition(unlocked?169:105,151);
 m_StatusTab.CheckButton(top);m_StatusProfileTab.CheckButton(m_StatusPageMode==1);m_NormalStatusBtn.CheckButton(normal);
 for(i=0;i<3;++i)m_StatusPagesTabs[i].CheckButton(m_StatusPageMode==i+2);
}
void KUiStatus::SelectPtPage(int mode){
 KPopupMenu::Cancel();m_PetOptions[0].Hide();m_PetOptions[1].Hide();m_PetOptionsMode=-1;m_StatusPageMode=mode;m_StatusPageSelected=-1;m_StatusPageLoaded=false;m_LastTitleChoice=-1;
 m_PtDetailMode=false;m_PtDetailBg.Hide();m_PtDetailClose.Hide();m_PtDetailTitle.Hide();for(int j=0;j<3;++j){m_PtTabs[j].Hide();m_PtTabBg[j].Hide();}for(j=0;j<14;++j)m_PtDetailLine[j].Hide();
 ApplyPtPageVisibility();m_StatusPageCount=0;
 if(mode==2){KTabFile book;if(book.Load("\\settings\\item\\001\\compoundbook.txt")){
  for(int row=2;row<=book.GetHeight()&&m_StatusPageCount<4096;++row){char name[80];book.GetString(row,2,"",name,sizeof(name));if(!name[0])continue;
   PTStatusPageEntry& entry=m_StatusPageEntries[m_StatusPageCount++];entry.Id=row;lstrcpyn(entry.Name,name[0]=='#'?name+1:name,sizeof(entry.Name));}
 }m_StatusPageList.SetContent((BYTE*)m_StatusPageEntries,m_StatusPageCount,sizeof(PTStatusPageEntry),offsetof(PTStatusPageEntry,Name));m_StatusPageDescription.SetText("");}
 UpdatePtPages();
}
void KUiStatus::UpdateProfilePage(){
 PT_STATUS_PAGES_STATE state;if(!g_pCoreShell->GetGameData(PT_STATUS_PAGES_GET,(unsigned int)&state,sizeof(state)))return;
 if(!m_StatusPageLoaded){
  if(state.Result<0)UIMessageBox("Khong luu duoc thong tin ca nhan.");const PT_PERSONAL_PROFILE&p=state.Profile;
  m_ProfileCombos[0].SelectId(p.Sex);m_ProfileCombos[1].SelectId(p.Age);m_ProfileCombos[2].SelectId(atoi(p.Relationship));m_ProfileCombos[3].SelectId(p.Activity);m_ProfileCombos[4].SelectId(p.Hour);
  if(p.BirthMonth)m_ProfileFields[0].SetIntText(p.BirthMonth);else m_ProfileFields[0].SetText("--");
  if(p.BirthDay)m_ProfileFields[1].SetIntText(p.BirthDay);else m_ProfileFields[1].SetText("--");m_ProfileStar.SetText("--");
  m_ProfileFields[2].SetText(p.Province);m_ProfileFields[3].SetText(p.Memo);m_StatusPageLoaded=true;
 }
}
void KUiStatus::UpdateTitlePage(){
 PT_STATUS_PAGES_STATE state;if(!g_pCoreShell->GetGameData(PT_STATUS_PAGES_GET,(unsigned int)&state,sizeof(state)))return;
 static KTabFile titles;static bool titlesLoaded=false;if(!titlesLoaded)titlesLoaded=titles.Load("\\settings\\playertitlescheme.txt")!=0;if(!titlesLoaded)return;
 if(!m_StatusPageLoaded){
  m_TitleCombos[0].ClearItems();for(int row=2;row<=titles.GetHeight();++row){int id;titles.GetInteger(row,1,0,&id);if(id<=0||!PtStatusHasTitle(state.Titles,id))continue;char name[80];titles.GetString(row,6,"",name,sizeof(name));m_TitleCombos[0].AddItem(id,name);}
  m_TitleCombos[0].SelectId(state.CurrentTitle);m_TitleCombos[1].ClearItems();m_TitleButtons[3].Enable(false);m_StatusPageLoaded=true;
  if(state.Result<0)UIMessageBox("Xung hieu chua duoc mo khoa.");
 }
 int id=m_TitleCombos[0].SelectedId();if(id==m_LastTitleChoice)return;m_LastTitleChoice=id;
 m_TitleInfo.SetText("");m_TitleName.SetText("");m_TitlePreview.RemoveImage();m_TitleTextPreview.SetText("");
 for(int row=2;row<=titles.GetHeight();++row){int value;titles.GetInteger(row,1,0,&value);if(value!=id)continue;char info[1024],name[128],image[256];int type=0;
  titles.GetString(row,6,"",name,sizeof(name));titles.GetString(row,7,"",info,sizeof(info));titles.GetInteger(row,2,0,&type);titles.GetString(row,8,"",image,sizeof(image));
  char display[2048];PtStatusDisplayText(display,sizeof(display),info);m_TitleName.SetText(name[0]=='#'?name+1:name);m_TitleInfo.SetText(display);
  if(type==1&&image[0]){char path[260];if(image[0]=='\\')lstrcpyn(path,image,sizeof(path));else{path[0]='\\';lstrcpyn(path+1,image,sizeof(path)-1);}m_TitlePreview.SetImage(ISI_T_SPR,path,false);}else m_TitleTextPreview.SetText(name[0]=='#'?name+1:name);break;
 }
 m_TitleButtons[0].Enable(id>0);m_TitleButtons[2].Enable(id>0);
}
void KUiStatus::UpdatePtPages(){
 m_NormalStatusBtn.BringToTop();m_StatusTab.BringToTop();m_StatusProfileTab.BringToTop();
 for(int tab=0;tab<3;++tab)m_StatusPagesTabs[tab].BringToTop();
 if(!m_StatusPageMode)return;
 if(m_StatusPageMode==1){UpdateProfilePage();return;}
 if(m_StatusPageMode==3){UpdatePetPage();return;}
 if(m_StatusPageMode==4){UpdateTitlePage();return;}
 if(m_StatusPageMode==5){m_JEvilStatusBtn.BringToTop();UpdateRealmPage();return;}
 int selected=m_StatusPageList.GetCurSel();if(selected!=m_StatusPageSelected&&selected>=0&&selected<m_StatusPageCount){m_StatusPageSelected=selected;KTabFile table;char text[4096];text[0]=0;
  if(table.Load("\\settings\\item\\001\\compoundbook.txt"))table.GetString(m_StatusPageEntries[selected].Id,3,"",text,sizeof(text));char display[4096];PtStatusDisplayText(display,sizeof(display),text);m_StatusPageDescription.SetText(display);
  int last=m_StatusPageDescription.GetLineCount()-9;if(last<0)last=0;m_StatusDescScroll.SetValueRange(0,last);m_StatusDescScroll.SetScrollPos(0);
 }m_StatusPageDescription.SetTopLine(m_StatusDescScroll.GetScrollPos());
}
bool KUiStatus::PtPagesClick(unsigned int param){
 if(param==(unsigned int)(KWndWindow*)&m_StatusTab){if(m_StatusPageMode!=0)SelectPtPage(0);return true;}
 if(param==(unsigned int)(KWndWindow*)&m_StatusProfileTab){if(m_StatusPageMode!=1)SelectPtPage(1);return true;}
 if(param==(unsigned int)(KWndWindow*)&m_NormalStatusBtn){if(m_StatusPageMode!=0)SelectPtPage(0);return true;}
 for(int i=0;i<3;++i)if(param==(unsigned int)(KWndWindow*)&m_StatusPagesTabs[i]){if(m_StatusPageMode!=i+2)SelectPtPage(i+2);return true;}
 if(m_StatusPageMode==3&&PetPageClick(param))return true;
 if(m_StatusPageMode==2){for(i=0;i<4;++i)if(param==(unsigned int)(KWndWindow*)&m_BookArrows[i]){(i<2?m_StatusPageScroll:m_StatusDescScroll).ScrollLine(i%2==0);UpdatePtPages();return true;}}
 PT_STATUS_PAGES_REQUEST q;memset(&q,0,sizeof(q));
 if(m_StatusPageMode==1&&param==(unsigned int)(KWndWindow*)&m_StatusPageSave){
  q.Operation=PT_STATUS_SAVE_PROFILE;q.Profile.Sex=m_ProfileCombos[0].SelectedId();q.Profile.Age=m_ProfileCombos[1].SelectedId();sprintf(q.Profile.Relationship,"%d",m_ProfileCombos[2].SelectedId());q.Profile.Activity=m_ProfileCombos[3].SelectedId();q.Profile.Hour=m_ProfileCombos[4].SelectedId();
  q.Profile.BirthMonth=(int)m_ProfileFields[0].GetIntNumber();q.Profile.BirthDay=(int)m_ProfileFields[1].GetIntNumber();m_ProfileFields[2].GetText(q.Profile.Province,sizeof(q.Profile.Province),false);m_ProfileFields[3].GetText(q.Profile.Memo,sizeof(q.Profile.Memo),false);
  if(!PtStatusValidProfile(q.Profile)){UIMessageBox("Ngay sinh chua hop le.");return true;}
 }else if(m_StatusPageMode==4){
  if(param==(unsigned int)(KWndWindow*)&m_TitleButtons[0]){q.Operation=PT_STATUS_SELECT_TITLE;q.Value=m_TitleCombos[0].SelectedId();}
  else if(param==(unsigned int)(KWndWindow*)&m_TitleButtons[1])q.Operation=PT_STATUS_HIDE_TITLE;
  else if(param==(unsigned int)(KWndWindow*)&m_TitleButtons[2]){m_LastTitleChoice=-1;UpdateTitlePage();return true;}
 }
 if(!q.Operation)return false;g_pCoreShell->TeamOperation(PT_STATUS_PAGES_ACTION,(unsigned int)&q,sizeof(q));m_StatusPageLoaded=false;return true;
}
#include "UiPtPetPage.inl"
