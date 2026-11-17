#include "UiPtCompanionHud.h"
#include "UiSkillTree.h"
#include "UiStatus.h"
#include "../../../../Headers/PhongThanStatusPages.h"
#include "../../../../Headers/PhongThanCompanionUi.h"
#include "../../../../Headers/PhongThanStatusText.h"
void KPtCompanionHud::Initialize(const char* scheme){
 // This container has no image: hit-test its visible children, never image alpha.
 KIniFile root;root.WriteInteger("Main","Left",6);root.WriteInteger("Main","Top",130);root.WriteInteger("Main","Width",220);root.WriteInteger("Main","Height",130);root.WriteInteger("Main","Trans",0);root.WriteInteger("Main","DummyWnd",1);Init(&root,"Main");
 for(int p=0;p<2;++p){char path[300];sprintf(path,"%s\\%s",scheme,p?"UiSummonHud.ini":"UiPetHud.ini");KIniFile ini;if(!ini.Load(path))continue;
  AddChild(&m_Panels[p]);m_Panels[p].Init(&ini,"Main");m_Panels[p].SetPosition(0,p?72:0);
  m_Panels[p].AddChild(&m_Name[p]);m_Name[p].Init(&ini,p?"Name_Text":"Pet_Name_Text");if(p)m_Name[p].SetPosition(53,4);
  m_Panels[p].AddChild(&m_Portrait[p]);m_Portrait[p].Init(&ini,p?"BtnPortrait":"Pet_Portrait");
  const char* bars[3]={"Pet_Health","Pet_Spirit","Pet_FeedValue"};
  for(int n=0;n<(p?1:3);++n){int index=p?3:n;char section[64];int x,y;const char* base=p?"Life":bars[n];ini.GetInteger(base,"Left",0,&x);ini.GetInteger(base,"Top",0,&y);
   sprintf(section,"%s_Image",base);m_Panels[p].AddChild(&m_Bars[index]);m_Bars[index].Init(&ini,section);int ix,iy;m_Bars[index].GetPosition(&ix,&iy);m_Bars[index].SetPosition(x+ix,y+iy);
   sprintf(section,"%s_Text",base);m_Panels[p].AddChild(&m_Values[index]);m_Values[index].Init(&ini,section);m_Values[index].GetPosition(&ix,&iy);m_Values[index].SetPosition(x+ix,y+iy);m_Values[index].SetSize(p?156:100,14);
   char tip[128];ini.GetString(base,"Tip","",tip,sizeof(tip));m_Values[index].SetToolTipInfo(tip[0]=='$'?tip+1:tip,strlen(tip[0]=='$'?tip+1:tip));
  }
  if(p){m_Panels[p].AddChild(&m_Level);m_Level.Init(&ini,"Level_Text");m_Level.SetPosition(28,5);m_Panels[p].AddChild(&m_AI);m_AI.Init(&ini,"AI");for(int tipIndex=0;tipIndex<2;++tipIndex){char tip[512];ini.GetString("AI",tipIndex?"CloseTip":"OpenTip","",tip,sizeof(tip));PtStatusDisplayText(m_AITips[tipIndex],sizeof(m_AITips[tipIndex]),tip);}m_AI.SetToolTipInfo(m_AITips[0],strlen(m_AITips[0]));}
  m_Panels[p].Hide();
 }
 Wnd_AddWindow(this,WL_NORMAL);Hide();
}
void KPtCompanionHud::Update(){
 if(!g_pCoreShell)return;for(int shortcut=0;shortcut<9;++shortcut)g_pCoreShell->PAIOperation(PT_PET_SHORTCUT,shortcut,KUiSkillTree::PetShortcut(shortcut),0);PT_STATUS_PAGES_STATE state;memset(&state,0,sizeof(state));bool pet=g_pCoreShell->GetGameData(PT_STATUS_PAGES_GET,(unsigned int)&state,sizeof(state))&&state.Pet.Owned;
 PT_COMPANION_HUD summon;memset(&summon,0,sizeof(summon));bool called=g_pCoreShell->GetGameData(PT_COMPANION_HUD_GET,(unsigned int)&summon,sizeof(summon))&&summon.Id;
 if(!pet&&!called){Hide();return;}Show();m_Panels[1].SetPosition(0,pet?72:0);
 if(pet){m_Panels[0].Show();m_Name[0].SetText(state.Pet.Name);m_Bars[0].SetPart(state.Pet.Strength,360000);m_Bars[1].SetPart(state.Pet.Spirit,300);m_Bars[2].Show();m_Values[2].Show();m_Bars[2].SetPart(state.Pet.FeedValue,300);
  char value[80];sprintf(value,"%d/100",state.Pet.Strength/3600);m_Values[0].SetText(value);sprintf(value,"%d/300",state.Pet.Spirit);m_Values[1].SetText(value);sprintf(value,"%d/300",state.Pet.FeedValue);m_Values[2].SetText(value);
  static KTabFile images;static bool loaded=false;if(!loaded)loaded=images.Load("\\settings\\petinfoex.txt")!=0;
  if(loaded&&state.Pet.Type>=0&&state.Pet.Type<images.GetHeight()-1){char path[256];images.GetString(state.Pet.Type+2,5,"",path,sizeof(path));if(strcmp(path,m_Portrait[0].GetImage()))m_Portrait[0].SetImage(ISI_T_SPR,path,false);}
 }else m_Panels[0].Hide();
 if(called){m_Panels[1].Show();m_Name[1].SetText(summon.Name);m_Level.SetIntText(summon.Level);m_Bars[3].SetPart(summon.Life,summon.MaxLife>0?summon.MaxLife:1);char life[64];sprintf(life,"%d/%d",summon.Life,summon.MaxLife);m_Values[3].SetText(life);m_AI.CheckButton(summon.Locked!=0);char* tip=m_AITips[summon.Locked?1:0];m_AI.SetToolTipInfo(tip,strlen(tip));
  if(summon.Portrait[0]&&strcmp(summon.Portrait,m_Portrait[1].GetImage()))m_Portrait[1].SetImage(ISI_T_SPR,summon.Portrait,false);
 }else m_Panels[1].Hide();
}
int KPtCompanionHud::WndProc(unsigned int msg,unsigned int param,int value){
 if(msg==WND_N_BUTTON_CLICK&&param==(unsigned int)(KWndWindow*)&m_Portrait[0]){KUiStatus::OpenPetPage();return 1;}
 if(msg==WND_N_BUTTON_CLICK&&param==(unsigned int)(KWndWindow*)&m_AI){PT_COMPANION_HUD state;memset(&state,0,sizeof(state));if(g_pCoreShell->GetGameData(PT_COMPANION_HUD_GET,(unsigned int)&state,sizeof(state))&&state.Id){PT_STATUS_PAGES_REQUEST q;memset(&q,0,sizeof(q));q.Operation=PT_STATUS_SUMMON_LOCK;q.Value=state.Locked?0:1;g_pCoreShell->TeamOperation(PT_STATUS_PAGES_ACTION,(unsigned int)&q,sizeof(q));}return 1;}
 return KWndImage::WndProc(msg,param,value);
}
