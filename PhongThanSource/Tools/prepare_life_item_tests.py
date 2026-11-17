from pathlib import Path
ROOT=Path(__file__).resolve().parents[2];SOURCE=ROOT/'PhongThanSource'
s=(SOURCE/'Tests/Native/SpecialBuffTests.cpp').read_text('utf8').split('int main(){')[0]
s=s.replace('int m_dwID;', 'int m_dwID;')
s=s.replace('struct ItemType{','struct ItemType{int GetDetailTypeX(){return detail;}')
s=s.replace('struct NpcType','struct NpcType')
# Reuse only the established inventory/store mocks. The tests call the exact
# production item consumer and exact buff application routines below.
s=s.replace('int m_CurrentLife,attrs[400];','int m_CurrentLife,attrs[400];')
s=s.replace('struct PlayerType{','struct PlayerType{')
s=s.replace('struct NpcType{DWORD m_dwID;int m_CurrentLife,attrs[400];','struct NpcType{DWORD m_dwID;int m_CurrentLife,m_CurrentLifeMax,m_CurrentMana,m_CurrentManaMax,m_Index,m_Level,attrs[400];struct {int nTime;}m_FreezeState;void SendSyncData(int,BOOL){}')
s=s.replace('#include "../../Sources/Core/Src/PhongThanSpecialBuffStats.inl"','#include "../../Sources/Core/Src/PhongThanSpecialBuffStats.inl"\n#include "../../Sources/Core/Src/PhongThanLifeItemStats.inl"')
s=s.replace('PtOriginalItemBuffApply(*this);','PtOriginalItemBuffApply(*this);PtLifeItemBuffApply(*this);')
s=s.replace('#include "SpecialBuffHandbook.inl"','#include "SpecialBuffHandbook.inl"\n#include "../../Sources/Core/Src/PhongThanLifeItemUse.inl"')
s=s.replace('enum{SAVE_DOING', 'enum{magic_life_v=87,magic_mana_v=91};\nenum{SAVE_DOING')
s=s.replace('struct PHONGTHAN_IBBUFF_ENTRY{','typedef NpcType KNpc;\nstruct PHONGTHAN_IBBUFF_ENTRY{',1)
s+=r"""
int main(){int tested=0;
 for(int i=0;i<sizeof(ptLifeItemUses)/sizeof(ptLifeItemUses[0]);++i){const PTLifeItemUse& d=ptLifeItemUses[i];Reset();Item[1].detail=d.detail;Item[1].part=d.part;Item[1].count=3;Npc[1].m_Index=1;Npc[1].m_Level=200;Npc[1].m_CurrentLifeMax=1000;Npc[1].m_CurrentLife=100;Npc[1].m_CurrentManaMax=1000;Npc[1].m_CurrentMana=100;
  Npc[1].m_Level=d.level-1;if(d.level>0)assert(!PhongThanUseLifeItem(1,1)&&Item[1].count==3);Npc[1].m_Level=200;
  Player[1].trading=true;assert(!PhongThanUseLifeItem(1,1)&&Item[1].count==3);Player[1].trading=false;
  removeFailure=true;assert(!PhongThanUseLifeItem(1,1)&&Item[1].count==3);removeFailure=false;
  if(d.kind==1){assert(PhongThanUseLifeItem(1,1)==1&&Item[1].count==2);int slot=FindIBBuffSlot(GetPlayerIBBuffStore(1),d.buff);assert(slot>=0&&g_PhongThanPlayerIBBuff[1].Entry[slot].dwExpireTime==now+d.seconds);
   for(int a=0;a<12;++a)if(d.attrs[a][0])assert(Npc[1].attrs[d.attrs[a][0]]==d.attrs[a][1]);
   Player[1].UpdataCurData();for(a=0;a<12;++a)if(d.attrs[a][0])assert(Npc[1].attrs[d.attrs[a][0]]==d.attrs[a][1]);
   assert(PhongThanUseLifeItem(1,1)==1&&Item[1].count==1);slot=FindIBBuffSlot(GetPlayerIBBuffStore(1),d.buff);assert(g_PhongThanPlayerIBBuff[1].Entry[slot].nTimes==1);
   for(a=0;a<12;++a)if(d.attrs[a][0])assert(Npc[1].attrs[d.attrs[a][0]]==d.attrs[a][1]);
   now+=d.seconds;Player[1].UpdataCurData();for(a=0;a<12;++a)if(d.attrs[a][0])assert(Npc[1].attrs[d.attrs[a][0]]==0);
  }else if(d.kind==2){assert(PhongThanUseLifeItem(1,1)==1&&Item[1].count==2);}
  else if(d.kind==3){assert(!PhongThanUseLifeItem(1,1)&&Item[1].count==3);Npc[1].m_FreezeState.nTime=100;assert(PhongThanUseLifeItem(1,1)==1&&Item[1].count==2&&!Npc[1].m_FreezeState.nTime);}
  else if(d.kind==4){assert(!PhongThanUseLifeItem(1,1)&&Item[1].count==3);assert(AddIBBuffToStore(GetPlayerIBBuffStore(1),1,1217,600,0));assert(PhongThanUseLifeItem(1,1)==1&&Item[1].count==2&&FindIBBuffSlot(GetPlayerIBBuffStore(1),1217)<0);}
  ++tested;
 }
 printf("PASS all %d crafted IB outputs: exact effects/time/particular, level/trade/remove failure, refresh without stacking, rebuild/expiry, instant recovery and cures\n",tested);return 0;
}
"""
(SOURCE/'Tests/Native/LifeItemUseTests.generated.cpp').write_text(s,'utf8')
