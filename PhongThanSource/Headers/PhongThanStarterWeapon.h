#ifndef PHONGTHAN_STARTER_WEAPON_H
#define PHONGTHAN_STARTER_WEAPON_H
#include "PhongThanCharacter.h"
#include <string.h>
// Explicit item provenance, persisted in the existing item parameter field.
inline int PhongThanStarterWeaponClass(int param){return (unsigned)param>=0x53570001U&&(unsigned)param<=0x53570003U?param-0x53570001:-1;}
inline int PhongThanStarterWeaponRow(int profession){return profession==0?90:profession==1?100:profession==2?110:-1;}
inline bool PhongThanInitializeStarterWeapon(PHONGTHAN_CHARACTER_ITEM_RECORD& item,int profession){
 int row=PhongThanStarterWeaponRow(profession);if(row<0)return false;
 memset(&item,0,sizeof(item));item.SchemaVersion=PHONGTHAN_EMBEDDED_ITEM_VERSION;
 item.TemplateRow=row;item.Genre=0;item.DetailType=0;item.ParticularType=9+profession;
 item.Level=1;item.Series=0;item.Container=2;item.SlotX=3;item.SlotY=0;
 item.StackCount=1;item.Durability=-2;item.RandomSeed=1;item.TableVersion=1;
 item.ScriptParam=0x53570001+profession;return true;
}
#endif
