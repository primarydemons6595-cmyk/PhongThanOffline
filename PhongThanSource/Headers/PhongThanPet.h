#ifndef PHONGTHAN_PET_H
#define PHONGTHAN_PET_H
#include <string.h>
// Reconstructed server state. Item names/descriptions remain in original tables.
enum { PT_PET_RENAME=10, PT_PET_TOGGLE=11, PT_PET_FEED=12,PT_PET_PICK_OPTIONS=14,PT_PET_SALE_OPTIONS=15,PT_PET_PROFILE_OPTIONS=17 };
enum { PT_PET_FOODS_GET=0x50545352,PT_PET_SHORTCUT=0x50545053 };
#ifndef _SERVER
void PhongThanSetPetShortcut(int slot,int skill);
#endif
enum { PT_PET_AUTO_FEED=1, PT_PET_AUTO_SKILL=2, PT_PET_CHAT=4,
 PT_PET_PICK=8, PT_PET_SELL=16, PT_PET_REPAIR=32, PT_PET_TRAIN=64,
 PT_PET_SEE_LIFE=128, PT_PET_PROTECT=256, PT_PET_REST=512 };
#pragma pack(push,1)
struct PT_PET_LOG {unsigned int Time;int Event,Value;};
struct PT_PET_FOOD {unsigned int Serial;int Detail;char Name[80],Image[128],Description[256];};
struct PT_PET_FOODS {int Count;PT_PET_FOOD Items[64];};
struct PT_PET_DATA {
 int Owned,Type,Strength,Spirit,AgeSeconds,Protect,Day,Flags;
 char Name[32];int Attributes[4],Aptitude,Sex,Skills[4];
 PT_PET_LOG Log[8];
 int PickExclude,SaleKeep,SaleLevel,SaleMagic;char PickName[64];
 int FeedValue,FeedDay,FeedCount;
 int Hue,Transparency;
 int PurifyStage,BaseAttributes[4];
};
#pragma pack(pop)
inline bool PtPetValid(const PT_PET_DATA& p){
 if(p.PurifyStage<0||p.PurifyStage>2||p.Sex<0||p.Sex>2||p.Aptitude<0||p.Aptitude>100)return false;
 for(int a=0;a<4;++a)if(p.Attributes[a]<0||p.Attributes[a]>10000||p.BaseAttributes[a]<0||p.BaseAttributes[a]>10000||p.Skills[a]<0||(p.Skills[a]&&((p.Skills[a]&65535)<1||(p.Skills[a]&65535)>4095||(p.Skills[a]>>16)<1||(p.Skills[a]>>16)>4)))return false;
 if(p.Hue<0||p.Hue>359||p.Transparency<0||p.Transparency>205)return false;
 if(p.FeedValue<0||p.FeedValue>300||p.FeedDay<0||p.FeedCount<0||p.FeedCount>5)return false;
 if(p.Owned<0||p.Owned>1||p.Type<0||p.Type>1023||p.Strength<0||p.Strength>360000||
    p.Spirit<0||p.Spirit>300||p.AgeSeconds<0||p.Protect<0||p.Protect>255||
    p.Flags<0||(p.Flags&~1023)||p.Name[31]||p.PickExclude<0||p.PickExclude>255||p.SaleKeep<0||p.SaleKeep>3||p.SaleLevel<0||p.SaleLevel>255||p.SaleMagic<0||p.SaleMagic>20||p.PickName[63])return false;
 for(int i=0;i<31&&p.Name[i];++i){unsigned char c=(unsigned char)p.Name[i];if(c<32||c==127||c=='<'||c=='>')return false;}
 for(i=0;i<63&&p.PickName[i];++i){unsigned char c=(unsigned char)p.PickName[i];if(c<32||c==127||c=='<'||c=='>')return false;}
 return true;
}
inline bool PtPetSlotOpen(const PT_PET_DATA& p,int slot){return p.Owned&&p.PurifyStage>0&&slot>=0&&slot<4&&(slot==0||(slot==1&&p.Spirit>=100)||(slot==2&&p.Spirit>=300)||(slot==3&&p.PurifyStage==2));}
inline int PtPetSkillBonus(const PT_PET_DATA& p,int skill){
 if(!p.Owned||p.Strength<=0||(p.Flags&PT_PET_REST))return 0;
 for(int i=0;i<4;++i)if(PtPetSlotOpen(p,i)&&(p.Skills[i]&65535)==skill)return p.Skills[i]>>16;return 0;
}
int PhongThanPetSkillBonus(int player,int skill);
inline int PtPetAttributeBonus(const PT_PET_DATA& p,int index){
 return index>=0&&index<4&&p.Owned&&p.PurifyStage>0&&p.Strength>0&&!(p.Flags&PT_PET_REST)?p.Attributes[index]:0;
}
inline unsigned int PtPetCombatSignature(const PT_PET_DATA& p){
 unsigned int hash=2166136261U;
 for(int i=0;i<4;++i){hash=(hash^PtPetAttributeBonus(p,i))*16777619U;
  hash=(hash^((p.Owned&&p.Strength>0&&!(p.Flags&PT_PET_REST)&&PtPetSlotOpen(p,i))?p.Skills[i]:0))*16777619U;}
 return hash;
}
int PhongThanPetAttributeBonus(int player,int index);
#ifdef _SERVER
bool PhongThanPetRead(int player,PT_PET_DATA* out);
int PhongThanPetAction(int player,int operation,int value,const char* name);
int PhongThanPetUseItem(int player,int item);
void PhongThanPetTick(int player);
void PhongThanPetRelease(int player);
int PhongThanPetType(int player);
int PhongThanPetVisualStyle(int npc,int* hue,int* transparency);
bool PhongThanPetTryProtect(int player);
#endif
#endif
