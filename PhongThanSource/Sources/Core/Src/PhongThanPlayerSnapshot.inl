#include "../../../Headers/PhongThanEquipmentWorldPresentation.h"
#include "PhongThanNation.h"
#include "PhongThanEquipmentEffectTrace.inl"

void KNpc::BuildPhongThanPlayerSnapshot(PHONGTHAN_PLAYER_SNAPSHOT* snapshot, bool full)
{
    PhongThanNationSyncPlayer(m_nPlayerIdx);
	ZeroMemory(snapshot, sizeof(*snapshot));
	PhongThanInitializeWireHeader(&snapshot->Header, PHONGTHAN_MSG_WORLD_PLAYER_SNAPSHOT,
		sizeof(*snapshot), PHONGTHAN_WIRE_FLAG_RESPONSE, 0);
	snapshot->MapId = SubWorld[m_SubWorldIndex].m_SubWorldID;
	snapshot->EntityId = m_dwID;
	snapshot->FullSnapshot = full ? 1 : 0;
	snapshot->TeamId = Player[m_nPlayerIdx].m_cTeam.m_nFlag ? Player[m_nPlayerIdx].m_cTeam.m_nID : -1;
    PhongThanCopyTeamName(m_nPlayerIdx,snapshot->TeamName,sizeof(snapshot->TeamName));
	PhongThanEncodeVisualPart(snapshot->Helm, m_Appearance.Helm);
	PhongThanEncodeVisualPart(snapshot->Armor, m_Appearance.Armor);
	PhongThanEncodeVisualPart(snapshot->Weapon, m_Appearance.Weapon);
	PhongThanEncodeVisualPart(snapshot->PhiPhong, m_Appearance.PhiPhong);
	PhongThanEncodeVisualPart(snapshot->Horse, m_Appearance.Horse);
	snapshot->MaskResource = m_MaskType;
	snapshot->ClanRole = -1;
	if (Player[m_nPlayerIdx].m_cTong.m_nFlag)
	{
		snapshot->ClanId = Player[m_nPlayerIdx].m_cTong.m_dwTongNameID;
		snapshot->ClanRole = Player[m_nPlayerIdx].m_cTong.m_nFigure;
		snapshot->ClanEmblem = Player[m_nPlayerIdx].m_cTong.m_nTongNationalEmblem;
		strncpy(snapshot->ClanName, Player[m_nPlayerIdx].m_cTong.m_szName, sizeof(snapshot->ClanName) - 1);
		strncpy(snapshot->ClanTitle, Player[m_nPlayerIdx].m_cTong.m_szAgname, sizeof(snapshot->ClanTitle) - 1);
	}
	strncpy(snapshot->PartnerName, Player[m_nPlayerIdx].m_cTask.GetSaveStr(TASKVALUE_BASEDATA_MATENAME), sizeof(snapshot->PartnerName) - 1);
	snapshot->Mounted = m_bRideHorse ? 1 : 0;
	snapshot->TitleId = m_RankID;
	strncpy(snapshot->TitleName, m_CurExpandRank.szName, sizeof(snapshot->TitleName) - 1);
	snapshot->TitleColor = m_CurExpandRank.dwColor;
	snapshot->TitleGraphic = m_CurExpandRank.nStateGraphics;
	snapshot->TitleRemainingTime = m_CurExpandRank.dwLeftTime;
	snapshot->Transcendence = m_byTranslife;
	snapshot->VipRank = Player[m_nPlayerIdx].m_cTask.GetSaveVal(TASKVALUE_STATTASK_VIPRANK);
	snapshot->Reputation = Player[m_nPlayerIdx].m_cTask.GetSaveVal(TASKVALUE_STATTASK_REPUTE);
	snapshot->Fortune = Player[m_nPlayerIdx].m_cTask.GetSaveVal(TASKVALUE_STATTASK_FUYUAN);
	snapshot->PkMode = Player[m_nPlayerIdx].m_cPK.GetNormalPKState();
	snapshot->PkValue = Player[m_nPlayerIdx].m_cPK.GetPKValue();
	snapshot->Portrait = Player[m_nPlayerIdx].m_ImagePlayer;
	snapshot->FortuneRank = m_byFortuneRankLevel;
	snapshot->WorldRank = Player[m_nPlayerIdx].m_nWorldStat;
	snapshot->Profession = Player[m_nPlayerIdx].m_cProfession.GetProfession();
	const int nWeaponItem = Player[m_nPlayerIdx].m_ItemList.GetEquipment(itempart_weapon);
	if (nWeaponItem > 0 && nWeaponItem < MAX_ITEM && Item[nWeaponItem].GetID() > 0)
	{
		snapshot->EquipmentEffects[0] = PhongThanResolveWeaponWorldEffect(
			Item[nWeaponItem].m_CommonAttrib.nUpgradeLvl,
			Player[m_nPlayerIdx].m_cProfession.GetProfession());
	}
    PT_EQUIPMENT_GLOW_PIECE glowItems[itempart_num];int glowCount=0;
    for(int slot=0;slot<itempart_num;++slot) {
        int index=Player[m_nPlayerIdx].m_ItemList.GetEquipment(slot);
        if(index<=0 || index>=MAX_ITEM || !Item[index].GetID() || Item[index].GetGenre()!=item_equip)continue;
        KItem& item=Item[index];PT_EQUIPMENT_GLOW_PIECE& piece=glowItems[glowCount++];
        piece.SetId=item.GetSetID();piece.Group=item.GetGroup();piece.Quality=item.GetPhongThanQuality();
        piece.Tier=item.GetLevel();piece.Upgrade=item.m_CommonAttrib.nUpgradeLvl;piece.Realm=0;
        for(int req=0;req<6;++req) {
            const KMagicAttrib* value=(const KMagicAttrib*)item.GetRequirement(req);
            if(value && ((value->nAttribType==magic_requiregdlevel && value->nValue[0]>0) ||
                (value->nAttribType==magic_requiregdflag && value->nValue[0]>0 && !(value->nValue[0]&1))))piece.Realm=1;
        }
    }
    PT_EQUIPMENT_GLOW_RESULT glow;
    PhongThanSelectEquipmentGlow(glowItems,glowCount,snapshot->Profession,m_nSex?1:0,glow);
    snapshot->EquipmentEffects[1]=glow.UpgradeEffect;
    snapshot->EquipmentEffects[2]=glow.BaseEffect;
    PhongThanEquipmentEffectTrace(11,this,glow.Pieces,glow.AllUpgrade12,glow.UpgradeEffect,
        "SET_RESULT entity=%u player=%d set=%d group=%d family=%d pieces=%d all12=%d sex=%d upgrade=%u base=%u",
        m_dwID,m_nPlayerIdx,glow.SetId,glow.Group,glow.Family,glow.Pieces,glow.AllUpgrade12,m_nSex,
        glow.UpgradeEffect,glow.BaseEffect);
	snapshot->ShopOpen = Player[m_nPlayerIdx].m_PTrade.nTrade ? 1 : 0;
	snapshot->ShopDestination = Player[m_nPlayerIdx].m_PTrade.nDest;
	strncpy(snapshot->ShopName, Player[m_nPlayerIdx].m_PTrade.cName, sizeof(snapshot->ShopName) - 1);
	if (Player[m_nPlayerIdx].m_nPaceBarTime > 0 && Player[m_nPlayerIdx].m_nPaceBarTimeMax > 0)
	{
		int percent = (int)((__int64)Player[m_nPlayerIdx].m_nPaceBarTime * 100 / Player[m_nPlayerIdx].m_nPaceBarTimeMax);
		snapshot->ProgressPercent = percent > 100 ? 100 : percent;
	}
	snapshot->FightMode = m_FightMode ? 1 : 0;
	snapshot->Sleeping = Player[m_nPlayerIdx].m_bSleepMode ? 1 : 0;
}
