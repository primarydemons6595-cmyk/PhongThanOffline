#include "PhongThanPlayerServices.inl"
#include "PhongThanRemotePeople.inl"
#include "PhongThanStatusPages.inl"
#include "PhongThanRankingWire.inl"
#include "PhongThanAuctionWire.inl"
#include "../../../Headers/PhongThanItemLink.h"
// Native world messages are consumed before inventory/UI side effects.
#include "PhongThanTeamState.inl"
#ifndef _SERVER
#include "../../../Headers/PhongThanCombatLog.h"
#include "PhongThanPositionConsumer.inl"
#include "../../../Headers/PhongThanVttBarrierProtocol.h"
#include "PhongThanVttArena.h"
bool KProtocolProcess::ProcessPhongThanWorldPacket(BYTE* data, unsigned int size)
{
	const PHONGTHAN_WIRE_HEADER* header = (const PHONGTHAN_WIRE_HEADER*)data;
    if(header->MessageType==PT_COMBAT_LOG_EVENT){
        if(PTCombatLogValid(data,size)){
            const PTCombatLogEvent& event=*(const PTCombatLogEvent*)data;int actor=Player[CLIENT_PLAYER_INDEX].m_nIndex;
            if(actor>0&&actor<MAX_NPC&&event.MapId==(unsigned)SubWorld[0].m_SubWorldID&&event.Recipient==Npc[actor].m_dwID){
                PTCombatLogEvent shown=event;
                int ids[2]={NpcSet.SearchID(event.Attacker),NpcSet.SearchID(event.Target)};
                for(int side=0;side<2;++side)if(ids[side]>0&&ids[side]<MAX_NPC&&Npc[ids[side]].m_SubWorldIndex==Npc[actor].m_SubWorldIndex)
                    g_StrCpyLen(side?shown.TargetName:shown.AttackerName,Npc[ids[side]].Name,32);
                CoreDataChanged(PT_GDC_COMBAT_LOG,(unsigned int)&shown,sizeof(shown));
            }
        }return true;
    }

    if(header->MessageType==PT_SOUL_REPLY){
        if(PhongThanWorldFixedPacket(data,size,PT_SOUL_REPLY,PHONGTHAN_WIRE_FLAG_RESPONSE,sizeof(PT_SOUL_MESSAGE))){const PT_SOUL_MESSAGE* s=(const PT_SOUL_MESSAGE*)data;
            PT_SOUL_STATE old;bool fresh=!PtSoulRead(s->Param,old);if(PtSoulRegister(s->Param,s->State)&&fresh){
                for(int i=1;i<MAX_ITEM;++i)if(Item[i].GetGenre()==item_equip&&PtSoulInnerParam(Item[i].GetParam())==s->Param){KItemNormalAttrib magic[MAX_ITEM_MAGICATTRIB];memcpy(magic,Item[i].m_aryMagicAttrib,sizeof(magic));int dur=Item[i].GetDurability();Item[i].SetParam(Item[i].GetParam());Item[i].SetMagicAttrib(magic);Item[i].SetDurability(dur);}
                int actor=Player[CLIENT_PLAYER_INDEX].m_nIndex;if(actor>0&&actor<MAX_NPC)Player[CLIENT_PLAYER_INDEX].UpdataCurData();
            }
        }return true;
    }
    if(header->MessageType==PT_SOCKET_REPLY){
        if(PhongThanWorldFixedPacket(data,size,PT_SOCKET_REPLY,PHONGTHAN_WIRE_FLAG_RESPONSE,sizeof(PT_SOCKET_MESSAGE))){const PT_SOCKET_MESSAGE* s=(const PT_SOCKET_MESSAGE*)data;
            bool fresh=PtSocketCache().find(s->Param)==PtSocketCache().end();if(PtSocketRegister(s->Param,s->State)&&fresh){
                for(int i=1;i<MAX_ITEM;++i)if(Item[i].GetGenre()==item_equip&&Item[i].GetParam()==s->Param){KItemNormalAttrib magic[MAX_ITEM_MAGICATTRIB];memcpy(magic,Item[i].m_aryMagicAttrib,sizeof(magic));int dur=Item[i].GetDurability();Item[i].SetParam(s->Param);Item[i].SetMagicAttrib(magic);Item[i].SetDurability(dur);}
                int actor=Player[CLIENT_PLAYER_INDEX].m_nIndex;if(actor>0&&actor<MAX_NPC)Player[CLIENT_PLAYER_INDEX].UpdataCurData();
            }
        }return true;
    }
    if(header->MessageType==PT_VTT_BARRIER_STATE){
        if(PhongThanWorldFixedPacket(data,size,PT_VTT_BARRIER_STATE,PHONGTHAN_WIRE_FLAG_RESPONSE,sizeof(PT_VTT_BARRIERS)))PhongThanVttApplyBarrierState(data,size);
        return true;
    }
    if(header->MessageType==PT_PEOPLE_REPLY){
        if(PhongThanWorldFixedPacket(data,size,PT_PEOPLE_REPLY,PHONGTHAN_WIRE_FLAG_RESPONSE,sizeof(PT_PEOPLE_RESULT)))PhongThanRemotePeopleReceive(*(const PT_PEOPLE_RESULT*)data);
        return true;
    }
    if(header->MessageType==PT_PS_REPLY){
        if(PhongThanWorldFixedPacket(data,size,PT_PS_REPLY,PHONGTHAN_WIRE_FLAG_RESPONSE,sizeof(PT_PS_STATE))){
            const PT_PS_STATE* state=(const PT_PS_STATE*)data;int actor=Player[CLIENT_PLAYER_INDEX].m_nIndex;
            if(actor>0&&actor<MAX_NPC&&state->ActorId==Npc[actor].m_dwID&&state->Count>=0&&state->Count<=PT_PS_MAIL_LIMIT&&state->Target[31]==0&&state->Message[255]==0&&PtPsValidMail(state->Mail)){s_PtPsState=*state;s_PtPsReceived=GetTickCount();}
        }return true;
    }

    if(header->MessageType==PT_AUCTION_REPLY){
        if(PhongThanWorldFixedPacket(data,size,PT_AUCTION_REPLY,PHONGTHAN_WIRE_FLAG_RESPONSE,sizeof(PTAuctionPage)))PhongThanAuctionReceive(*(const PTAuctionPage*)data);
        return true;
    }
    if(header->MessageType==PT_RANK_REPLY){
        if(PhongThanWorldFixedPacket(data,size,PT_RANK_REPLY,PHONGTHAN_WIRE_FLAG_RESPONSE,sizeof(PTRankingPage)))PhongThanRankingReceive(*(const PTRankingPage*)data);
        return true;
    }
    if(header->MessageType==PT_STATUS_PAGES_RESPONSE){
        if(PhongThanWorldFixedPacket(data,size,PT_STATUS_PAGES_RESPONSE,PHONGTHAN_WIRE_FLAG_RESPONSE,sizeof(PT_STATUS_PAGES_STATE))){
            const PT_STATUS_PAGES_STATE* state=(const PT_STATUS_PAGES_STATE*)data;
            int actor=Player[CLIENT_PLAYER_INDEX].m_nIndex;
            if(actor>0&&actor<MAX_NPC&&state->ActorId==Npc[actor].m_dwID&&state->MapId==(unsigned int)SubWorld[0].m_SubWorldID&&PtStatusValidProfile(state->Profile)&&PtPetValid(state->Pet)){
                if(!ptStatusPagesPending||state->Header.Sequence==ptStatusPagesPending){
                    bool petChanged=PtPetCombatSignature(ptStatusPages.Pet)!=PtPetCombatSignature(state->Pet);
                    ptStatusPages=*state;ptStatusPagesReceived=GetTickCount();ptStatusPagesPending=0;
                    if(petChanged)Player[CLIENT_PLAYER_INDEX].UpdataCurData();
                }
            }
        }return true;
    }
    if(header->MessageType==PT_ITEM_LINK_MESSAGE){PTReceiveItemLink(data,size);return true;}
    if(header->MessageType==PT_TEAM_SNAPSHOT){
        if(PhongThanWorldFixedPacket(data,size,PT_TEAM_SNAPSHOT,PHONGTHAN_WIRE_FLAG_RESPONSE,sizeof(PT_TEAM_STATE))){
            const PT_TEAM_STATE* s=(const PT_TEAM_STATE*)data;int actor=Player[CLIENT_PLAYER_INDEX].m_nIndex;
            if(actor>0&&actor<MAX_NPC&&s->ActorId==Npc[actor].m_dwID&&s->Count<=8){
                bool valid=s->Name[31]==0&&s->ShowName<=1;for(unsigned int i=0;i<s->Count;++i)if(!s->Members[i].Id||s->Members[i].Name[31]!=0||s->Members[i].Life<0||s->Members[i].MaxLife<=0)valid=false;
                if(valid){s_PtTeamState=*s;s_PtTeamReceiveTime=GetTickCount();}
            }
        }return true;
    }
    if(header->MessageType==PT_SOUL_COMPOSE_REPLY){
        if(PhongThanWorldFixedPacket(data,size,PT_SOUL_COMPOSE_REPLY,PHONGTHAN_WIRE_FLAG_RESPONSE,sizeof(PHONGTHAN_COMPOSE_RESULT))){const PHONGTHAN_COMPOSE_RESULT* r=(const PHONGTHAN_COMPOSE_RESULT*)data;int a=Player[CLIENT_PLAYER_INDEX].m_nIndex;
            if(a>0&&a<MAX_NPC&&r->MapId==(unsigned)SubWorld[0].m_SubWorldID&&r->EntityId==Npc[a].m_dwID&&(PtSocketComposeMode()==8||PtSocketComposeMode()==9)&&r->Header.Sequence&&(int)(r->Header.Sequence-(unsigned)PtSoulUiNonce())>=0){PtSoulUiNonce()=r->Header.Sequence;CoreDataChanged(PHONGTHAN_COMPOSE_CORE_RESULT,r->Result,0);}
        }return true;
    }
	if(header->MessageType==PHONGTHAN_COMPOSE_RESPONSE)
	{
		if(PhongThanWorldFixedPacket(data,size,PHONGTHAN_COMPOSE_RESPONSE,PHONGTHAN_WIRE_FLAG_RESPONSE,sizeof(PHONGTHAN_COMPOSE_RESULT)))
		{
			const PHONGTHAN_COMPOSE_RESULT* result=(const PHONGTHAN_COMPOSE_RESULT*)data;
			const int player=Player[CLIENT_PLAYER_INDEX].m_nIndex;
			if(player>0 && player<MAX_NPC && result->MapId==(PHONGTHAN_U32)SubWorld[0].m_SubWorldID && result->EntityId==Npc[player].m_dwID)
				{
                    if(result->Header.Sequence){extern bool PhongThanSmithClientResult(const PHONGTHAN_COMPOSE_RESULT*);PhongThanSmithClientResult(result);}
                    else CoreDataChanged(PHONGTHAN_COMPOSE_CORE_RESULT,result->Result,0);
                }
		}
		return true;
	}
	if (header->MessageType == PHONGTHAN_MSG_GAMEPLAY_EXPERIENCE || header->MessageType == PHONGTHAN_MSG_GAMEPLAY_LEVEL ||
		header->MessageType == PHONGTHAN_MSG_WORLD_TITLE || header->MessageType == PHONGTHAN_MSG_WORLD_MOUNT || header->MessageType == PHONGTHAN_MSG_UI_PLAYER_EVENT)
	{
		if (!PhongThanValidatePlayerEventPacket(data, size)) return true;
		PHONGTHAN_U32 mapId, entityId;
		memcpy(&mapId, data + sizeof(PHONGTHAN_WIRE_HEADER), sizeof(mapId));
		memcpy(&entityId, data + sizeof(PHONGTHAN_WIRE_HEADER) + sizeof(mapId), sizeof(entityId));
		if (mapId != (PHONGTHAN_U32)SubWorld[0].m_SubWorldID) return true;
		const int index = NpcSet.SearchID(entityId);
		if (index <= 0 || index >= MAX_NPC) return true;
		if (header->MessageType == PHONGTHAN_MSG_WORLD_TITLE)
		{
			const PHONGTHAN_TITLE_EVENT* event = (const PHONGTHAN_TITLE_EVENT*)data;
			if (event->Expanded)
			{
				KExpandRank title;
				title.Release();
				g_StrCpyLen(title.szName, event->Name, sizeof(title.szName));
				title.dwColor = event->Color; title.nStateGraphics = event->Graphic; title.dwLeftTime = event->RemainingTime;
				Npc[index].SetExpandRank(&title);
			}
			else Npc[index].m_RankID = event->RankId;
		}
		else if (header->MessageType == PHONGTHAN_MSG_WORLD_MOUNT)
			// Server already validated the mount. Remote players have no local
			// inventory; consume state exactly as the player snapshot does.
			Npc[index].m_bRideHorse = ((const PHONGTHAN_MOUNT_EVENT*)data)->Mounted;
		else if(header->MessageType==PHONGTHAN_MSG_UI_PLAYER_EVENT &&
            ((const PHONGTHAN_PLAYER_EVENT*)data)->Operation==PHONGTHAN_PLAYER_ESCORT_RIDER)
            PhongThanSetEscortRider(index,(DWORD)((const PHONGTHAN_PLAYER_EVENT*)data)->Value);
        else if(header->MessageType==PHONGTHAN_MSG_UI_PLAYER_EVENT &&
            ((const PHONGTHAN_PLAYER_EVENT*)data)->Operation==PHONGTHAN_PLAYER_ESCORT_OWNER)
            PhongThanSetEscortOwner(index,(DWORD)((const PHONGTHAN_PLAYER_EVENT*)data)->Value);
        else if(header->MessageType==PHONGTHAN_MSG_UI_PLAYER_EVENT &&
            ((const PHONGTHAN_PLAYER_EVENT*)data)->Operation==PHONGTHAN_PLAYER_PROFESSION){
            const int profession=((const PHONGTHAN_PLAYER_EVENT*)data)->Value;
            Npc[index].m_Series=profession;Npc[index].m_nProfession=profession;
            Npc[index].ResetNpcTypeName(1);
            if(index==Player[CLIENT_PLAYER_INDEX].m_nIndex){
                Player[CLIENT_PLAYER_INDEX].m_cProfession.SetProfession(profession);
                CoreDataChanged(GDCNI_PLAYER_BASE_INFO,0,0);
            }
        }
        else if(header->MessageType==PHONGTHAN_MSG_UI_PLAYER_EVENT &&
            ((const PHONGTHAN_PLAYER_EVENT*)data)->Operation==PHONGTHAN_PLAYER_SMITH){
            if(index==Player[CLIENT_PLAYER_INDEX].m_nIndex){extern void PhongThanSmithClientOpen(DWORD);CoreDataChanged(PHONGTHAN_SMITH_CORE_OPEN,0,0);PhongThanSmithClientOpen((DWORD)((const PHONGTHAN_PLAYER_EVENT*)data)->Value);}
        }
        else if (index == Player[CLIENT_PLAYER_INDEX].m_nIndex)
		{
			if (header->MessageType == PHONGTHAN_MSG_GAMEPLAY_EXPERIENCE) s2cPlayerExp(data);
			else if (header->MessageType == PHONGTHAN_MSG_GAMEPLAY_LEVEL) s2cLevelUp(data);
			else if(header->MessageType==PHONGTHAN_MSG_UI_PLAYER_EVENT &&
                    ((const PHONGTHAN_PLAYER_EVENT*)data)->Operation==PHONGTHAN_PLAYER_RPS_ROUND) {
                TRADE_DECISION_SYNC round;round.ProtocolType=s2c_tradedecision;
                round.m_btDecision=(BYTE)((const PHONGTHAN_PLAYER_EVENT*)data)->Value;
                Player[CLIENT_PLAYER_INDEX].s2cTradeDecision((BYTE*)&round);
            } else if(header->MessageType==PHONGTHAN_MSG_UI_PLAYER_EVENT &&
                ((const PHONGTHAN_PLAYER_EVENT*)data)->Operation==PHONGTHAN_PLAYER_TRADE_COMMIT) {
                KPlayer& player=Player[CLIENT_PLAYER_INDEX];
                if(((const PHONGTHAN_PLAYER_EVENT*)data)->Value==1) {
                    if(player.CheckTrading(true) && !player.m_cTrade.m_bTradeFolkGame) {
                        TRADE_DECISION_SYNC commit;commit.ProtocolType=s2c_tradedecision;commit.m_btDecision=1;
                        player.s2cTradeDecision((BYTE*)&commit);
                    }
                } else {
                    CoreDataChanged(GDCNI_CONTAINER_OBJECT_CHANGED,UOC_ITEM_TAKE_WITH,0);
                    player.m_ItemList.MenuSetMouseItem();
                }
            } else s2cPlayerSync(data);
		}
		Npc[index].m_SyncSignal = SubWorld[0].m_dwCurrentTime;
		return true;
	}
	if (header->MessageType == PHONGTHAN_MSG_WORLD_OBJECT_SNAPSHOT)
	{
		if (!PhongThanValidateObjectSnapshot(data, size)) return true;
		if (((const PHONGTHAN_OBJECT_SNAPSHOT*)data)->MapId == (PHONGTHAN_U32)SubWorld[0].m_SubWorldID)
			SyncObjectAdd(data);
		return true;
	}
	if (header->MessageType == PHONGTHAN_MSG_WORLD_OBJECT_STATE ||
		header->MessageType == PHONGTHAN_MSG_WORLD_OBJECT_DIRECTION ||
		header->MessageType == PHONGTHAN_MSG_WORLD_OBJECT_REMOVE)
	{
		if (!PhongThanValidateObjectEvent(data, size, header->MessageType)) return true;
		const PHONGTHAN_OBJECT_EVENT* event = (const PHONGTHAN_OBJECT_EVENT*)data;
		if (event->MapId != (PHONGTHAN_U32)SubWorld[0].m_SubWorldID) return true;
		const int index = ObjSet.FindID(event->ObjectId);
		if (index <= 0)
		{
			if (header->MessageType != PHONGTHAN_MSG_WORLD_OBJECT_REMOVE && g_pClient)
			{
				PHONGTHAN_OBJECT_REFERENCE request;
				PhongThanInitializeWireHeader(&request.Header, PHONGTHAN_MSG_WORLD_OBJECT_REQUEST,
					sizeof(request), PHONGTHAN_WIRE_FLAG_REQUEST, 0);
				request.MapId = event->MapId; request.RegionId = event->RegionId; request.ObjectId = event->ObjectId;
				g_pClient->SendPackToServer(&request, sizeof(request));
			}
			return true;
		}
		if (header->MessageType == PHONGTHAN_MSG_WORLD_OBJECT_REMOVE) Object[index].Remove(event->Value);
		else if (header->MessageType == PHONGTHAN_MSG_WORLD_OBJECT_DIRECTION) Object[index].SetDir(event->Value);
		else
		{
			if (Object[index].m_nRegionIdx < 0)
			{
				const int region = SubWorld[0].FindRegion(event->RegionId);
				if (region < 0) return true;
				Object[index].m_nRegionIdx = region;
				SubWorld[0].m_Region[region].AddObj(index);
			}
			Object[index].SetState(event->Value);
		}
		return true;
	}
	if (header->MessageType == PHONGTHAN_MSG_WORLD_ENTITY_POSITION)
	{
		if (PhongThanValidateEntityPosition(data, size))
			PhongThanApplyPositionEvent((const PHONGTHAN_ENTITY_POSITION*)data);
		return true;
	}
	if (header->MessageType == PHONGTHAN_MSG_WORLD_ENTITY_STATUS)
	{
		if (!PhongThanValidateEntityStatus(data, size)) return true;
		const PHONGTHAN_ENTITY_STATUS* status = (const PHONGTHAN_ENTITY_STATUS*)data;
		if (status->MapId != (PHONGTHAN_U32)SubWorld[0].m_SubWorldID) return true;
		const int index = NpcSet.SearchID(status->EntityId);
		if (index <= 0 || index >= MAX_NPC) return true;
		if (index == Player[CLIENT_PLAYER_INDEX].m_nIndex &&
            (status->Kind == PHONGTHAN_STATUS_DEATH || status->Kind == PHONGTHAN_STATUS_REVIVE))
            PhongThanReviveIdentityTrace(status->Kind == PHONGTHAN_STATUS_DEATH ? "death_status" : "revive_status", CLIENT_PLAYER_INDEX, status->EntityId);
        if(index==Player[CLIENT_PLAYER_INDEX].m_nIndex &&
            (status->Kind==PHONGTHAN_STATUS_DEATH||status->Kind==PHONGTHAN_STATUS_REVIVE||status->Kind==PHONGTHAN_STATUS_HURT||status->Kind==PHONGTHAN_STATUS_SIT))
            PhongThanClearPredictedMoves();
		switch (status->Kind)
		{
		case PHONGTHAN_STATUS_SIT:
            if(status->Value==62&&!Npc[index].m_bRideHorse){
                Npc[index].BeginMiningSit();
            }else if (Player[CLIENT_PLAYER_INDEX].ConformIdx(index))
				Npc[index].SendCommand(Npc[index].m_bRideHorse ? do_stand : do_sit);
			break;
		case PHONGTHAN_STATUS_DEATH:
			Npc[index].ProcNetCommand(do_death);
			Npc[index].m_CurrentLife = 0;
			break;
		case PHONGTHAN_STATUS_HURT:
			Npc[index].ProcNetCommand(do_hurt, status->Value, status->X, status->Y);
			break;
		case PHONGTHAN_STATUS_REVIVE:
			if (!Npc[index].IsPlayer() && status->Value == REMOTE_REVIVE_TYPE)
				SubWorld[0].m_WorldMessage.Send(GWM_NPC_DEL, index);
			else {
                if(Npc[index].IsPlayer()) {
                    Npc[index].m_FightMode=status->Value==LOCAL_REVIVE_TYPE?enumFightActive:enumFightNone;
                    if(Npc[index].m_CurrentLife<=0)Npc[index].m_CurrentLife=Npc[index].m_CurrentLifeMax;
                }
                Npc[index].ProcNetCommand(do_revive);
            }
			break;
		case PHONGTHAN_STATUS_BASE_CAMP: Npc[index].SetCamp(status->Value); break;
		case PHONGTHAN_STATUS_CURRENT_CAMP: Npc[index].SetCurrentCamp(status->Value); break;
		}
		Npc[index].m_SyncSignal = SubWorld[0].m_dwCurrentTime;
		return true;
	}
	if (header->MessageType == PHONGTHAN_MSG_GAMEPLAY_ATTRIBUTE_UPDATE ||
		header->MessageType == PHONGTHAN_MSG_GAMEPLAY_SKILL_LEVEL_UPDATE ||
		header->MessageType == PHONGTHAN_MSG_GAMEPLAY_STATE_CLEAR)
	{
		PHONGTHAN_U32 mapId = 0, entityId = 0;
		switch (header->MessageType)
		{
		case PHONGTHAN_MSG_GAMEPLAY_ATTRIBUTE_UPDATE:
			if (!PhongThanValidateAttributeUpdate(data, size)) return true;
			mapId = ((const PHONGTHAN_ATTRIBUTE_UPDATE*)data)->MapId;
			entityId = ((const PHONGTHAN_ATTRIBUTE_UPDATE*)data)->EntityId;
			break;
		case PHONGTHAN_MSG_GAMEPLAY_SKILL_LEVEL_UPDATE:
			if (!PhongThanValidateSkillLevelUpdate(data, size)) return true;
			mapId = ((const PHONGTHAN_SKILL_LEVEL_UPDATE*)data)->MapId;
			entityId = ((const PHONGTHAN_SKILL_LEVEL_UPDATE*)data)->EntityId;
			break;
		case PHONGTHAN_MSG_GAMEPLAY_STATE_CLEAR:
			if (!PhongThanValidateStateClear(data, size)) return true;
			mapId = ((const PHONGTHAN_STATE_CLEAR*)data)->MapId;
			entityId = ((const PHONGTHAN_STATE_CLEAR*)data)->EntityId;
			break;
		}
		const int index = Player[CLIENT_PLAYER_INDEX].m_nIndex;
		if (index <= 0 || index >= MAX_NPC || entityId != Npc[index].m_dwID ||
			mapId != (PHONGTHAN_U32)SubWorld[0].m_SubWorldID) return true;
		if (header->MessageType == PHONGTHAN_MSG_GAMEPLAY_ATTRIBUTE_UPDATE) s2cGetCurAttribute(data);
		else if (header->MessageType == PHONGTHAN_MSG_GAMEPLAY_SKILL_LEVEL_UPDATE) s2cGetSkillLevel(data);
		else IgnoreState(data);
		return true;
	}
	if (header->MessageType == PHONGTHAN_MSG_UI_SCRIPT_ACTION)
	{
		FILE* dialogLog = fopen("ui_action_diag.log", "a");
		if (dialogLog)
		{
			fprintf(dialogLog, "client receive tick=%lu size=%u valid=%d\n", (unsigned long)GetTickCount(), size, PhongThanValidateUiAction(data, size));
			fclose(dialogLog);
		}
		if (PhongThanValidateUiAction(data, size)) SyncScriptAction(data);
		dialogLog = fopen("ui_action_diag.log", "a");
		if (dialogLog) { fprintf(dialogLog, "client dispatch returned\n"); fclose(dialogLog); }
		return true;
	}
	if (header->MessageType == PHONGTHAN_MSG_GAMEPLAY_SKILL_LIST ||
		header->MessageType == PHONGTHAN_MSG_GAMEPLAY_STATE_EFFECT)
	{
		const bool isList = header->MessageType == PHONGTHAN_MSG_GAMEPLAY_SKILL_LIST;
		if (isList ? !PhongThanValidateSkillList(data, size) : !PhongThanValidateStateEffect(data, size)) return true;
		// Both schemas begin with the same fixed scalar map/entity prefix.
		const PHONGTHAN_U32 mapId = isList ? ((const PHONGTHAN_SKILL_LIST_HEADER*)data)->MapId :
			((const PHONGTHAN_STATE_EFFECT_HEADER*)data)->MapId;
		const PHONGTHAN_U32 entityId = isList ? ((const PHONGTHAN_SKILL_LIST_HEADER*)data)->EntityId :
			((const PHONGTHAN_STATE_EFFECT_HEADER*)data)->EntityId;
		const int index = Player[CLIENT_PLAYER_INDEX].m_nIndex;
		if (index <= 0 || index >= MAX_NPC || mapId != (PHONGTHAN_U32)SubWorld[0].m_SubWorldID ||
			entityId != Npc[index].m_dwID) return true;
		if (isList) s2cSyncAllSkill(data);
		else SyncStateEffect(data);
		return true;
	}
	if (header->MessageType == PHONGTHAN_MSG_WORLD_MAP_SNAPSHOT)
	{
		if (PhongThanValidateMapSnapshot(data, size)) SyncWorld(data);
		// Clear any pending item-click throttle when applying a map sync,
		// so interaction can resume after the transition completes.
		if (PhongThanValidateMapSnapshot(data, size))
			Player[CLIENT_PLAYER_INDEX].m_dwRightMouse = 0;
		FILE* mapLog = fopen("ui_action_diag.log", "a");
		if (mapLog) {
			fprintf(mapLog, "client map-sync valid=%d current=%d\n", PhongThanValidateMapSnapshot(data, size), SubWorld[0].m_SubWorldID);
			fclose(mapLog);
		}
		return true;
	}
	if (header->MessageType == PHONGTHAN_MSG_WORLD_SELF_SNAPSHOT)
	{
		if (!PhongThanValidateSelfSnapshot(data, size)) return true;
		const PHONGTHAN_SELF_SNAPSHOT* player = (const PHONGTHAN_SELF_SNAPSHOT*)data;
		if (player->MapId == (PHONGTHAN_U32)SubWorld[0].m_SubWorldID) SyncCurPlayer(data);
		return true;
	}
	if (header->MessageType == PHONGTHAN_MSG_WORLD_SELF_VITALS)
	{
		if (!PhongThanValidateSelfVitals(data, size)) return true;
		const PHONGTHAN_SELF_VITALS* player = (const PHONGTHAN_SELF_VITALS*)data;
		const int index = Player[CLIENT_PLAYER_INDEX].m_nIndex;
		if (index > 0 && index < MAX_NPC && player->EntityId == Npc[index].m_dwID &&
			player->MapId == (PHONGTHAN_U32)SubWorld[0].m_SubWorldID) SyncCurNormalData(data);
		return true;
	}
	if (header->MessageType == PHONGTHAN_MSG_WORLD_SYNC_COMPLETE)
	{
		if (PhongThanWorldFixedPacket(data, size, PHONGTHAN_MSG_WORLD_SYNC_COMPLETE,
			PHONGTHAN_WIRE_FLAG_RESPONSE, sizeof(PHONGTHAN_WORLD_SYNC_FENCE))) SyncEnd(data);
		return true;
	}
	if (header->MessageType == PHONGTHAN_MSG_WORLD_PLAYER_SNAPSHOT)
	{
		const int valid = PhongThanValidatePlayerSnapshot(data, size);
		if (!valid) return true;
		const PHONGTHAN_PLAYER_SNAPSHOT* player = (const PHONGTHAN_PLAYER_SNAPSHOT*)data;
		if (player->MapId == (PHONGTHAN_U32)SubWorld[0].m_SubWorldID)
			SyncPlayer(data);
		return true;
	}
    if (header->MessageType == PHONGTHAN_MSG_GAMEPLAY_SUMMON_OWNER)
    {
        if (!PhongThanValidateSummonOwnerSync(data,size)) return true;
        const PHONGTHAN_SUMMON_OWNER_SYNC* owner=(const PHONGTHAN_SUMMON_OWNER_SYNC*)data;
        if (owner->MapId==(PHONGTHAN_U32)SubWorld[0].m_SubWorldID && owner->PetId &&
            owner->OwnerId && owner->PetId!=owner->OwnerId &&
            owner->SkillLevel>0 && owner->SkillLevel<64 && owner->TargetLocked<=1)
            PhongThanSummonBindClientOwner(owner->PetId,owner->OwnerId,owner->SkillLevel,owner->TargetLocked);
        return true;
    }
	if (header->MessageType == PHONGTHAN_MSG_GAMEPLAY_SKILL_CAST)
	{
		if (!PhongThanValidateSkillCast(data, size)) return true;
		const PHONGTHAN_SKILL_CAST* skill = (const PHONGTHAN_SKILL_CAST*)data;
		if (skill->MapId != (PHONGTHAN_U32)SubWorld[0].m_SubWorldID ||
			skill->SkillId >= MAX_SKILL || skill->SkillLevel >= MAX_SKILLLEVEL) return true;
		if (skill->DirectEffect) s2cDirectlyCastSkill(data);
		else NetCommandSkill(data);
		return true;
	}
	if (header->MessageType == PHONGTHAN_MSG_WORLD_NPC_SNAPSHOT)
	{
		if (!PhongThanValidateNpcSnapshot(data, size)) return true;
		const PHONGTHAN_NPC_SNAPSHOT* npc = (const PHONGTHAN_NPC_SNAPSHOT*)data;
		if (npc->MapId == (PHONGTHAN_U32)SubWorld[0].m_SubWorldID)
			SyncNpc(data);
		return true;
	}
	if (header->MessageType == PHONGTHAN_MSG_WORLD_NPC_UPDATE)
	{
		if (!PhongThanValidateNpcUpdate(data, size)) return true;
		const PHONGTHAN_NPC_UPDATE* npc = (const PHONGTHAN_NPC_UPDATE*)data;
		if (npc->MapId == (PHONGTHAN_U32)SubWorld[0].m_SubWorldID)
			SyncNpcMin(data);
		return true;
	}
	if (header->MessageType == PHONGTHAN_MSG_WORLD_ENTITY_MOVE)
	{
		if (!PhongThanValidateEntityMove(data, size)) return true;
		const PHONGTHAN_ENTITY_MOVE* move = (const PHONGTHAN_ENTITY_MOVE*)data;
		if (move->MapId != (PHONGTHAN_U32)SubWorld[0].m_SubWorldID) return true;
		const int index = NpcSet.SearchID(move->EntityId);
		if (index <= 0 || index >= MAX_NPC) return true;
		if (move->Mode == PHONGTHAN_MOVE_WALK && Npc[index].m_HideState.nTime > 0)
			Npc[index].Madnessto(move->X, move->Y);
		else if (Player[CLIENT_PLAYER_INDEX].ConformIdx(index))
			Npc[index].SendCommand(move->Mode == PHONGTHAN_MOVE_RUN ? do_run :
				(move->Mode == PHONGTHAN_MOVE_JUMP ? do_jump : do_walk), move->X, move->Y);
		Npc[index].m_SyncSignal = SubWorld[0].m_dwCurrentTime;
		return true;
	}
	if (header->MessageType == PHONGTHAN_MSG_WORLD_ENTITY_REMOVE ||
		header->MessageType == PHONGTHAN_MSG_WORLD_ENTITY_NOT_FOUND)
	{
		const bool missing = header->MessageType == PHONGTHAN_MSG_WORLD_ENTITY_NOT_FOUND;
		if (!PhongThanValidateEntityReference(data, size, header->MessageType,
			missing ? (PHONGTHAN_WIRE_FLAG_RESPONSE | PHONGTHAN_WIRE_FLAG_ERROR) :
			PHONGTHAN_WIRE_FLAG_RESPONSE)) return true;
		const PHONGTHAN_ENTITY_REFERENCE* entity = (const PHONGTHAN_ENTITY_REFERENCE*)data;
		if (entity->MapId != (PHONGTHAN_U32)SubWorld[0].m_SubWorldID) return true;
		if (NpcSet.IsNpcRequestExist(entity->EntityId))
			NpcSet.RemoveNpcRequest(entity->EntityId);
		if (missing) return true;
		const int index = NpcSet.SearchID(entity->EntityId);
		if (index <= 0 || index >= MAX_NPC || !Player[CLIENT_PLAYER_INDEX].ConformIdx(index))
			return true;
		const int region = Npc[index].m_RegionIndex;
		if (region >= 0 && region < SubWorld[0].m_nTotalRegion)
		{
			SubWorld[0].m_Region[region].RemoveNpc(index);
			SubWorld[0].m_Region[region].DecRef(Npc[index].m_MapX, Npc[index].m_MapY, obj_npc);
		}
		NpcSet.Remove(index);
		return true;
	}
	return false;
}
#else
bool KProtocolProcess::ProcessPhongThanWorldPacket(int playerIndex, BYTE* data, unsigned int size)
{
    const PHONGTHAN_WIRE_HEADER* header = (const PHONGTHAN_WIRE_HEADER*)data;
    if(header->MessageType==PT_PEOPLE_QUERY){
        if(PhongThanWorldFixedPacket(data,size,PT_PEOPLE_QUERY,PHONGTHAN_WIRE_FLAG_REQUEST,sizeof(PT_PEOPLE_COMMAND)))PhongThanRemotePeopleRequest(playerIndex,*(const PT_PEOPLE_COMMAND*)data);
        return true;
    }
    if(header->MessageType==PT_PS_QUERY){
        if(PhongThanWorldFixedPacket(data,size,PT_PS_QUERY,PHONGTHAN_WIRE_FLAG_REQUEST,sizeof(PT_PS_COMMAND)))PhongThanPlayerServiceRequest(playerIndex,*(const PT_PS_COMMAND*)data);
        return true;
    }
    if(header->MessageType==PT_AUCTION_QUERY){
        if(PhongThanWorldFixedPacket(data,size,PT_AUCTION_QUERY,PHONGTHAN_WIRE_FLAG_REQUEST,sizeof(PTAuctionCommand)))PhongThanAuctionRequest(playerIndex,*(const PTAuctionCommand*)data);
        return true;
    }
    if(header->MessageType==PT_RANK_QUERY){
        if(PhongThanWorldFixedPacket(data,size,PT_RANK_QUERY,PHONGTHAN_WIRE_FLAG_REQUEST,sizeof(PTRankingQuery)))PhongThanRankingRequest(playerIndex,*(const PTRankingQuery*)data);
        return true;
    }
    if(header->MessageType==PT_STATUS_PAGES_QUERY){
        if(PhongThanWorldFixedPacket(data,size,PT_STATUS_PAGES_QUERY,PHONGTHAN_WIRE_FLAG_REQUEST,sizeof(PT_STATUS_PAGES_REQUEST)))
            PhongThanStatusPagesRequest(playerIndex,*(const PT_STATUS_PAGES_REQUEST*)data);
        return true;
    }
    if(header->MessageType==PT_TEAM_REQUEST){
        if(PhongThanWorldFixedPacket(data,size,PT_TEAM_REQUEST,PHONGTHAN_WIRE_FLAG_REQUEST,sizeof(PT_TEAM_QUERY)))PhongThanSendTeamState(playerIndex,(const PT_TEAM_QUERY*)data);
        return true;
    }
	if (header->MessageType == PHONGTHAN_MSG_WORLD_OBJECT_REQUEST || header->MessageType == PHONGTHAN_MSG_WORLD_OBJECT_INTERACT)
	{
		if (!PhongThanValidateObjectReference(data, size, header->MessageType) || playerIndex <= 0 || playerIndex >= MAX_PLAYER) return true;
		const PHONGTHAN_OBJECT_REFERENCE* request = (const PHONGTHAN_OBJECT_REFERENCE*)data;
		const int index = Player[playerIndex].m_nIndex;
		if (index <= 0 || index >= MAX_NPC) return true;
		const int world = Npc[index].m_SubWorldIndex, currentRegion = Npc[index].m_RegionIndex;
		if (world < 0 || world >= MAX_SUBWORLD || currentRegion < 0 || currentRegion >= SubWorld[world].m_nTotalRegion ||
			request->MapId != (PHONGTHAN_U32)SubWorld[world].m_SubWorldID) return true;
		const int targetRegion = SubWorld[world].FindRegion(request->RegionId);
		if (targetRegion < 0) return true;
		bool nearby = targetRegion == currentRegion;
		for (int i = 0; i < 8 && !nearby; ++i)
			nearby = SubWorld[world].m_Region[currentRegion].m_nConnectRegion[i] == targetRegion;
		if (!nearby) return true;
		if (header->MessageType == PHONGTHAN_MSG_WORLD_OBJECT_INTERACT) ObjMouseClick(playerIndex, data);
		else
		{
			const int object = SubWorld[world].m_Region[targetRegion].FindObject(request->ObjectId);
			if (object > 0) Object[object].SyncAdd(Player[playerIndex].m_nNetConnectIdx);
		}
		return true;
	}
	if (header->MessageType == PHONGTHAN_MSG_WORLD_POSE_REQUEST)
	{
		if (!PhongThanValidatePoseRequest(data, size) || playerIndex <= 0 || playerIndex >= MAX_PLAYER) return true;
		const PHONGTHAN_POSE_REQUEST* pose = (const PHONGTHAN_POSE_REQUEST*)data;
		const int index = Player[playerIndex].m_nIndex;
		if (index <= 0 || index >= MAX_NPC) return true;
		const int world = Npc[index].m_SubWorldIndex;
		if (world < 0 || world >= MAX_SUBWORLD || pose->MapId != (PHONGTHAN_U32)SubWorld[world].m_SubWorldID) return true;
		if (pose->Kind >= PHONGTHAN_POSE_REVIVE && pose->Kind <= PHONGTHAN_POSE_REVIVE_LOCAL) NpcReviveCommand(playerIndex, data);
		else if (Npc[index].m_CurrentLife > 0 && Npc[index].m_Doing != do_death)
		{
			if (pose->Kind == PHONGTHAN_POSE_TOGGLE_MOUNT)
			{
				if (Npc[index].CanSwitchRideHorse()) Npc[index].SwitchRideHorse(!Npc[index].m_bRideHorse);
			}
			else Npc[index].SendCommand(pose->Kind == PHONGTHAN_POSE_SIT && !Npc[index].m_bRideHorse ? do_sit : do_stand);
		}
		return true;
	}
    if (header->MessageType == PHONGTHAN_MSG_GAMEPLAY_SUMMON_TARGET_REQUEST)
    {
        if (!PhongThanWorldFixedPacket(data, size, PHONGTHAN_MSG_GAMEPLAY_SUMMON_TARGET_REQUEST,
            PHONGTHAN_WIRE_FLAG_REQUEST, sizeof(PHONGTHAN_ENTITY_REFERENCE)) ||
            playerIndex <= 0 || playerIndex >= MAX_PLAYER ||
            Player[playerIndex].m_cProfession.GetProfession() != 2 || Player[playerIndex].CheckTrading()) return true;
        int owner = Player[playerIndex].m_nIndex;
        const PHONGTHAN_ENTITY_REFERENCE* request = (const PHONGTHAN_ENTITY_REFERENCE*)data;
        if (owner <= 0 || owner >= MAX_NPC || !Npc[owner].IsAlive() || !Npc[owner].m_FightMode) return true;
        int world = Npc[owner].m_SubWorldIndex;
        if (world < 0 || world >= MAX_SUBWORLD || request->MapId != (PHONGTHAN_U32)SubWorld[world].m_SubWorldID) return true;
        int target = NpcSet.SearchID(request->EntityId);
        if (target > 0 && target < MAX_NPC && Npc[target].m_RegionIndex >= 0 &&
            NpcSet.GetDistance(owner, target) <= PhongThanSummonTargetRange(owner))
            PhongThanSummonOwnerAttack(owner, target);
        return true;
    }
	if (header->MessageType == PHONGTHAN_MSG_GAMEPLAY_ATTRIBUTE_REQUEST ||
		header->MessageType == PHONGTHAN_MSG_GAMEPLAY_SKILL_LEVEL_REQUEST)
	{
		if (playerIndex <= 0 || playerIndex >= MAX_PLAYER) return true;
		const int index = Player[playerIndex].m_nIndex;
		if (index <= 0 || index >= MAX_NPC) return true;
		const int world = Npc[index].m_SubWorldIndex;
		if (world < 0 || world >= MAX_SUBWORLD) return true;
		if (header->MessageType == PHONGTHAN_MSG_GAMEPLAY_ATTRIBUTE_REQUEST)
		{
			if (!PhongThanValidateAttributeRequest(data, size)) return true;
			const PHONGTHAN_ATTRIBUTE_REQUEST* request = (const PHONGTHAN_ATTRIBUTE_REQUEST*)data;
			if (request->MapId != (PHONGTHAN_U32)SubWorld[world].m_SubWorldID) return true;
			Player[playerIndex].AddBaseAttribute(data);
		}
		else
		{
			if (!PhongThanValidateSkillLevelRequest(data, size)) return true;
			const PHONGTHAN_SKILL_LEVEL_REQUEST* request = (const PHONGTHAN_SKILL_LEVEL_REQUEST*)data;
			if (request->MapId != (PHONGTHAN_U32)SubWorld[world].m_SubWorldID || request->SkillId >= MAX_SKILL ||
				request->Points >= MAX_SKILLLEVEL) return true;
			Player[playerIndex].AddSkillPoint(data);
		}
		Player[playerIndex].SetLastNetOperationTime(g_SubWorldSet.GetGameTime());
		return true;
	}
	if (header->MessageType == PHONGTHAN_MSG_UI_CHOICE_REQUEST)
	{
		if (playerIndex > 0 && playerIndex < MAX_PLAYER && PhongThanValidateUiChoice(data, size))
			Player[playerIndex].ProcessPlayerSelectFromUI(data);
		return true;
	}
	if (header->MessageType == PHONGTHAN_MSG_UI_NUMBER_INPUT)
	{
		if (playerIndex > 0 && playerIndex < MAX_PLAYER && PhongThanValidateNumberRequest(data, size)) {
			extern void PhongThanStarterBagInput(KPlayer&, const PHONGTHAN_UI_NUMBER_REQUEST&);
			extern bool PhongThanEventNumberInput(KPlayer&, const PHONGTHAN_UI_NUMBER_REQUEST&);
			if (!PhongThanEventNumberInput(Player[playerIndex], *(const PHONGTHAN_UI_NUMBER_REQUEST*)data))
				PhongThanStarterBagInput(Player[playerIndex], *(const PHONGTHAN_UI_NUMBER_REQUEST*)data);
		}
		return true;
	}
	if (header->MessageType != PHONGTHAN_MSG_WORLD_MOVE_REQUEST &&
		header->MessageType != PHONGTHAN_MSG_WORLD_ENTITY_REQUEST &&
		header->MessageType != PHONGTHAN_MSG_GAMEPLAY_SKILL_REQUEST &&
		header->MessageType != PHONGTHAN_MSG_NPC_INTERACT_REQUEST)
		return false;
	if (playerIndex <= 0 || playerIndex >= MAX_PLAYER) return true;
	const int index = Player[playerIndex].m_nIndex;
	if (index <= 0 || index >= MAX_NPC) return true;
	const int world = Npc[index].m_SubWorldIndex;
	if (world < 0 || world >= MAX_SUBWORLD || Npc[index].m_RegionIndex < 0 ||
		Npc[index].m_RegionIndex >= SubWorld[world].m_nTotalRegion) return true;
	if (header->MessageType == PHONGTHAN_MSG_GAMEPLAY_SKILL_REQUEST)
	{
		if (!PhongThanValidateSkillRequest(data, size)) return true;
		const PHONGTHAN_SKILL_REQUEST* skill = (const PHONGTHAN_SKILL_REQUEST*)data;
		if (skill->MapId == (PHONGTHAN_U32)SubWorld[world].m_SubWorldID)
			NpcSkillCommand(playerIndex, data);
		return true;
	}
	if (header->MessageType == PHONGTHAN_MSG_NPC_INTERACT_REQUEST)
	{
		if (!PhongThanValidateEntityReference(data, size, PHONGTHAN_MSG_NPC_INTERACT_REQUEST,
			PHONGTHAN_WIRE_FLAG_REQUEST)) return true;
		const PHONGTHAN_ENTITY_REFERENCE* npc = (const PHONGTHAN_ENTITY_REFERENCE*)data;
		if (npc->MapId == (PHONGTHAN_U32)SubWorld[world].m_SubWorldID)
			Player[playerIndex].DialogNpc(data);
		return true;
	}
	if (header->MessageType == PHONGTHAN_MSG_WORLD_MOVE_REQUEST)
	{
		if (!PhongThanValidateMoveRequest(data, size)) return true;
		const PHONGTHAN_MOVE_REQUEST* move = (const PHONGTHAN_MOVE_REQUEST*)data;
        if(Npc[index].m_CurrentLife<=0 || Npc[index].m_Doing==do_death || Npc[index].m_Doing==do_revive)return true;
		if (move->MapId != (PHONGTHAN_U32)SubWorld[world].m_SubWorldID) return true;
		int region, x, y, offX, offY;
		SubWorld[world].Mps2Map(move->X, move->Y, &region, &x, &y, &offX, &offY);
		if (region < 0 || region >= SubWorld[world].m_nTotalRegion) return true;
		if (Player[playerIndex].m_nPaceBarTime)
		{
			Player[playerIndex].m_nPaceBarTime = 0;
			Player[playerIndex].m_nPaceBarTimeMax = 0;
			Player[playerIndex].LoadScriptProgressBar(2);
		}
		const NPCCMD command = move->Mode == PHONGTHAN_MOVE_RUN ? do_run : do_walk;
        // Zero/unrecognized sequence retains legacy reconciliation. Never
        // accept a client position, tick count, speed or extra simulation.
        Npc[index].m_PtMoveSequence = move->Header.Sequence <= 65535 ? (unsigned short)move->Header.Sequence : 0;
        Npc[index].m_PtMoveStep = 0;
		Npc[index].SendCommand(command, move->X, move->Y);
		Npc[index].m_NowCommand.CmdKind = command;
		Npc[index].m_NowCommand.Param_X = move->X;
		Npc[index].m_NowCommand.Param_Y = move->Y;
		Npc[index].m_NowCommand.Param_Z = -1;
		Player[playerIndex].SetLastNetOperationTime(g_SubWorldSet.GetGameTime());
		return true;
	}
	if (!PhongThanValidateEntityReference(data, size, PHONGTHAN_MSG_WORLD_ENTITY_REQUEST,
		PHONGTHAN_WIRE_FLAG_REQUEST)) return true;
	const PHONGTHAN_ENTITY_REFERENCE* request = (const PHONGTHAN_ENTITY_REFERENCE*)data;
	if (request->MapId != (PHONGTHAN_U32)SubWorld[world].m_SubWorldID) return true;
	const int target = request->EntityId == Npc[index].m_dwID ? index :
		Player[playerIndex].FindAroundNpc(request->EntityId);
	if (target > 0 && target < MAX_NPC && Npc[target].m_SubWorldIndex == world)
		Npc[target].SendSyncData(Player[playerIndex].m_nNetConnectIdx);
	else if (g_pServer)
	{
		PHONGTHAN_ENTITY_REFERENCE response = *request;
		PhongThanInitializeWireHeader(&response.Header, PHONGTHAN_MSG_WORLD_ENTITY_NOT_FOUND,
			sizeof(response), PHONGTHAN_WIRE_FLAG_RESPONSE | PHONGTHAN_WIRE_FLAG_ERROR,
			request->Header.Sequence);
		g_pServer->PackDataToClient(Player[playerIndex].m_nNetConnectIdx, &response, sizeof(response));
	}
	return true;
}
#endif
