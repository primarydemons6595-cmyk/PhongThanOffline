#include "../../../../Headers/PhongThanLifeRecipeUi.h"
#include "../Elem/WndMessageListBox.h"
// Existing VNG production frame and controls; requests carry only a recipe ID.
class KUiLifeProduction:public KWndImage {
 KIniFile m_Ini;KWndButton m_Close,m_Tabs[6],m_Make,m_Icons[4],m_Output[2];
 KWndImage m_Slots[4],m_OutputSlots[2],m_Exp;
 KWndMessageListBox m_List;KWndScrollBar m_Scroll;
 KWndText80 m_Name,m_Level,m_Energy,m_Strength,m_Cost,m_Chance,m_CurrentEnergy,m_CurrentStrength,m_Proficiency;
 KWndText80 m_Count[4],m_OutputText[2];KWndText256 m_Warning;
 KWndText80 m_Labels[7];int m_Selected,m_Skill,m_LastSignature;DWORD m_Pending;
 static KUiLifeProduction* s_Window;
 void Text(KWndText& wnd,const char* section){char text[256];m_Ini.GetString(section,"Text","",text,sizeof(text));wnd.SetText(text,PhongThanEncodeLifeText(text));}
 void Select(int index){
  m_Selected=index;const PT_LIFE_RECIPE_UI* r=index>=0?&PT_LIFE_RECIPE_CATALOG[index]:0;
  m_Name.SetText(r?r->name:"\x43\x68\xad\x61\x20\x63\xe3\x20\x63\xab\x6e\x67\x20\x74\x68\xf8\x63\x20\x70\x68\xef\x20\x68\xee\x70");char value[80];
  for(int i=0;i<4;++i){m_Icons[i].RemoveImage();m_Count[i].SetText("");if(r&&r->icons[i][0]){m_Icons[i].SetImage(ISI_T_SPR,r->icons[i]);m_Icons[i].SetToolTipInfo((char*)r->needs[i],strlen(r->needs[i]));const char* qty=strrchr(r->needs[i],'x');m_Count[i].SetText(qty?qty:r->needs[i]);}}
  for(i=0;i<2;++i){m_Output[i].RemoveImage();m_OutputText[i].SetText("");if(r&&r->outputIcons[i][0]){m_Output[i].SetImage(ISI_T_SPR,r->outputIcons[i]);m_Output[i].SetToolTipInfo((char*)r->outputs[i],strlen(r->outputs[i]));sprintf(value,"%d%%",r->outputChance[i]);m_OutputText[i].SetText(value);}}
  sprintf(value,"%d",r?r->energy:0);m_Energy.SetText(value);sprintf(value,"%d",r?r->strength:0);m_Strength.SetText(value);
  sprintf(value,"\x4c\xad\xee\x6e\x67\x3a\x20\x25\x64",r?r->cost:0);m_Cost.SetText(value);m_Chance.SetText("");m_Make.Enable(r!=0);
 }
 void Refresh(bool force){
  if(!g_pCoreShell)return;PT_LIFE_SKILLS_VIEW view;memset(&view,0,sizeof(view));if(!g_pCoreShell->GetGameData(PT_LIFE_SKILLS_GET,(unsigned int)&view,0))return;
  int signature=view.Levels[m_Skill];for(int i=1;i<=128;++i)signature=signature*33+view.FormulaUses[i];
  if(force||signature!=m_LastSignature){int previous=m_Selected;m_LastSignature=signature;m_List.Clear();
   for(i=0;i<128;++i){const PT_LIFE_RECIPE_UI& r=PT_LIFE_RECIPE_CATALOG[i];if(view.Levels[m_Skill]<1||r.skill!=m_Skill||r.rank>view.Levels[m_Skill]||(!r.known&&view.FormulaUses[r.id]<=0))continue;m_List.AddOneMessage(r.name,strlen(r.name),0,i);}
   int row=0;for(i=0;i<m_List.GetMsgCount();++i)if(m_List.GetOneMessageData(i)==previous)row=i;
   m_List.SetCurSel(m_List.GetMsgCount()?row:-1);Select(m_List.GetMsgCount()?m_List.GetOneMessageData(row):-1);
  }
  char text[80];int exp=max(0,min(10000,view.Experience[m_Skill]));sprintf(text,"%d.%02d%%",exp/100,exp%100);m_Proficiency.SetText(text);if(view.Levels[m_Skill]>0)sprintf(text,"%d",view.Levels[m_Skill]);else strcpy(text,"--");m_Level.SetText(text);sprintf(text,"%d/%d",view.Energy,view.MaxEnergy);m_CurrentEnergy.SetText(text);sprintf(text,"%d/%d",view.Strength,view.MaxStrength);m_CurrentStrength.SetText(text);
  m_Warning.SetText(view.Levels[m_Skill]<1?"\x43\x68\xAD\x61\x20\x68\xE4\x63\x20\x6E\x67\x68\xD2\x20\x6E\xB5\x79\x2E\x20\x48\xB7\x79\x20\x67\xC6\x70\x20\x4E\x50\x43\x20\x64\xB9\x79\x20\x6E\x67\x68\xD2\x2E":m_Selected<0?"\x43\x68\xAD\x61\x20\x63\xE3\x20\x63\xAB\x6E\x67\x20\x74\x68\xF8\x63\x20\x70\x68\xEF\x20\x68\xEE\x70\x20\x76\xED\x69\x20\x63\xCA\x70\x20\x6E\x67\x68\xD2\x2E":"\x4D\xE7\x69\x20\x6C\xC7\x6E\x20\x31\x20\x6D\xCE\x2E\x20\x54\x68\xCA\x74\x20\x62\xB9\x69\x20\x76\xC9\x6E\x20\x6D\xCA\x74\x20\x6E\x67\x75\x79\xAA\x6E\x20\x6C\x69\xD6\x75\x2E");
  if(m_Selected>=0){const PT_LIFE_RECIPE_UI& r=PT_LIFE_RECIPE_CATALOG[m_Selected];sprintf(text,"%d%%",min(100,r.chance+(view.Levels[m_Skill]-r.rank)*r.bonus));m_Chance.SetText(text);}
  if(m_Pending){PT_STATUS_PAGES_STATE status;memset(&status,0,sizeof(status));if(g_pCoreShell->GetGameData(PT_STATUS_PAGES_GET,(unsigned int)&status,0)&&GetTickCount()-m_Pending>500){m_Pending=0;m_Make.Enable(m_Selected>=0);}}
 }
 void Category(int skill){m_Skill=skill;const int ids[6]={10,9,8,5,6,7};for(int i=0;i<6;++i)m_Tabs[i].CheckButton(ids[i]==skill);Refresh(true);}
 bool Initialize(){
  if(!m_Ini.LoadPakEntry(0x5D000061UL))return false;KWndImage::Init(&m_Ini,"Main");m_Close.Init(&m_Ini,"CloseBtn");AddChild(&m_Close);
  for(int i=0;i<6;++i){char section[40];sprintf(section,"ProduceBtn_%d",i);m_Tabs[i].Init(&m_Ini,section);AddChild(&m_Tabs[i]);}
  m_List.Init(&m_Ini,"Product_List");m_Scroll.Init(&m_Ini,"Scroll");m_List.SetScrollbar(&m_Scroll);AddChild(&m_List);AddChild(&m_Scroll);
  m_Make.Init(&m_Ini,"ProduceBtn_Single");AddChild(&m_Make);
  for(i=0;i<4;++i){char section[40];sprintf(section,"Item_Kuang_%d",i);m_Slots[i].Init(&m_Ini,section);AddChild(&m_Slots[i]);m_Ini.WriteInteger(section,"Up",0);m_Ini.WriteInteger(section,"Down",0);m_Ini.WriteInteger(section,"Over",0);m_Icons[i].Init(&m_Ini,section);m_Icons[i].RemoveImage();AddChild(&m_Icons[i]);sprintf(section,"Item_Num_%d",i);m_Count[i].Init(&m_Ini,section);AddChild(&m_Count[i]);}
  for(i=0;i<2;++i){char section[40];sprintf(section,"Product_Box_%d",i);m_OutputSlots[i].Init(&m_Ini,section);AddChild(&m_OutputSlots[i]);m_Ini.WriteInteger(section,"Up",0);m_Ini.WriteInteger(section,"Down",0);m_Ini.WriteInteger(section,"Over",0);m_Output[i].Init(&m_Ini,section);m_Output[i].RemoveImage();AddChild(&m_Output[i]);sprintf(section,"Produce_Percent_%d",i);m_OutputText[i].Init(&m_Ini,section);AddChild(&m_OutputText[i]);}
  const char* labels[7]={"Produce_name_0","Produce_energy_Text","Produce_Strength_Text","Current_Energy_Text","Current_Strength_Text","Live_proficiency","Produce_name_1"};
  for(i=0;i<7;++i){m_Labels[i].Init(&m_Ini,labels[i]);Text(m_Labels[i],labels[i]);AddChild(&m_Labels[i]);}m_Labels[6].Hide();m_Labels[3].SetText("\x54\x69\x6e\x68\x20\x6c\xf9\x63\x3a");m_Labels[4].SetText("\x54\x68\xd3\x20\x6c\xf9\x63\x3a");m_Labels[5].SetText("\x54\x68\x75\xc7\x6e\x20\x74\x68\xf4\x63");m_Labels[5].SetSize(64,18);m_Exp.Init(&m_Ini,"ExpBackground");AddChild(&m_Exp);
  m_Level.Init(&m_Ini,"Live_Level");AddChild(&m_Level);m_Energy.Init(&m_Ini,"Produce_energy");AddChild(&m_Energy);m_Strength.Init(&m_Ini,"Produce_Strength");AddChild(&m_Strength);
  m_CurrentEnergy.Init(&m_Ini,"Current_Energy");AddChild(&m_CurrentEnergy);m_CurrentStrength.Init(&m_Ini,"Current_Strength");AddChild(&m_CurrentStrength);m_Chance.Init(&m_Ini,"Percent_Success");AddChild(&m_Chance);
  KIniFile custom;const char* sections[4]={"Name","Cost","Warning","Proficiency"};const int tops[4]={164,246,184,231};const int widths[4]={280,280,280,190};
  for(i=0;i<4;++i){custom.WriteInteger(sections[i],"Left",i==3?290:210);custom.WriteInteger(sections[i],"Top",tops[i]);custom.WriteInteger(sections[i],"Width",widths[i]);custom.WriteInteger(sections[i],"Height",18);custom.WriteInteger(sections[i],"Font",i==2?11:12);custom.WriteString(sections[i],"Color","235,255,200");}
  m_Proficiency.Init(&custom,"Proficiency");AddChild(&m_Proficiency);m_Name.Init(&custom,"Name");AddChild(&m_Name);m_Cost.Init(&custom,"Cost");AddChild(&m_Cost);m_Warning.Init(&custom,"Warning");AddChild(&m_Warning);
  // Explicit one-at-a-time action. Do not enable the original All button until
  // a bounded server transaction exists for production batches.
  m_Warning.SetText("\x4d\xe7\x69\x20\x6c\xc7\x6e\x20\x31\x20\x6d\xce\x2e\x20\x54\x68\xca\x74\x20\x62\xb9\x69\x20\x76\xc9\x6e\x20\x6d\xca\x74\x20\x6e\x67\x75\x79\xaa\x6e\x20\x6c\x69\xd6\x75\x2e");
  m_Selected=-1;m_Skill=6;m_LastSignature=-1;m_Pending=0;Wnd_AddWindow(this);Hide();return true;
 }
 int WndProc(unsigned int msg,unsigned int param,int data){
  if(msg==WND_N_LIST_ITEM_ACTIVE&&param==(unsigned int)(KWndWindow*)&m_List){Select(m_List.GetOneMessageData(data));return 1;}
  if(msg==WND_N_BUTTON_CLICK){if(param==(unsigned int)&m_Close){Hide();return 1;}const int ids[6]={10,9,8,5,6,7};for(int i=0;i<6;++i)if(param==(unsigned int)&m_Tabs[i]){Category(ids[i]);return 1;}
   if(param==(unsigned int)&m_Make&&m_Selected>=0&&!m_Pending){PT_STATUS_PAGES_REQUEST q;memset(&q,0,sizeof(q));q.Operation=PT_STATUS_LIFE_MAKE;q.Value=PT_LIFE_RECIPE_CATALOG[m_Selected].id;if(g_pCoreShell&&g_pCoreShell->TeamOperation(PT_STATUS_PAGES_ACTION,(unsigned int)&q,sizeof(q))){m_Pending=GetTickCount();m_Make.Enable(FALSE);}return 1;}}
  if(msg==WM_KEYDOWN&&param==VK_ESCAPE){Hide();return 1;}return KWndImage::WndProc(msg,param,data);
 }
 void Breathe(){Refresh(false);}
public:
 static void Open(int skill){if(!s_Window){s_Window=new KUiLifeProduction;if(!s_Window->Initialize()){delete s_Window;s_Window=0;return;}}s_Window->Category(skill);s_Window->Show();s_Window->BringToTop();}
 static void Close(bool destroy){if(s_Window){if(destroy){s_Window->Destroy();s_Window=0;}else s_Window->Hide();}}
};
KUiLifeProduction* KUiLifeProduction::s_Window=0;
