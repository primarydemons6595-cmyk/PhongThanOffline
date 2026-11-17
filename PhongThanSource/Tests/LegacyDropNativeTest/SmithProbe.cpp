#include "KCore.h"
#include "KPakList.h"
#include "LegacyDropItemAdapter.h"
#include "PhongThanConsumableData.h"
#include <stdio.h>
KPakList* g_pPakList=0;
int main(int argc,char**argv){if(argc!=2||!SetCurrentDirectoryA(argv[1]))return 2;g_SetRootPath(argv[1]);g_SetFilePath("\\");KPakList packs;if(!packs.Open("\\package.ini"))return 3;g_pPakList=&packs;g_SetPakFileMode(1);if(!ItemGen.Init())return 4;ItemSet.Init();LegacyDropItemAdapter adapter;if(!adapter.Init(ItemGen))return 5;
 const int parts[]={4,27,31};
 for(int n=0;n<3;++n){PhongThanQuestItemTuple t;t.genre=0;t.detail=0;t.particular=parts[n];t.level=4;t.series=0;t.luck=0;int index=0;std::string error;if(!adapter.GenerateMapped(t,index,error)){printf("GEN_FAIL %s\n",error.c_str());return 6;}KItem item=Item[index];
 const KBASICPROP_EQUIPMENT* row=ItemGen.CatalogEquipment(item.GetDetailType(),item.GetRow());printf("ITEM part=%d row=%d tier=%d upgrade=%d eligible=%d\n",item.GetParticularType(),item.GetRow(),item.GetLevel(),item.GetUpgradeState(),item.ImprovePhongThanProperty(-1));
 for(int j=5;j<MAX_ITEM_BASEATTRIB;++j)if(row->m_aryPropBasic[j].nType)printf("SOURCE slot=%d type=%d min=%d max=%d\n",j,row->m_aryPropBasic[j].nType,row->m_aryPropBasic[j].sRange.nMin,row->m_aryPropBasic[j].sRange.nMax);
 for(int m=0;m<MAX_ITEM_NORMAL_MAGICATTRIB;++m)if(item.m_aryMagicAttrib[m].nAttribType)printf("ACTUAL slot=%d type=%d values=%d,%d,%d\n",m,item.m_aryMagicAttrib[m].nAttribType,item.m_aryMagicAttrib[m].nValue[0],item.m_aryMagicAttrib[m].nValue[1],item.m_aryMagicAttrib[m].nValue[2]);
 if(parts[n]==31){
  const int minimum[]={1,50,1,5},maximum[]={10,100,10,15};for(int slot=0;slot<4;++slot)item.m_aryMagicAttrib[slot].nValue[0]=minimum[slot];
  if(!item.ApplyUpgradeState(PhongThanEncodeUpgradeState(12,1)))return 7;
  KItem original=item;int state=item.GetUpgradeState();if(!item.ImprovePhongThanProperty(-1))return 8;
  if(memcmp(&original,&item,sizeof(item)))return 9; // quote cannot mutate the item
  int changes=0;while(item.ImprovePhongThanProperty(-1)){if(!item.ImprovePhongThanProperty(0)||++changes>64)return 10;if(item.GetUpgradeState()!=state||item.GetParticularType()!=31||item.GetRow()!=313)return 11;}
  for(int capSlot=0;capSlot<4;++capSlot)if(item.m_aryMagicAttrib[capSlot].nValue[0]!=maximum[capSlot])return 12;
  for(int baseSlot=0;baseSlot<MAX_ITEM_BASEATTRIB;++baseSlot)if(memcmp(&original.P0BaseAttributes()[baseSlot],&item.P0BaseAttributes()[baseSlot],sizeof(KItemNormalAttrib)))return 13;
  KItem restored=original;
  if(!restored.SetMagicAttrib(item.m_aryMagicAttrib)||restored.GetQuality()!=item.GetQuality()||memcmp(restored.m_aryMagicAttrib,item.m_aryMagicAttrib,sizeof(item.m_aryMagicAttrib))||restored.GetUpgradeState()!=state)return 14;
  printf("PASS NV31 property regression: changes=%d preserved_strengthen12=1 correct_family_caps=1 quote_readonly=1 saved_magic_restore=1\n",changes);
 }
 ItemSet.Remove(index);
 }g_pPakList=0;return 0;}
