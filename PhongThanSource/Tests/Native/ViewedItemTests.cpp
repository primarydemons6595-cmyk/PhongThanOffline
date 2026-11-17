// Real KItem layout, production wire projection and upgrade/socket methods.
// Basic setters are isolated from renderer, inventory and live accounts.
#define main UpgradeStateUnitMain
#include "ItemUpgradeStateTests.cpp"
#undef main
#include "KProtocol.h"
#include "../../Headers/PhongThanForgeState.h"
#include "../../Headers/PhongThanPurpleEquipment.h"
#include "../../Sources/Core/Src/PhongThanSocketRegistry.inl"
void KItem::SetParam(int p){m_CommonAttrib.nParam=p;}
void KItem::SetDurability(int n){m_nCurrentDur=n;}
BOOL KItem::SetBaseAttrib(const KItemNormalAttrib* a){memcpy(m_aryBaseAttrib,a,sizeof(m_aryBaseAttrib));return TRUE;}
BOOL KItem::SetRequireAttrib(const KItemNormalAttrib* a){memcpy(m_aryRequireAttrib,a,sizeof(m_aryRequireAttrib));return TRUE;}
BOOL KItem::SetMagicAttrib(const KItemNormalAttrib* a){memcpy(m_aryMagicAttrib,a,sizeof(m_aryMagicAttrib));m_nCurrentDur=-1;return TRUE;}
#include "../../Sources/Core/Src/PhongThanViewedItem.inl"
static int checks=0;
#define CHECK(x) do{++checks;if(!(x)){printf("FAIL line=%d %s\n",__LINE__,#x);return 1;}}while(0)
int main(int argc,char** argv){
 if(argc!=2||!SetCurrentDirectoryA(argv[1]))return 2;
 g_SetRootPath(NULL);g_SetFilePath("\\");KPakList packs;CHECK(packs.Open("\\package.ini")!=0);g_pPakList=&packs;g_SetPakFileMode(1);
 CHECK(sizeof(VIEW_EQUIP_SYNC)<65535);
 VIEW_EQUIP_SYNC owner;ZeroMemory(&owner,sizeof(owner));owner.m_dwNpcID=123;owner.m_OwnerLevel=90;owner.m_OwnerMale=1;strcpy(owner.m_Owner.Name,"FarAwayPlayer");
 CHECK(PhongThanViewedOwnerValid(owner));owner.m_OwnerMale=2;CHECK(!PhongThanViewedOwnerValid(owner));owner.m_OwnerMale=1;
 memset(owner.m_Owner.Name,'x',sizeof(owner.m_Owner.Name));CHECK(!PhongThanViewedOwnerValid(owner));strcpy(owner.m_Owner.Name,"FarAwayPlayer");
 owner.m_OwnerLevel=0;CHECK(!PhongThanViewedOwnerValid(owner));owner.m_OwnerLevel=90;CHECK(PhongThanViewedOwnerValid(owner));
 for(int family=0;family<5;++family){
  KItem source;source.SetGenre(item_equip);source.SetDetailType(0);source.SetRow(71);source.SetLevel(1);
  strcpy(source.m_CommonAttrib.szItemName,family==2?"<c=pk>Tinh Qu\xA9" "n fixture":"Original equipment");
  source.m_CommonAttrib.nParam=family==1?PhongThanForgeParam(1):0;
  source.m_CommonAttrib.nSetID=family==0?301:0;source.m_CommonAttrib.nGroup=401;
  source.m_CommonAttrib.nNeedToActive1=3;source.m_CommonAttrib.nNeedToActive2=5;
  source.m_CommonAttrib.nPhysicVal=567;source.m_CommonAttrib.nMagicVal=789;
  source.m_aryBaseAttrib[0].nAttribType=28;source.m_aryBaseAttrib[0].nValue[0]=100;
  source.m_aryRequireAttrib[0].nAttribType=family==3?magic_requiregdlevel:36;source.m_aryRequireAttrib[0].nValue[0]=50;
  for(int a=0;a<20;++a){source.m_aryMagicAttrib[a].nAttribType=85+a;source.m_aryMagicAttrib[a].nValue[0]=100+a;source.m_aryMagicAttrib[a].nValue[1]=-a;source.m_aryMagicAttrib[a].nValue[2]=a*3;}
  CHECK(source.ApplyUpgradeState(PhongThanEncodeUpgradeState(12,1))!=0);
  source.SetDurability(37);source.SetFortune(17);source.SetExpireTime(2000000000);
  PT_SOCKET_STATE sockets;ZeroMemory(&sockets,sizeof(sockets));
  if(family==4){sockets.BaseParam=PhongThanForgeParam(1);sockets.Holes[0]=1;sockets.Gems[0]=1;sockets.Attributes[0].Type=121;sockets.Attributes[0].Value[0]=10;
   source.m_CommonAttrib.nParam=(int)0xc2000012U;CHECK(PtSocketRegister(source.GetParam(),sockets));}
  PHONGTHAN_VIEW_ITEM_INFO wire;ZeroMemory(&wire,sizeof(wire));wire.m_nID=123;wire.m_btGenre=item_equip;wire.m_btDetail=0;wire.m_nTemplateRow=71;wire.m_nUpgradeState=source.GetUpgradeState();
  CHECK(PhongThanWriteViewedItem(source,wire));
  unsigned char bytes[sizeof(wire)];memcpy(bytes,&wire,sizeof(wire));ZeroMemory(&wire,sizeof(wire));memcpy(&wire,bytes,sizeof(wire));
  PtSocketCache().clear();KItem view;view.SetGenre(item_equip);view.m_aryBaseAttrib[0].nAttribType=28;view.m_aryBaseAttrib[0].nValue[0]=100;
  CHECK(PhongThanReadViewedItem(view,wire));CHECK(view.GetParam()==source.GetParam());CHECK(view.GetUpgradeState()==source.GetUpgradeState());
  CHECK(!memcmp(view.P0BaseAttributes(),source.P0BaseAttributes(),sizeof(KItemNormalAttrib)*20));
  CHECK(!memcmp(view.TooltipRequirements(),source.TooltipRequirements(),sizeof(KItemNormalAttrib)*6));
  CHECK(!memcmp(view.m_aryMagicAttrib,source.m_aryMagicAttrib,sizeof(source.m_aryMagicAttrib)));
  CHECK(!strcmp(view.GetName(),source.GetName()));CHECK(view.GetSetID()==source.GetSetID()&&view.GetGroup()==source.GetGroup());
  CHECK(view.GetDurability()==37&&view.GetFortune()==17&&view.GetExpireTime()==2000000000);
  CHECK(view.GetNeedToActive1()==3&&view.GetNeedToActive2()==5&&view.GetBasePower()==567&&view.GetMagicPower()==789);
  if(family==1)CHECK(PhongThanForgeRecipe(view.GetParam())==1);
  if(family==2)CHECK(PhongThanIsTinhQuanEquipment(view.GetName()));
  if(family==3)CHECK(view.TooltipRequirements()[0].nAttribType==magic_requiregdlevel);
  if(family==4){PT_SOCKET_STATE restored;CHECK(PtSocketRead(view.GetParam(),restored));CHECK(!memcmp(&restored,&sockets,sizeof(sockets)));CHECK(PhongThanForgeRecipe(view.GetUnderlyingParam())==1);}
  CHECK(PhongThanReadViewedItem(view,wire));CHECK(!memcmp(view.P0BaseAttributes(),source.P0BaseAttributes(),sizeof(KItemNormalAttrib)*20));
  PHONGTHAN_VIEW_ITEM_INFO bad=wire;bad.m_WireVersion=PHONGTHAN_EMBEDDED_ITEM_VERSION;CHECK(!PhongThanViewedItemValid(bad));
  bad=wire;memset(bad.m_Name,'X',80);CHECK(!PhongThanViewedItemValid(bad));bad=wire;bad.m_Magic[19].Type=magic_normal_end;CHECK(!PhongThanViewedItemValid(bad));
  bad=wire;bad.m_nUpgradeState=0x41000101;CHECK(!PhongThanViewedItemValid(bad));bad=wire;bad.m_HasSockets=2;CHECK(!PhongThanViewedItemValid(bad));
 }
 printf("PASS VIEWED_ITEM %d checks: green/orange/TinhQuan/realm/socket; all attributes; no double upgrade; invalid wire rejection; packet=%u\n",checks,(unsigned)sizeof(VIEW_EQUIP_SYNC));g_pPakList=NULL;return 0;
}
