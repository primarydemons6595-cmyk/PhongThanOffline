// Offline editor bridge using the production generator and upgrade methods.
// Input/output: canonical item record (268) + companion entry (748).
#include "KCore.h"
#include "KPakList.h"
#include "KItemGenerator.h"
#include "KItemSet.h"
#include "PhongThanConsumableData.h"
#include "P0ItemMagicPersistence.h"
#include "PhongThanComposeRecipe.h"
#include <stdio.h>
KPakList* g_pPakList=0;
#pragma pack(push,1)
struct EditorItem { PHONGTHAN_CHARACTER_ITEM_RECORD record; P0ItemMagic::Entry entry; };
#pragma pack(pop)
typedef char EditorItemSize[(sizeof(EditorItem)==1016)?1:-1];
static int Fail(int code,const char* message){printf("UPGRADE_ERROR %s\n",message);return code;}
static bool Generate(const EditorItem& disk,KItem& item){
 const PHONGTHAN_CHARACTER_ITEM_RECORD& r=disk.record;
 ZeroMemory(&item,sizeof(item));
 item.m_CommonAttrib.nDetailType=r.DetailType;item.m_CommonAttrib.nParticularType=r.ParticularType;
 item.m_CommonAttrib.nLevel=r.Level;item.m_CommonAttrib.nSeries=r.Series;item.m_CommonAttrib.nRow=r.TemplateRow;
 item.m_GeneratorParam.nVersion=r.TableVersion;item.m_GeneratorParam.uRandomSeed=r.RandomSeed;item.m_GeneratorParam.nLuck=r.Luck;
 memcpy(item.m_GeneratorParam.nGeneratorLevel,r.GeneratorLevel,sizeof(r.GeneratorLevel));
 if(!ItemGen.Gen_ExistEquipmentByTemplateRow(r.DetailType,r.TemplateRow,r.Series,r.GeneratorLevel,r.Luck,r.TableVersion,&item))return false;
 item.m_GeneratorParam.uRandomSeed=r.RandomSeed;
 if(r.Durability!=-2)item.SetDurability(r.Durability);
 item.SetStackNum(r.StackCount);item.SetRow(r.TemplateRow);item.SetOwner(r.OwnerId);
 item.m_CommonAttrib.nPhysicVal=r.PhysicalValue;item.m_CommonAttrib.nMagicVal=r.MagicValue;
 if(!item.ApplyUpgradeState(r.UpgradeLevel))return false;
 std::string error;return P0ItemMagic::Restore(item,disk.entry,error);
}
static bool AllowedRule(KItem& item,int rule){
 KTabFile table;if(!table.Load("\\settings\\item\\001\\zhuang_bei_he_cheng_gui_ze_biao.txt"))return false;
 for(int row=2;row<=table.GetHeight();++row){PhongThanComposeRecipe r;
  if(!PhongThanReadComposeRecipe(table,row,r)||r.type!=1||r.group!=1||r.requiredRecipe[0]||r.attributeId!=rule)continue;
  if(r.inputs[0].genre==item_equip&&r.inputs[0].quantity==1&&r.inputs[0].detail==item.GetDetailType()&&r.inputs[0].part==item.GetParticular()&&r.output.genre==item_equip&&r.output.detail==r.inputs[0].detail&&r.output.part==r.inputs[0].part)return true;
 }return false;
}
int main(int argc,char** argv){
 if(argc!=6)return Fail(2,"arguments");
 const int level=atoi(argv[3]),rule=atoi(argv[4]);if(level<0||level>12||rule<0||rule>65535||(level>0&&rule==0))return Fail(3,"level/rule outside supported range");
 EditorItem input;FILE* f=fopen(argv[2],"rb");if(!f)return Fail(4,"input unreadable");bool ok=fread(&input,1,sizeof(input),f)==sizeof(input)&&fgetc(f)==EOF;fclose(f);
 if(!ok||input.record.SchemaVersion!=1||input.record.Genre!=item_equip||!input.entry.serial)return Fail(5,"input schema/genre");
 if(!SetCurrentDirectoryA(argv[1]))return Fail(6,"resource directory");
 g_SetRootPath(argv[1]);g_SetFilePath("\\");KPakList packs;if(!packs.Open("\\package.ini"))return Fail(7,"resource package");g_pPakList=&packs;g_SetPakFileMode(1);
 if(!ItemGen.Init())return Fail(8,"native generator init");
 KItem item;if(!Generate(input,item))return Fail(9,"input native reconstruction/companion mismatch");
 if(level>0&&item.m_CommonAttrib.nUpgradeLvl>0&&item.m_CommonAttrib.nUpgradeRule>0&&item.m_CommonAttrib.nUpgradeRule!=rule)return Fail(10,"restore to +0 before changing upgrade mode");
 if(level>0&&!AllowedRule(item,rule))return Fail(10,"rule does not belong to this equipment recipe");
 const int state=PhongThanEncodeUpgradeState(level,level?rule:0);if(state<0||!item.ApplyUpgradeState(state))return Fail(11,"native upgrade rejected");
 EditorItem output=input;output.record.UpgradeLevel=item.GetUpgradeState();P0ItemMagic::Capture(item,input.entry.ordinal,output.entry);
 if(output.entry.serial!=input.entry.serial||output.entry.quality!=input.entry.quality)return Fail(12,"serial/quality changed");
 // A fresh native reconstruction must accept the new companion. This also
 // catches upgrade types that would otherwise select a different base slot.
 KItem cold;if(!Generate(output,cold))return Fail(13,"upgraded item cannot reload natively");
 P0ItemMagic::Entry restored;P0ItemMagic::Capture(cold,output.entry.ordinal,restored);
 if(memcmp(&restored,&output.entry,sizeof(restored)))return Fail(14,"cold attributes differ");
 f=fopen(argv[5],"wb");if(!f)return Fail(15,"output create");ok=fwrite(&output,1,sizeof(output),f)==sizeof(output)&&fflush(f)==0;fclose(f);if(!ok)return Fail(16,"output write");
 printf("UPGRADE_OK from=%d to=%d rule=%d serial=%lu cold_reload=PASS\n",input.record.UpgradeLevel,output.record.UpgradeLevel,rule,(unsigned long)output.entry.serial);g_pPakList=0;return 0;
}
