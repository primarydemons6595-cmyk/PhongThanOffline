#include "../../../../Headers/PhongThanLifeSkills.h"
#include "../../../../Headers/PhongThanLifeText.h"
#include "../../../../Headers/PhongThanStatusPages.h"
#include "../Elem/WndPureTextBtn.h"
#include "UiLifeProduction.inl"
// Recovered VNG living-skill window. This is separate from the combat selector.
class KUiLifeSkills : public KWndImage {
 KIniFile m_Ini;KWndButton m_Close,m_Skill[9];KWndText80 m_Name;KWndText512 m_Intro,m_Content;
 KWndText32 m_Level,m_EnergyLabel,m_Energy,m_StrengthLabel,m_Strength;
 KWndPureTextBtn m_Action,m_Soul;KWndText80 m_Progress;int m_Selected;
 static KUiLifeSkills* s_Window;
 void Text(KWndText& window,const char* section){char text[512];m_Ini.GetString(section,"Text","",text,sizeof(text));int length=PhongThanEncodeLifeText(text);window.SetText(text,length);}
 void Select(int at){m_Selected=at;PT_LIFE_SKILLS_VIEW state;memset(&state,0,sizeof(state));if(g_pCoreShell)g_pCoreShell->GetGameData(PT_LIFE_SKILLS_GET,(unsigned int)&state,0);const int ids[9]={1,3,4,10,9,8,5,6,7};char rank[32];if(state.Levels[ids[at]]>0)sprintf(rank,"%d",state.Levels[ids[at]]);else strcpy(rank,"--");m_Level.SetText(rank);char key[40];for(int i=0;i<9;++i)m_Skill[i].CheckButton(i==at);sprintf(key,"Live_Name_%d",at);Text(m_Name,key);sprintf(key,"Introduce_%d",at);Text(m_Intro,key);sprintf(key,"Content_%d",at);Text(m_Content,key);}
 bool Initialize(){
  if(!m_Ini.LoadPakEntry(0x8D484BA4UL))return false;
  KWndImage::Init(&m_Ini,"Main");m_Close.Init(&m_Ini,"CloseBtn");AddChild(&m_Close);
  for(int i=0;i<9;++i){char key[32];sprintf(key,"LiveBtn_%d",i);m_Skill[i].Init(&m_Ini,key);AddChild(&m_Skill[i]);}
  m_Name.Init(&m_Ini,"Live_Name");m_Intro.Init(&m_Ini,"Live_Introduce");m_Content.Init(&m_Ini,"Live_Content");
  AddChild(&m_Name);AddChild(&m_Intro);AddChild(&m_Content);
  m_Level.Init(&m_Ini,"Live_Level");m_Level.SetText("--");AddChild(&m_Level);
  m_EnergyLabel.Init(&m_Ini,"Current_Energy_Text");m_Energy.Init(&m_Ini,"Current_Energy");
  m_StrengthLabel.Init(&m_Ini,"Current_Strength_Text");m_Strength.Init(&m_Ini,"Current_Strength");
  Text(m_EnergyLabel,"Current_Energy_Text");Text(m_StrengthLabel,"Current_Strength_Text");
  AddChild(&m_EnergyLabel);AddChild(&m_Energy);AddChild(&m_StrengthLabel);AddChild(&m_Strength);
  KIniFile controls;controls.WriteInteger("Action","Left",260);controls.WriteInteger("Action","Top",305);controls.WriteInteger("Action","Width",116);controls.WriteInteger("Action","Height",18);controls.WriteInteger("Action","Font",14);controls.WriteString("Action","Color","255,225,90");
  controls.WriteString("Action","OverColor","255,255,255");controls.WriteInteger("Action","CentreAlign",1);m_Action.Init(&controls,"Action");m_Action.SetText("\x54\x68\xf9\x63\x20\x68\x69\xd6\x6e");AddChild(&m_Action);
  controls.WriteInteger("Progress","Left",120);controls.WriteInteger("Progress","Top",329);controls.WriteInteger("Progress","Width",260);controls.WriteInteger("Progress","Height",18);controls.WriteInteger("Progress","Font",14);controls.WriteString("Progress","Color","235,255,200");m_Progress.Init(&controls,"Progress");AddChild(&m_Progress);
  controls.WriteInteger("Soul","Left",120);controls.WriteInteger("Soul","Top",305);controls.WriteInteger("Soul","Width",110);controls.WriteInteger("Soul","Height",18);controls.WriteInteger("Soul","Font",14);controls.WriteString("Soul","Color","255,225,90");controls.WriteString("Soul","OverColor","255,255,255");controls.WriteInteger("Soul","CentreAlign",1);m_Soul.Init(&controls,"Soul");m_Soul.SetText("\x48\x6f\x3f\x6e\x20\x48\x3f\x6e");AddChild(&m_Soul);
  Select(0);Wnd_AddWindow(this);Hide();return true;
 }
 int WndProc(unsigned int msg,unsigned int param,int data){
  if(msg==WND_N_BUTTON_CLICK&&param==(unsigned int)(KWndWindow*)&m_Soul){PT_STATUS_PAGES_REQUEST q;memset(&q,0,sizeof(q));q.Operation=PT_STATUS_SOUL_OPEN;q.Value=8;if(g_pCoreShell)g_pCoreShell->TeamOperation(PT_STATUS_PAGES_ACTION,(unsigned int)&q,sizeof(q));Hide();return 1;}
  if(msg==WND_N_BUTTON_CLICK&&param==(unsigned int)(KWndWindow*)&m_Action){
   const int ids[9]={1,3,4,10,9,8,5,6,7};if(ids[m_Selected]>=5){KUiLifeProduction::Open(ids[m_Selected]);Hide();return 1;}PT_STATUS_PAGES_REQUEST q;memset(&q,0,sizeof(q));q.Operation=PT_STATUS_LIFE_OPEN;q.Value=ids[m_Selected];
   if(g_pCoreShell)g_pCoreShell->TeamOperation(PT_STATUS_PAGES_ACTION,(unsigned int)&q,sizeof(q));return 1;
  }
  if(msg==WND_N_BUTTON_CLICK){if(param==(unsigned int)&m_Close){Hide();return 1;}for(int i=0;i<9;++i)if(param==(unsigned int)&m_Skill[i]){Select(i);return 1;}}
  if(msg==WM_KEYDOWN&&param==VK_ESCAPE){Hide();return 1;}return KWndImage::WndProc(msg,param,data);
 }
 void Breathe(){
  if(!g_pCoreShell)return;PT_LIFE_SKILLS_VIEW state;memset(&state,0,sizeof(state));if(!g_pCoreShell->GetGameData(PT_LIFE_SKILLS_GET,(unsigned int)&state,0))return;
  const int ids[9]={1,3,4,10,9,8,5,6,7};int skill=ids[m_Selected];char text[80];if(state.Levels[skill]>0)sprintf(text,"%d",state.Levels[skill]);else strcpy(text,"--");m_Level.SetText(text);
  sprintf(text,"%d/%d",state.Energy,state.MaxEnergy);m_Energy.SetText(text);sprintf(text,"%d/%d",state.Strength,state.MaxStrength);m_Strength.SetText(text);
  if(state.Levels[skill]<1){m_Progress.SetText("\x43\x68\xAD\x61\x20\x68\xE4\x63\x20\x6E\x67\x68\xD2\x20\x6E\xB5\x79");return;}
  int progress=state.Experience[skill];sprintf(text,"\x54\x68\x75\xc7\x6e\x20\x74\x68\xf4\x63\x3a\x20\x25\x64\x2e\x25\x30\x32\x64\x25\x25",progress/100,progress%100);m_Progress.SetText(text);
 }
public:
 static void Close(bool destroy){if(s_Window){if(destroy){s_Window->Destroy();s_Window=NULL;}else s_Window->Hide();}}
 static void Toggle(){if(!s_Window){s_Window=new KUiLifeSkills;if(!s_Window->Initialize()){delete s_Window;s_Window=NULL;return;}}if(s_Window->IsVisible())s_Window->Hide();else{s_Window->Show();s_Window->BringToTop();}}
};
KUiLifeSkills* KUiLifeSkills::s_Window=NULL;
void PhongThanToggleLifeSkills(){KUiLifeSkills::Toggle();}

void PhongThanCloseLifeSkills(bool destroy){KUiLifeProduction::Close(destroy);KUiLifeSkills::Close(destroy);}
