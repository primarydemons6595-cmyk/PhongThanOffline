from pathlib import Path
ROOT=Path(__file__).resolve().parents[2]/'PhongThanSource'
s=r'''
#include <windows.h>
#include <stdio.h>
#include <assert.h>
#include "../../Headers/PhongThanPet.h"
enum{item_ibitem=8,item_materials=3};
struct PetItem{int detail,count;int GetGenre(){return 3;}int GetDetailType(){return detail;}int GetParticular(){return 0;}int GetID(){return 101;}int GetStackNum(){return count;}}Item[3];
bool removeFailure=false,saveFailure=false;
struct PetBag{bool RemoveItem(int,int count){if(removeFailure)return false;Item[1].count-=count;return true;}};
struct PetPlayer{PetBag m_ItemList;bool GetLockState(){return false;}bool CheckTrading(){return false;}}Player[2];
struct PT_PET_RUNTIME{PT_PET_DATA Data;int Journal,Dirty,Loaded;DWORD SaveTick;}runtime;
PT_PET_RUNTIME* PtPetLoad(int){return &runtime;}
bool PtPetCarried(int,int){return Item[1].count>0;}
int PtPetSetting(const char*,const char*,int value){return value;}
void PtPetLog(PT_PET_DATA&,int,int){}
void PtPetInitialAttributes(PT_PET_DATA&,int){}
bool PtPetSave(int,const PT_PET_DATA&,DWORD=0,int=0,const PT_PET_DATA*=0);
bool PtPetSave(int,const PT_PET_DATA&,DWORD,int,const PT_PET_DATA*){return !saveFailure;}
int g_Random(int){return 0;}
'''.replace('const PT_PET_DATA*=','const PT_PET_DATA* =')
body=(ROOT/'Sources/Core/Src/PhongThanPetServer.inl').read_bytes().split(b'int PhongThanPetUseItem(',1)[1].split(b'int PhongThanPetAction(',1)[0]
s+='int PhongThanPetUseItem('+body.decode('ascii')
s+=r'''
void Reset(){ZeroMemory(&runtime,sizeof(runtime));runtime.Data.Owned=1;runtime.Data.Strength=360000;runtime.Data.PurifyStage=1;runtime.Data.Type=13;Item[1].detail=1184;Item[1].count=3;removeFailure=saveFailure=false;}
int main(){Reset();assert(PhongThanPetUseItem(1,1)&&Item[1].count==2&&runtime.Data.FeedValue==6);assert(PhongThanPetUseItem(1,1)&&Item[1].count==1&&runtime.Data.FeedValue==12);
 Reset();runtime.Data.FeedValue=299;assert(PhongThanPetUseItem(1,1)&&runtime.Data.FeedValue==300&&Item[1].count==2);assert(!PhongThanPetUseItem(1,1)&&Item[1].count==2);
 Reset();removeFailure=true;assert(!PhongThanPetUseItem(1,1)&&runtime.Data.FeedValue==0&&Item[1].count==3);
 Reset();saveFailure=true;assert(!PhongThanPetUseItem(1,1)&&runtime.Data.FeedValue==0&&Item[1].count==3);
 Reset();runtime.Data.Strength=0;assert(!PhongThanPetUseItem(1,1)&&Item[1].count==3);
 Reset();runtime.Data.PurifyStage=0;assert(!PhongThanPetUseItem(1,1)&&Item[1].count==3);
 puts("PASS crafted pet essence: repeated growth, cap/no charge when full, invalid pet, durable-intent/save/inventory failure no consumption");return 0;}
'''
(ROOT/'Tests/Native/CraftedPetTests.generated.cpp').write_text(s,'ascii')
