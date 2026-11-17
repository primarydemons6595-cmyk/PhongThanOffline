// Each role already has a private UI directory. Keep all four profiles there,
// apart from the legacy AutoPlay.ini which is cleared by its existing writer.
#include "UiSkillTree.h"
#include "../../../../Headers/PhongThanStatusPages.h"
static int s_PetProfile=0;
static bool PtPetProfilePath(int slot,char* path){
 if(slot<0||slot>3||!g_UiBase.GetUserPrivateDataFolder(path,256))return false;
 char file[32];sprintf(file,"pt_danh%d.ini",slot+1);strcat(path,file);return true;
}
int KUiAutoPlay::CurrentPetProfile(){return s_PetProfile;}
bool KUiAutoPlay::SavePetProfile(){
 if(!s_PtAutoPanel||!g_pCoreShell)return false;
 char path[300];if(!PtPetProfilePath(s_PetProfile,path))return false;
 KIniFile file;s_PtAutoPanel->Save(&file);KUiSkillTree::SaveConfig(&file);
 PT_STATUS_PAGES_STATE state;
 if(g_pCoreShell->GetGameData(PT_STATUS_PAGES_GET,(unsigned int)&state,sizeof(state))){
  file.WriteInteger("PetFilters","Pick",state.Pet.PickExclude);file.WriteInteger("PetFilters","Keep",state.Pet.SaleKeep);
  file.WriteInteger("PetFilters","Level",state.Pet.SaleLevel);file.WriteInteger("PetFilters","Magic",state.Pet.SaleMagic);
  file.WriteString("PetFilters","Name",state.Pet.PickName);
 }else{
  // A packet can be pending while saving. Retain the last authoritative filters.
  KIniFile previous;if(previous.Load(path)){
   const char* keys[]={"Pick","Keep","Level","Magic","Name"};
   for(int i=0;i<5;++i){char value[128];if(previous.GetString("PetFilters",keys[i],"",value,sizeof(value)))file.WriteString("PetFilters",keys[i],value);}
  }
 }
 file.WriteInteger("Profile","Version",1);if(!file.Save(path))return false;
 char manifest[300];g_UiBase.GetUserPrivateDataFolder(manifest,256);strcat(manifest,"pt_danh.ini");
 KIniFile selected;selected.WriteInteger("Profile","Selected",s_PetProfile);return selected.Save(manifest)!=0;
}
bool KUiAutoPlay::SelectPetProfile(int slot){
 if(slot<0||slot>3||!g_pCoreShell)return false;
 PT_STATUS_PAGES_STATE state;
 if(!g_pCoreShell->GetGameData(PT_STATUS_PAGES_GET,(unsigned int)&state,sizeof(state))||!state.Pet.Owned)return false;
 if(!SavePetProfile())return false;
 if(slot==s_PetProfile)return true;
 char path[300];if(!PtPetProfilePath(slot,path))return false;KIniFile file;
 if(!file.Load(path)){
  // A new slot starts as a copy of the current build, never an empty hotbar.
  s_PtAutoPanel->Save(&file);KUiSkillTree::SaveConfig(&file);
 }
 int active=g_pCoreShell->PAIOperation(GPI_ISACTIVE,0,0,0);
 s_PtAutoPanel->Load(&file);KUiSkillTree::LoadConfig(&file);s_PetProfile=slot;
 int pick,keep,level,magic;
 file.GetInteger("PetFilters","Pick",state.Pet.PickExclude,&pick);file.GetInteger("PetFilters","Keep",state.Pet.SaleKeep,&keep);
 file.GetInteger("PetFilters","Level",state.Pet.SaleLevel,&level);file.GetInteger("PetFilters","Magic",state.Pet.SaleMagic,&magic);
 if(pick>=0&&pick<=255&&keep>=0&&keep<=3&&level>=0&&level<=255&&magic>=0&&magic<=20){
  PT_STATUS_PAGES_REQUEST q;memset(&q,0,sizeof(q));q.Operation=PT_PET_PROFILE_OPTIONS;
  q.Value=pick|(keep<<8)|(level<<16)|(magic<<24);
  file.GetString("PetFilters","Name",state.Pet.PickName,q.Profile.Memo,64);
  file.WriteInteger("PetFilters","Pick",pick);file.WriteInteger("PetFilters","Keep",keep);
  file.WriteInteger("PetFilters","Level",level);file.WriteInteger("PetFilters","Magic",magic);
  file.WriteString("PetFilters","Name",q.Profile.Memo);file.Save(path);
  g_pCoreShell->TeamOperation(PT_STATUS_PAGES_ACTION,(unsigned int)&q,sizeof(q));
 }
 g_pCoreShell->PAIOperation(GPI_SWITCH_ACTIVE,active,0,0);SavePetProfile();return true;
}
void KUiAutoPlay::LoadPetProfiles(){
 s_PetProfile=0;char path[300];if(!g_UiBase.GetUserPrivateDataFolder(path,256))return;
 strcat(path,"pt_danh.ini");KIniFile manifest;int selected=0;
 if(manifest.Load(path))manifest.GetInteger("Profile","Selected",0,&selected);
 if(selected<0||selected>3)selected=0;s_PetProfile=selected;
 if(!PtPetProfilePath(selected,path))return;KIniFile file;
 if(file.Load(path)){PtAutoGet()->Load(&file);KUiSkillTree::LoadConfig(&file);}
}
