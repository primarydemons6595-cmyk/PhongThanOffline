#ifndef PT_P2_TEST_CHARACTER_SETUP_H
#define PT_P2_TEST_CHARACTER_SETUP_H
// Explicit user-requested combat support. Never invokes the drop/death path.
inline void P2TestCharacterSetup(LegacyDropDebugLogger& logger){
 if(!P2BlueScope())return;
 FILE* f=fopen("p2_combat_setup.request","r");if(!f)return;char nonce[65];int got=fscanf(f,"%64s",nonce);fclose(f);if(got!=1)return;
 if(GetFileAttributesA("p2_combat_setup.done")!=(DWORD)-1)return;
 int p=0;for(int i=1;i<MAX_PLAYER;++i){int n=Player[i].m_nIndex;if(n>0&&n<MAX_NPC&&Player[i].m_nNetConnectIdx>=0&&!strcmp(Player[i].AccountName,"p1blueonly")&&!strcmp(Npc[n].Name,"TestDropRate")){p=i;break;}}
 if(!p)return;
 if(!MoveFileA("p2_combat_setup.request","p2_combat_setup.running"))return;
 KPlayer& player=Player[p];KNpc& npc=Npc[player.m_nIndex];LegacyDrop::Fields log;log["nonce"]=nonce;log["scope"]="USER_REQUESTED_COMBAT_SUPPORT_NOT_DROP";log["role"]="TestDropRate";
 player.SetLevel(10);player.m_nSkillPoint=0;if(!npc.m_SkillList.GetLevel(27)&&player.m_cProfession.GetProfession()==0&&PhongThanNativeCanLearn(27,npc.m_SkillList,npc.m_Level))npc.m_SkillList.Add(27,1,0);player.SendSyncData_Skill();
 LegacyDropItemAdapter adapter;PhongThanQuestItemTuple tuple;int item=0;std::string error;bool ok=adapter.Init(ItemGen)&&adapter.Map(0,0,0,tuple);tuple.level=1;tuple.series=0;
 if(ok)ok=adapter.GenerateMapped(tuple,item,error);
 if(ok){int required=0;for(int r=0;r<6;++r){const KItemNormalAttrib* a=(const KItemNormalAttrib*)Item[item].GetRequirement(r);if(a->nAttribType==magic_requirelevel&&a->nValue[0]>required)required=a->nValue[0];}
  ok=Item[item].GetQuality()==equip_normal&&required<=npc.m_Level&&npc.m_Level==10;
  if(ok){POINT size;size.x=Item[item].GetWidth();size.y=Item[item].GetHeight();int added=0;
   if(!player.m_ItemList.GetEquipment(itempart_weapon)&&player.m_ItemList.CanEquip(item,itempart_weapon))added=player.m_ItemList.Add(item,pos_equip,itempart_weapon,0,false);
   else added=player.m_ItemList.Add(item,size,false);
   ok=added>0;log["serial"]=P2Blue::Num(Item[item].GetID());log["tuple"]="0/0/0";log["native_level"]=P2Blue::Num(Item[item].GetLevel());log["required_level"]=P2Blue::Num(required);log["quality"]=P2Blue::Num(Item[item].GetQuality());log["equipped"]=player.m_ItemList.GetEquipment(itempart_weapon)==item?"1":"0";
  }if(!ok)ItemSet.Remove(item);
 }
 player.UpdataCurData();npc.m_CurrentLife=npc.m_CurrentLifeMax;npc.m_CurrentMana=npc.m_CurrentManaMax;player.m_uMustSave=SAVE_REQUEST;
 log["player_level"]=P2Blue::Num(npc.m_Level);log["starter_skill_27_level"]=P2Blue::Num(npc.m_SkillList.GetLevel(27));log["unspent_skill_points"]=P2Blue::Num(player.m_nSkillPoint);log["result"]=ok?"READY":"SUPPORT_ITEM_FAILED";log["error"]=error;logger.Write("TEST_SUPPORT_ITEM_GRANT","",log);
 f=fopen(ok?"p2_combat_setup.done":"p2_combat_setup.failed","w");if(f){fprintf(f,"nonce=%s level=%d skill_points=%d serial=%lu result=%s\n",nonce,npc.m_Level,player.m_nSkillPoint,item>0?Item[item].GetID():0,ok?"READY":"FAILED");fclose(f);}DeleteFileA("p2_combat_setup.running");
}
#endif
