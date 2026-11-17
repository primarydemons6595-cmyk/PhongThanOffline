// Layout and button sprites come from the original UiStatus.ini.
#include "UiAutoPlay.h"
static const char* PtPetFlagSections[10]={"Pet_BtnAutoFeedPet","Pet_BtnAutoSkill","Pet_BtnChatWithMaster","Pet_BtnPickSomething","Pet_BtnCleanBadGoods","Pet_BtnSpecialRepaire","Pet_BtnTrainPet","Pet_BtnLookLife","Pet_BtnSpecialResume","Pet_BtnHide"};
static const char* PtPetLabelSections[10]={"Pet_TxtAutoFeedPet_Text","Pet_TxtAutoSkill_Text","Pet_TxtChatWithMaster_Text","Pet_TxtPickSomething_Text","Pet_TxtCleanBadGoods_Text","Pet_TxtSpecialRepaire_Text","Pet_TxtTrainPet_Text","Pet_TxtLookLife_Text","Pet_TxtSpecialResume_Text","Pet_TxtPet_Hide_Text"};
void KUiStatus::InitPetPage(KIniFile* ini){
 m_PetFoodIndex=-1;m_PetFoodSerial=0;m_PetChatTick=0;
 AddChild(&m_PetPage);m_PetPage.Init(ini,"PetPage_Main");
 m_PetPage.AddChild(&m_PetName);m_PetName.Init(ini,"Pet_TxtPetName");
 m_PetPage.AddChild(&m_PetPictureFrame);m_PetPictureFrame.Init(ini,"Pet_ImgPictureWnd");
 for(int slot=0;slot<4;++slot){char section[32];sprintf(section,"Pet_Slot_%d",slot);m_PetPage.AddChild(&m_PetSlots[slot]);m_PetSlots[slot].Init(ini,section);m_PetSlots[slot].Enable(true);}
 m_PetPage.AddChild(&m_PetPicture);KIniFile picture;PtPageControl(picture,"Preview",16,21,98,94);m_PetPicture.Init(&picture,"Preview");m_PetPicture.Enable(false);
 const char* attrLabels[4]={"Pet_TxtStrength","Pet_TxtDexterity","Pet_TxtConstitution","Pet_TxtIntellect"};
 for(int a=0;a<4;++a){m_PetPage.AddChild(&m_PetAttributeLabels[a]);m_PetAttributeLabels[a].Init(ini,attrLabels[a]);}
 const char* values[7]={"Pet_TxtStrengthContent","Pet_TxtDexterityContent","Pet_TxtConstitutionContent","Pet_TxtIntellectContent","Pet_TxtAptitudeContent","Pet_TxtSexContent","txtPetKind"};
 for(int i=0;i<7;++i){m_PetPage.AddChild(&m_PetValues[i]);m_PetValues[i].Init(ini,values[i]);}
 m_PetValues[5].SetPosition(10,120);m_PetValues[5].SetSize(215,14);
 m_PetValues[6].SetPosition(10,324);m_PetValues[6].SetSize(215,14);
 for(i=0;i<10;++i){
  m_PetPage.AddChild(&m_PetFlags[i]);m_PetFlags[i].Init(ini,PtPetFlagSections[i]);
  m_PetPage.AddChild(&m_PetLabels[i]);m_PetLabels[i].Init(ini,PtPetLabelSections[i]);
  m_PetLabels[i].SetPosition(i%2?143:54,211+(i/2)*19);m_PetLabels[i].SetSize(i%2?92:78,15);
 }
 m_PetPage.AddChild(&m_PetFeed);m_PetFeed.Init(ini,"Pet_BtnFeed");
 m_PetPage.AddChild(&m_PetRename);m_PetRename.Init(ini,"Pet_BtnChangeName");
 m_PetPage.AddChild(&m_PetLog);m_PetLog.Init(ini,"Pet_BtnLog");
 m_PetPage.AddChild(&m_PetFoodButton);m_PetFoodButton.Init(ini,"Pet_ObjBoxFeed1");m_PetFoodButton.SetSize(38,38);
 m_PetFoodButton.AddChild(&m_PetFoodImage);m_PetFoodImage.Init(ini,"Pet_ObjBoxFeed1");m_PetFoodImage.SetPosition(0,0);m_PetFoodImage.Enable(false);m_PetFoodImage.RemoveImage();
 KIniFile edit;PtPageControl(edit,"Name",10,101,145,16);edit.WriteString("Name","Color","255,217,78");
 m_PetPage.AddChild(&m_PetNameEdit);m_PetNameEdit.Init(&edit,"Name");m_PetNameEdit.SetLimitText(31);m_PetNameEdit.Hide();
 const char* selects[2]={"Pet_BtnPickSelect","Pet_BtnCleanSelect"};
 for(int select=0;select<2;++select){m_PetPage.AddChild(&m_PetSelectButtons[select]);m_PetSelectButtons[select].Init(ini,selects[select]);}
 for(i=0;i<4;++i){char section[40];sprintf(section,"Pet_ObjBoxSkill%d",i);m_PetPage.AddChild(&m_PetSkillImages[i]);m_PetSkillImages[i].Init(ini,section);m_PetPage.AddChild(&m_PetSkillIcons[i]);m_PetSkillIcons[i].Init(ini,section);m_PetSkillIcons[i].SetPosition(23+i*50,171);m_PetSkillIcons[i].SetSize(32,32);m_PetSkillIcons[i].RemoveImage();}
 InitPetOptions();InitPetLog();m_PetPage.Hide();
}
void KUiStatus::SelectPetFood(int advance){
 PT_PET_FOODS foods;if(!g_pCoreShell->GetGameData(PT_PET_FOODS_GET,(unsigned int)&foods,sizeof(foods))||!foods.Count){m_PetFoodSerial=0;m_PetFoodImage.RemoveImage();return;}
 if(advance||m_PetFoodIndex<0)m_PetFoodIndex=(m_PetFoodIndex+1)%foods.Count;
 if(m_PetFoodIndex>=foods.Count)m_PetFoodIndex=0;
 const PT_PET_FOOD& food=foods.Items[m_PetFoodIndex];m_PetFoodSerial=food.Serial;
 m_PetFoodImage.SetImage(ISI_T_SPR,food.Image,false);
 char tip[256];lstrcpyn(tip,food.Description,sizeof(tip));m_PetFoodButton.SetToolTipInfo(tip,strlen(tip));
}
void KUiStatus::UpdatePetPage(){
 PT_STATUS_PAGES_STATE state;
 if(!g_pCoreShell->GetGameData(PT_STATUS_PAGES_GET,(unsigned int)&state,sizeof(state)))return;
 const PT_PET_DATA& pet=state.Pet;
 m_PetPicture.PetHue=pet.Hue;m_PetPicture.PetTransparency=pet.Transparency;
 if(!m_StatusPageLoaded){
  if(state.Result<0){
   const char* reason=state.Result==-2?"Kh«ng ®äc/l­u ®­îc Linh thó.":state.Result==-3?"Hµnh trang ®ang khãa hoÆc ®ang giao dÞch.":state.Result==-6?"Chøc n¨ng nµy ch­a ®­îc më.":state.Result==-7?"Linh thó cÇn Ýt nhÊt 29 ®iÓm søc kháe ®Ó hç trî. NÕu søc kháe b»ng 0, h·y dïng Phôc Ma Linh.":state.Result==-8?"Linh thó ch­a ®ñ thêi gian tr­ëng thµnh. Xem ®iÒu kiÖn trªn nót chøc n¨ng.":state.Result==-9?"Ch­a ®ñ linh lùc hoÆc søc kháe. Linh Nh·n cÇn 150 linh lùc; Hé chñ cÇn 200 linh lùc vµ 29 søc kháe.":state.Result==-10?"§æi tªn cÇn <color=cyan>2 T­íng Qu©n LÖnh<color> trong hµnh trang.":"Ch­a ®ñ ®iÒu kiÖn hoÆc nguyªn liÖu.";
   UIMessageBox(reason);
  }
  m_PetNameEdit.SetText(pet.Name);m_StatusPageLoaded=true;
 }
 m_PetName.SetText(pet.Owned?pet.Name:"Chua nhan nuoi Linh thu");
 for(int i=0;i<4;++i)m_PetValues[i].SetIntText(pet.Attributes[i]);{char aptitude[80];sprintf(aptitude,"Tu chat: %d",pet.Aptitude);m_PetValues[4].SetText(aptitude);}
 char text[160];sprintf(text,"Suc khoe: %d   Linh luc: %d",pet.Strength/3600,pet.Spirit);m_PetValues[5].SetText(text);m_PetValues[5].Hide();
 sprintf(text,"Truong thanh: %d gio   Ho chu: %d",pet.AgeSeconds/3600,pet.Protect);m_PetValues[6].SetText(text);m_PetValues[6].Hide();
 static KTabFile petSkills;static bool petSkillsLoaded=false;if(!petSkillsLoaded)petSkillsLoaded=petSkills.Load("\\settings\\skills.txt")!=0;
 for(int slot=0;slot<4;++slot){
  bool open=PtPetSlotOpen(pet,slot);if(open)m_PetSkillImages[slot].Hide();else m_PetSkillImages[slot].Show();
  m_PetSkillIcons[slot].RemoveImage();int id=pet.Skills[slot]&65535;
  if(open&&id&&petSkillsLoaded){for(int row=2;row<=petSkills.GetHeight();++row){int skill=0;petSkills.GetInteger(row,"SkillId",0,&skill);if(skill==id){char icon[256],name[80],tip[160];petSkills.GetString(row,"SkillIcon","",icon,sizeof(icon));petSkills.GetString(row,"SkillName","",name,sizeof(name));m_PetSkillIcons[slot].SetImage(ISI_T_SPR,icon,false);sprintf(tip,"%s +%d",name,pet.Skills[slot]>>16);m_PetSkillIcons[slot].SetToolTipInfo(tip,strlen(tip));break;}}}
 }
 const int supported=1023&~PT_PET_TRAIN;
 static const char* conditions[10]={"Tù ®éng cho ¨n Ph¸c Ngäc khi søc kháe d­íi 34.","Tr­ëng thµnh 25 giê; søc kháe Ýt nhÊt 29. Dïng c¸c kü n¨ng hç trî ®· ®Æt ë phÝm t¾t.","Tr­ëng thµnh 1 giê.","Tr­ëng thµnh 2 giê; søc kháe Ýt nhÊt 29. Chän lo¹i vËt phÈm t¹i ThËp kim kiÕm b¶o.","Tr­ëng thµnh 3 giê; søc kháe Ýt nhÊt 29. Chän bé läc t¹i KhÝ vËt thanh lý.","Tr­ëng thµnh 5 giê; søc kháe Ýt nhÊt 29. Tiªu hao b¹c ®Ó söa trang bÞ ®ang mÆc.","Tu luyÖn.","CÇn 150 linh lùc vµ søc kháe Ýt nhÊt 29.","CÇn 200 linh lùc vµ søc kháe Ýt nhÊt 29. ChÆn mét ®ßn chÝ m¹ng khi cßn l­ît Hé chñ trong ngµy.","Linh thó nghØ ng¬i, dõng c¸c chøc n¨ng hç trî."};
 for(i=0;i<10;++i){m_PetFlags[i].CheckButton((pet.Flags&(1<<i))!=0);m_PetFlags[i].Enable(pet.Owned&&(supported&(1<<i))!=0);m_PetFlags[i].SetToolTipInfo((char*)conditions[i],strlen(conditions[i]));m_PetLabels[i].SetToolTipInfo((char*)conditions[i],strlen(conditions[i]));}
 for(int profile=0;profile<4;++profile){m_PetSlots[profile].Enable(pet.Owned);m_PetSlots[profile].SetFrame(profile==KUiAutoPlay::CurrentPetProfile()?1:0);}
 m_PetFeed.Enable(pet.Owned);m_PetRename.Enable(pet.Owned);m_PetLog.Enable(pet.Owned);m_PetFoodButton.Enable(pet.Owned);
 if(pet.Owned){
  static KTabFile pictures;static bool picturesLoaded=false;if(!picturesLoaded)picturesLoaded=pictures.Load("\\settings\\petinfoex.txt")!=0;if(picturesLoaded&&pet.Type<pictures.GetHeight()-1){char image[256];pictures.GetString(pet.Type+2,1,"",image,sizeof(image));if(strcmp(m_PetPicture.GetImage(),image))m_PetPicture.SetImage(ISI_T_SPR,image,false);}
  SelectPetFood(0);
 }else m_PetPicture.RemoveImage();
 if(m_PetOptionsMode>=0)m_PetOptions[m_PetOptionsMode].BringToTop();
 if(m_PetLogVisible){UpdatePetLog();m_PetLogFrame.Show();m_PetLogFrame.BringToTop();}
}
bool KUiStatus::PetPageClick(unsigned int param){
 for(int profile=0;profile<4;++profile)if(param==(unsigned int)(KWndWindow*)&m_PetSlots[profile]){KUiAutoPlay::SelectPetProfile(profile);return true;}
 if(PetLogClick(param))return true;
 if(PetOptionsClick(param))return true;
 PT_STATUS_PAGES_REQUEST q;memset(&q,0,sizeof(q));
 for(int choice=0;choice<2;++choice)if(param==(unsigned int)(KWndWindow*)&m_PetSelectButtons[choice]){OpenPetOptions(choice);return true;}
 for(int i=0;i<10;++i)if(param==(unsigned int)(KWndWindow*)&m_PetFlags[i]){q.Operation=PT_PET_TOGGLE;q.Value=1<<i;break;}
 if(param==(unsigned int)(KWndWindow*)&m_PetFoodButton){SelectPetFood(1);return true;}
 if(param==(unsigned int)(KWndWindow*)&m_PetFeed){if(!m_PetFoodSerial){UIMessageBox("Can mang thuc an Linh thu trong hanh trang. Bam o thuc an de chon.");return true;}q.Operation=PT_PET_FEED;q.Value=(int)m_PetFoodSerial;}
 if(param==(unsigned int)(KWndWindow*)&m_PetRename){char name[32];m_PetNameEdit.GetText(name,sizeof(name),false);KUiGetString::OpenWindow(GSA_NORMAL,"Doi ten Linh thu",name,this,9001,0,1,31);return true;}
 if(param==(unsigned int)(KWndWindow*)&m_PetLog){
  OpenPetLog();return true;
 }
 if(!q.Operation)return false;
 g_pCoreShell->TeamOperation(PT_STATUS_PAGES_ACTION,(unsigned int)&q,sizeof(q));m_StatusPageLoaded=false;return true;
}
#include "UiPtPetLog.inl"

// These two windows use the original pet-rule INIs, including their checkbox art.
void KUiStatus::InitPetOptions(){
 m_PetOptionsMode=-1;char scheme[256];g_UiBase.GetCurSchemePath(scheme,sizeof(scheme));
 for(int page=0;page<2;++page){char path[320];sprintf(path,"%s\\%s",scheme,page?"UiPetSale.ini":"UiPetPick.ini");KIniFile ini;
  AddChild(&m_PetOptions[page]);if(!ini.Load(path)){m_PetOptions[page].Hide();continue;}
  m_PetOptions[page].Init(&ini,page?"PetSaleMain":"PetPickMain");m_PetOptions[page].SetPosition(238,0);
  int count=page?2:7;for(int n=0;n<count;++n){int id=page?7+n:n,sectionIndex=(!page&&n==6)?7:n;char section[64];
   sprintf(section,page?"Pet_SaleNoCheck%d":"Pet_PickNoCheck%d",sectionIndex);m_PetOptions[page].AddChild(&m_PetOptionChecks[id]);m_PetOptionChecks[id].Init(&ini,section);
   char label[80];ini.GetString(section,"Label","",label,sizeof(label));m_PetOptionChecks[id].SetLabel(label[0]=='$'?label+1:label);
   sprintf(section,page?"Pet_SaleNoImage%d":"Pet_PickNoImage%d",sectionIndex);m_PetOptions[page].AddChild(&m_PetOptionMarks[id]);m_PetOptionMarks[id].Init(&ini,section);
  }
  m_PetOptions[page].AddChild(&m_PetOptionClose[page]);m_PetOptionClose[page].Init(&ini,page?"Pet_SaleBtnClose":"Pet_PickBtnClose");
  m_PetOptions[page].AddChild(&m_PetOptionSave[page]);m_PetOptionSave[page].Init(&ini,page?"Pet_SaleBtnCancel":"Pet_PickBtnCancel");
  if(page){
   const char* labels[4]={"Pet_Txt_LevelLimit","Pet_Txt_AtrrLimit","Pet_Txt_LevelLimit1","Pet_Txt_AtrrLimit1"};
   for(n=0;n<4;++n){m_PetOptions[page].AddChild(&m_PetSaleLabels[n]);m_PetSaleLabels[n].Init(&ini,labels[n]);char text[80];ini.GetString(labels[n],"Text","",text,sizeof(text));m_PetSaleLabels[n].SetText(text[0]=='$'?text+1:text);}
   m_PetOptions[page].AddChild(&m_PetSaleIntro);m_PetSaleIntro.Init(&ini,"Pet_Intro");char intro[512];ini.GetString("Pet_Intro","Text","",intro,sizeof(intro));m_PetSaleIntro.SetText(intro[0]=='$'?intro+1:intro);
   const char* frames[2]={"Pet_SaleEditBackImageLevel","Pet_SaleEditBackImageAttr"};const char* edits[2]={"Pet_Edt_SaleLevelLimit","Pet_Edt_SaleAtrrLimit"};
   for(n=0;n<2;++n){m_PetOptions[page].AddChild(&m_PetSaleBorders[n]);m_PetSaleBorders[n].Init(&ini,frames[n]);m_PetOptions[page].AddChild(&m_PetSaleFields[n]);m_PetSaleFields[n].Init(&ini,edits[n]);m_PetSaleFields[n].SetLimitText(n?2:3);}
  }else{const char* logs[2]={"Pet_BtnSelfLog","Pet_BtnTeamLog"};for(n=0;n<2;++n){m_PetOptions[page].AddChild(&m_PetPickupLogs[n]);m_PetPickupLogs[n].Init(&ini,logs[n]);}m_PetPickupLogs[1].Enable(false);}
  m_PetOptions[page].Hide();
 }
}
void KUiStatus::OpenPetOptions(int page){
 PT_STATUS_PAGES_STATE state;if(!g_pCoreShell->GetGameData(PT_STATUS_PAGES_GET,(unsigned int)&state,sizeof(state)))return;
 m_PetOptionsMode=page;m_PetPickMask=state.Pet.PickExclude;m_PetSaleMask=state.Pet.SaleKeep;
 for(int i=0;i<7;++i)m_PetOptionMarks[i].SetFrame((m_PetPickMask&(1<<(i==6?7:i)))?1:0);
 for(i=0;i<2;++i)m_PetOptionMarks[7+i].SetFrame((m_PetSaleMask&(1<<i))?1:0);
 m_PetSaleFields[0].SetIntText(state.Pet.SaleLevel);m_PetSaleFields[1].SetIntText(state.Pet.SaleMagic);
 m_PetOptions[1-page].Hide();m_PetOptions[page].Show();m_PetOptions[page].BringToTop();
}
bool KUiStatus::PetOptionsClick(unsigned int param){
 if(m_PetOptionsMode<0)return false;int page=m_PetOptionsMode;
 if(param==(unsigned int)(KWndWindow*)&m_PetOptionClose[page]){m_PetOptions[page].Hide();m_PetOptionsMode=-1;return true;}
 for(int i=0;i<(page?2:7);++i){int id=page?7+i:i;if(param==(unsigned int)(KWndWindow*)&m_PetOptionChecks[id]){
  int bit=1<<(!page&&i==6?7:i);int& mask=page?m_PetSaleMask:m_PetPickMask;mask^=bit;m_PetOptionMarks[id].SetFrame((mask&bit)?1:0);return true;
 }}
 if(!page&&param==(unsigned int)(KWndWindow*)&m_PetPickupLogs[0])return PetPageClick((unsigned int)(KWndWindow*)&m_PetLog);
 if(param!=(unsigned int)(KWndWindow*)&m_PetOptionSave[page])return false;
 PT_STATUS_PAGES_REQUEST q;memset(&q,0,sizeof(q));
 if(page){int level=(int)m_PetSaleFields[0].GetIntNumber(),magic=(int)m_PetSaleFields[1].GetIntNumber();if(level<0||level>255||magic<0||magic>20){UIMessageBox("Gia tri gioi han chua hop le.");return true;}q.Operation=PT_PET_SALE_OPTIONS;q.Value=m_PetSaleMask|(level<<8)|(magic<<16);}
 else {q.Operation=PT_PET_PICK_OPTIONS;q.Value=m_PetPickMask;}
 g_pCoreShell->TeamOperation(PT_STATUS_PAGES_ACTION,(unsigned int)&q,sizeof(q));m_PetOptions[page].Hide();m_PetOptionsMode=-1;m_StatusPageLoaded=false;return true;
}
