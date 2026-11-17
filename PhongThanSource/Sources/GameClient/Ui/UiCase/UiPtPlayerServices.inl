#include "../../../../Headers/PhongThanPlayerServices.h"
#include "../../../Core/Src/PhongThanInventoryUi.h"
#include "../Elem/WndEdit.h"
#include "../Elem/WndLabeledButton.h"
#include "UiItem.h"
#include "UiGetString.h"
#include "UiMsgCentrePad.h"
#include "UiInformation.h"
class KPtMailFrame : public KWndImage {
public:int WndProc(unsigned int m,unsigned int p,int v){
 if((m==WND_N_BUTTON_CLICK||m==WND_N_SCORLLBAR_POS_CHANGED)&&m_pParentWnd)return m_pParentWnd->WndProc(m,p,v);
 return KWndImage::WndProc(m,p,v);
}};
class KPtPlayerServices : public KWndImage {
 KIniFile ini;KPtMailFrame panel;
 KWndButton close,inbox,compose,send,cancel,read,claim,erase,previous,next,attachments[9];
 KWndImage title,frames[9],pictures[9];KWndText80 labels[4];KWndText256 status;
 KWndEdit80 recipient,subject,money;KWndEdit512 body;
 KWndButton rows[6];KWndText256 rowTexts[6];unsigned int ids[9],rowIds[6],selected,lastSequence,lastPoll,lastInvite;
 int previews[9],mode,page;char reportTarget[32];unsigned int reportId;
 bool ready;
 void Query(int operation,unsigned int mail=0,const char* target=0,unsigned int targetId=0){
  PT_PS_COMMAND q;ZeroMemory(&q,sizeof(q));q.Operation=operation;q.MailId=mail;q.TargetId=targetId;
  if(target)lstrcpyn(q.Target,target,32);g_pCoreShell->TeamOperation(PT_PS_ACTION,(unsigned int)&q,sizeof(q));
 }
 void ReleasePreviews(){for(int i=0;i<9;++i){if(previews[i])g_pCoreShell->TeamOperation(PT_PS_RELEASE,previews[i],0);previews[i]=0;pictures[i].RemoveImage();}}
 void Page(int value){
  mode=value;ReleasePreviews();
  KWndWindow* fields[]={&recipient,&subject,&body,&money,&send};for(int i=0;i<5;++i){fields[i]->Enable(value==1);if(value)fields[i]->Show();else fields[i]->Hide();}
  for(i=0;i<4;++i){if(value)labels[i].Show();else labels[i].Hide();}
  for(i=0;i<9;++i){if(value){attachments[i].Show();frames[i].Show();pictures[i].Show();}else{attachments[i].Hide();frames[i].Hide();pictures[i].Hide();}}
  for(i=0;i<6;++i){if(!value)rows[i].Show();else rows[i].Hide();}
  KWndWindow* buttons[]={&read,&claim,&erase,&previous,&next};for(i=0;i<5;++i){if(!value||(value==2&&(i==1||i==2)))buttons[i]->Show();else buttons[i]->Hide();}
  inbox.CheckButton(value==0);compose.CheckButton(value==1);title.Init(&ini,value==1?"SendMail_Title":"InMail_Title");
  if(value==1){recipient.Enable(true);subject.Enable(true);body.Enable(true);money.Enable(true);send.Show();}
  if(value==2){send.Hide();recipient.Enable(false);subject.Enable(false);body.Enable(false);money.Enable(false);for(i=0;i<9;++i)attachments[i].Enable(false);}
  else for(i=0;i<9;++i)attachments[i].Enable(true);
 }
 void List(const PT_PS_STATE& state){
  if(page*6>=state.Count)page=0;
  for(int i=0;i<6;++i){int n=page*6+i;rowIds[i]=0;char text[192];text[0]=0;
   if(n<state.Count){const PT_PS_MAIL_HEADER& h=state.Mails[n];rowIds[i]=h.Id;_snprintf(text,sizeof(text)-1,"%.28s\n%.28s%s",h.Subject,h.Sender,h.Claimed==1?" (Æ∑ nhÀn)":"");text[sizeof(text)-1]=0;}
   rowTexts[i].SetText(text);rows[i].Enable(rowIds[i]!=0);}
 }
 void ShowMail(const PT_PS_MAIL& mail){
  Page(2);recipient.SetText(mail.Sender);subject.SetText(mail.Subject);body.SetText(mail.Body);money.SetIntText(mail.Money);selected=mail.Id;
  for(int i=0;i<mail.Count&&i<9;++i){previews[i]=g_pCoreShell->TeamOperation(PT_PS_PREVIEW,(unsigned int)&mail.Items[i],sizeof(PT_PS_ITEM));
   char image[256];image[0]=0;if(previews[i]&&g_pCoreShell->GetGameData(GDI_CHAT_ITEM_IMAGE,(unsigned int)image,previews[i]))pictures[i].SetImage(ISI_T_SPR,image,false);}
  status.SetText(mail.Claimed==1?"B≠u ki÷n Æ∑ nhÀn.":"Ch‰n NhÀn trong hÈp th≠ Æ” l y vÀt ph»m vµ ti“n.");
 }
public:
 KPtPlayerServices():selected(0),lastSequence(0),lastPoll(0),lastInvite(0),mode(0),page(0),ready(false){ZeroMemory(ids,sizeof(ids));ZeroMemory(rowIds,sizeof(rowIds));ZeroMemory(previews,sizeof(previews));reportTarget[0]=0;}
 bool Initialize(){
  char scheme[256],path[320];g_UiBase.GetCurSchemePath(scheme,sizeof(scheme));sprintf(path,"%s\\UiPtMail.ini",scheme);if(!ini.Load(path))return false;
  Init(&ini,"Main");SetPosition(260,120);AddChild(&title);title.Init(&ini,"InMail_Title");
  AddChild(&close);close.Init(&ini,"CloseBtn");AddChild(&inbox);inbox.Init(&ini,"InMail_Btn");AddChild(&compose);compose.Init(&ini,"SendMail_Btn");
  AddChild(&panel);panel.Init(&ini,"SendMail_Main");panel.SetSize(294,350);
  const char* keys[]={"SendMail_TxtNameTitle","SendMail_TxtHeaderTitle","SendMail_TxtMoneyTitle","SendMail_TxtMailPayTitle"};
  for(int i=0;i<4;++i){panel.AddChild(&labels[i]);labels[i].Init(&ini,keys[i]);}
  labels[3].SetSize(115,18);labels[3].SetPosition(150,206);labels[3].SetText("Ph›: 1000 l≠Óng");
  panel.AddChild(&recipient);recipient.Init(&ini,"SendMail_EdtName");panel.AddChild(&subject);subject.Init(&ini,"SendMail_EdtHeaderTitle");
  panel.AddChild(&body);body.Init(&ini,"SendMail_EdtContent");body.SetLimitText(256);
  panel.AddChild(&money);money.Init(&ini,"SendMail_EdtMoneyliang");money.SetPosition(174,255);money.SetSize(90,18);money.SetLimitText(9);money.SetIntText(0);
  panel.AddChild(&send);send.Init(&ini,"SendMail_BtnOK");panel.AddChild(&cancel);cancel.Init(&ini,"SendMail_BtnCloseMailBox");
  const char* commands[]={"InMail_BtnOpenMail","LookMail_BtnAllGet","InMail_BtnDelMail","InMail_BtnPrePage","InMail_BtnNextPage"};
  KWndButton* btn[]={&read,&claim,&erase,&previous,&next};
  for(i=0;i<5;++i){panel.AddChild(btn[i]);btn[i]->Init(&ini,commands[i]);btn[i]->SetPosition(i<3?i*90:60+(i-3)*120,i<3?275:315);}
  claim.SetToolTipInfo("NhÀn vÀt ph»m vµ ti“n",strlen("NhÀn vÀt ph»m vµ ti“n"));
  for(i=0;i<6;++i){char section[32];sprintf(section,"InMail_List_B%d",i+1);panel.AddChild(&rows[i]);rows[i].Init(&ini,section);rows[i].SetPosition(5,10+i*40);rows[i].SetSize(255,38);rows[i].AddChild(&rowTexts[i]);ini.WriteInteger("PtMailRow","Width",250);ini.WriteInteger("PtMailRow","Height",38);ini.WriteInteger("PtMailRow","Font",14);ini.WriteInteger("PtMailRow","MultiLine",1);ini.WriteInteger("PtMailRow","DummyWnd",1);ini.WriteString("PtMailRow","Color","255,255,204");rowTexts[i].Init(&ini,"PtMailRow");}
  for(i=0;i<9;++i){char section[24];sprintf(section,"Item%d",i+1);panel.AddChild(&frames[i]);frames[i].Init(&ini,section);frames[i].SetPosition(4+i%3*41,190+i/3*26);frames[i].SetSize(24,24);
   panel.AddChild(&pictures[i]);pictures[i].Init(&ini,section);pictures[i].RemoveImage();pictures[i].SetPosition(4+i%3*41,190+i/3*26);pictures[i].SetSize(24,24);
   panel.AddChild(&attachments[i]);attachments[i].Init(&ini,"InMail_List_B1");attachments[i].SetPosition(4+i%3*41,190+i/3*26);attachments[i].SetSize(24,24);attachments[i].SetToolTipInfo("B· vÀt ph»m kh·i b≠u ki÷n",24);}
  ini.WriteInteger("PtStatus","Left",8);ini.WriteInteger("PtStatus","Top",382);ini.WriteInteger("PtStatus","Width",305);ini.WriteInteger("PtStatus","Height",16);ini.WriteInteger("PtStatus","Font",12);ini.WriteString("PtStatus","Color","255,255,128");
  AddChild(&status);status.Init(&ini,"PtStatus");Wnd_AddWindow(this,WL_NORMAL);ready=true;Hide();return true;
 }
 void Reset(){ReleasePreviews();ZeroMemory(ids,sizeof(ids));lastSequence=lastPoll=lastInvite=0;Hide();}
 void Open(const char* target){
  if(!ready)return;ZeroMemory(ids,sizeof(ids));recipient.SetText(target?target:"");subject.SetText("");body.SetText("");money.SetIntText(0);selected=0;page=0;Page(target&&target[0]?1:0);
  status.SetText("ChuÈt ph∂i vÀt ph»m trong F4 Æ” Æ›nh kÃm.");Show();BringToTop();if(mode==0)Query(PT_PS_INBOX);else KUiItem::OpenWindow();
 }
 bool Put(const KUiDraggedObject* item,int container){
  if(!IsVisible()||mode!=1)return false;if(!item||!item->uId)return true;
  if(container!=UOC_ITEM_TAKE_WITH&&container!=UOC_ITEM_TAKE_WITH_EX&&container!=UOC_PT_BAG2&&container!=UOC_PT_BAG3)return true;
  unsigned int serial=g_pCoreShell->GetGameData(PHONGTHAN_GDI_ITEM_NETWORK_ID,0,item->uId);if(!serial)return true;
  for(int i=0;i<9;++i)if(ids[i]==serial)return true;
  for(i=0;i<9;++i)if(!ids[i]){ids[i]=serial;char image[256];image[0]=0;if(g_pCoreShell->GetGameData(GDI_CHAT_ITEM_IMAGE,(unsigned int)image,item->uId))pictures[i].SetImage(ISI_T_SPR,image,false);return true;}
  status.SetText("TËi Æa 9 vÀt ph»m.");return true;
 }
 void Action(const KUiPlayerItem& target,int operation){
  if(operation==PT_PS_SEND_MAIL){Open(target.Name);return;}
  if(operation==PT_PS_REPORT){lstrcpyn(reportTarget,target.Name,32);reportId=target.uId;KUiGetString::OpenWindow(GSA_NORMAL,"L˝ do b∏o c∏o","",this,0x7210,0,1,30);return;}
  Query(operation,0,target.Name,target.uId);
 }
 void Tick(){
  if(!g_pCoreShell||!ready)return;
  PT_PS_STATE* state=new PT_PS_STATE;bool got=g_pCoreShell->TeamOperation(PT_PS_GET,(unsigned int)state,sizeof(*state))!=0;
  if(got&&state->Header.Sequence!=lastSequence){lastSequence=state->Header.Sequence;
   char feeText[64];sprintf(feeText,"Ph›: %d l≠Óng",state->SendFee);labels[3].SetText(feeText);
   if(state->Operation==PT_PS_POSITION&&state->Result>0){char text[160];sprintf(text,"%s: khu v˘c %d, t‰a ÆÈ %d, %d",state->Target,state->TargetMap,state->X,state->Y);UIMessageBox(text);}
   else if(state->Operation==PT_PS_REPORT)UIMessageBox(state->Result>0?"ß∑ ghi nhÀn b∏o c∏o.":"Kh´ng ghi Æ≠Óc b∏o c∏o.");
   else if(state->Operation==PT_PS_INVITE_PUPIL)UIMessageBox(state->Result>0?"ß∑ gˆi lÍi mÍi nhÀn Æ÷ tˆ.":"Kh´ng th” nhÀn Æ÷ tˆ nµy.");
   else if(state->Operation==PT_PS_REPLY_PUPIL)UIMessageBox(state->Result>0?"ß∑ xˆ l˝ lÍi mÍi s≠ ÆÂ.":"LÍi mÍi kh´ng cﬂn hi÷u l˘c.");
   else{List(*state);if(state->Result<0){char text[96];sprintf(text,"Thao t∏c th t bπi (%d). Ki”m tra ti“n, kh„a vµ chÁ trËng.",state->Result);status.SetText(text);}
    else if(state->Operation==PT_PS_READ_MAIL)ShowMail(state->Mail);
    else if(state->Operation==PT_PS_SEND_MAIL){ZeroMemory(ids,sizeof(ids));Page(0);status.SetText("ß∑ gˆi; Æang l≠u nh©n vÀt Æ” giao b≠u ki÷n.");}
    else if(state->Operation==PT_PS_CLAIM_MAIL)status.SetText("ß∑ nhÀn; Æang l≠u nh©n vÀt.");}
   if(!state->PendingPupil)lastInvite=0;
   if(state->PendingPupil&&state->Message[0]&&PtPsHash(state->Message,strlen(state->Message))!=lastInvite){
    lastInvite=PtPsHash(state->Message,strlen(state->Message));lstrcpyn(reportTarget,state->Message,32);
    char titleText[80];sprintf(titleText,"%.31s nhÀn bπn lµm Æ÷ tˆ? 1=NhÀn",reportTarget);KUiGetString::OpenWindow(GSA_NUMBER_INPUT,titleText,"",this,0x7211,1,1,1);
   }
  }delete state;
  DWORD now=GetTickCount();if(now-lastPoll>2500){lastPoll=now;if(!KUiGetString::GetIfVisible())Query(PT_PS_INBOX);}
 }
 int WndProc(unsigned int msg,unsigned int param,int data){
  if(msg==WND_M_OTHER_WORK_RESULT&&data!=-1){PT_PS_COMMAND q;ZeroMemory(&q,sizeof(q));lstrcpyn(q.Target,reportTarget,32);
   if(param==0x7210){q.Operation=PT_PS_REPORT;q.TargetId=reportId;const char* text=*(const char**)data;if(text)lstrcpyn(q.Text,text,257);}
   else if(param==0x7211){q.Operation=PT_PS_REPLY_PUPIL;q.Value=data==1;}
   if(q.Operation)g_pCoreShell->TeamOperation(PT_PS_ACTION,(unsigned int)&q,sizeof(q));return 1;
  }
  if(msg==WND_N_BUTTON_CLICK){
   if(param==(unsigned int)(KWndWindow*)&close||param==(unsigned int)(KWndWindow*)&cancel){ReleasePreviews();Hide();return 1;}
   if(param==(unsigned int)(KWndWindow*)&inbox){Page(0);Query(PT_PS_INBOX);return 1;}
   if(param==(unsigned int)(KWndWindow*)&compose){Open("");Page(1);KUiItem::OpenWindow();return 1;}
   if(param==(unsigned int)(KWndWindow*)&send){PT_PS_COMMAND q;ZeroMemory(&q,sizeof(q));q.Operation=PT_PS_SEND_MAIL;recipient.GetText(q.Target,32,true);subject.GetText(q.Subject,32,false);body.GetText(q.Text,257,false);q.Money=money.GetIntNumber();memcpy(q.Items,ids,sizeof(ids));
    if(!q.Target[0]||!q.Subject[0]){status.SetText("NhÀp ng≠Íi nhÀn vµ ti™u Æ“.");return 1;}g_pCoreShell->TeamOperation(PT_PS_ACTION,(unsigned int)&q,sizeof(q));return 1;}
   if(param==(unsigned int)(KWndWindow*)&read&&selected){Query(PT_PS_READ_MAIL,selected);return 1;}
   if(param==(unsigned int)(KWndWindow*)&claim&&selected){Query(PT_PS_CLAIM_MAIL,selected);return 1;}
   if(param==(unsigned int)(KWndWindow*)&erase&&selected){Query(PT_PS_DELETE_MAIL,selected);return 1;}
   if(param==(unsigned int)(KWndWindow*)&previous||param==(unsigned int)(KWndWindow*)&next){page=max(0,min(2,page+(param==(unsigned int)(KWndWindow*)&next?1:-1)));Query(PT_PS_INBOX);return 1;}
   for(int i=0;i<6;++i)if(param==(unsigned int)(KWndWindow*)&rows[i]){selected=rowIds[i];for(int j=0;j<6;++j)rows[j].CheckButton(i==j);return 1;}
   for(i=0;i<9;++i)if(param==(unsigned int)(KWndWindow*)&attachments[i]){ids[i]=0;pictures[i].RemoveImage();return 1;}
  }return KWndImage::WndProc(msg,param,data);
 }
};
static KPtPlayerServices* s_PtPlayerServices=0;
static KPtPlayerServices* PtServices(){if(!s_PtPlayerServices){s_PtPlayerServices=new KPtPlayerServices;if(!s_PtPlayerServices->Initialize()){delete s_PtPlayerServices;s_PtPlayerServices=0;}}return s_PtPlayerServices;}
static void PhongThanRemotePeopleTick();
void PhongThanPlayerServicesBreathe(){PhongThanRemotePeopleTick();KPtPlayerServices* ui=PtServices();if(ui)ui->Tick();}
void PhongThanOpenMail(const char* target){KPtPlayerServices* ui=PtServices();if(ui)ui->Open(target);}
bool PhongThanMailPutInventoryItem(const KUiDraggedObject* item,int container){return s_PtPlayerServices&&s_PtPlayerServices->Put(item,container);}
static void PhongThanServicePeople(const KUiPlayerItem& player,int action){KPtPlayerServices* ui=PtServices();if(ui)ui->Action(player,action);}

void PhongThanClosePlayerServices(){if(s_PtPlayerServices)s_PtPlayerServices->Reset();}
