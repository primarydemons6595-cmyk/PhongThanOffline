// Retain the existing production socket transaction tests, then exercise the
// material roll that must survive polishing, inventory, save/load and inlay.
#define PT_SOCKET_TEST_MAIN LegacySocketTests
#include "LeiSocketTests.cpp"
#undef PT_SOCKET_TEST_MAIN
int main(int argc,char** argv){
 if(LegacySocketTests(argc,argv))return 1;
 KPakList packs;assert(packs.Open("\\package.ini"));g_pPakList=&packs;g_SetPakFileMode(1);
 CHECK(PtGem::Tables());int rolls=0;
 for(int gem=1;gem<=53;++gem){
  int row=PtGem::Row(gem);CHECK(row>=2);
  for(unsigned pick=0;pick<100;++pick){
   int low=0,high=0;CHECK(PtGem::Roll(gem,pick,0,low));PT_SOCKET_ATTRIBUTE lo,hi;
   CHECK(PtGem::Decode(low,gem,lo));int col=9+((((unsigned)low)>>20)&15)*5;
   CHECK(PtGem::Roll(gem,pick,PtGem::Num(row,col+2)-PtGem::Num(row,col+1),high));
   CHECK(PtGem::Decode(high,gem,hi)&&hi.Type==lo.Type&&hi.Value[0]==PtGem::Num(row,col+2));
   CHECK(lo.Value[0]==PtGem::Num(row,col+1));CHECK(PtGem::MaximumValue(gem,hi.Type,hi.Value[0]));CHECK(PtGem::MaximumValue(gem,lo.Type,lo.Value[0])==(lo.Value[0]==hi.Value[0]));
   KItem material;material.SetGenre(3);material.SetDetailType(PtGem::Num(row,3));material.SetParticular(PtGem::Num(row,4));material.SetLevel(PtGem::Num(row,5));
   CHECK(PtGem::Apply(material,low));CHECK(PtGem::Maximum(material,0)==(lo.Value[0]==hi.Value[0]));
   CHECK(PtGem::Apply(material,high)&&PtGem::Maximum(material,0)&&!PtGem::Maximum(material,1));
   PHONGTHAN_CHARACTER_ITEM_RECORD stored;ZeroMemory(&stored,sizeof(stored));stored.Genre=material.GetGenre();stored.DetailType=material.GetDetailType();stored.ParticularType=material.GetParticularType();stored.Level=material.GetLevel();stored.ScriptParam=material.GetParam();
   KItem restored;restored.SetGenre(stored.Genre);restored.SetDetailType(stored.DetailType);restored.SetParticular(stored.ParticularType);restored.SetLevel(stored.Level);
   CHECK(PtGem::Restore(restored,stored.ScriptParam)&&restored.m_aryMagicAttrib[0].nAttribType==hi.Type&&restored.m_aryMagicAttrib[0].nValue[0]==hi.Value[0]);
   PtSocketRules::Plan plan;ZeroMemory(&plan,sizeof(plan));plan.Gem=gem;plan.GemParam=high;plan.Position=0;plan.Before.Holes[0]=1;
   PT_SOCKET_STATE after;CHECK(PtSocketRules::Resolve(plan,9999,99,999999,after));
   CHECK(after.Attributes[0].Type==hi.Type&&after.Attributes[0].Value[0]-after.Bonus[0]==hi.Value[0]);
   CHECK(!PtGem::Decode(high,gem==53?1:gem+1,hi));
   ++rolls;
  }
  KItem old;old.SetGenre(3);old.SetDetailType(PtGem::Num(row,3));old.SetParticular(PtGem::Num(row,4));old.SetLevel(PtGem::Num(row,5));
  CHECK(PtGem::Restore(old,-1));int migrated=old.GetParam();CHECK(PtGem::Restore(old,-1)&&old.GetParam()==migrated);CHECK(PtGem::Restore(old,0)&&old.GetParam()==migrated);
 }
 CHECK(PtGem::Unpolished(253)&&!PtGem::Unpolished(281));CHECK(!PtGem::Identity(3,253,0,0));CHECK(!PtGem::Tagged(-1));
 printf("PASS polished gems %d low/max rolls, green max star predicate, canonical param, deterministic legacy migration, exact inlay without reroll\n",rolls);g_pPakList=0;return 0;
}
