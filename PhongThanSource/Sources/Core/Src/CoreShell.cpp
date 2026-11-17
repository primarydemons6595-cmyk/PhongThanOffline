/*****************************************************************************************
//	������Core�ӿڷ���
//	Copyright : Kingsoft 2002
//	Author	:
//	CreateTime:	2002-9-12
------------------------------------------------------------------------------------------
*****************************************************************************************/
#include "KCore.h"
#include "../../../Headers/PhongThanRealmUi.h"
#include "../../../Headers/PhongThanIslandContract.h"
#include "../../../Headers/PhongThanLifeItemUseData.h"
#include "../../../Headers/PhongThanStall.h"
#include "../../../Headers/PhongThanStallMessages.h"
#include "../../../Headers/PhongThanF2Balance.h"
#include "../../../Headers/PhongThanBank.h"
#include "../../../Headers/PhongThanEventShop.h"
#include "PhongThanCarry.h"
#include "../../../Headers/PhongThanLifeSkills.h"
#include "../../../Headers/PhongThanLifeData.h"
#include "../../../Headers/PhongThanPlayerTarget.h"
#include "../../../Headers/PhongThanPlayerServices.h"
#include "../../../Headers/PhongThanRemotePeople.h"
static bool s_PtMailPreviews[MAX_ITEM];
#ifndef _SERVER
static DWORD s_PtSmithNonce=0,s_PtSmithUntil=0,s_PtSmithPending=0,s_PtSmithPendingUntil=0;static int s_PtSmithMap=0;
void PhongThanSmithClientOpen(DWORD nonce){s_PtSmithPending=0;s_PtSmithNonce=nonce;s_PtSmithUntil=GetTickCount()+120000;s_PtSmithMap=SubWorld[0].m_SubWorldID;}
static unsigned int s_PtSelectedPlayerId=0;
static int s_PtSelectedPlayerIndex=0;
#endif

#include "../../../Headers/PhongThanNpcNameColor.h"
#include "../../../Headers/PhongThanStatusPages.h"
#include "../../../Headers/PhongThanConnectionInfo.h"
#include "../../../Headers/PhongThanRanking.h"
#include "../../../Headers/PhongThanAuction.h"
#include "../../../Headers/PhongThanCompanionUi.h"
#include "PhongThanSummon.h"
#include "../../../Headers/PhongThanVisibility.h"
#include "../../../Headers/PhongThanCrossRoute.h"
#include "../../../Headers/PhongThanCommerceNavigation.h"
#include "../../../Headers/PhongThanNationProtocol.h"

#include "../../../Headers/PhongThanItemLink.h"
#include "../../../Headers/PhongThanTeamProtocol.h"

#include "../../../Headers/PhongThanAutoRecovery.h"
#include "../../../Headers/PhongThanStatusUi.h"
#include "../../../Headers/PhongThanGraphicsOptions.h"
#ifndef _SERVER
int g_PhongThanAnimationQuality = 0;
int g_PhongThanMaxVisiblePlayers = 255;
#endif
#include "PhongThanCompose.h"
#include "../../../Headers/PhongThanSmithFeedback.h"

#include "../../../Headers/PhongThanItemDisplay.h"
#include "GameDataDef.h"
#include "CoreShell.h"
#ifndef _SERVER
bool PhongThanSmithClientResult(const PHONGTHAN_COMPOSE_RESULT* r){if(!r||!r->Header.Sequence||r->Header.Sequence!=s_PtSmithPending)return false;s_PtSmithPending=0;s_PtSmithNonce=0;CoreDataChanged(PHONGTHAN_SMITH_CORE_RESULT,r->Result,r->OutputDetail);return true;}
#endif
#include "PhongThanInventoryUi.h"
#include "../../../Headers/PhongThanRoleUi.h"
#include "../../../Headers/PhongThanIBBuffUi.h"
#include "../../../Headers/PhongThanF4Protocol.h"
#include "KPlayerTask.h"
#include "PhongThanConsumables.h"
#include "../../../Headers/PhongThanConsumableUi.h"
#include "../../../Headers/PhongThanBuffVisibility.h"
// A bank query must never intercept the timed-buff output buffer.
typedef char PTBankBuffQueriesMustDiffer[(PT_BANK_LOCK_GET!=PT_GDI_CONSUMABLE_BUFFS)?1:-1];
#include "../../../Headers/PhongThanStatusUi.h"
#include "KEngine.h"
#include "KTaskFuns.h"

#include "CoreDrawGameObj.h"
#include "ImgRef.h"

#include "KPlayer.h"
#include "KPlayerSet.h"
#include "KObjSet.h"
#include "KItemList.h"
#include "../../../Headers/PhongThanMailItem.h"
#include "KSubWorldSet.h"
#include "KProtocolProcess.h"

#include "KNpcResList.h"
#include "Scene/KScenePlaceC.h"
#include "kskills.h"
#include "GameDataDef.h"
#include "MsgGenreDef.h"
#include "KOption.h"
#include "KSubWorld.h"
#include "KViewItem.h"
#include "KSellItem.h"
#include "KItemSet.h"
#include "KTongData.h"
#include "KTongProtocol.h"
#include "KMagicDesc.h"
#include "malloc.h"
#include "ItemActionDiag.h"

#include "KBuySell.h"
#include "KThiefSkill.h"
#include "KPhongThanAppearance.h"
#include "../GameClient/Ui/UiCase/UiAutoPlay.h"

#include "PhongThanComposePreview.inl"

#define	NPC_TRADE_BOX_WIDTH		6
#define	NPC_TRADE_BOX_HEIGHT	10
#define	MAX_TRADE_ITEM_WIDTH	2
#define	MAX_TRADE_ITEM_HEIGHT	4
IClientCallback* l_pDataChangedNotifyFunc = 0;

// Item ids carried by the UI are runtime indexes into the client Item table.
// Keep every public CoreShell lookup inside the table bounds; malformed or
// stale UI objects should simply produce no description instead of crashing
// the client while hovering/clicking an item.
static BOOL IsValidClientItemIndex(unsigned int uItemId)
{
	return (uItemId > 0 && uItemId < MAX_ITEM);
}

class KCoreShell : public iCoreShell
{
public:
	int	 GetProtocolSize(BYTE byProtocol);
	int	 Debug(unsigned int uDataId, unsigned int uParam, int nParam);
	int	 OperationRequest(unsigned int uOper, unsigned int uParam, int nParam);
	void ProcessInput(unsigned int uMsg, unsigned int uParam, int nParam);
	int GetTargetNPC();
	int	 FindSelectNPC(int x, int y, int nRelation, bool bSelect, void* pReturn, int& nKind, bool bFind);
	int FindSelectObject(int x, int y, bool bSelect, int& nObjectIdx, int& nKind);
	int FindSpecialNPC(char* Name, void* pReturn, int& nKind);
	int ChatSpecialPlayer(void* pPlayer, const char* pMsgBuff, unsigned short nMsgLength);
	void ApplyAddTeam(void* pPlayer);
	void TradeApplyStart(void* pPlayer, bool bFolkGame);
	void JoinTongReply(void* pPlayer);
	int UseSkill(int x, int y, int nSkillID);
	int LockSomeoneUseSkill(int nTargetIndex, int nSkillID);
	int LockSomeoneAction(int nTargetIndex);
	int LockObjectAction(int nTargetIndex);
	void GotoWhere(int x, int y, int mode);	//mode 0 is auto, 1 is walk, 2 is run
	void GetWhere(int x, int y, void* pInfo);
	void Goto(int nDir, int mode);	//nDir 0~63, mode 0 is auto, 1 is walk, 2 is run
	void Turn(int nDir);	//nDir 0 is left, 1 is right, 2 is back
	int ThrowAwayItem();
	int GetNPCRelation(int nIndex);
	BOOL CheckMouse();



	//���ͼ��صĲ���,uOper��ȡֵ���� GAME_SCENE_MAP_OPERATION_INDEX
	int	SceneMapOperation(unsigned int uOper, unsigned int uParam, int nParam);
	//������صĲ���, uOper��ȡֵ���� GAME_TONG_OPERATION_INDEX
	int	TongOperation(unsigned int uOper, unsigned int uParam, int nParam);
	//�������صĲ�����uOper��ȡֵ���� GAME_TEAM_OPERATION_INDEX
	int TeamOperation(unsigned int uOper, unsigned int uParam, int nParam);

	int PAIOperation(unsigned int uOper, unsigned int uParam, int nParam, int nParam1);

	int	 GetGameData(unsigned int uDataId, unsigned int uParam, int nParam);

	void DrawGameObj(unsigned int uObjGenre, unsigned int uId, int x, int y, int Width, int Height, int nParam);
	void DrawGameSpace();
	void DrawGameSpace1();
	DWORD GetPing();
	//void SendPing();
	int	 SetCallDataChangedNofify(IClientCallback* pNotifyFunc);
	void NetMsgCallbackFunc(void* pMsgData, unsigned int nMsgSize);
	void SetRepresentShell(struct iRepresentShell* pRepresent);
	void SetMusicInterface(void* pMusicInterface);
	void SetRepresentAreaSize(int nWidth, int nHeight);
	int  Breathe();
	void Release();	//�ͷŽӿڶ���
	void SetClient(LPVOID pClient);
	void SendNewDataToServer(void* pData, int nLength);
	void DirectFindPos(unsigned int uParam, int nParam, BOOL bSync, BOOL bPaintLine);
	BOOL GetPaintMode();
	void SetPaintMode(BOOL nIndex);
	BOOL GetFlagMode();
	void SetFlagMode(BOOL nIndex);
	void GetSizeItem(unsigned int uId, int *nW, int *nH);
	int	GetExtPoint();
	int GetItemQuality(unsigned int uItemId, unsigned int uGenre);
	int GetGenreItem(unsigned int uItemId, unsigned int uGenre);
	int IsDamage(unsigned int uItemId);
	BOOL GetLockState();
	int GetStackNum(unsigned int uItemId);
	BOOL GetSkillData(int nSkillId, int *nLevel);
	void GetSkillName(int nSkillId, char* szSkillName);
	int GetTradePrice(unsigned int uItemId);
	int GetTradeState(int nIndex, int uId);
	int	GetFightState();
	void BreakItem(unsigned int uParam, int nParam, BOOL bIsBreakAll);
	DWORD GetdwIDPlayer(unsigned int uId);
	int	GetOwnValue(int nMoneyUnit);
	int GetDataSuperShop(int nSaleId, unsigned int uParam, int nParam);
	int GetDataDynamicShop(int nSaleId, unsigned int uParam, int nParam);
	int GetNextSkill(int nType, int nIndex);
//	BOOL IsSkillAura(int nSkillId);

	//TamLTM kham nam xanh
	int GetDetailItem(unsigned int uId );
	int GetParticularItem(unsigned int uId );
	int GetLevelItem(unsigned int uId );
	int GetSeriesItem(unsigned int uId );
	int GetItemColor(unsigned int uItemId);
	//End code

	//TamLTM Auto run
	void SetAutoRun(BOOL autoRun);
	void SetFlagAutoRun(BOOL autoRunFlag, int x, int y);
	void CheckHoverMouseMiniMap(BOOL hover);
	//end code

};

static KCoreShell	g_CoreShell;

CORE_API BOOL g_InitCore();
#ifndef _STANDALONE
extern "C" __declspec(dllexport)
#endif
iCoreShell* CoreGetShell()
{
	if (!g_InitCore())
		return NULL;
	return &g_CoreShell;
}

//--------------------------------------------------------------------------
//	���ܣ�������Ϸ�������ݸı��֪ͨ����
//	���أ���δ��ע��֪ͨ��������ֱ�ӷ���0�����򷵻�֪ͨ����ִ�н����
//--------------------------------------------------------------------------

//--------------------------------------------------------------------
void CoreDataChanged(unsigned int uDataId, unsigned int uParam, int nParam)
{
	if (l_pDataChangedNotifyFunc)
		l_pDataChangedNotifyFunc->CoreDataChanged(uDataId, uParam, nParam);
}


void KCoreShell::Release()
{
	g_ReleaseCore();
}

//--------------------------------------------------------------------------
//	���ܣ���������ɴ���������Ϣ
//--------------------------------------------------------------------------
void KCoreShell::NetMsgCallbackFunc(void* pMsgData, unsigned int nMsgSize)
{
	g_ProtocolProcess.ProcessNetMsg((BYTE *)pMsgData, nMsgSize);
}
//--------------------------------------------------------------------------
//	���ܣ�������Ϸ�������ݸı��֪ͨ����
//	������fnCoreDataChangedCallback pNotifyFunc --> ֪ͨ������ָ�롣
//	���أ�����ֵΪ��0ֵ��ʾע��ɹ��������ʾʧ�ܡ�
//--------------------------------------------------------------------------
int	KCoreShell::SetCallDataChangedNofify(IClientCallback* pNotifyFunc)
{
	l_pDataChangedNotifyFunc = pNotifyFunc;
	return true;
}

//--------------------------------------------------------------------------
//	���ܣ�����Ϸ�����ȡ����
//	������unsigned int uDataId --> ��ʾ��ȡ��Ϸ���ݵ�������������������ֵΪ÷������
//							GAMEDATA_INDEX��ȡֵ֮һ��
//		  unsigned int uParam  --> ����uDataId��ȡֵ�������
//		  int nParam --> ����uDataId��ȡֵ�������
//	���أ�����uDataId��ȡֵ���������
//--------------------------------------------------------------------------
#include "PhongThanStatusOriginal.inl"

int g_PtBankLocked=1;

// F2 wallet counting is shared with server payment validation in KBuySell.cpp.

int	KCoreShell::GetGameData(unsigned int uDataId, unsigned int uParam, int nParam)
{
    if(uDataId==PT_EVENT_SHOP_TOOLTIP)return PhongThanEventShopTooltip((unsigned int)nParam,(PTEventShopTooltip*)uParam);
    if(uDataId==PT_EVENT_SHOP_GET){return uParam&&nParam==sizeof(PTEventShopData)&&PhongThanEventShopGet((PTEventShopData*)uParam);}
    if(uDataId==PT_F2_CURRENCY_GET){const PTF2Currency* c=PhongThanF2CurrencyConfig(nParam);if(!c||!uParam)return 0;memcpy((void*)uParam,c,sizeof(*c));return 1;}
    if(uDataId==PT_F2_BALANCE_GET)return uParam?PhongThanF2WalletCount(CLIENT_PLAYER_INDEX,*(PTF2BalanceQuery*)uParam):0;
    if(uDataId==PT_BANK_LOCK_GET)return g_PtBankLocked;
    if(uDataId==PT_BANK_EXPIRY_GET){
        if(nParam<2||nParam>5)return 0;
        KPlayer& owner=Player[CLIENT_PLAYER_INDEX];
        if(!owner.m_cTask.GetSaveVal(PT_BANK_MIGRATED_TASK))return nParam-2<owner.m_btRepositoryNum?PT_BANK_PERMANENT:0;
        return owner.m_cTask.GetSaveVal(PT_BANK_FIRST_TASK+nParam-2);
    }
    if(uDataId==PT_PET_FOODS_GET){
        if(!uParam||nParam!=sizeof(PT_PET_FOODS))return 0;
        PT_PET_FOODS* out=(PT_PET_FOODS*)uParam;ZeroMemory(out,sizeof(*out));
        PlayerItem* owned=Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
        while(owned&&out->Count<64){
            int index=owned->nIdx,place=owned->nPlace;
            if(index>0&&index<MAX_ITEM&&(place==pos_equiproom||place==pos_equiproomex||place==pos_ptbag2||place==pos_ptbag3||place==pos_immediacy)&&
               ((Item[index].GetGenre()==item_ibitem&&Item[index].GetParticular()==2&&((Item[index].GetDetailType()>=414&&Item[index].GetDetailType()<=417)||Item[index].GetDetailType()==1056||Item[index].GetDetailType()==903))||
                (Item[index].GetGenre()==item_materials&&(Item[index].GetDetailType()==114||Item[index].GetDetailType()==115||Item[index].GetDetailType()==1184||(Item[index].GetDetailType()>=5108&&Item[index].GetDetailType()<=5115))))){
                PT_PET_FOOD& food=out->Items[out->Count++];food.Serial=Item[index].GetID();food.Detail=Item[index].GetDetailType();
                lstrcpyn(food.Name,Item[index].GetName(),sizeof(food.Name));lstrcpyn(food.Image,Item[index].GetImageName(),sizeof(food.Image));
                char description[8192];Item[index].GetDesc(description);lstrcpyn(food.Description,description,sizeof(food.Description));
            }
            owned=Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
        }return out->Count;
    }
    if(uDataId==PT_RANK_GET){if(!uParam||nParam!=sizeof(PTRankingPage))return 0;return PhongThanRankingGet((PTRankingPage*)uParam);}
    if(uDataId==PT_CONNECTION_INFO_QUERY){if(!uParam||nParam!=sizeof(PT_CONNECTION_INFO))return 0;PT_CONNECTION_INFO& out=*(PT_CONNECTION_INFO*)uParam;out.HasSample=g_SubWorldSet.HasPingSample()?1:0;DWORD ping=g_SubWorldSet.GetPing();out.RoundTripMs=ping<=0x7fffffff?ping*2:0xffffffffU;out.SampleAgeMs=g_SubWorldSet.GetPingAge();return 1;}
    if(uDataId==PT_STATUS_PAGES_GET){if(!uParam||nParam!=sizeof(PT_STATUS_PAGES_STATE))return 0;return PhongThanGetStatusPages((PT_STATUS_PAGES_STATE*)uParam);}
	int nRet = 0;
	switch(uDataId)
	{
	case GDI_PLAYER_IS_MALE:
		{
            if(nParam&&(DWORD)nParam==g_cViewItem.GetViewedId())return g_cViewItem.m_OwnerMale;
			int nIndex = 0;
			if (nParam == 0)
				nIndex = Player[CLIENT_PLAYER_INDEX].m_nIndex;
			else
				nIndex = NpcSet.SearchID(nParam);

			if (nIndex)
				nRet = (Npc[nIndex].m_NpcSettingIdx == PLAYER_MALE_NPCTEMPLATEID);
			else
				nRet = 1;	//����ʱ������
		}
		break;
	case GDI_REPAIR_ITEM_PRICE:
		if (uParam)
		{
			KUiObjAtContRegion *pObj = (KUiObjAtContRegion *)uParam;
			KItem*	pItem = NULL;

			switch(pObj->Obj.uGenre)
			{
			case CGOG_ITEM:
				{
					if (pObj->Obj.uId > 0)
					{
						pItem = &Item[pObj->Obj.uId];
					}
				}
				break;
			default:
				break;
			}

			if (!pItem)
				break;

			KUiItemBuySelInfo *pInfo = (KUiItemBuySelInfo *)nParam;

			if (pObj->eContainer == UOC_NPC_SHOP)
				break;
			pInfo->nTemplateRow = pItem->GetRow();
			pInfo->bNewArrival = FALSE;
			pInfo->nMoneyUnit = 0; // RepairItem charges carried silver, independent of shop currency.
			pInfo->nCurPrice = pItem->GetRepairPrice();
			PhongThanPlainItemName(pItem->GetName(), pInfo->szItemName,
				sizeof(pInfo->szItemName));
			nRet = pItem->CanBeRepaired();
		}
		else
		{
			nParam = 0;
			nRet = 0;
		}
		break;

	case GDI_TRADE_ITEM_PRICE:
		if (uParam	&& !Player[CLIENT_PLAYER_INDEX].m_ItemList.IsLockOperation())
		{
			KUiObjAtContRegion *pObj = (KUiObjAtContRegion *)uParam;
			KItem*	pItem = NULL;

			switch(pObj->Obj.uGenre)
			{
			case CGOG_PLAYERSELLITEM:
				{
					if (pObj->Obj.uId > 0)
					{
						KUiItemBuySelInfo *pInfo = (KUiItemBuySelInfo *)nParam;
						pInfo->nTemplateRow = Item[pObj->Obj.uId].GetRow();
						pInfo->bNewArrival = FALSE;
						pInfo->nMoneyUnit = moneyunit_money; // Player stalls settle silver, never the last NPC shop currency.
						pInfo->nCurPrice = Item[pObj->Obj.uId].GetTradePrice();
						g_StrCpyLen(pInfo->szItemName,Item[pObj->Obj.uId].GetName(),sizeof(pInfo->szItemName));
						nRet = 1;
						return nRet;
					}
				}
				break;
			case CGOG_ITEM:
				{
					if (pObj->Obj.uId > 0)
					{
						pItem = &Item[pObj->Obj.uId];
					}
				}
				break;
			case CGOG_NPCSELLITEM:
				{
					int	nBuyIdx = Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nShopIdx[Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nCurShop];
					if (nBuyIdx != -1)
					{
						int nIndex = BuySell.GetItemIndex(nBuyIdx, pObj->Obj.uId);
						if (nIndex >= 0)
							pItem = BuySell.GetItem(nIndex);
					}
				}
				break;
			default:
				break;
			}

			if (!pItem)
				break;

			KUiItemBuySelInfo *pInfo = (KUiItemBuySelInfo *)nParam;
			pInfo->nTemplateRow = pItem->GetRow();
			int nCurrentShop = -1;
			int nCurrentShopPage = Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nCurShop;
			if (nCurrentShopPage >= 0 && nCurrentShopPage < MAX_SUPERSHOP_SHOPTAB)
				nCurrentShop = Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nShopIdx[nCurrentShopPage];
			BOOL bPhongThanIBShop = pObj->Obj.uGenre == CGOG_NPCSELLITEM &&
				nCurrentShop >= PHONGTHAN_IBSHOP_FIRST_SHOP_INDEX &&
				nCurrentShop < PHONGTHAN_IBSHOP_FIRST_SHOP_INDEX + PHONGTHAN_IBSHOP_TAB_COUNT;
			pInfo->nOldPrice = pItem->GetOrgPrice();
			if (pObj->eContainer == UOC_NPC_SHOP)
				pInfo->nCurPrice = pItem->GetCurPrice();
			else
				pInfo->nCurPrice = pItem->GetSalePrice();
			pInfo->bNewArrival = pItem->GetNewArrival();
			g_StrCpyLen(pInfo->szItemName, pItem->GetName(),sizeof(pInfo->szItemName));
            pInfo->nMoneyUnit = bPhongThanIBShop?PT_F2_MONEY_SLOT1+BuySell.GetPaymentSlot(pItem)-1:Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nMoneyUnit;
			nRet = 1;
		}
		else
		{
			nParam = 0;
			nRet = 0;
		}
		break;
	//��Ϸ��������˵���ı���
	//uParam = (KUiGameObject*) ������Ϸ����Ľṹ���ݵ�ָ��
	//nParam = (char*) ָ��һ����������ָ�룬��ռ䲻����256�ֽڡ�
	case GDI_GAME_OBJ_DESC_INCLUDE_REPAIRINFO:
	case GDI_GAME_OBJ_DESC_INCLUDE_TRADEINFO:
		if (nParam && uParam)
		{
			KUiObjAtContRegion* pObj = (KUiObjAtContRegion *)uParam;
			char* pszDescript = (char *)nParam;
			pszDescript[0] = 0;
			switch(pObj->Obj.uGenre)
			{
			case CGOG_PLAYERSELLITEM:
				if (!IsValidClientItemIndex(pObj->Obj.uId))
					break;
					Item[pObj->Obj.uId].GetDesc(pszDescript, true, true);
				break;
			case CGOG_ITEM:
				{
					if (!IsValidClientItemIndex(pObj->Obj.uId))
						break;
					if (pObj->eContainer == UOC_EQUIPTMENT)
					{
						Item[pObj->Obj.uId].GetDesc(pszDescript, true, true, Player[CLIENT_PLAYER_INDEX].m_ItemList.GetActiveAttribNum(pObj->Obj.uId),
							Player[CLIENT_PLAYER_INDEX].m_ItemList.GetGoldActiveAttribNum(pObj->Obj.uId));
					}
					else
						Item[pObj->Obj.uId].GetDesc(pszDescript, true, true);
				}
				break;
			case CGOG_NPCSELLITEM:
				{
					int nIdx = -1;
					if (-1 == Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nShopIdx[Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nCurShop])
						break;
					nIdx = BuySell.GetItemIndex(Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nShopIdx[Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nCurShop], pObj->Obj.uId);

					KItem* pItem = NULL;
					if (nIdx < 0)
						break;
					pItem = BuySell.GetItem(nIdx);

					if (!pItem)
						break;
					pItem->GetDesc(pszDescript, true);
				}
				break;
			}
		}
		break;
case GDI_GAME_OBJ_DESC:
		if (nParam && uParam)
		{
			KUiObjAtContRegion* pObj = (KUiObjAtContRegion *)uParam;
			char* pszDescript = (char *)nParam;
			pszDescript[0] = 0;
			switch(pObj->Obj.uGenre)
			{
			case CGOG_ITEM:
				{
					if (!IsValidClientItemIndex(pObj->Obj.uId))
						break;
					if (pObj->eContainer == UOC_EQUIPTMENT)
					{
						Item[pObj->Obj.uId].GetDesc(pszDescript, false, false, Player[CLIENT_PLAYER_INDEX].m_ItemList.GetActiveAttribNum(pObj->Obj.uId),
							Player[CLIENT_PLAYER_INDEX].m_ItemList.GetGoldActiveAttribNum(pObj->Obj.uId));
					}
					else
						Item[pObj->Obj.uId].GetDesc(pszDescript);
				}
				break;
			case CGOG_SKILL:
			case CGOG_SKILL_FIGHT:
			case CGOG_SKILL_SHORTCUT:
				{
					int nLevel = 0;
					if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetLevel(pObj->Obj.uId) > 0)
						nLevel = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetCurrentLevel(pObj->Obj.uId);
					_ASSERT(nLevel >= 0);
					if (pObj->Obj.uId >0)
					{

						ISkill * pISkill = g_SkillManager.GetSkill(pObj->Obj.uId, 1);
						if (!pISkill)
							break;
						eSkillStyle eStyle = (eSkillStyle) pISkill->GetSkillStyle();

						switch(eStyle)
						{
						case SKILL_SS_Missles:			//	�ӵ���		���������ڷ����ӵ���
						case SKILL_SS_Melee:
						case SKILL_SS_InitiativeNpcState:	//	������		���������ڸı䵱ǰNpc������״̬
						case SKILL_SS_PassivityNpcState:		//	������		���������ڸı�Npc�ı���״̬
						case SKILL_SS_CreateNpc:
						case SKILL_SS_PhongThanAttack:
						case SKILL_SS_PhongThanProduce:
						case SKILL_SS_Reserved14:
		case SKILL_SS_PhongThanAwaken:
                        default: // Other native KSkill styles also expose descriptions.
							{
								KSkill::GetDesc(
									pObj->Obj.uId,
									nLevel,
									pszDescript,
									Player[CLIENT_PLAYER_INDEX].m_nIndex,
									(pObj->Obj.uGenre == CGOG_SKILL_SHORTCUT)?false:true
									);
							}
							break;
						case SKILL_SS_Thief:
							{
								((KThiefSkill *)pISkill)->GetDesc(
									pObj->Obj.uId,
									nLevel,
									pszDescript,
									Player[CLIENT_PLAYER_INDEX].m_nIndex,
									(pObj->Obj.uGenre == CGOG_SKILL_SHORTCUT)?false:true
								);

							}break;

						}
					}

				}
				break;
			case CGOG_NPCSELLITEM:
				{
					int nIdx = -1;
					if (-1 == Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nShopIdx[Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nCurShop])
						break;
					nIdx = BuySell.GetItemIndex(Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nShopIdx[Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nCurShop], pObj->Obj.uId);

					KItem* pItem = NULL;
					if (nIdx < 0)
						break;
					pItem = BuySell.GetItem(nIdx);

					if (!pItem)
						break;
					pItem->GetDesc(pszDescript);
				}
				break;
			case CGOG_IME_ITEM:
				{
				if (!IsValidClientItemIndex(pObj->Obj.uId))
					return 0;
				Item[pObj->Obj.uId].GetDesc(pszDescript);
				}
				break;
			case CGOG_PLAYERSELLITEM:
				if (!IsValidClientItemIndex(pObj->Obj.uId))
					break;
					Item[pObj->Obj.uId].GetDesc(pszDescript, true, true, 0);
				break;
			case CGOG_PLAYERVIEWITEM:
				if (!IsValidClientItemIndex(pObj->Obj.uId))
					break;
					Item[pObj->Obj.uId].GetDesc(pszDescript);
				break;
			}
		}
		break;
	case GDI_GAME_OBJ_DESC_INCLUDE_MOUSEHOVER:
		if (nParam && uParam)
		{
			KUiObjAtContRegion* pObj = (KUiObjAtContRegion *)uParam;
			char* pszFileName = (char*)nParam;
			// The hover image buffer is reused by the UI.  Clear it before
			// resolving the new object, otherwise a non-image object (including
			// an NPC) inherits the image returned for the previous object.
			pszFileName[0] = 0;
			switch(pObj->Obj.uGenre)
			{
			case CGOG_PLAYERVIEWITEM:
			case CGOG_PLAYERSELLITEM:
			case CGOG_ITEM:
            case CGOG_IME_ITEM:
					if (!IsValidClientItemIndex(pObj->Obj.uId))
                        break;
                    nRet = Item[pObj->Obj.uId].GetTooltipImageName(pszFileName, 256);
                break;
			case CGOG_NPCSELLITEM:
				{
					int nIdx = -1;
					if (-1 == Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nShopIdx[Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nCurShop])
						break;
					nIdx = BuySell.GetItemIndex(Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nShopIdx[Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nCurShop], pObj->Obj.uId);

					KItem* pItem = NULL;
					if (nIdx < 0)
						break;
					pItem = BuySell.GetItem(nIdx);

                    if (!pItem) break;
                    nRet = pItem->GetTooltipImageName(pszFileName, 256);
				}
				break;
			}
			if(pszFileName[0] && !nRet)
				nRet = 1;
		}
		break;
	case GDI_PLAYER_UID:
			nRet = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_dwID;
		break;
	case GDI_PLAYER_CURCAMP:
			nRet = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentCamp;
		break;
	//���ǵ�һЩ���ױ������
	//uParam = (KUiPlayerBaseInfo*)pInfo
	case PT_VIEW_PROFILE_GET:
        if(uParam&&nParam==(int)g_cViewItem.GetViewedId()){
            *(PT_PERSONAL_PROFILE*)uParam=g_cViewItem.m_Profile;return 1;
        }return 0;
    case GDI_PLAYER_BASE_INFO:
        if(uParam&&nParam&&(DWORD)nParam==g_cViewItem.GetViewedId()){*(KUiPlayerBaseInfo*)uParam=g_cViewItem.m_Owner;return 1;}
		if (uParam)
		{
			KUiPlayerBaseInfo* pInfo = (KUiPlayerBaseInfo*)uParam;
			int nIndex = 0;
			if (nParam == 0)
			{
				nIndex = Player[CLIENT_PLAYER_INDEX].m_nIndex;
				pInfo->nProfession = Player[CLIENT_PLAYER_INDEX].m_cProfession.m_nProfession;
				pInfo->nCurTong = Player[CLIENT_PLAYER_INDEX].m_cTong.GetTongNameID();
				pInfo->nMissionGroup = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nMissionGroup;
				pInfo->nRoomId = Player[CLIENT_PLAYER_INDEX].m_cRoom.m_nID;
				pInfo->nProfessionRank = Player[CLIENT_PLAYER_INDEX].m_nProfessionRank;
			}
			else
			{
				nIndex = NpcSet.SearchID(nParam);
				pInfo->nProfession = -1;
				pInfo->nCurTong = 0;
				pInfo->nMissionGroup = -1;
				pInfo->nRoomId = -1;
				pInfo->nProfessionRank = 0;
			}
			if (nIndex) //////////////////////////////
			{
				{unsigned int color=0;PhongThanParseNpcName(Npc[nIndex].Name,pInfo->Name,sizeof(pInfo->Name),color);}
				strcpy(pInfo->szMateName, Npc[nIndex].MateName);
				strcpy(pInfo->szTongName, Npc[nIndex].m_szTongName);
				pInfo->Title[0] = 0;
				if (Npc[nIndex].m_RankID)
				{
					char szRankId[5];
					itoa(Npc[nIndex].m_RankID, szRankId, 10);
					g_RankTabSetting.GetString(szRankId, "RANKSTR", "", pInfo->Title, sizeof(pInfo->Title));
				}
				pInfo->nRankInWorld = nParam == 0 ?
                    Player[CLIENT_PLAYER_INDEX].m_cTask.GetSaveVal(TASKVALUE_PT_UI_WORLD_RANK) : Npc[nIndex].m_nRankInWorld;
				pInfo->nFuYuan = Npc[nIndex].m_nFuYuan;
				pInfo->nRepute = nParam == 0 ? Player[CLIENT_PLAYER_INDEX].m_cTask.GetSaveVal(TASKVALUE_PT_CREDIT) : Npc[nIndex].m_nRepute;
				pInfo->nPKValue = Npc[nIndex].m_nPKValue;
				pInfo->nTranslife = Npc[nIndex].m_byTranslife;
				memset(pInfo->StatusDesc, 0, sizeof(pInfo->StatusDesc));

				switch (Npc[nIndex].m_Series)
				{
					case series_metal:
						strcpy(pInfo->StatusDesc, "H� Kim");
					break;
					case series_wood:
						strcpy(pInfo->StatusDesc, "H� M�c");
					break;
					case series_water:
						strcpy(pInfo->StatusDesc, "H� Th�y");
					break;
					case series_fire:
						strcpy(pInfo->StatusDesc, "H� H�a");
					break;
					case series_earth:
						strcpy(pInfo->StatusDesc, "H� Th�");
					break;
					default:
						strcpy(pInfo->StatusDesc, "V� H�");
					break;
				}
                if(Npc[nIndex].m_Kind==kind_player && Npc[nIndex].m_Series==PHONGTHAN_PROFESSION_VU_SI)strcpy(pInfo->StatusDesc,g_Profession.GetName(PHONGTHAN_PROFESSION_VU_SI));
			}

		}
		break;

	//���ǵ�һЩ�ױ������
	//uParam = (KUiPlayerRuntimeInfo*)pInfo
	case GDI_PLAYER_RT_INFO:
		if (uParam)
		{
			KUiPlayerRuntimeInfo* pInfo = (KUiPlayerRuntimeInfo*)uParam;
			pInfo->nLifeFull = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentLifeMax;		//������ֵ
			pInfo->nLife = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentLife;				//����
			pInfo->nManaFull = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentManaMax;		//������ֵ
			pInfo->nMana = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentMana;				//����
			pInfo->nStaminaFull = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentStaminaMax;//������ֵ
			pInfo->nStamina = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentStamina;		//����
			pInfo->nExperienceFull = Player[CLIENT_PLAYER_INDEX].m_nNextLevelExp;		//������ֵ
			pInfo->nExperience = Player[CLIENT_PLAYER_INDEX].m_nExp;					//����
			pInfo->nCurLevelExperience = Player[CLIENT_PLAYER_INDEX].m_nNextLevelExp;

			pInfo->byAction = PA_NONE;

			if (Player[CLIENT_PLAYER_INDEX].m_RunStatus)
				pInfo->byAction |= PA_RUN;

			if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Doing == do_sit)
				pInfo->byAction |= PA_SIT;

			if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_bRideHorse == TRUE)
				pInfo->byAction |= PA_RIDE;

		}
		break;

	//���ǵ�һЩ�ױ����������
	//uParam = (KUiPlayerAttribute*)pInfo
	case GDI_PLAYER_RT_ATTRIBUTE:
		if (uParam)
		{
			KUiPlayerAttribute* pInfo = (KUiPlayerAttribute*)uParam;
			KNpc*	pNpc = &Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex];
			pInfo->nMoney = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetMoney(room_equipment);				//����
			pInfo->nBARemainPoint = Player[CLIENT_PLAYER_INDEX].m_nAttributePoint;						//��������ʣ�����
			pInfo->nStrength = Player[CLIENT_PLAYER_INDEX].m_nCurStrength;								//����
			pInfo->nDexterity = Player[CLIENT_PLAYER_INDEX].m_nCurDexterity;								//����
			pInfo->nVitality = Player[CLIENT_PLAYER_INDEX].m_nCurVitality;								//����
			pInfo->nEnergy = Player[CLIENT_PLAYER_INDEX].m_nCurEngergy;									//����
			Player[CLIENT_PLAYER_INDEX].GetEchoDamage(&pInfo->nKillMIN, &pInfo->nKillMAX, 0);				//�����Сɱ����
			Player[CLIENT_PLAYER_INDEX].GetEchoDamage(&pInfo->nRightKillMin , &pInfo->nRightKillMax, 1);

			int nLeftSkillId = Player[CLIENT_PLAYER_INDEX].GetLeftSkill();
			if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetLevel(nLeftSkillId) > 0)
			{
				KSkill * pOrdinSkill = (KSkill *)g_SkillManager.GetSkill(nLeftSkillId, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetCurrentLevel(nLeftSkillId));
				if (pOrdinSkill->IsUseAR()) {
					pInfo->nLeftAttack = pNpc->m_CurrentAttackRating;				//������
					Player[CLIENT_PLAYER_INDEX].GetEchoAttack(&pInfo->nLeftAttack,0); //TamLTM Damage attack ho tro chinh xac
				}
				else
					pInfo->nLeftAttack = 0;
			}
			else
				pInfo->nLeftAttack = 0;
			int nRightSkillId = Player[CLIENT_PLAYER_INDEX].GetRightSkill();
			if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetLevel(nRightSkillId) > 0)
			{
				KSkill * pOrdinSkill = (KSkill *)g_SkillManager.GetSkill(nRightSkillId, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetCurrentLevel(nRightSkillId));
				if (pOrdinSkill->IsUseAR()) {
					pInfo->nRightAttack = pNpc->m_CurrentAttackRating;				//������
					Player[CLIENT_PLAYER_INDEX].GetEchoAttack(&pInfo->nRightAttack,1); //TamLTM Damage attack ho tro chinh xac
				}
				else
					pInfo->nRightAttack = 0;
			}
			else
				pInfo->nRightAttack = 0;

			pInfo->nDefence = pNpc->m_CurrentDefend;					//������

			if (Player[CLIENT_PLAYER_INDEX].m_RunStatus)
				pInfo->nMoveSpeed = pNpc->m_CurrentRunSpeed;				//�ƶ��ٶ�
			else
				pInfo->nMoveSpeed = pNpc->m_CurrentWalkSpeed;				//�ƶ��ٶ�

			pInfo->nAttackSpeed = pNpc->m_CurrentAttackSpeed;
			pInfo->nCastSpeed = pNpc->m_CurrentCastSpeed;//�����ٶ�
			//�������
			//if (pNpc->m_CurrentPhysicsResistMax >= pNpc->m_CurrentPhysicsResist)
			//{
			pInfo->nPhyDef = pNpc->m_PhysicsArmor.nValue[0];
			pInfo->nPhyDefPlus = 0;
			//}
			// else
			// {
			// 	pInfo->nPhyDef = pNpc->m_CurrentPhysicsResistMax;
			// 	pInfo->nPhyDefPlus = (pNpc->m_CurrentPhysicsResist - pNpc->m_CurrentPhysicsResistMax) / RESIST_PLUS_SCALE;
			// }
			//��������
			if (pNpc->m_CurrentColdResistMax >= pNpc->m_CurrentColdResist)
			{
				pInfo->nCoolDef = pNpc->m_CurrentColdResist;
				pInfo->nCoolDefPlus = 0;
			}
			else
			{
				pInfo->nCoolDef = pNpc->m_CurrentColdResistMax;
				pInfo->nCoolDefPlus = (pNpc->m_CurrentColdResist - pNpc->m_CurrentColdResistMax) / RESIST_PLUS_SCALE;
			}
			//�������
			if (pNpc->m_CurrentLightResistMax >= pNpc->m_CurrentLightResist)
			{
				pInfo->nLightDef = pNpc->m_CurrentLightResist;
				pInfo->nLightDefPlus = 0;
			}
			else
			{
				pInfo->nLightDef = pNpc->m_CurrentLightResistMax;
				pInfo->nLightDefPlus = (pNpc->m_CurrentLightResist - pNpc->m_CurrentLightResistMax) / RESIST_PLUS_SCALE;
			}
			// tho
			if (pNpc->m_CurrentEarthResistMax >= pNpc->m_CurrentEarthResist)
			{
				pInfo->nEarthDef = pNpc->m_CurrentEarthResist;
				pInfo->nEarthDefPlus = 0;
			}
			else
			{
				pInfo->nEarthDef = pNpc->m_CurrentEarthResistMax;
				pInfo->nEarthDefPlus = (pNpc->m_CurrentEarthResist - pNpc->m_CurrentEarthResistMax) / RESIST_PLUS_SCALE;
			}
			//�������
			if (pNpc->m_CurrentFireResistMax >= pNpc->m_CurrentFireResist)
			{
				pInfo->nFireDef = pNpc->m_CurrentFireResist;
				pInfo->nFireDefPlus = 0;
			}
			else
			{
				pInfo->nFireDef = pNpc->m_CurrentFireResistMax;
				pInfo->nFireDefPlus = (pNpc->m_CurrentFireResist - pNpc->m_CurrentFireResistMax) / RESIST_PLUS_SCALE;
			}
			//���ط���
			if (pNpc->m_CurrentPoisonResistMax >= pNpc->m_CurrentPoisonResist)
			{
				pInfo->nPoisonDef = pNpc->m_CurrentPoisonResist;
				pInfo->nPoisonDefPlus = 0;
			}
			else
			{
				pInfo->nPoisonDef = pNpc->m_CurrentPoisonResistMax;
				pInfo->nPoisonDefPlus = (pNpc->m_CurrentPoisonResist - pNpc->m_CurrentPoisonResistMax) / RESIST_PLUS_SCALE;
			}
			pInfo->nLevel = PhongThanIsClientSummon(pNpc->m_Index)?PhongThanSummonShownLevel(pNpc->m_Level,PhongThanClientSummonRank(pNpc->m_Index)):pNpc->m_Level;

			// �������״̬��ʾ ״̬���� not end ��Ӧ����״̬��Ӧ������������ spe
			memset(pInfo->StatusDesc, 0, sizeof(pInfo->StatusDesc));
			switch(pNpc->m_Series)
			{
			case series_water:
				strcpy(pInfo->StatusDesc, "H� Th�y");
				break;
			case series_wood:
				strcpy(pInfo->StatusDesc, "H� M�c");
				break;
			case series_metal:
				strcpy(pInfo->StatusDesc, "H� Kim");
				break;
			case series_fire:
				strcpy(pInfo->StatusDesc, "H� H�a");
				break;
			case series_earth:
				strcpy(pInfo->StatusDesc, "H� Th�");
				break;
			}

			if(pNpc->m_Series==PHONGTHAN_PROFESSION_VU_SI)strcpy(pInfo->StatusDesc,g_Profession.GetName(pNpc->m_Series));
			pInfo->nPKValue = Player[CLIENT_PLAYER_INDEX].m_cPK.GetPKValue();
			pInfo->nRepute = Player[CLIENT_PLAYER_INDEX].m_cTask.GetSaveVal(TASKVALUE_PT_CREDIT);
			pInfo->nFuYuan = Player[CLIENT_PLAYER_INDEX].m_cTask.GetSaveVal(TASKVALUE_STATTASK_FUYUAN);
			pInfo->nTranslife = pNpc->m_byTranslife;
			pInfo->nRankInWorld = Player[CLIENT_PLAYER_INDEX].m_cTask.GetSaveVal(TASKVALUE_PT_UI_WORLD_RANK);
			pInfo->Title[0] = 0;
			if (pNpc->m_RankID)
			{
				char szRankId[5];
				itoa(pNpc->m_RankID, szRankId, 10);
				g_RankTabSetting.GetString(szRankId, "RANKSTR", "", pInfo->Title, sizeof(pInfo->Title));
			}
		}
		break;

	//���ǵ�����ʹ����Ʒ���书
	//uParam = (KUiPlayerImmedItemSkill*)pInfo
	case GDI_PLAYER_IMMED_ITEMSKILL:
		if (uParam)
		{
			KUiPlayerImmedItemSkill* pInfo = (KUiPlayerImmedItemSkill*)uParam;
			memset(pInfo,0,sizeof(KUiPlayerImmedItemSkill));
			pInfo->IMmediaSkill[0].uGenre	= CGOG_SKILL_SHORTCUT;
			pInfo->IMmediaSkill[0].uId		= Player[CLIENT_PLAYER_INDEX].GetLeftSkill();
			pInfo->IMmediaSkill[1].uGenre	= CGOG_SKILL_SHORTCUT;
			pInfo->IMmediaSkill[1].uId		= Player[CLIENT_PLAYER_INDEX].GetRightSkill();

			for (int i = 0; i < MAX_IMMEDIACY_ITEM; i++)
			{
				pInfo->ImmediaItem[i].uId = Player[CLIENT_PLAYER_INDEX].m_ItemList.m_Room[room_immediacy].FindItem(i, 0);
				if (pInfo->ImmediaItem[i].uId > 0)
				{
					pInfo->ImmediaItem[i].uGenre = CGOG_ITEM;
				}
				else
				{
					pInfo->ImmediaItem[i].uGenre = CGOG_NOTHING;
				}
			}
		}
		break;

	//��������Я����Ǯ
	//nRet = ��������Я����Ǯ
	case GDI_PLAYER_HOLD_MONEY:
		if (uParam == moneyunit_money)
			nRet = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetMoney(room_equipment);
		else if (uParam == moneyunit_extpoint) //TamLTM fix xu;
			nRet = Player[CLIENT_PLAYER_INDEX].GetExtPoint();
		break;
	//��������Я������Ʒ��
	//uParam = (KUiObjAtRegion*) pInfo -> KUiObjAtRegion�ṹ�����ָ�룬KUiObjAtRegion
	//				�ṹ���ڴ洢��Ʒ�����书�����ݼ����������λ����Ϣ��
	//nParam = pInfo�����а���KUiObjAtRegion�ṹ����Ŀ
	//Return = �������ֵС�ڵ��ڴ������nParam����ֵ��ʾpInfo�����е�ǰ���ٸ�KUiObjAtRegion
	//			�ṹ���������Ч�����ݣ������ʾ��Ҫ����������ٸ�KUiObjAtRegion�ṹ������
	//			�Ź��洢ȫ��������Я������Ʒ��Ϣ��

	// flying�����һ���Ƿ�������������������ʵ�֣�gg����һ�ޣ�mm���KFCŶ
	case GDI_PLAYER_CAN_RIDE:
		// ʵ�ַ�֧�Ĵ������������
		break;

	case GDI_ITEM_TAKEN_WITH:
		nRet = 0;
		if (uParam)
		{
			int nCount = 0;
			KUiObjAtRegion* pInfo = (KUiObjAtRegion*)uParam;
			PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
			if (pItem && pItem->nPlace == pos_equiproom)
			{
				pInfo->Obj.uGenre = CGOG_ITEM;
				pInfo->Obj.uId = pItem->nIdx;
				pInfo->Region.h = pItem->nX;
				pInfo->Region.v = pItem->nY;
				pInfo->Region.Width = Item[pItem->nIdx].GetWidth();
				pInfo->Region.Height = Item[pItem->nIdx].GetHeight();
				nCount++;
				pInfo++;
			}
			while(pItem)
			{
				pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
				if (pItem && pItem->nPlace == pos_equiproom)
				{
					pInfo->Obj.uGenre = CGOG_ITEM;
					pInfo->Obj.uId = pItem->nIdx;
					pInfo->Region.h = pItem->nX;
					pInfo->Region.v = pItem->nY;
					pInfo->Region.Width = Item[pItem->nIdx].GetWidth();
					pInfo->Region.Height = Item[pItem->nIdx].GetHeight();
					nCount++;
					pInfo++;
				}
				if (nCount > nParam)
					break;
			}
			nRet = nCount;
		}
		else
		{
			int nCount = 0;
			PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
			if (pItem && pItem->nPlace == pos_equiproom)
				nCount++;
			while(pItem)
			{
				pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
				if (pItem && pItem->nPlace == pos_equiproom)
					nCount++;
			}
			nRet = nCount;
		}
		break;

	//����װ����Ʒ
	//uParam = (KUiObjAtRegion*)pInfo -> ����10��Ԫ�ص�KUiObjAtRegion�ṹ����ָ�룬
	//			KUiObjAtRegion�ṹ���ڴ洢װ�������ݺͷ���λ����Ϣ��
	//			KUiObjAtRegion::Region::h ��ʾ���ڵڼ���װ��
	//			KUiObjAtRegion::Region::v ��ʾ�����ĸ�λ�õ�װ��,��ֵΪ÷������
	//			UI_EQUIPMENT_POSITION��ȡֵ֮һ����ο�UI_EQUIPMENT_POSITION��ע�͡�
	//nParam =	Ҫ��ȡ���ǵڼ���װ����Ϣ
	//Return =  ��ֵ��ʾpInfo�����е�ǰ���ٸ�KUiObjAtRegion�ṹ���������Ч�����ݡ�
	// new add
		//
	case GDI_EQUIPMENT:
		nRet = 0;
		if (uParam)
		{
			// TODO����ʱû�����ڶ���װ��
			if (nParam == 1)
				break;

			int PartConvert[itempart_num] =
			{
				UIEP_HEAD,		UIEP_BODY,
				UIEP_WAIST,		UIEP_HAND,
				UIEP_FOOT,		UIEP_FINESSE,
				UIEP_NECK,		UIEP_FINGER1,
				UIEP_FINGER2,	UIEP_WAIST_DECOR,
				UIEP_HORSE,		UIEP_SIGNET,
				UIEP_SHIPIN,
			};

			int nCount = 0;
			KUiObjAtRegion* pInfo = (KUiObjAtRegion*)uParam;

			for (int i = 0; i < itempart_num; i++)
			{
				pInfo->Obj.uId = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipment(i);
				if (pInfo->Obj.uId)
				{
					pInfo->Obj.uGenre = CGOG_ITEM;

					pInfo->Region.Width = Item[pInfo->Obj.uId].GetWidth();
					pInfo->Region.Height = Item[pInfo->Obj.uId].GetHeight();
					pInfo->Region.h = 0;
					pInfo->Region.v = PartConvert[i];
				}
				else
				{
					pInfo->Obj.uGenre = CGOG_NOTHING;
				}
				nCount++;
				pInfo++;
			}
			nRet = nCount;
		}
		break;
	case GDI_PARADE_EQUIPMENT:
		nRet = 0;
		if (uParam)
		{
			// TODO����ʱû�����ڶ���װ��
			if (nParam == 1)
				break;

			int PartConvert[itempart_num] =
			{
				UIEP_HEAD,		UIEP_BODY,
				UIEP_WAIST,		UIEP_HAND,
				UIEP_FOOT,		UIEP_FINESSE,
				UIEP_NECK,		UIEP_FINGER1,
				UIEP_FINGER2,	UIEP_WAIST_DECOR,
				UIEP_HORSE,		UIEP_SIGNET,
				UIEP_SHIPIN,
			};

			int nCount = 0;
			KUiObjAtRegion* pInfo = (KUiObjAtRegion*)uParam;

			for (int i = 0; i < itempart_num; i++)
			{
				pInfo->Obj.uId = g_cViewItem.m_sItem[i].nIdx;
				if (pInfo->Obj.uId)
				{
					pInfo->Obj.uGenre = CGOG_PLAYERVIEWITEM;

					pInfo->Region.Width = Item[pInfo->Obj.uId].GetWidth();
					pInfo->Region.Height = Item[pInfo->Obj.uId].GetHeight();
					pInfo->Region.h = 0;
					pInfo->Region.v = PartConvert[i];
				}
				else
				{
					pInfo->Obj.uGenre = CGOG_NOTHING;
				}
				nCount++;
				pInfo++;
			}
			nRet = nCount;
		}
		break;

	//���ײ�����ص�����
	//uParam = (UI_TRADE_OPER_DATA)eOper ���庬���UI_TRADE_OPER_DATA
	//nParam ����Ӧ���뺬����uParam��ȡֵ״������,��UI_TRADE_OPER_DATA��˵��
	//Return ���庬����uParam��ȡֵ״������,��UI_TRADE_OPER_DATA��˵��
	//���ǵ����������
	case GDI_TRADE_OPER_DATA:
		if (uParam == UTOD_IS_LOCKED)
			nRet = Player[CLIENT_PLAYER_INDEX].m_cTrade.m_nTradeLock;
		else if (uParam == UTOD_IS_TRADING)
			nRet = Player[CLIENT_PLAYER_INDEX].m_cTrade.m_nTradeState;
		else if (uParam == UTOD_IS_OTHER_LOCKED)
			nRet = Player[CLIENT_PLAYER_INDEX].m_cTrade.m_nTradeDestLock;
		else if (uParam == UTOD_IS_OTHER_TRADING)
			nRet = Player[CLIENT_PLAYER_INDEX].m_cTrade.m_nTradeDestState;
		else if (uParam == UTOD_IS_WILLING)
		{
			nRet = (Player[CLIENT_PLAYER_INDEX].m_cMenuState.m_nState == PLAYER_MENU_STATE_TRADEOPEN);
		}
		break;
	//�������յĸ��������
	//uParam = (unsigned int*) pSkills -> ����10��unsigned int���������ڴ洢��������ܵ�id��

	//ʣ��ս�����ܵ���
	//Return = ʣ��ս�����ܵ���
	case GDI_FIGHT_SKILL_POINT:
		//to do:no implements in this version;
		nRet = Player[CLIENT_PLAYER_INDEX].m_nSkillPoint;
		break;

	//�������յĸ���ս������
	//uParam = (unsigned int*) pSkills -> ����30��unsigned int���������ڴ洢����ս�����ܵ�id��
	case GDI_FIGHT_SKILLS:
		if (uParam)
		{
			KUiSkillData* pSkills = (KUiSkillData*)uParam;
			Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetSkillSortList(pSkills);
		}
		break;
	//��ʾ��������б�
	//uParam = (KUiSkillData*) pSkills -> ����65��KUiSkillData���������ڴ洢�����ܵ����ݡ�
	//								KUiSkillData::nLevel��������������ʾ�ڵڼ���
	//Return = ������Ч���ݵ�Skills����Ŀ
	case GDI_LEFT_ENABLE_SKILLS:
		{
			KUiSkillData * pSkills = (KUiSkillData*)uParam;
			int nCount = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetLeftSkillSortList(pSkills);
			return nCount;

		}
		break;
	//��ʾ�Ҽ������б�
	//uParam = (KUiSkillData*) pSkills -> ����65��KUiSkillData���������ڴ洢�����ܵ����ݡ�
	//								KUiSkillData::nLevel��������������ʾ�ڵڼ���
	//Return = ������Ч���ݵ�Skills����Ŀ
	case GDI_RIGHT_ENABLE_SKILLS:
		{
			KUiSkillData * pSkills = (KUiSkillData*)uParam;
			int nCount = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetRightSkillSortList(pSkills);
			return nCount;
		}
		break;
	//���ǵ��Դ��书
	//uParam = (unsigned int*) pSkills -> ����5��unsigned int���������ڴ洢�����Դ��书��id��
	case GDI_CUSTOM_SKILLS:
		if (uParam)
		{
			//to do: no implements in this version;
			KUiSkillData* pSkills = (KUiSkillData*)uParam;
			memset(pSkills,0,sizeof(KUiSkillData)*5);
		}
		break;
	//��ȡ��Χ��ҵ��б�
	//uParam = (KUiPlayerItem*)pList -> ��Ա��Ϣ�б�
	//			KUiPlayerItem::nData = 0
	//nParam = pList�����а���KUiPlayerItem�ṹ����Ŀ
	//Return = �������ֵС�ڵ��ڴ������nParam����ֵ��ʾpList�����е�ǰ���ٸ�KUiPlayerItem
	//			�ṹ���������Ч�����ݣ������ʾ��Ҫ����������ٸ�KUiPlayerItem�ṹ������
	//			�Ź��洢ȫ����Ա��Ϣ��
	case GDI_NEARBY_PLAYER_LIST:
		nRet = NpcSet.GetAroundPlayer((KUiPlayerItem*)uParam, nParam);
		break;

	//��ȡ��Χ�µ������������ҵ��б�
	//��������ͬGDI_NEARBY_PLAYER_LIST
	case GDI_NEARBY_IDLE_PLAYER_LIST:
		nRet = NpcSet.GetAroundPlayerForTeamInvite((KUiPlayerItem*)uParam, nParam);
		break;

	//����ͳ˧������ص�����
	//uParam = (KUiPlayerLeaderShip*) -> ����ͳ˧������ص����ݽṹָ��
	case GDI_PLAYER_LEADERSHIP:
		if (uParam)
		{
			KUiPlayerLeaderShip* pInfo = (KUiPlayerLeaderShip*)uParam ;
			pInfo->nLeaderShipExperience = Player[CLIENT_PLAYER_INDEX].m_nLeadExp ;		//ͳ˧������ֵ
			//to do: waiting for...;
			pInfo->nLeaderShipExperienceFull = Player[CLIENT_PLAYER_INDEX].m_nNextLevelLeadExp;//ͳ˧������ֵ
			pInfo->nLeaderShipLevel = Player[CLIENT_PLAYER_INDEX].m_nLeadLevel ;			//ͳ˧���ȼ�
		}
		break;
	//�����Ʒ��ĳ������λ�õ�����״̬
	//uParam = (KUiGameObject*)pObj����nParam==0ʱ����Ʒ����Ϣ
	//uParam = (KUiObjAtContRegion*)pObj����nParam!=0ʱ����Ʒ����Ϣ
	//nParam = (int)(bool)bJustTry  �Ƿ�ֻ�ǳ��Է���
	//Return = (ITEM_IN_ENVIRO_PROP)eProp ��Ʒ������״̬
	case GDI_ITEM_IN_ENVIRO_PROP:
		{
			if (!nParam)
			{
				KUiGameObject *pObj = (KUiGameObject *)uParam;
				if (pObj->uGenre != CGOG_ITEM && pObj->uGenre != CGOG_NPCSELLITEM && pObj->uGenre != CGOG_PLAYERSELLITEM && pObj->uGenre != CGOG_PLAYERVIEWITEM)
					break;

				KItem* pItem = NULL;

				if ((pObj->uGenre == CGOG_ITEM || pObj->uGenre == CGOG_PLAYERSELLITEM || pObj->uGenre == CGOG_PLAYERVIEWITEM) &&
					IsValidClientItemIndex(pObj->uId))
				{
					pItem = &Item[pObj->uId];
				}
				else if (pObj->uGenre == CGOG_NPCSELLITEM)
				{
					int nIdx = BuySell.GetItemIndex(Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nShopIdx[Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nCurShop], pObj->uId);
					pItem = BuySell.GetItem(nIdx);
				}
				_ASSERT(pItem);
				if (!pItem || pItem->GetGenre() != item_equip)
					break;

				if (Player[CLIENT_PLAYER_INDEX].m_ItemList.CanEquip(pItem))
				{
					nRet = IIEP_NORMAL;
				}
				else
				{
					nRet = IIEP_NOT_USEABLE;
				}
			}
			else
			{
				KUiObjAtContRegion *pObj = (KUiObjAtContRegion *)uParam;
				if (pObj->Obj.uGenre != CGOG_ITEM && pObj->Obj.uGenre != CGOG_NPCSELLITEM &&
					pObj->Obj.uGenre != CGOG_PLAYERSELLITEM && pObj->Obj.uGenre != CGOG_PLAYERVIEWITEM)
					break;
				if (pObj->Obj.uGenre != CGOG_NPCSELLITEM &&
					!IsValidClientItemIndex(pObj->Obj.uId))
					break;

				int PartConvert[itempart_num] =
				{
					itempart_head,		itempart_weapon,
					itempart_amulet,	itempart_cuff,
					itempart_body,		itempart_belt,
					itempart_ring1,		itempart_ring2,
					itempart_pendant,	itempart_foot,
					itempart_horse,		itempart_signet,
					itempart_shipin,
				};

				_ASSERT(pObj->eContainer < itempart_num);
				if (pObj->eContainer >= itempart_num || pObj->eContainer < 0)
					break;

				if (Item[pObj->Obj.uId].GetGenre() != item_equip)
					break;

				int nPlace = PartConvert[pObj->eContainer];
				if (Player[CLIENT_PLAYER_INDEX].m_ItemList.CanEquip(pObj->Obj.uId, nPlace))
				{
					nRet = IIEP_NORMAL;
				}
				else
				{
					nRet = IIEP_NOT_USEABLE;
				}
			}
		}
		break;
	case GDI_IMMEDIATEITEM_NUM:
		if (uParam >= 0 && uParam < IMMEDIACY_ROOM_WIDTH)
			nRet = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetSameDetailItemNum(uParam);
		break;
	//��NPC������
	//uParam = (KUiObjAtContRegion*) pInfo -> KUiObjAtRegion�ṹ�����ָ�룬KUiObjAtRegion
	//				�ṹ���ڴ洢��Ʒ�����ݼ����������λ����Ϣ��
	//				����KUiObjAtContRegion::nContainerֵ��ʾ�ڼ�ҳ����Ʒ
	//nParam = pInfo�����а���KUiObjAtRegion�ṹ����Ŀ
	//Return = �������ֵС�ڵ��ڴ������nParam����ֵ��ʾpInfo�����е�ǰ���ٸ�KUiObjAtRegion
	//			�ṹ���������Ч�����ݣ������ʾ��Ҫ����������ٸ�KUiObjAtRegion�ṹ������
	//			�Ź��洢ȫ����npc�г������׵���Ʒ��Ϣ��
	case GDI_TRADE_NPC_ITEM:
		nRet = 0;
		if (uParam)
		{
			int nCount = 0;
			int nPage = 0;
			int nIndex = 0;
			KUiObjAtContRegion* pInfo = (KUiObjAtContRegion *)uParam;
			int	nBuyIdx = Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nShopIdx[Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nCurShop];

			if (nBuyIdx == -1)
				break;
			if (nBuyIdx >= BuySell.GetHeight())
				break;
			if (!BuySell.m_pShopRoom)
				break;
			BuySell.m_pShopRoom->Clear();
			for (int i = 0; i < BuySell.GetWidth(); i++)
			{
				nIndex = BuySell.GetItemIndex(nBuyIdx, i);
				KItem* pItem = BuySell.GetItem(nIndex);

				if (nIndex >= 0 && pItem)
				{
					// Set pInfo->Obj.uGenre
					pInfo->Obj.uGenre = CGOG_NPCSELLITEM;
					// Set pInfo->Obj.uId
					pInfo->Obj.uId = i;

					POINT	Pos;
					if (BuySell.m_pShopRoom->FindRoom(pItem->GetWidth(), pItem->GetHeight(), &Pos))
					{
						// nIndex + 1��֤��Ϊ0
						BuySell.m_pShopRoom->PlaceItem(Pos.x, Pos.y, nIndex + 1, pItem->GetWidth(), pItem->GetHeight());
					}
					else
					{
						nPage++;
						BuySell.m_pShopRoom->Clear();
						// Clear��ɺ��Ȼ�ɹ�������û���ж�
						BuySell.m_pShopRoom->FindRoom(pItem->GetWidth(), pItem->GetHeight(), &Pos);
						BuySell.m_pShopRoom->PlaceItem(Pos.x, Pos.y, nIndex + 1, pItem->GetWidth(), pItem->GetHeight());
					}
					pInfo->Region.h = Pos.x;
					pInfo->Region.v = Pos.y;
					pInfo->Region.Width = pItem->GetWidth();
					pInfo->Region.Height = pItem->GetHeight();
					pInfo->nContainer = nPage;
					nCount++;
					pInfo++;
				}
			}
			nRet = nCount;
		}
		else
		{
			int nCount = 0;
			int nIndex = 0;
			int	nBuyIdx = Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nShopIdx[Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nCurShop];
			if (nBuyIdx == -1)
				break;
			if (nBuyIdx >= BuySell.GetHeight())
				break;
			for (int i = 0; i < BuySell.GetWidth(); i++)
			{
				nIndex = BuySell.GetItemIndex(nBuyIdx, i);
				KItem* pItem = BuySell.GetItem(nIndex);

				if (nIndex >= 0 && pItem)
				{
					nCount++;
				}
			}
			nRet = nCount;
		}
		break;
	case GDI_ITEM_IN_STORE_BOX:
		nRet = 0;
		if (uParam)
		{
			int nCount = 0;
			KUiObjAtRegion* pInfo = (KUiObjAtRegion*)uParam;
			pInfo->Obj.uGenre = CGOG_MONEY;
			pInfo->Obj.uId = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetMoney(room_repository);
			nCount++;
			pInfo++;
			PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
			if (pItem && pItem->nPlace == pos_repositoryroom)
			{
				pInfo->Obj.uGenre = CGOG_ITEM;
				pInfo->Obj.uId = pItem->nIdx;
				pInfo->Region.h = pItem->nX;
				pInfo->Region.v = pItem->nY;
				pInfo->Region.Width = Item[pItem->nIdx].GetWidth();
				pInfo->Region.Height = Item[pItem->nIdx].GetHeight();
				nCount++;
				pInfo++;
			}
			while(pItem)
			{
				pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
				if (pItem && pItem->nPlace == pos_repositoryroom)
				{
					pInfo->Obj.uGenre = CGOG_ITEM;
					pInfo->Obj.uId = pItem->nIdx;
					pInfo->Region.h = pItem->nX;
					pInfo->Region.v = pItem->nY;
					pInfo->Region.Width = Item[pItem->nIdx].GetWidth();
					pInfo->Region.Height = Item[pItem->nIdx].GetHeight();
					nCount++;
					pInfo++;
				}
				if (nCount > nParam)
					break;
			}
			nRet = nCount;
		}
		else
		{
			int nCount = 0;
			// ��һ����Ǯ
			nCount++;
			// ������װ��
			PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
			if (pItem && pItem->nPlace == pos_repositoryroom)
				nCount++;
			while(pItem)
			{
				pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
				if (pItem && pItem->nPlace == pos_repositoryroom)
					nCount++;
			}
			nRet = nCount;
		}
		break;
	case GDI_ITEM_IN_STORE_BOX1:
		nRet = 0;
		if (uParam)
		{
			int nCount = 0;
			KUiObjAtRegion* pInfo = (KUiObjAtRegion*)uParam;
			pInfo->Obj.uGenre = CGOG_MONEY;
			pInfo->Obj.uId = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetMoney(room_repository);
			nCount++;
			pInfo++;
			PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
			if (pItem && pItem->nPlace == pos_repositoryroom1)
			{
				pInfo->Obj.uGenre = CGOG_ITEM;
				pInfo->Obj.uId = pItem->nIdx;
				pInfo->Region.h = pItem->nX;
				pInfo->Region.v = pItem->nY;
				pInfo->Region.Width = Item[pItem->nIdx].GetWidth();
				pInfo->Region.Height = Item[pItem->nIdx].GetHeight();
				nCount++;
				pInfo++;
			}
			while(pItem)
			{
				pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
				if (pItem && pItem->nPlace == pos_repositoryroom1)
				{
					pInfo->Obj.uGenre = CGOG_ITEM;
					pInfo->Obj.uId = pItem->nIdx;
					pInfo->Region.h = pItem->nX;
					pInfo->Region.v = pItem->nY;
					pInfo->Region.Width = Item[pItem->nIdx].GetWidth();
					pInfo->Region.Height = Item[pItem->nIdx].GetHeight();
					nCount++;
					pInfo++;
				}
				if (nCount > nParam)
					break;
			}
			nRet = nCount;
		}
		else
		{
			int nCount = 0;
			// ��һ����Ǯ
			nCount++;
			// ������װ��
			PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
			if (pItem && pItem->nPlace == pos_repositoryroom1)
				nCount++;
			while(pItem)
			{
				pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
				if (pItem && pItem->nPlace == pos_repositoryroom1)
					nCount++;
			}
			nRet = nCount;
		}
		break;
	case GDI_ITEM_IN_STORE_BOX2:
		nRet = 0;
		if (uParam)
		{
			int nCount = 0;
			KUiObjAtRegion* pInfo = (KUiObjAtRegion*)uParam;
			pInfo->Obj.uGenre = CGOG_MONEY;
			pInfo->Obj.uId = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetMoney(room_repository);
			nCount++;
			pInfo++;
			PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
			if (pItem && pItem->nPlace == pos_repositoryroom2)
			{
				pInfo->Obj.uGenre = CGOG_ITEM;
				pInfo->Obj.uId = pItem->nIdx;
				pInfo->Region.h = pItem->nX;
				pInfo->Region.v = pItem->nY;
				pInfo->Region.Width = Item[pItem->nIdx].GetWidth();
				pInfo->Region.Height = Item[pItem->nIdx].GetHeight();
				nCount++;
				pInfo++;
			}
			while(pItem)
			{
				pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
				if (pItem && pItem->nPlace == pos_repositoryroom2)
				{
					pInfo->Obj.uGenre = CGOG_ITEM;
					pInfo->Obj.uId = pItem->nIdx;
					pInfo->Region.h = pItem->nX;
					pInfo->Region.v = pItem->nY;
					pInfo->Region.Width = Item[pItem->nIdx].GetWidth();
					pInfo->Region.Height = Item[pItem->nIdx].GetHeight();
					nCount++;
					pInfo++;
				}
				if (nCount > nParam)
					break;
			}
			nRet = nCount;
		}
		else
		{
			int nCount = 0;
			// ��һ����Ǯ
			nCount++;
			// ������װ��
			PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
			if (pItem && pItem->nPlace == pos_repositoryroom2)
				nCount++;
			while(pItem)
			{
				pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
				if (pItem && pItem->nPlace == pos_repositoryroom2)
					nCount++;
			}
			nRet = nCount;
		}
		break;
	case GDI_ITEM_IN_STORE_BOX3:
		nRet = 0;
		if (uParam)
		{
			int nCount = 0;
			KUiObjAtRegion* pInfo = (KUiObjAtRegion*)uParam;
			pInfo->Obj.uGenre = CGOG_MONEY;
			pInfo->Obj.uId = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetMoney(room_repository);
			nCount++;
			pInfo++;
			PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
			if (pItem && pItem->nPlace == pos_repositoryroom3)
			{
				pInfo->Obj.uGenre = CGOG_ITEM;
				pInfo->Obj.uId = pItem->nIdx;
				pInfo->Region.h = pItem->nX;
				pInfo->Region.v = pItem->nY;
				pInfo->Region.Width = Item[pItem->nIdx].GetWidth();
				pInfo->Region.Height = Item[pItem->nIdx].GetHeight();
				nCount++;
				pInfo++;
			}
			while(pItem)
			{
				pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
				if (pItem && pItem->nPlace == pos_repositoryroom3)
				{
					pInfo->Obj.uGenre = CGOG_ITEM;
					pInfo->Obj.uId = pItem->nIdx;
					pInfo->Region.h = pItem->nX;
					pInfo->Region.v = pItem->nY;
					pInfo->Region.Width = Item[pItem->nIdx].GetWidth();
					pInfo->Region.Height = Item[pItem->nIdx].GetHeight();
					nCount++;
					pInfo++;
				}
				if (nCount > nParam)
					break;
			}
			nRet = nCount;
		}
		else
		{
			int nCount = 0;
			// ��һ����Ǯ
			nCount++;
			// ������װ��
			PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
			if (pItem && pItem->nPlace == pos_repositoryroom3)
				nCount++;
			while(pItem)
			{
				pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
				if (pItem && pItem->nPlace == pos_repositoryroom3)
					nCount++;
			}
			nRet = nCount;
		}
		break;
	case GDI_ITEM_IN_STORE_BOX4:
		nRet = 0;
		if (uParam)
		{
			int nCount = 0;
			KUiObjAtRegion* pInfo = (KUiObjAtRegion*)uParam;
			pInfo->Obj.uGenre = CGOG_MONEY;
			pInfo->Obj.uId = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetMoney(room_repository);
			nCount++;
			pInfo++;
			PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
			if (pItem && pItem->nPlace == pos_repositoryroom4)
			{
				pInfo->Obj.uGenre = CGOG_ITEM;
				pInfo->Obj.uId = pItem->nIdx;
				pInfo->Region.h = pItem->nX;
				pInfo->Region.v = pItem->nY;
				pInfo->Region.Width = Item[pItem->nIdx].GetWidth();
				pInfo->Region.Height = Item[pItem->nIdx].GetHeight();
				nCount++;
				pInfo++;
			}
			while(pItem)
			{
				pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
				if (pItem && pItem->nPlace == pos_repositoryroom4)
				{
					pInfo->Obj.uGenre = CGOG_ITEM;
					pInfo->Obj.uId = pItem->nIdx;
					pInfo->Region.h = pItem->nX;
					pInfo->Region.v = pItem->nY;
					pInfo->Region.Width = Item[pItem->nIdx].GetWidth();
					pInfo->Region.Height = Item[pItem->nIdx].GetHeight();
					nCount++;
					pInfo++;
				}
				if (nCount > nParam)
					break;
			}
			nRet = nCount;
		}
		else
		{
			int nCount = 0;
			// ��һ����Ǯ
			nCount++;
			// ������װ��
			PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
			if (pItem && pItem->nPlace == pos_repositoryroom4)
				nCount++;
			while(pItem)
			{
				pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
				if (pItem && pItem->nPlace == pos_repositoryroom4)
					nCount++;
			}
			nRet = nCount;
		}
		break;
	case GDI_ITEM_IN_STORE_BOX5:
		nRet = 0;
		if (uParam)
		{
			int nCount = 0;
			KUiObjAtRegion* pInfo = (KUiObjAtRegion*)uParam;
			pInfo->Obj.uGenre = CGOG_MONEY;
			pInfo->Obj.uId = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetMoney(room_repository);
			nCount++;
			pInfo++;
			PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
			if (pItem && pItem->nPlace == pos_repositoryroom5)
			{
				pInfo->Obj.uGenre = CGOG_ITEM;
				pInfo->Obj.uId = pItem->nIdx;
				pInfo->Region.h = pItem->nX;
				pInfo->Region.v = pItem->nY;
				pInfo->Region.Width = Item[pItem->nIdx].GetWidth();
				pInfo->Region.Height = Item[pItem->nIdx].GetHeight();
				nCount++;
				pInfo++;
			}
			while(pItem)
			{
				pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
				if (pItem && pItem->nPlace == pos_repositoryroom5)
				{
					pInfo->Obj.uGenre = CGOG_ITEM;
					pInfo->Obj.uId = pItem->nIdx;
					pInfo->Region.h = pItem->nX;
					pInfo->Region.v = pItem->nY;
					pInfo->Region.Width = Item[pItem->nIdx].GetWidth();
					pInfo->Region.Height = Item[pItem->nIdx].GetHeight();
					nCount++;
					pInfo++;
				}
				if (nCount > nParam)
					break;
			}
			nRet = nCount;
		}
		else
		{
			int nCount = 0;
			// ��һ����Ǯ
			nCount++;
			// ������װ��
			PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
			if (pItem && pItem->nPlace == pos_repositoryroom5)
				nCount++;
			while(pItem)
			{
				pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
				if (pItem && pItem->nPlace == pos_repositoryroom5)
					nCount++;
			}
			nRet = nCount;
		}
		break;
	case GDI_ITEM_EXPANDITEM:
		nRet = 0;
		if (uParam)
		{
			int nCount = 0;
			KUiObjAtRegion* pInfo = (KUiObjAtRegion*)uParam;
			pInfo->Obj.uGenre = CGOG_MONEY;
			pInfo->Obj.uId = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetMoney(room_equipmentex);
			nCount++;
			pInfo++;
			PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
			if (pItem && pItem->nPlace == pos_equiproomex)
			{
				pInfo->Obj.uGenre = CGOG_ITEM;
				pInfo->Obj.uId = pItem->nIdx;
				pInfo->Region.h = pItem->nX;
				pInfo->Region.v = pItem->nY;
				pInfo->Region.Width = Item[pItem->nIdx].GetWidth();
				pInfo->Region.Height = Item[pItem->nIdx].GetHeight();
				nCount++;
				pInfo++;
			}
			while(pItem)
			{
				pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
				if (pItem && pItem->nPlace == pos_equiproomex)
				{
					pInfo->Obj.uGenre = CGOG_ITEM;
					pInfo->Obj.uId = pItem->nIdx;
					pInfo->Region.h = pItem->nX;
					pInfo->Region.v = pItem->nY;
					pInfo->Region.Width = Item[pItem->nIdx].GetWidth();
					pInfo->Region.Height = Item[pItem->nIdx].GetHeight();
					nCount++;
					pInfo++;
				}
				if (nCount > nParam)
					break;
			}
			nRet = nCount;
		}
		else
		{
			int nCount = 0;
			// ��һ����Ǯ
			nCount++;
			// ������װ��
			PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
			if (pItem && pItem->nPlace == pos_equiproomex)
				nCount++;
			while(pItem)
			{
				pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
				if (pItem && pItem->nPlace == pos_equiproomex)
					nCount++;
			}
			nRet = nCount;
		}
		break;
	case GDI_PK_SETTING:					//��ȡpk����
		nRet = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentCamp; // PT raw camp
		break;
	case GDI_SHOW_PLAYERS_NAME:			//��ȡ��ʾ���������
		nRet = NpcSet.CheckShowName();
		break;
	case GDI_SHOW_PLAYERS_LIFE:			//��ȡ��ʾ���������
		nRet = NpcSet.CheckShowLife();
		break;
	case GDI_ITEM_GIVE:
		nRet = 0;
		if (uParam)
		{
			int nCount = 0;
			KUiObjAtRegion* pInfo = (KUiObjAtRegion*)uParam;

			PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
			if (pItem && pItem->nPlace == pos_give)
			{
				pInfo->Obj.uGenre = CGOG_ITEM;
				pInfo->Obj.uId = pItem->nIdx;
				pInfo->Region.h = pItem->nX;
				pInfo->Region.v = pItem->nY;
				pInfo->Region.Width = Item[pItem->nIdx].GetWidth();
				pInfo->Region.Height = Item[pItem->nIdx].GetHeight();
				nCount++;
				pInfo++;
			}
			while(pItem)
			{
				pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
				if (pItem && pItem->nPlace == pos_give)
				{
					pInfo->Obj.uGenre = CGOG_ITEM;
					pInfo->Obj.uId = pItem->nIdx;
					pInfo->Region.h = pItem->nX;
					pInfo->Region.v = pItem->nY;
					pInfo->Region.Width = Item[pItem->nIdx].GetWidth();
					pInfo->Region.Height = Item[pItem->nIdx].GetHeight();
					nCount++;
					pInfo++;
				}
				if (nCount > nParam)
					break;
			}
			nRet = nCount;
		}
		else
		{
			int nCount = 0;

//			nCount++;

			PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
			if (pItem && pItem->nPlace == pos_give)
			{
				nCount++;
			}
			while(pItem)
			{
				pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
				if (pItem && pItem->nPlace == pos_give)
				{
					nCount++;
				}
			}
			nRet = nCount;
		}
		break;
	case GDI_EXBOX_NUM:
			nRet = Player[CLIENT_PLAYER_INDEX].m_btRepositoryNum;
			break;
	case GDI_EQUIPEX_TIME:
			nRet = Player[CLIENT_PLAYER_INDEX].m_dwEquipExpandTime - KSG_GetCurSec();
			break;
	#include "PhongThanInventoryUiQuery.inl"
	case GDI_ITEM_NAME:
		if(uParam)
		{
			char* pzName = (char*)uParam;
			pzName[0] = 0;
			if (IsValidClientItemIndex((unsigned int)nParam))
				PTPlainItemName(pzName,64,Item[nParam].GetName());
		}
		break;
	case GDI_ITEM_QUALITY:
		if(IsValidClientItemIndex(uParam))
		{
			nRet = Item[uParam].GetQuality();
		}
		break;
	case GDI_ITEM_DESC:
		if (uParam)
		{
			char* pszDescript = (char *)uParam;
			pszDescript[0] = 0;
			if (IsValidClientItemIndex((unsigned int)nParam))
				Item[nParam].GetDesc(pszDescript);
		}
		break;
	case GDI_CHAT_ITEM_IMAGE: // TamLTM fix post item;
		if (uParam)
		{
			char* pszImage = (char*)uParam;
			pszImage[0] = 0;
			if (IsValidClientItemIndex((unsigned int)nParam))
			{
				strcpy(pszImage, Item[nParam].GetImageName());
				if (pszImage[0])
					nRet = 1;
			}
		}
		break;
	case GDI_GET_ITEM_PARAM:
        if(uParam && IsValidClientItemIndex((unsigned)nParam)) {
            sprintf((char*)uParam,"PTI:%u",Item[nParam].GetID());nRet=1;
        }
        break;
    case GDI_ITEM_CHAT:
        if(uParam && nParam)nRet=PTCreateItemLinkPreview((const char*)nParam);
        else if(!uParam)PTReleaseItemLinkPreview(nParam);
        break;
	case GDI_SHOW_OBJ_NAME:
			nRet = NpcSet.CheckShowObj();
			break;
	case GDI_TRADE_PLAYER_ITEM:
		nRet = 0;
		if (uParam)
		{
			int nCount = 0;
			KUiObjAtContRegion* pInfo = (KUiObjAtContRegion*)uParam;

			for (int i = 0; i < MAX_EQUIPMENT_ITEM; i++)
			{
				if (g_cSellItem.m_sItem[i].nIdx)
				{
					pInfo->Obj.uGenre = CGOG_PLAYERSELLITEM;
					pInfo->Obj.uId = g_cSellItem.m_sItem[i].nIdx;

					pInfo->Region.Width = Item[pInfo->Obj.uId].GetWidth();
					pInfo->Region.Height = Item[pInfo->Obj.uId].GetHeight();
					pInfo->Region.h = g_cSellItem.m_sItem[i].nX;
					pInfo->Region.v = g_cSellItem.m_sItem[i].nY;
					nCount++;
					pInfo++;
				}
			}

			nRet = nCount;
		}
		else
		{
			nRet = g_cSellItem.GetCount();
		}
		break;
	case GDI_COMPOUND:
		nRet = 0;
		if (uParam)
		{
			// TODO����ʱû�����ڶ���װ��
			if (nParam == 1)
				break;

			int PartCompoundConvert[compound_num] =
			{
				MOSAICENCRUSTED_UIEP_BOX_1,
				MOSAICENCRUSTED_UIEP_BOX_2,
				MOSAICENCRUSTED_UIEP_BOX_3,
			};

			int nCount = 0;
			KUiObjAtRegion* pInfo = (KUiObjAtRegion*)uParam;

			for (int i = 0; i < MAX_COMPOUND_ITEM; i++)
			{
				pInfo->Obj.uId = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetCompoundItem(i);
				if (pInfo->Obj.uId)
				{
					pInfo->Obj.uGenre = CGOG_ITEM;

					pInfo->Region.Width = Item[pInfo->Obj.uId].GetWidth();
					pInfo->Region.Height = Item[pInfo->Obj.uId].GetHeight();
					pInfo->Region.h = 0;
					pInfo->Region.v = PartCompoundConvert[i];
				}
				else
				{
					pInfo->Obj.uGenre = CGOG_NOTHING;
				}
				nCount++;
				pInfo++;
			}
			nRet = nCount;
		}
		break;
		//TamLTM kham nam
	case GDI_BUILD_ITEM:
		nRet = 0;
		if (uParam)
		{
			// TODO?????�????????????
			if (nParam == 1)
				break;

			int PartBuildItem[MAX_PART_BUILD] =
			{
				UIEP_BUILDITEM1,
				UIEP_BUILDITEM2,
				UIEP_BUILDITEM3,
				UIEP_BUILDITEM4,
				UIEP_BUILDITEM5,
				UIEP_BUILDITEM6,
				UIEP_BUILDITEM7,
				UIEP_BUILDITEM8,
				UIEP_BUILDITEM9,
			};

			int nCount = 0;
			KUiObjAtRegion* pInfo = (KUiObjAtRegion*)uParam;

			for (int i = 0; i < MAX_PART_BUILD; i++)
			{
				pInfo->Obj.uId = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetBuildItem(i);
				if (pInfo->Obj.uId)
				{
					pInfo->Obj.uGenre = CGOG_ITEM;

					pInfo->Region.Width = Item[pInfo->Obj.uId].GetWidth();
					pInfo->Region.Height = Item[pInfo->Obj.uId].GetHeight();
					pInfo->Region.h = 0;
					pInfo->Region.v = PartBuildItem[i];
				}
				else
				{
					pInfo->Obj.uGenre = CGOG_NOTHING;
				}
				nCount++;
				pInfo++;
			}
			nRet = nCount;
		}
		break;
		//End code
	case GDI_COMPOUND_BOX:
		nRet = 0;
		if (uParam)
		{
			int nCount = 0;
			KUiObjAtRegion* pInfo = (KUiObjAtRegion*)uParam;

			PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
			if (pItem && pItem->nPlace == pos_compoundroom)
			{
				pInfo->Obj.uGenre = CGOG_ITEM;
				pInfo->Obj.uId = pItem->nIdx;
				pInfo->Region.h = pItem->nX;
				pInfo->Region.v = pItem->nY;
				pInfo->Region.Width = Item[pItem->nIdx].GetWidth();
				pInfo->Region.Height = Item[pItem->nIdx].GetHeight();
				nCount++;
				pInfo++;
			}
			while(pItem)
			{
				pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
				if (pItem && pItem->nPlace == pos_compoundroom)
				{
					pInfo->Obj.uGenre = CGOG_ITEM;
					pInfo->Obj.uId = pItem->nIdx;
					pInfo->Region.h = pItem->nX;
					pInfo->Region.v = pItem->nY;
					pInfo->Region.Width = Item[pItem->nIdx].GetWidth();
					pInfo->Region.Height = Item[pItem->nIdx].GetHeight();
					nCount++;
					pInfo++;
				}
				if (nCount > nParam)
					break;
			}
			nRet = nCount;
		}
		else
		{
			int nCount = 0;
			// ��һ����Ǯ
			nCount++;
			// ������װ��
			PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
			if (pItem && pItem->nPlace == pos_compoundroom)
				nCount++;
			while(pItem)
			{
				pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
				if (pItem && pItem->nPlace == pos_compoundroom)
					nCount++;
			}
			nRet = nCount;
		}
		break;
	case GDI_LOCKMOVE:
		nRet = Player[CLIENT_PLAYER_INDEX].GetLockMove()->bLock;
		break;
    case PT_SOCKET_VISUAL_QUERY:
        if(uParam&&nParam){KUiObjAtContRegion* obj=(KUiObjAtContRegion*)uParam;int id=obj->Obj.uId;if((obj->Obj.uGenre==CGOG_ITEM||obj->Obj.uGenre==CGOG_IME_ITEM||obj->Obj.uGenre==CGOG_PLAYERVIEWITEM||obj->Obj.uGenre==CGOG_PLAYERSELLITEM)&&IsValidClientItemIndex(id))return PtSocketVisuals(Item[id],*(PT_SOCKET_VISUALS*)nParam)?1:0;}return 0;
    case PHONGTHAN_COMPOSE_CORE_MODE:return PtSocketComposeMode();
    case PHONGTHAN_COMPOSE_CORE_PREVIEW:
        if(uParam && nParam==sizeof(PHONGTHAN_COMPOSE_PREVIEW)){PhongThanGetComposePreview(*(PHONGTHAN_COMPOSE_PREVIEW*)uParam);return 1;}
        break;
	case PT_GDI_ROLE_VALUES:
        if(uParam && nParam==sizeof(PTRoleValues) && Player[CLIENT_PLAYER_INDEX].m_nIndex>0) {
            PTRoleValues* values=(PTRoleValues*)uParam;
            memset(values,0,sizeof(*values));
            KPlayer& owner=Player[CLIENT_PLAYER_INDEX];
            extern int PhongThanXuCountForShop(int);
            int xu=PhongThanXuCountForShop(CLIENT_PLAYER_INDEX);
            if(xu>=8)values->UnlockCloth|=4;if(xu>=128)values->UnlockCloth|=8;
            values->RealmStage=owner.m_cTask.GetSaveVal(4871);
            values->RealmSide=owner.m_cTask.GetSaveVal(TASKVALUE_PT_REALM_SIDE);
            values->RealmLevel=owner.m_cTask.GetSaveVal(TASKVALUE_PT_REALM_LEVEL);
            values->EquipmentRealm=owner.m_ItemList.EquipmentRealm();
            values->TienReputation=owner.m_cTask.GetSaveVal(PtIslandTask::TienCredit);
            values->MaReputation=owner.m_cTask.GetSaveVal(PtIslandTask::MaCredit);
            if(PhongThanRealmUnlocked(values->RealmStage,values->RealmSide,values->RealmLevel)){
                const int profession=owner.m_cProfession.GetProfession();
                const PT_REALM_GROWTH* current=PhongThanRealmGrowth(profession,values->RealmLevel);
                const PT_REALM_GROWTH* next=PhongThanRealmGrowth(profession,values->RealmLevel+1);
                if(current){
                    int experience=owner.m_cTask.GetSaveVal(PT_REALM_EXP_TASK);
                    if(experience<current->experience)experience=current->experience;
                    values->RealmExperience=experience-current->experience;
                    values->RealmExperienceNext=next?next->experience-current->experience:0;
                    if(values->RealmExperienceNext>0&&values->RealmExperience>values->RealmExperienceNext)values->RealmExperience=values->RealmExperienceNext;
                    values->RealmGrowth[0]=current->strength;values->RealmGrowth[1]=current->dexterity;
                    values->RealmGrowth[2]=current->vitality;values->RealmGrowth[3]=current->energy;
                }
            }
            values->Credit=owner.m_cTask.GetSaveVal(TASKVALUE_PT_CREDIT);
            // Bang cong is the sum of equipped items, never quest exploit.
            for(int part=0;part<itempart_num;++part) {
                int index=owner.m_ItemList.GetEquipment(part),power=0;
                if(index<=0 || index>=MAX_ITEM)continue;
                if(Item[index].GetEquipmentPower(power)) {
                    if(power>MAX_INT-values->Exploit){values->Exploit=MAX_INT;break;}
                    values->Exploit+=power;
                }
            }
            values->WorldRank=owner.m_cTask.GetSaveVal(TASKVALUE_PT_UI_WORLD_RANK);
            for(int page=1;page<=3;++page)values->BagRows[page-1]=owner.m_ItemList.GetBagRows(page);
            // Original 0x7334e0: carried rooms + quick slots + hand; no bank/equipment.
            PlayerItem* item=owner.m_ItemList.GetFirstItem();
            while(item) {
                if((item->nPlace==pos_hand || item->nPlace==pos_equiproom ||
                    item->nPlace==pos_equiproomex || item->nPlace==pos_ptbag2 || item->nPlace==pos_ptbag3 || item->nPlace==pos_immediacy) &&
                    item->nIdx>0 && item->nIdx<MAX_ITEM) {
                    if((item->nPlace==pos_equiproom||item->nPlace==pos_equiproomex||item->nPlace==pos_ptbag2||item->nPlace==pos_ptbag3) && Item[item->nIdx].GetGenre()==item_ibitem &&
                       Item[item->nIdx].GetParticularType()==2 && !Item[item->nIdx].GetLock()->IsLock() && Item[item->nIdx].GetStackNum()>0) {
                        if(Item[item->nIdx].GetDetailType()==269)values->UnlockCloth|=1;
                        if(Item[item->nIdx].GetDetailType()==270)values->UnlockCloth|=2;
                    }
                    int weight=Item[item->nIdx].GetWeight();
                    int count=Item[item->nIdx].GetInventoryCount();if(count<1)count=1;
                    if(weight>0) {
                        if(weight>MAX_INT/count || values->CarryWeight>MAX_INT-weight*count)
                            values->CarryWeight=MAX_INT;
                        else values->CarryWeight+=weight*count;
                    }
                }
                item=owner.m_ItemList.GetNextItem();
            }
            values->CarryCapacity=PhongThanCarryCapacity(CLIENT_PLAYER_INDEX);
            if(values->CarryCapacity<0)values->CarryCapacity=0;
            values->BoundMoney=owner.m_cTask.GetSaveVal(TASKVALUE_PT_BOUND_MONEY);
            values->PillCount=owner.m_cTask.GetSaveVal(TASKVALUE_PT_PILLS_COUNT);
            values->PillNew=owner.m_cTask.GetSaveVal(TASKVALUE_PT_PILLS_NEW);
            values->PillBonus=owner.m_cTask.GetSaveVal(TASKVALUE_PT_PILLS_UNTIL)>(int)KSG_GetCurSec()?
                owner.m_cTask.GetSaveVal(TASKVALUE_PT_PILLS_BONUS):0;
            int today=((int)KSG_GetCurSec()+25200)/86400;
            values->PillAvailable=owner.m_cTask.GetSaveVal(TASKVALUE_PT_PILLS_DAY)==today?
                12-owner.m_cTask.GetSaveVal(TASKVALUE_PT_PILLS_MADE):12;
            nRet=1;
        }
        break;
    case PT_LIFE_SKILLS_GET:
        if(uParam){PT_LIFE_SKILLS_VIEW* view=(PT_LIFE_SKILLS_VIEW*)uParam;memset(view,0,sizeof(*view));
            KPlayer& owner=Player[CLIENT_PLAYER_INDEX];
            for(int i=1;i<=10;++i){view->Levels[i]=owner.m_cTask.GetSaveVal(PT_LIFE_LEVEL_BEGIN+i);view->Experience[i]=owner.m_cTask.GetSaveVal(PT_LIFE_EXP_BEGIN+i);}
            for(int recipe=1;recipe<=128;++recipe)view->FormulaUses[recipe]=owner.m_cTask.GetSaveVal(PT_LIFE_FORMULA_BEGIN+recipe);
            view->MiningBook=Npc[owner.m_nIndex].m_SkillList.GetLevel(62);view->BuildingBook=Npc[owner.m_nIndex].m_SkillList.GetLevel(128);
            if(view->MiningBook>view->Levels[3])view->Levels[3]=view->MiningBook;
            const int* limits=PT_LIFE_LIMITS[max(0,min(200,Npc[owner.m_nIndex].m_Level))];
            view->Energy=owner.m_cTask.GetSaveVal(PT_LIFE_ENERGY);view->Strength=owner.m_cTask.GetSaveVal(PT_LIFE_STRENGTH);view->MaxEnergy=limits[0];view->MaxStrength=limits[1];nRet=1;
        }break;
    case PT_GDI_BAG_FIRST:
    case PT_GDI_BAG_FIRST+1:
    case PT_GDI_BAG_FIRST+2:
    case PT_GDI_BAG_FIRST+3:
        {
            int page=(int)uDataId-PT_GDI_BAG_FIRST;
            int place=page==0?pos_equiproom:page==1?pos_equiproomex:page==2?pos_ptbag2:pos_ptbag3;
            KItemList& bag=Player[CLIENT_PLAYER_INDEX].m_ItemList;
            PlayerItem* entry=bag.GetFirstItem();int count=0;
            while(entry) {
                if(entry->nPlace==place && entry->nIdx>0 && entry->nIdx<MAX_ITEM) {
                    if(uParam && count<nParam) {
                        KUiObjAtRegion& target=((KUiObjAtRegion*)uParam)[count];
                        target.Obj.uGenre=CGOG_ITEM;target.Obj.uId=entry->nIdx;
                        target.Region.h=entry->nX;target.Region.v=entry->nY;
                        target.Region.Width=Item[entry->nIdx].GetWidth();target.Region.Height=Item[entry->nIdx].GetHeight();
                    }
                    ++count;
                }
                entry=bag.GetNextItem();
            }
            nRet=count;
        }
        break;
    case PT_GDI_STATUS_ORIGINAL:
        if(uParam && nParam==sizeof(PTStatusOriginal) && Player[CLIENT_PLAYER_INDEX].m_nIndex>0 && Player[CLIENT_PLAYER_INDEX].m_nIndex<MAX_NPC) {
            PhongThanFillStatusOriginal(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex],*(PTStatusOriginal*)uParam);
            nRet=1;
        }
        break;
    case PT_GDI_STATUS_DETAIL:
        if(uParam && nParam==sizeof(PTStatusDetail) && Player[CLIENT_PLAYER_INDEX].m_nIndex>0 && Player[CLIENT_PLAYER_INDEX].m_nIndex<MAX_NPC) {
            PTStatusDetail* detail=(PTStatusDetail*)uParam;
            memset(detail,0,sizeof(*detail));
            Player[CLIENT_PLAYER_INDEX].m_cProfession.GetName(detail->Profession);
            KNpc& npc=Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex];
            KMagicAttrib* attrs[4]={&npc.m_CurrentLightDamage,&npc.m_CurrentEarthDamage,&npc.m_CurrentColdDamage,&npc.m_CurrentFireDamage};
            for(int i=0;i<4;++i){detail->ElementMin[i]=attrs[i]->nValue[0];detail->ElementMax[i]=attrs[i]->nValue[2];}
            detail->CriticalPercent=npc.m_CurrentDeadlyStrikeEnhanceP;
            detail->AttackRating=npc.m_CurrentAttackRating;
            detail->Evasion=npc.m_CurrentDefend;
            // Phòng thủ is the effective extra-defense channel used by hit resolution;
            // physical armor is a separate elemental armor bucket.
            detail->Defence=npc.m_CurrentExDefend;
            detail->HitRecover=npc.m_CurrentHitRecover;
            detail->FreezeTimeReduce=npc.m_CurrentFreezeTimeReducePercent;
            detail->IgnoreDefense=npc.m_CurrentIgnoreDefensePercent;
            detail->LifeReplenish=npc.m_CurrentLifeReplenish;
            detail->ManaReplenish=npc.m_CurrentManaReplenish;
            detail->Focus[0]=npc.m_CurrentAllMagicDeadlyStrike;
            detail->Focus[1]=npc.m_CurrentFireDeadlyStrike;
            detail->Focus[2]=npc.m_CurrentColdDeadlyStrike;
            detail->Focus[3]=npc.m_CurrentLightDeadlyStrike;
            detail->Focus[4]=npc.m_CurrentEarthDeadlyStrike;
            detail->IgnoreResist[0]=npc.m_CurrentIgnoreFireResist;
            detail->IgnoreResist[1]=npc.m_CurrentIgnoreColdResist;
            detail->IgnoreResist[2]=npc.m_CurrentIgnoreLightResist;
            detail->IgnoreResist[3]=npc.m_CurrentIgnoreEarthResist;
            nRet=1;
        }
        break;
    case PT_PLAYER_TARGET_GET:
        if(uParam && nParam==sizeof(PT_PLAYER_TARGET)) {
            PT_PLAYER_TARGET* target=(PT_PLAYER_TARGET*)uParam;ZeroMemory(target,sizeof(*target));
            int i=s_PtSelectedPlayerIndex;
            if(i>0 && i<MAX_NPC && Npc[i].m_dwID==s_PtSelectedPlayerId && Npc[i].m_Kind==kind_player && Npc[i].m_RegionIndex>=0 && Npc[i].m_CurrentLife>0) {
                target->Id=Npc[i].m_dwID;target->Index=i;target->Life=(int)Npc[i].m_CurrentLife;target->MaxLife=(int)Npc[i].m_CurrentLifeMax;
                target->Level=Npc[i].m_Level;target->Head=Npc[i].m_HeadImage;target->Sex=Npc[i].m_nSex;g_StrCpyLen(target->Name,Npc[i].Name,sizeof(target->Name));
            }
            return 1;
        }break;
    case PT_COMPANION_HUD_GET:
        if(uParam&&nParam==sizeof(PT_COMPANION_HUD)){
            PT_COMPANION_HUD* hud=(PT_COMPANION_HUD*)uParam;memset(hud,0,sizeof(*hud));
            int actor=Player[CLIENT_PLAYER_INDEX].m_nIndex;
            if(actor>0&&actor<MAX_NPC)for(int pet=NpcSet.GetNextIdx(0);pet>0;pet=NpcSet.GetNextIdx(pet)){
                if(PhongThanClientSummonOwner(pet)!=actor||Npc[pet].m_RegionIndex<0||Npc[pet].m_CurrentLife<=0)continue;
                hud->Id=Npc[pet].m_dwID;hud->Level=PhongThanSummonShownLevel(Npc[pet].m_Level,PhongThanClientSummonRank(pet));hud->Locked=PhongThanClientSummonLocked(pet);hud->Life=(int)Npc[pet].m_CurrentLife;hud->MaxLife=(int)Npc[pet].m_CurrentLifeMax;
                unsigned int color=0;PhongThanParseNpcName(Npc[pet].Name,hud->Name,sizeof(hud->Name),color);
                int row=Npc[pet].m_NpcSettingIdx+2;g_NpcSetting.GetString(row,"HeadImage","",hud->Portrait,sizeof(hud->Portrait));
                break;
            }
            nRet=1;
        }break;
    case PT_GDI_STALL_STATUS:
        if(uParam&&nParam==sizeof(PTStallStatus)){
            PTStallStatus* state=(PTStallStatus*)uParam;memset(state,0,sizeof(*state));
            state->Sales=Player[CLIENT_PLAYER_INDEX].m_cTask.GetSaveVal(PT_STALL_SALES_TASK);
            state->Turnover=Player[CLIENT_PLAYER_INDEX].m_cTask.GetSaveVal(PT_STALL_TURNOVER_TASK);
            state->Level=PTStallLevel(state->Sales);state->NextSales=state->Level<10?state->Level*state->Level*10:0;nRet=1;
        }break;
    case PT_GDI_IBBUFFS:
        if(uParam && nParam==sizeof(PTIBBuffUiState)) {
            PTIBBuffUiState* state=(PTIBBuffUiState*)uParam;memset(state,0,sizeof(*state));
            DWORD now=KSG_GetCurSec();KPlayerTask& task=Player[CLIENT_PLAYER_INDEX].m_cTask;
            for(int i=0;i<PT_IBBUFF_UI_MAX;++i) {
                DWORD packed=(DWORD)task.GetSaveVal(TASKVALUE_PT_IBBUFF_BEGIN+i*2),end=(DWORD)task.GetSaveVal(TASKVALUE_PT_IBBUFF_BEGIN+i*2+1);
                int id=packed&0x3fff,times=(packed>>22)&0x3ff;
                if(id<=0 || (id>9999&&!PtLifeBuff(id)) || times<=0 || (end && end<=now))continue;
                PTIBBuffUiEntry& entry=state->Entries[state->Count++];
                entry.Id=id;entry.Level=(packed>>14)&0xff;entry.Stacks=times;entry.Seconds=end?end-now:0;
            }
            nRet=1;
        }
        break;
    case PT_GDI_CONSUMABLE_BUFFS:
        nRet = PhongThanConsumableUi((void*)uParam,nParam);
        break;
    case PT_GDI_STATUS_SKILLS:
        if (uParam && nParam == sizeof(PTStatusSkillsUi))
        {
            PTStatusSkillsUi* out = (PTStatusSkillsUi*)uParam;
            memset(out, 0, sizeof(*out));
            int npc = Player[CLIENT_PLAYER_INDEX].m_nIndex;
            if (npc <= 0 || npc >= MAX_NPC) break;
            KStateNode* node = (KStateNode*)Npc[npc].m_StateSkillList.GetHead();
            while (node && out->Count < PT_STATUS_UI_MAX)
            {
                if (node->m_SkillID > 0 && node->m_SkillID < MAX_SKILL &&
                    node->m_Level > 0 && node->m_Level < MAX_SKILLLEVEL && node->m_LeftTime != 0)
                {
                    KSkill* skill = (KSkill*)g_SkillManager.GetSkill(node->m_SkillID, node->m_Level);
                    if (skill && PTShowSkillBuff(node->m_LeftTime,skill->GetSkillStyle()==SKILL_SS_PassivityNpcState,node->m_SkillID,Npc[npc].m_ActiveAuraID))
                    {
                        PTStatusSkillUi& entry = out->Entries[out->Count++];
                        entry.SkillId = node->m_SkillID; entry.LeftTicks = node->m_LeftTime;
                        g_StrCpyLen(entry.Image, skill->m_szSkillIcon, sizeof(entry.Image));
                        char rankedName[160];sprintf(rankedName,"\045\163\040\055\040\143\312\160\040\045\144",skill->GetSkillName(),node->m_Level);
                        g_StrCpyLen(entry.Name, rankedName, sizeof(entry.Name));
                        skill->DescribeState(entry.Description,sizeof(entry.Description));
                    }
                }
                node = (KStateNode*)node->GetNext();
            }
            nRet = 1;
        }
        break;
    case GDI_NPC_STATE_SKILL:
		nRet = 0;
		if (uParam)
		{
			KStateNode* pNode;
			pNode = (KStateNode *)Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_StateSkillList.GetHead();
			KStateControl* pInfo = (KStateControl *)uParam;

			while(pNode)
			{
				if (pNode->m_SkillID > 0 && pNode->m_SkillID < MAX_SKILL && pNode->m_Level > 0 && pNode->m_Level < MAX_SKILLLEVEL)
				{
					KSkill * pOrdinSkill = (KSkill *)g_SkillManager.GetSkill(pNode->m_SkillID, pNode->m_Level);
					// A persisted state can outlive a removed or temporarily unavailable
					// Skills.txt row.  Never let one bad state bring down the whole client.
					if (pOrdinSkill && (pOrdinSkill->GetSkillStyle() == SKILL_SS_Missles ||
						pOrdinSkill->GetSkillStyle() == SKILL_SS_InitiativeNpcState ||
						pOrdinSkill->GetSkillStyle() == SKILL_SS_PhongThanAttack ||
						pOrdinSkill->GetSkillStyle() == SKILL_SS_PhongThanProduce ||
						pOrdinSkill->GetSkillStyle() == SKILL_SS_PhongThanAwaken))
					{
						pInfo->nSkillId = pNode->m_SkillID;
						pInfo->nLeftTime = pNode->m_LeftTime;
						pInfo++;
					}
				}
				pNode = (KStateNode *)pNode->GetNext();
			}
		}
		else
		{
			int nCount = 0;
			KStateNode* pNode;
			pNode = (KStateNode *)Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_StateSkillList.GetHead();
			while(pNode)
			{
				if (pNode->m_SkillID > 0 && pNode->m_SkillID < MAX_SKILL && pNode->m_Level > 0 && pNode->m_Level < MAX_SKILLLEVEL)
				{
					KSkill * pOrdinSkill = (KSkill *)g_SkillManager.GetSkill(pNode->m_SkillID, pNode->m_Level);
					if (pOrdinSkill && (pOrdinSkill->GetSkillStyle() == SKILL_SS_Missles ||
						pOrdinSkill->GetSkillStyle() == SKILL_SS_InitiativeNpcState ||
						pOrdinSkill->GetSkillStyle() == SKILL_SS_PhongThanAttack ||
						pOrdinSkill->GetSkillStyle() == SKILL_SS_PhongThanProduce ||
						pOrdinSkill->GetSkillStyle() == SKILL_SS_PhongThanAwaken))
					{
						nCount++;
					}
				}
				pNode = (KStateNode *)pNode->GetNext();
			}
			nRet = nCount;
		}
		break;
	case GDI_MISSION_SELFDATA:
		if (uParam)
		{
			TMissionLadderSelfInfo *pRankData = (TMissionLadderSelfInfo *)uParam;
			*pRankData = Player[CLIENT_PLAYER_INDEX].m_MissionData;
			g_DebugLog("GDI_MISSION_SELFDATA");
		}
		break;
	case GDI_MISSION_RANKDATA:
		if (uParam)
		{
			TMissionLadderInfo *pRankData = (TMissionLadderInfo *)uParam;
			*pRankData = Player[CLIENT_PLAYER_INDEX].m_MissionRank[nParam];
			g_DebugLog("GDI_MISSION_RANKDATA");
		}
		break;
	case GDI_CHATROOM_NAME:
		if (uParam)
		{
			char *pRoomName = (char *)uParam;
			strcpy(pRoomName, Player[CLIENT_PLAYER_INDEX].m_cRoom.m_szRoomName);
		}
		break;

	case GDI_IS_CHECK_IMAGE:			//Kiem tra hinh anh nguoi choi da chon
		nRet = Player[CLIENT_PLAYER_INDEX].m_ImagePlayer;
		break;
	//TamTLM Get Version game
	case GDI_GET_VERSION_GAME:
		switch (uParam)
		{
			case 0:
			{
				return Player[CLIENT_PLAYER_INDEX].GetVersionGame();
			}
			break;
			default:
				break;
		}
		break;
	//end code
        case GDI_SELL_ITEM:
        {
            if (uParam == 1)
            {
                nRet = Player[CLIENT_PLAYER_INDEX].m_cAI.m_bSellItem;
            }
            else if (uParam == 2)
            {
                nRet = Player[CLIENT_PLAYER_INDEX].m_cAI.m_bIsActive;
            }

        }
        break;
	}
	return nRet;
}

//--------------------------------------------------------------------------
//	���ܣ�����Ϸ���Ͳ���
//	������unsigned int uDataId --> Core�ⲿ�ͻ���core�Ĳ����������������
//							��ֵΪ÷������GAMEOPERATION_INDEX��ȡֵ֮һ��
//		  unsigned int uParam  --> ����uOperId��ȡֵ�������
//		  int nParam --> ����uOperId��ȡֵ�������
//	���أ�����ɹ����Ͳ������󣬺������ط�0ֵ�����򷵻�0ֵ��
//--------------------------------------------------------------------------
#ifndef _SERVER
static int s_PtVisibilityMode=3;
bool PhongThanClientVisualHidden(int index){
    if(index<=0||index>=MAX_NPC)return false;
    int self=Player[CLIENT_PLAYER_INDEX].m_nIndex;
    if(self<=0||self>=MAX_NPC)return false;
    KNpc& n=Npc[index];
    bool team=n.m_nTeamServerID>=0&&Npc[self].m_nTeamServerID>=0&&n.m_nTeamServerID==Npc[self].m_nTeamServerID;
    bool pet=n.GetNpcRes()->m_PetStyle>0||PhongThanIsClientSummon(index);
    return PhongThanHideVisualRule(s_PtVisibilityMode,index==self,n.m_Kind==kind_player,team,pet);
}
#endif

int	KCoreShell::OperationRequest(unsigned int uOper, unsigned int uParam, int nParam)
{
	int nRet = 1;
	switch(uOper)
	{
    case PT_EVENT_SHOP_PURCHASE:return PhongThanEventShopPurchase(uParam,(unsigned int)nParam);
    case PT_EVENT_SHOP_DISMISS:PhongThanEventShopDismiss();return 1;
    case PT_GOI_BANK_SECURITY:
        if(uParam && nParam==sizeof(PTBankSecurityRequest)){
            PTBankSecurityRequest q=*(const PTBankSecurityRequest*)uParam;int actor=Player[CLIENT_PLAYER_INDEX].m_nIndex;
            if(actor<=0||actor>=MAX_NPC||Npc[actor].m_SubWorldIndex<0||Npc[actor].m_SubWorldIndex>=MAX_SUBWORLD)return 0;
            PhongThanInitializeWireHeader(&q.Header,PT_BANK_SECURITY_REQUEST,sizeof(q),PHONGTHAN_WIRE_FLAG_REQUEST,0);
            q.MapId=SubWorld[Npc[actor].m_SubWorldIndex].m_SubWorldID;q.EntityId=Npc[actor].m_dwID;
            if(!PTValidateBankSecurity(&q,sizeof(q)))return 0;g_pClient->SendPackToServer(&q,sizeof(q));ZeroMemory(&q,sizeof(q));
        }break;
    case PT_GOI_F4:
        {
            int actor=Player[CLIENT_PLAYER_INDEX].m_nIndex;
            if(!g_pClient || actor<=0 || actor>=MAX_NPC || nParam<0 || nParam>0x703)return 0;
            int world=Npc[actor].m_SubWorldIndex;
            if(world<0 || world>=MAX_SUBWORLD)return 0;
            PTF4Command command;memset(&command,0,sizeof(command));
            PhongThanInitializeWireHeader(&command.Header,PT_F4_COMMAND,sizeof(command),PHONGTHAN_WIRE_FLAG_REQUEST,0);
            command.MapId=SubWorld[world].m_SubWorldID;command.EntityId=Npc[actor].m_dwID;
            command.Action=(BYTE)uParam;command.Page=(BYTE)(nParam&255);command.ExpectedRows=(BYTE)(nParam>>8);
            if(!PTValidateF4Command(&command,sizeof(command)))return 0;
            g_pClient->SendPackToServer(&command,sizeof(command));
            nRet=1;
        }
        break;
    case GOI_QUERY_RANK_INFORMATION:
		SendClientCmdQueryLadder(uParam);
		break;
	//uParam = (const char*)pszFileName
	case GOI_PLAY_SOUND:
		if (uParam)
		{
			static KCacheNode* pSndNode = NULL;
			KWavSound* pSound = NULL;
			pSndNode	= (KCacheNode*)g_SoundCache.GetNode((char *)uParam, (KCacheNode * )pSndNode);
			pSound		= (KWavSound*)pSndNode->m_lpData;
			if (pSound)
			{
				if (pSound->IsPlaying())
					break;
				pSound->Play(0, -10000 + Option.GetSndVolume() * 100, 0);
			}
		}
		break;
	case GOI_PLAYER_RENASCENCE:
        SendClientCmdRevive((int)uParam);
        break;
	case GOI_MONEY_INOUT_STORE_BOX:
		{
			BOOL	bIn = (BOOL)uParam;
			int		nMoney = nParam;
			int		nSrcRoom, nDesRoom;


			if (bIn)
			{
				nSrcRoom = room_equipment;
				nDesRoom = room_repository;
			}
			else
			{
				nDesRoom = room_equipment;
				nSrcRoom = room_repository;
			}
			Player[CLIENT_PLAYER_INDEX].m_ItemList.ExchangeMoney(nSrcRoom, nDesRoom, nMoney);
		}
		break;
		//�뿪��Ϸ
	case GOI_EXIT_GAME: //TamLTM co the Lam offline for uy thac.
		g_SubWorldSet.Close();
		g_ScenePlace.ClosePlace();
		//Player[CLIENT_PLAYER_INDEX].dacheck = FALSE;//TamLTM fix;
		break;
	case GOI_GAMESPACE_DISCONNECTED:
		g_SubWorldSet.Close(); //TamLTM fix;
		//Player[CLIENT_PLAYER_INDEX].dacheck = FALSE;//TamLTM fix;
		break;
	case GOI_TRADE_NPC_BUY:
		{
			KUiObjAtContRegion* pObject1 = (KUiObjAtContRegion*)uParam;
			if (!pObject1 || pObject1->Obj.uGenre != CGOG_NPCSELLITEM ||
				pObject1->Obj.uId == (unsigned int)-1)
				break;

			if (nParam <= 0 || nParam > 255 ||
				Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nCurShop < 0 ||
				Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nCurShop >=
					Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nShopNum ||
				Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nCurShop >= MAX_SUPERSHOP_SHOPTAB)
				break;

			// TamLTM add bang hoi chiem linh , Pos.nX, Pos.nY);
			SendClientCmdBuy(Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nCurShop, pObject1->Obj.uId, nParam); // TamLTM add bang hoi chiem linh , Pos.nX, Pos.nY);
		}
		break;
	case GOI_TRADE_NPC_SELL:
		{
			KUiObjAtContRegion* pObject1 = (KUiObjAtContRegion*)uParam;

			if (CGOG_ITEM != pObject1->Obj.uGenre)
				break;
			//����ȥ�Ķ�����Ϊ�գ�������������
			int nIdx = pObject1->Obj.uId;	//Player[CLIENT_PLAYER_INDEX].m_ItemList.Hand();
			if (nIdx > 0 && nIdx < MAX_ITEM)
			{
				SendClientCmdSell(Item[nIdx].GetID(), nParam);
				return 1;
			}
			else
			{
				return 0;
			}
		}
		break;
	case GOI_TRADE_NPC_REPAIR: // Sua trang bi do ben
		{
			KUiObjAtContRegion* pObject1 = (KUiObjAtContRegion*)uParam;

			if (CGOG_ITEM != pObject1->Obj.uGenre)
				break;
			//����ȥ�Ķ�����Ϊ�գ�������������
			int nIdx = pObject1->Obj.uId;	//Player[CLIENT_PLAYER_INDEX].m_ItemList.Hand();
			if (nIdx > 0 && nIdx < MAX_ITEM)
			{
				if (Item[nIdx].GetGenre() != item_equip)
					return 0;
				else if (Item[nIdx].GetDurability() == -1 || Item[nIdx].GetDurability() >= Item[nIdx].GetMaxDurability())
				{
					return 0;
				}
				else if (Item[nIdx].GetRepairPrice() <= Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipmentMoney())
				{
					SendClientCmdRepair(Item[nIdx].GetID());
				}
				else
				{
					KSystemMessage	sMsg;
					sprintf(sMsg.szMessage, MSG_SHOP_NO_MONEY);
					sMsg.eType = SMT_NORMAL;
					sMsg.byConfirmType = SMCT_NONE;
					sMsg.byPriority = 0;
					sMsg.byParamSize = 0;
					CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
					return 0;
				}

				return 1;
			}
			else
			{
				return 0;
			}
		}
		break;

	case GOI_SWITCH_OBJECT:
		//uParam = (KUiObjAtContRegion*)pObject1 -> �������Ʒ����ǰ����Ϣ
		//nParam = (KUiObjAtContRegion*)pObject2 -> ���µ���Ʒ���������Ϣ
		{
			ItemPos	P1, P2;
			P1.nPlace = P1.nX = -1;
			P2.nPlace = P2.nX = -1;
			P1.nY = P2.nY = 0;
			int PartConvert[itempart_num] =
			{
				itempart_head,		itempart_weapon,
				itempart_amulet,	itempart_cuff,
				itempart_body,		itempart_belt,
				itempart_ring1,		itempart_ring2,
				itempart_pendant,	itempart_foot,
				itempart_horse,		itempart_signet,
				itempart_shipin,
			};

			//TamLTM kham nam
			int PartBuildItem[MAX_PART_BUILD] =
			{
				itempart_build1,	itempart_build2,
				itempart_build3,	itempart_build4,
				itempart_build5,	itempart_build6,
				itempart_build7,	itempart_build8,
				itempart_build9,
			};
			//End code

			int PartCompoundConvert[MAX_COMPOUND_ITEM] =
			{
				compound_box1,	compound_box2,
				compound_box3,
			};
			KUiObjAtContRegion* pObject1 = (KUiObjAtContRegion*)uParam;
			KUiObjAtContRegion* pObject2 = (KUiObjAtContRegion*)nParam;

			if (!pObject1 && !pObject2)
				break;

			if (pObject1)
			{
				if (pObject1->eContainer != UOC_NPC_SHOP &&
					(pObject1->Obj.uGenre != CGOG_ITEM ||
					 !IsValidClientItemIndex(pObject1->Obj.uId)))
					return 0;
				switch(pObject1->eContainer)
				{
				case UOC_STORE_BOX:
					P1.nPlace = pos_repositoryroom;
					P1.nX = pObject1->Region.h;
					P1.nY = pObject1->Region.v;
					break;
				case UOC_STORE_BOX1:
					P1.nPlace = pos_repositoryroom1;
					P1.nX = pObject1->Region.h;
					P1.nY = pObject1->Region.v;
					break;
				case UOC_STORE_BOX2:
					P1.nPlace = pos_repositoryroom2;
					P1.nX = pObject1->Region.h;
					P1.nY = pObject1->Region.v;
					break;
				case UOC_STORE_BOX3:
					P1.nPlace = pos_repositoryroom3;
					P1.nX = pObject1->Region.h;
					P1.nY = pObject1->Region.v;
					break;
				case UOC_STORE_BOX4:
					P1.nPlace = pos_repositoryroom4;
					P1.nX = pObject1->Region.h;
					P1.nY = pObject1->Region.v;
					break;
				case UOC_STORE_BOX5:
					P1.nPlace = pos_repositoryroom5;
					P1.nX = pObject1->Region.h;
					P1.nY = pObject1->Region.v;
					break;
				case UOC_IMMEDIA_ITEM:
					P1.nPlace = pos_immediacy;
					P1.nX = pObject1->Region.h;
					P1.nY = pObject1->Region.v;
					break;
				case UOC_ITEM_TAKE_WITH:
					P1.nPlace = pos_equiproom;
					P1.nX = pObject1->Region.h;
					P1.nY = pObject1->Region.v;
					break;
				case UOC_PT_BAG2:
					P1.nPlace = pos_ptbag2;
					P1.nX = pObject1->Region.h;
					P1.nY = pObject1->Region.v;
					break;
				case UOC_PT_BAG3:
					P1.nPlace = pos_ptbag3;
					P1.nX = pObject1->Region.h;
					P1.nY = pObject1->Region.v;
					break;
				case UOC_ITEM_TAKE_WITH_EX:
					P1.nPlace = pos_equiproomex;
					P1.nX = pObject1->Region.h;
					P1.nY = pObject1->Region.v;
					break;
				case UOC_EQUIPTMENT:
					{
						// TODO:��ʱû�еڶ�װ��
						if (pObject1->Region.h == 1 ||
							pObject1->Region.v < 0 || pObject1->Region.v >= itempart_num)
							return 0;
						P1.nPlace = pos_equip;
						P1.nX = PartConvert[pObject1->Region.v];
					}
					break;
				case UOC_TO_BE_TRADE:
					P1.nPlace = pos_traderoom;
					P1.nX = pObject1->Region.h;
					P1.nY = pObject1->Region.v;
					break;
				case UOC_ITEM_GIVE:
					P1.nPlace = pos_give;
					P1.nX = pObject1->Region.h;
					P1.nY = pObject1->Region.v;
					break;
				case UOC_COMPOUND:
					{
						if (pObject1->Region.h == 1 || pObject1->Region.v < 0 ||
							pObject1->Region.v >= MAX_COMPOUND_ITEM)
							return 0;
						P1.nPlace = pos_compound;
						P1.nX = PartCompoundConvert[pObject1->Region.v];
					}
					break;
				//TamLTM kham nam xanh
				case UOC_BUILD_ITEM:
					{
						if (pObject1->Region.h == 1 || pObject1->Region.v < 0 ||
							pObject1->Region.v >= MAX_PART_BUILD)
							return 0;
						P1.nPlace = pos_builditem;
						P1.nX = PartBuildItem[pObject1->Region.v];
					}
					break;
				//End code
				case UOC_COMPOUND_BOX:
					P1.nPlace = pos_compoundroom;
					P1.nX = pObject1->Region.h;
					P1.nY = pObject1->Region.v;
					break;
				case UOC_NPC_SHOP:
					if (CGOG_NPCSELLITEM != pObject1->Obj.uGenre)
						break;

					// TamLTM add bang hoi chiem linh , Pos.nX, Pos.nY);
					int nIdx = 0;
					KItem* pItem = NULL;

					nIdx = BuySell.GetItemIndex(Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nBuyIdx, pObject1->Obj.uId);
					pItem = BuySell.GetItem(nIdx);
					if (nIdx < 0 || !pItem)
						return 0;

					int nWidth, nHeight;
					ItemPos	Pos;

					nWidth = pItem->GetWidth();
					nHeight = pItem->GetHeight();
					if (!Player[CLIENT_PLAYER_INDEX].m_ItemList.SearchPosition(nWidth, nHeight, &Pos))
					{
						nRet = 0;
						KSystemMessage	sMsg;

						strcpy(sMsg.szMessage, MSG_SHOP_NO_ROOM);
						sMsg.eType = SMT_SYSTEM;
						sMsg.byConfirmType = SMCT_CLICK;
						sMsg.byPriority = 1;
						sMsg.byParamSize = 0;
						CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
						break;
					}
					if (Pos.nPlace != pos_equiproom && Pos.nPlace != pos_equiproomex && Pos.nPlace != pos_ptbag2 && Pos.nPlace != pos_ptbag3)
					{
						nRet = 0;
						KSystemMessage	sMsg;

						strcpy(sMsg.szMessage, MSG_SHOP_NO_ROOM);
						sMsg.eType = SMT_SYSTEM;
						sMsg.byConfirmType = SMCT_CLICK;
						sMsg.byPriority = 1;
						sMsg.byParamSize = 0;
						CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
						break;
					}
					if (Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipmentMoney() < pItem->GetCurPrice())
					{
						nRet = 0;
						KSystemMessage	sMsg;

						strcpy(sMsg.szMessage, MSG_SHOP_NO_MONEY);
						sMsg.eType = SMT_SYSTEM;
						sMsg.byConfirmType = SMCT_CLICK;
						sMsg.byPriority = 1;
						sMsg.byParamSize = 0;
						CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
						break;
					}
					//end code

					SendClientCmdBuy(Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nCurShop, pObject1->Obj.uId, nParam); // TamLTM add bang hoi chiem linh , Pos.nX, Pos.nY);
					return 1;
				}
				if (P1.nPlace <= 0 || P1.nPlace >= pos_num)
					return 0;
			}

			if (pObject2)
			{
				if (pObject2->Obj.uGenre != CGOG_ITEM ||
					!IsValidClientItemIndex(pObject2->Obj.uId))
					return 0;
				switch(pObject2->eContainer)
				{
				case UOC_STORE_BOX:
					P2.nPlace = pos_repositoryroom;
					P2.nX = pObject2->Region.h;
					P2.nY = pObject2->Region.v;
					break;
				case UOC_STORE_BOX1:
					P2.nPlace = pos_repositoryroom1;
					P2.nX = pObject2->Region.h;
					P2.nY = pObject2->Region.v;
					break;
				case UOC_STORE_BOX2:
					P2.nPlace = pos_repositoryroom2;
					P2.nX = pObject2->Region.h;
					P2.nY = pObject2->Region.v;
					break;
				case UOC_STORE_BOX3:
					P2.nPlace = pos_repositoryroom3;
					P2.nX = pObject2->Region.h;
					P2.nY = pObject2->Region.v;
					break;
				case UOC_STORE_BOX4:
					P2.nPlace = pos_repositoryroom4;
					P2.nX = pObject2->Region.h;
					P2.nY = pObject2->Region.v;
					break;
				case UOC_STORE_BOX5:
					P2.nPlace = pos_repositoryroom5;
					P2.nX = pObject2->Region.h;
					P2.nY = pObject2->Region.v;
					break;
				case UOC_IMMEDIA_ITEM:
					P2.nPlace = pos_immediacy;
					P2.nX = pObject2->Region.h;
					P2.nY = pObject2->Region.v;
					break;
				case UOC_ITEM_TAKE_WITH:
					P2.nPlace = pos_equiproom;
					P2.nX = pObject2->Region.h;
					P2.nY = pObject2->Region.v;
					break;
				case UOC_PT_BAG2:
					P2.nPlace = pos_ptbag2;
					P2.nX = pObject2->Region.h;
					P2.nY = pObject2->Region.v;
					break;
				case UOC_PT_BAG3:
					P2.nPlace = pos_ptbag3;
					P2.nX = pObject2->Region.h;
					P2.nY = pObject2->Region.v;
					break;
				case UOC_ITEM_TAKE_WITH_EX:
					P2.nPlace = pos_equiproomex;
					P2.nX = pObject2->Region.h;
					P2.nY = pObject2->Region.v;
					break;
				case UOC_EQUIPTMENT:
					{
						// TODO:��ʱû�еڶ�װ��
						if (pObject2->Region.h == 1 ||
							pObject2->Region.v < 0 || pObject2->Region.v >= itempart_num)
							return 0;
						P2.nPlace = pos_equip;
						P2.nX = PartConvert[pObject2->Region.v];
					}
					break;
				case UOC_TO_BE_TRADE:
					P2.nPlace = pos_traderoom;
					P2.nX = pObject2->Region.h;
					P2.nY = pObject2->Region.v;
					break;
				case UOC_ITEM_GIVE:
					P2.nPlace = pos_give;
					P2.nX = pObject2->Region.h;
					P2.nY = pObject2->Region.v;
					break;
				case UOC_COMPOUND:
					{
						if (pObject2->Region.h == 1 || pObject2->Region.v < 0 ||
							pObject2->Region.v >= MAX_COMPOUND_ITEM)
							return 0;
						P2.nPlace = pos_compound;
						P2.nX = PartCompoundConvert[pObject2->Region.v];
					}
					break;
				//TamLTM code kham nam xanh
				case UOC_BUILD_ITEM:
					{
						// TODO:check doi tuong region chieu cao cua khung box
						if (pObject2->Region.h == 1 || pObject2->Region.v < 0 ||
							pObject2->Region.v >= MAX_PART_BUILD)
							return 0;
						P2.nPlace = pos_builditem;
						P2.nX = PartBuildItem[pObject2->Region.v];
					}
					break;
				//End code
				case UOC_COMPOUND_BOX:
					P2.nPlace = pos_compoundroom;
					P2.nX = pObject2->Region.h;
					P2.nY = pObject2->Region.v;
					break;
				case UOC_NPC_SHOP:
					return 0;
				}
				if (P2.nPlace <= 0 || P2.nPlace >= pos_num)
					return 0;
				if (Player[CLIENT_PLAYER_INDEX].GetLockMove()->bLock &&
					P2.nPlace == Item[pObject2->Obj.uId].GetBackLocal()->nPlace)
				{
					P2.nPlace = Item[pObject2->Obj.uId].GetBackLocal()->nPlace;
					P2.nX = Item[pObject2->Obj.uId].GetBackLocal()->nX;
					P2.nY = Item[pObject2->Obj.uId].GetBackLocal()->nY;
				}
			}
			if (!pObject1)
			{
				memcpy(&P1, &P2, sizeof(P1));
			}
			if (!pObject2)
			{
				memcpy(&P2, &P1, sizeof(P1));
			}
			Player[CLIENT_PLAYER_INDEX].MoveItem(P1, P2);
		}
		break;

	//��ҵ���Ի���
    case PT_PLAYER_TARGET_SELECT:
        if(!uParam){s_PtSelectedPlayerId=0;s_PtSelectedPlayerIndex=0;Player[CLIENT_PLAYER_INDEX].SetTargetNpc(0);return 1;}
        if(uParam<MAX_NPC && Npc[uParam].m_dwID==(unsigned int)nParam && Npc[uParam].m_Kind==kind_player && Npc[uParam].m_RegionIndex>=0 && Npc[uParam].m_CurrentLife>0) {
            s_PtSelectedPlayerIndex=uParam;s_PtSelectedPlayerId=Npc[uParam].m_dwID;Player[CLIENT_PLAYER_INDEX].SetTargetNpc(uParam);return 1;
        }return 0;
    case PHONGTHAN_SMITH_CORE_STATE:
        if(nParam==-2){s_PtSmithPending=0;s_PtSmithNonce=0;return 0;}
        if(nParam==-1){s_PtSmithNonce=0;return 0;}
        if(s_PtSmithPending&&(int)(GetTickCount()-s_PtSmithPendingUntil)>=0)return 3;
        if(s_PtSmithNonce&&(s_PtSmithMap!=SubWorld[0].m_SubWorldID||(int)(GetTickCount()-s_PtSmithUntil)>=0))return 2;
        return s_PtSmithNonce?1:0;
    case PHONGTHAN_SMITH_CORE_CLICK:
        {
            if(!s_PtSmithNonce||s_PtSmithMap!=SubWorld[0].m_SubWorldID||(int)(GetTickCount()-s_PtSmithUntil)>=0){s_PtSmithNonce=0;CoreDataChanged(PHONGTHAN_SMITH_CORE_RESULT,PT_SMITH_RESULT_UNAVAILABLE,0);return 0;}
            if(!uParam||uParam>=MAX_ITEM||!Item[uParam].GetID()){s_PtSmithNonce=0;CoreDataChanged(PHONGTHAN_SMITH_CORE_RESULT,PT_SMITH_RESULT_WRONG_ITEM,0);return 0;}
            PHONGTHAN_COMPOSE_COMMAND request;ZeroMemory(&request,sizeof(request));
            PhongThanInitializeWireHeader(&request.Header,PHONGTHAN_COMPOSE_REQUEST,sizeof(request),PHONGTHAN_WIRE_FLAG_REQUEST,s_PtSmithNonce);
            request.MapId=SubWorld[0].m_SubWorldID;request.Items[0]=Item[uParam].GetID();request.Quantities[0]=1;
            s_PtSmithPending=s_PtSmithNonce;s_PtSmithPendingUntil=GetTickCount()+10000;s_PtSmithNonce=0;
            if(g_pClient&&SUCCEEDED(g_pClient->SendPackToServer(&request,sizeof(request))))return 1;
            s_PtSmithPending=0;CoreDataChanged(PHONGTHAN_SMITH_CORE_RESULT,PT_SMITH_RESULT_SEND_FAILED,0);return 0;
        }
	case PHONGTHAN_COMPOSE_CORE_OP:
        {
            if(!uParam || nParam!=sizeof(PHONGTHAN_COMPOSE_COMMAND) || !g_pClient)return 0;
            PHONGTHAN_COMPOSE_COMMAND request=*(PHONGTHAN_COMPOSE_COMMAND*)uParam;
            PhongThanInitializeWireHeader(&request.Header,(PtSocketComposeMode()==8||PtSocketComposeMode()==9)?PT_SOUL_COMPOSE_REQUEST:PHONGTHAN_COMPOSE_REQUEST,sizeof(request),PHONGTHAN_WIRE_FLAG_REQUEST,(PtSocketComposeMode()==8||PtSocketComposeMode()==9)?PtSoulUiNonce():0);
            request.MapId=SubWorld[0].m_SubWorldID;
            for(int slot=0;slot<9;++slot){if(request.Items[slot]>=MAX_ITEM)return 0;if(request.Items[slot])request.Items[slot]=Item[request.Items[slot]].GetID();}
            return SUCCEEDED(g_pClient->SendPackToServer(&request,sizeof(request)));
        }
    case GOI_INFORMATION_CONFIRM_NOTIFY:
	{
		KPhongThanUiSelection command;
		command.Selection = nParam;
		command.View = SELECT_TALKUI;
		Player[CLIENT_PLAYER_INDEX].OnSelectFromUI(&command, UI_TALKDIALOG);
		break;
	}

	//����ѡ���
	//nParma = nAnswerIndex
	case GOI_QUESTION_CHOOSE:
		if (g_bUISelLastSelCount == 0 )
			break;
		{
			KPhongThanUiSelection command;
			command.Selection = nParam;
			command.View = SELECT_SELECTUI;
			Player[CLIENT_PLAYER_INDEX].OnSelectFromUI(&command, UI_SELECTDIALOG);
		}
		break;

	//TamLTM Toi uu hinh anh game
	case GOI_TOI_UU_IMAGE_COMMAND:
        {
            switch(uParam)
            {
				case 0:
				{
					if (nParam == false)
					{
						Player[CLIENT_PLAYER_INDEX].m_bIsHideNpc = TRUE;
					//	g_DebugLog("TRUE");
						return 1;
					}
					else
					{
						Player[CLIENT_PLAYER_INDEX].m_bIsHideNpc = FALSE;
						return 1;
					}
				}
				break;
				case 1:
				{
					if (nParam == false)
					{
						s_PtVisibilityMode=2;Player[CLIENT_PLAYER_INDEX].m_bIsHidePlayer = TRUE;
						return 1;
					}
					else
					{
						s_PtVisibilityMode=3;Player[CLIENT_PLAYER_INDEX].m_bIsHidePlayer = FALSE;
						return 1;
					}
				}
					break;
                case 3:
                    if(nParam<0||nParam>3)return 0;
                    s_PtVisibilityMode=nParam;Player[CLIENT_PLAYER_INDEX].m_bIsHidePlayer=(nParam==2);return 1;
				case 2:
				{
					//Break
				}
				break;
			default:
				break;
			}
		}
       break;
		//end

	//������������
	//uParam = (KUiGameObject*)pSKill, ������Ϣ
	//nParam = ����λ�ã�0��ʾΪ�Ҽ����ܣ�1��4��ʾΪF1��F4����
	//ʹ����Ʒ
	//uParam = (KUiObjAtRegion*)pInfo -> ��Ʒ�������Լ���Ʒԭ���ڷŵ�λ��
	//nParam = ��Ʒʹ��ǰ���õ�λ�ã�ȡֵΪö������UIOBJECT_CONTAINER��
	case GOI_USE_ITEM:
		//to do: waiting for...
		if (uParam)
		{
			KUiObjAtRegion* pInfo = (KUiObjAtRegion*) uParam;
			SO_ItemActionDiag("CoreShell_USE_ITEM", pInfo->Obj.uGenre, pInfo->Obj.uId, pInfo->Region.h, pInfo->Region.v, pInfo->Region.Width, pInfo->Region.Height, nParam, 0);
			int nPlace = nParam;
			ItemPos	Pos;
			switch(nPlace)
			{
			case UOC_ITEM_TAKE_WITH:
				Pos.nPlace = pos_equiproom;
				break;
			case UOC_PT_BAG2:
				Pos.nPlace = pos_ptbag2;
				break;
			case UOC_PT_BAG3:
				Pos.nPlace = pos_ptbag3;
				break;
			case UOC_ITEM_TAKE_WITH_EX:
				Pos.nPlace = pos_equiproomex;
				break;
			case UOC_IMMEDIA_ITEM:
				Pos.nPlace = pos_immediacy;
				break;
			default:
				Pos.nPlace = -1;
				break;
			}
			Pos.nX = pInfo->Region.h;
			Pos.nY = pInfo->Region.v;
			if ((pInfo->Obj.uGenre == CGOG_ITEM || pInfo->Obj.uGenre == CGOG_IME_ITEM) &&
				IsValidClientItemIndex(pInfo->Obj.uId) && Pos.nPlace != -1)
			{
				int nApplyRet = Player[CLIENT_PLAYER_INDEX].ApplyUseItem(pInfo->Obj.uId, Pos);
				SO_ItemActionDiag("CoreShell_USE_ITEM_RESULT", pInfo->Obj.uGenre, pInfo->Obj.uId, Pos.nX, Pos.nY, 0, 0, Pos.nPlace, nApplyRet);
			}
		}
		break;
	case GOI_SET_IMMDIA_SKILL:
		if (uParam)
		{
			KUiGameObject* pSkill = (KUiGameObject*)uParam;
			if (nParam == 0)
				//to do : modify;
			{
				if ( (int)pSkill->uId > 0 )
					Player[CLIENT_PLAYER_INDEX].SetLeftSkill((int)pSkill->uId);
				else if ((int)(pSkill->uId) == -1) //���õ�ǰ������Ϊ����
				{
					int nDetailType = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetWeaponType();
					int nParticularType = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetWeaponParticular();

					//��������
					if (nDetailType == 0)
					{
						Player[CLIENT_PLAYER_INDEX].SetLeftSkill(g_nMeleeWeaponSkill[nParticularType]);
					}//Զ������
					else if (nDetailType == 1)
					{
						Player[CLIENT_PLAYER_INDEX].SetLeftSkill(g_nRangeWeaponSkill[nParticularType]);
					}//����
					else if (nDetailType == -1)
					{
						Player[CLIENT_PLAYER_INDEX].SetLeftSkill(g_nHandSkill);
					}

				}
			}
			else if (nParam == 1)
			{
				if ((int)pSkill->uId > 0)
					Player[CLIENT_PLAYER_INDEX].SetRightSkill((int)pSkill->uId);
				else if ((int)(pSkill->uId) == -1)
				{
					int nDetailType = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetWeaponType();
					int nParticularType = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetWeaponParticular();

					if (nDetailType == 0)
					{
						Player[CLIENT_PLAYER_INDEX].SetRightSkill(g_nMeleeWeaponSkill[nParticularType]);
					}
					else if (nDetailType == 1)
					{
						Player[CLIENT_PLAYER_INDEX].SetRightSkill(g_nRangeWeaponSkill[nParticularType]);
					}
					else if (nDetailType == -1)
					{
						Player[CLIENT_PLAYER_INDEX].SetRightSkill(g_nHandSkill);
					}
				}
			}
		}
		break;

	//��ǿһ�ּ��ܣ���һ�μ�һ��
	//uParam = ��������
	//nParam = (uint)����id
	case GOI_TONE_UP_SKILL:
		Player[CLIENT_PLAYER_INDEX].ApplyAddSkillLevel((int)nParam, 1);
		break;

	//��ǿһЩ���Ե�ֵ��һ�μ�һ��
	//uParam = ��ʾҪ��ǿ�����ĸ����ԣ�ȡֵΪUI_PLAYER_ATTRIBUTE��÷��ֵ֮һ
	case GOI_TONE_UP_ATTRIBUTE:
			if (nParam)
			Player[CLIENT_PLAYER_INDEX].ApplyAddBaseAttribute(uParam, nParam);
		break;

	//��Ӧ/�ܾ���������
	//uParam = (KUiPlayerItem*)pRequestPlayer ������������
	//nParam = (int)(bool)bAccept �Ƿ��������
	case GOI_TRADE_INVITE_RESPONSE:
		if (uParam)
		{
			KTrade::ReplyInvite(((KUiPlayerItem*)uParam)->nIndex, nParam);
		}
		break;
	case GOI_TRADE_FOLKGAME_INVITE_RESPONSE:
		if (uParam)
		{
			KTrade::ReplyInvite(((KUiPlayerItem*)uParam)->nIndex, nParam, true);
		}
		break;
	//����һ������������Ʒ
	//uParam = (KUiObjAtRegion*) pObject -> ��Ʒ��Ϣ������������ϢΪ�ڽ��׽����е�����
	//nParam = bAdd -> 0ֵ��ʾ���٣�1ֵ��ʾ����
	//Remark : �����Ʒ�ǽ�Ǯ�Ļ�����KUiObjAtRegion::Obj::uId��ʾ�ѽ�Ǯ�����Ϊ���ֵ����nParam�����塣
	case GOI_TRADE_DESIRE_ITEM:
		if (uParam)
		{
			KUiObjAtRegion* pInfo = (KUiObjAtRegion*) uParam;
			if (pInfo->Obj.uGenre != CGOG_MONEY)
				break;
			Player[CLIENT_PLAYER_INDEX].TradeMoveMoney(pInfo->Obj.uId);
		}
		break;

	//���޽�������
	//nParam = bWilling
	case GOI_TRADE_WILLING:
		if (!nParam || Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].GetMenuState() == PLAYER_MENU_STATE_TRADEOPEN)
			Player[CLIENT_PLAYER_INDEX].TradeApplyClose();
		else
			Player[CLIENT_PLAYER_INDEX].TradeApplyOpen((char*)uParam, nParam);
		break;

	//��������
	//nParam = (int)(book)bLock �Ƿ�����
	case GOI_TRADE_LOCK:
		if ( !Player[CLIENT_PLAYER_INDEX].CheckTrading() )
			break;
			Player[CLIENT_PLAYER_INDEX].TradeApplyLock();
		break;

	//����
	case GOI_TRADE:
		if ( !Player[CLIENT_PLAYER_INDEX].CheckTrading() )
			break;
		if (Player[CLIENT_PLAYER_INDEX].m_cTrade.m_nTradeLock != 1 || Player[CLIENT_PLAYER_INDEX].m_cTrade.m_nTradeDestLock != 1)
			break;
		if (Player[CLIENT_PLAYER_INDEX].m_cTrade.m_nTradeState == 0)
			Player[CLIENT_PLAYER_INDEX].TradeDecision(1, uParam);		// ����ȷ��

		break;
	case GOI_TRADE_FOLKGAMEID:
		if ( !Player[CLIENT_PLAYER_INDEX].CheckTrading() )
			break;
		if (Player[CLIENT_PLAYER_INDEX].m_cTrade.m_nTradeLock != 1 || Player[CLIENT_PLAYER_INDEX].m_cTrade.m_nTradeDestLock != 1)
			break;
        if (Player[CLIENT_PLAYER_INDEX].m_cTrade.m_nTradeState ||
            Player[CLIENT_PLAYER_INDEX].m_cTrade.m_bTradeFolkGame)
			Player[CLIENT_PLAYER_INDEX].TradeDecision(2, uParam);
		break;
	//����ȡ��
	case GOI_TRADE_CANCEL:
		if ( !Player[CLIENT_PLAYER_INDEX].CheckTrading() )
			break;
		Player[CLIENT_PLAYER_INDEX].TradeDecision(0);		// ����ȡ��
		break;
	case GOI_SPAR_INVITE_RESPONSE:
		if (uParam)
			Player[CLIENT_PLAYER_INDEX].m_cPK.ReplyInvite(((KUiPlayerItem*)uParam)->uId, nParam);
		break;
	//������������
	//uParam = (KUiPlayerItem*)pFriend
	//			KUiPlayerItem::nData = 0
	case GOI_CHAT_FRIEND_ADD:
		{
			if (g_pClient)
			{
				size_t pckgsize = sizeof(tagExtendProtoHeader) + sizeof(ASK_ADDFRIEND_CMD);

				tagExtendProtoHeader* pExHeader = (tagExtendProtoHeader*)_alloca(pckgsize);
				pExHeader->ProtocolType = c2s_extendfriend;
				pExHeader->wLength = pckgsize - 1;

				ASK_ADDFRIEND_CMD* pAafCmd = (ASK_ADDFRIEND_CMD*)(pExHeader + 1);
				pAafCmd->ProtocolFamily = pf_friend;
				pAafCmd->ProtocolID = friend_c2c_askaddfriend;
				pAafCmd->pckgid = -1;
				strncpy(pAafCmd->dstrole, ((KUiPlayerItem*)uParam)->Name, _NAME_LEN);
				g_pClient->SendPackToServer(pExHeader, pckgsize);

				KSystemMessage	sMsg;
				sprintf(sMsg.szMessage, MSG_CHAT_APPLY_ADD_FRIEND, ((KUiPlayerItem*)uParam)->Name);
				sMsg.eType = SMT_NORMAL;
				sMsg.byConfirmType = SMCT_NONE;
				sMsg.byPriority = 0;
				sMsg.byParamSize = 0;
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
			}
		}
		break;
	//�Ա���Ҫ���Լ�Ϊ���ѵĻظ�
	//uParam = (KUiPlayerItem*)pRequestPlayer ������������
	//nParam = (int)(bool)bAccept �Ƿ��������
	case GOI_CHAT_FRIEND_INVITE:
		if (uParam)
		{
			if (g_pClient)
			{
				size_t pckgsize = sizeof(tagExtendProtoHeader) + sizeof(REP_ADDFRIEND_CMD);

				tagExtendProtoHeader* pExHeader = (tagExtendProtoHeader*)_alloca(pckgsize);
				pExHeader->ProtocolType = c2s_extendfriend;
				pExHeader->wLength = pckgsize - 1;

				REP_ADDFRIEND_CMD* pRafCmd = (REP_ADDFRIEND_CMD*)(pExHeader + 1);
				pRafCmd->ProtocolFamily = pf_friend;
				pRafCmd->ProtocolID = friend_c2c_repaddfriend;
				pRafCmd->pckgid = -1;
				strncpy(pRafCmd->dstrole, ((KUiPlayerItem*)uParam)->Name, _NAME_LEN);
				pRafCmd->answer = nParam ? answerAgree : answerDisagree;
				g_pClient->SendPackToServer(pExHeader, pckgsize);
			}
		}
		break;

	case GOI_OPTION_SETTING:			//ѡ������
		if (uParam == OPTION_DYNALIGHT)
		{
			g_ScenePlace.EnableDynamicLights(nParam != 0);
			if (g_pRepresent)
				g_pRepresent->SetOption(DYNAMICLIGHT, nParam != 0);
		}
		else if (uParam == OPTION_PERSPECTIVE)
		{
			if (g_pRepresent)
				g_pRepresent->SetOption(PERSPECTIVE, nParam != 0);
		}
		else if (uParam == OPTION_MUSIC_VALUE)
			Option.SetMusicVolume(nParam);
		else if (uParam == OPTION_SOUND_VALUE)
			Option.SetSndVolume(nParam);
		else if (uParam == OPTION_BRIGHTNESS)
			Option.SetGamma(nParam);
        else if (uParam == OPTION_ANIMATION_QUALITY)
            g_PhongThanAnimationQuality = nParam < 0 ? 0 : (nParam > 3 ? 3 : nParam);
        else if (uParam == OPTION_MAX_VISIBLE_PLAYERS)
            g_PhongThanMaxVisiblePlayers = nParam < 0 ? 0 : (nParam > 255 ? 255 : nParam);
		else if (uParam == OPTION_WEATHER)
			g_ScenePlace.EnableWeather(nParam);
		break;
	case GOI_VIEW_PLAYERITEM:
		g_cViewItem.ApplyViewEquip(uParam);
		break;
	case GOI_VIEW_PLAYERITEM_END:
		g_cViewItem.DeleteAll();
		break;
	case GOI_PLAYER_ACTION:
		{
			if (Player[CLIENT_PLAYER_INDEX].CheckTrading())
				return 0;

			switch(uParam)
			{
			case PA_RUN:
				Player[CLIENT_PLAYER_INDEX].m_RunStatus = !Player[CLIENT_PLAYER_INDEX].m_RunStatus;
				break;
			case PA_SIT:
				if (!Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_bRideHorse)
				{
					if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Doing != do_sit)
					{
						Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SendCommand(do_sit);
						SendClientCmdSit(true);
					}
					else
					{
						Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SendCommand(do_stand);
						SendClientCmdSit(false);
					}
				}
				else
				{
					KSystemMessage	sMsg;
					strcpy(sMsg.szMessage, MSG_CANT_SIT_RIDE);
					sMsg.eType = SMT_NORMAL;
					sMsg.byConfirmType = SMCT_NONE;
					sMsg.byPriority = 0;
					sMsg.byParamSize = 0;
					CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				}
				break;
			case PA_RIDE:
				SendClientCmdRide();
				break;
			}
		}
		break;
	case GOI_PK_SETTING:		//����PK
		{
			Player[CLIENT_PLAYER_INDEX].m_cPK.ApplySetNormalPKState(nParam);
		}
		break;
	//��ɱĳ��
	//uParam = (KUiPlayerItem*) pTarget	��ɱĿ��
	case GOI_REVENGE_SOMEONE:
		if (uParam)
		{
			KUiPlayerItem	*pTarget = (KUiPlayerItem*)uParam;
			Player[CLIENT_PLAYER_INDEX].m_cPK.ApplyEnmityPK(pTarget->uId);
		}
		break;
	case GOI_SPAR_SOMEONE:
		if (uParam)
		{
			KUiPlayerItem	*pTarget = (KUiPlayerItem*)uParam;
			Player[CLIENT_PLAYER_INDEX].m_cPK.SparApplyStart(pTarget->uId);
		}
		break;
	//����ĳ��
	//uParam = (KUiPlayerItem*) pTarget	����Ŀ��
	case GOI_FOLLOW_SOMEONE:
		if (uParam)
		{
			KUiPlayerItem	*pTarget = (KUiPlayerItem*)uParam;
			if (Npc[pTarget->nIndex].m_Kind == kind_player &&
				!Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_PTrade.nTrade)
				Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nPeopleIdx = pTarget->nIndex;
		}
		break;
	case GOI_VIEW_PLAYERSHOP:
		if (uParam)
		{
			if (Player[CLIENT_PLAYER_INDEX].CheckTrading())
				return 0;

			KUiPlayerItem	*pTarget = (KUiPlayerItem*)uParam;
			if (Npc[pTarget->nIndex].m_Kind == kind_player)
				g_cSellItem.Update(TRUE, Npc[pTarget->nIndex].m_dwID);
		}
		break;
	//��ʾ���������
	//nParam = (int)(bool)bShow	�Ƿ���ʾ
	case GOI_SHOW_PLAYERS_NAME:
		NpcSet.SetShowNameFlag(nParam);
		break;
	//��ʾ���������
	//nParam = (int)(bool)bShow	�Ƿ���ʾ
	case GOI_SHOW_PLAYERS_LIFE:
		NpcSet.SetShowLifeFlag(nParam);
		break;
	case 0x505453: // Roulette completion: only acknowledge this panel's token.
		{
			KPlayer& player = Player[CLIENT_PLAYER_INDEX];
			if (!g_pClient || player.m_UiDialogView != PHONGTHAN_VIEW_ROULETTE || !uParam || player.m_UiDialogToken != uParam) return 0;
			PHONGTHAN_UI_NUMBER_REQUEST request; memset(&request, 0, sizeof(request));
			PhongThanInitializeWireHeader(&request.Header, PHONGTHAN_MSG_UI_NUMBER_INPUT, sizeof(request), PHONGTHAN_WIRE_FLAG_REQUEST, 0);
			request.MapId = SubWorld[0].m_SubWorldID; request.DialogToken = uParam; request.Value = 1;
			const HRESULT sent = g_pClient->SendPackToServer(&request, sizeof(request));
			if (FAILED(sent)) return 0;
			player.m_UiDialogToken = 0; player.m_UiDialogView = 0;
			return 1;
		}
	case GOI_INPUT:
		if(uParam)
		{
			KPlayer& player = Player[CLIENT_PLAYER_INDEX];
			if (player.m_UiDialogView == PHONGTHAN_VIEW_NUMBER_INPUT) {
				if (!g_pClient || !player.m_UiDialogToken) break;
				const char* text = (const char*)uParam;
				int value = -1;
				if (*text) {
					value = 0;
					for (int i = 0; text[i]; ++i) {
						if (i >= 6 || text[i] < '0' || text[i] > '9') return 0;
						value = value * 10 + text[i] - '0';
					}
				}
				PHONGTHAN_UI_NUMBER_REQUEST request;
				memset(&request, 0, sizeof(request));
				PhongThanInitializeWireHeader(&request.Header, PHONGTHAN_MSG_UI_NUMBER_INPUT,
					sizeof(request), PHONGTHAN_WIRE_FLAG_REQUEST, 0);
				request.MapId = SubWorld[0].m_SubWorldID; request.DialogToken = player.m_UiDialogToken; request.Value = value;
				if (PhongThanValidateNumberRequest(&request, sizeof(request))) g_pClient->SendPackToServer(&request, sizeof(request));
				player.m_UiDialogToken = 0;
				player.m_UiDialogView = 0;
				break;
			}
			C2S_BUF_COMMAND sIP;
			sIP.ProtocolType = c2s_inputcommand;
			strcpy(sIP.szBuf, (char*)uParam);
			g_pClient->SendPackToServer((BYTE*)&sIP, sizeof(C2S_BUF_COMMAND));
		}
		break;
	case GOI_LOCKSTATE:
		{
			PLAYER_COMMAND	sLS;
			memset(&sLS,0,sizeof(sLS));
			sLS.ProtocolType = c2s_playercommand;
			sLS.m_wMsgID = enumC2S_PLAYERCOMMAND_ID_LOCKSTATE;
			sLS.m_lpBuf = 0;
			sLS.m_wLength = sizeof(PLAYER_COMMAND) - 1;
			g_pClient->SendPackToServer((BYTE*)&sLS,sizeof(sLS));
		}
		break;
	case GOI_GIVE:
		{
			PLAYER_COMMAND	sGv;
			sGv.ProtocolType = c2s_playercommand;
			sGv.m_wMsgID = enumC2S_PLAYERCOMMAND_ID_GIVE;
			sGv.m_lpBuf = (void *)uParam;
			sGv.m_wLength = sizeof(SHOW_MSG_SYNC) - 1;
			g_pClient->SendPackToServer((BYTE*)&sGv, sizeof(PLAYER_COMMAND));
		}
		break;
	case GOI_LOCKITEM: //Khoa dinh' item
		if (uParam)
		{
			KUiObjAtContRegion *pObj = (KUiObjAtContRegion *)uParam;
			if (pObj->Obj.uId > 0 && pObj->Obj.uId < MAX_ITEM)
			{
				KItem*	pItem = &Item[pObj->Obj.uId];
				if (pItem)
				{
					int nIdx = pObj->Obj.uId;

					if (pItem -> GetGenre() != item_equip)
						return 0;

					if (Item[nIdx].GetLock()->nState == LOCK_STATE_LOCK)
						return 0;

					PLAYER_LOCK_ITEM	sLI;
					sLI.ProtocolType = c2s_playerlockitem;
					sLI.m_wMsgID = enumC2S_PLAYERCOMMAND_ID_LOCKITEM;

					if (pItem->GetID() != 0)
					{
						sLI.m_lpBufLockItem = (void *)MAKELONG(pItem->GetID(), nParam); // Bi then nay
					}

				//	g_DebugLog("pItem->GetID(%d) - %d", (void *)pItem->GetID(), nParam);
					sLI.m_wLength = sizeof(SHOW_MSG_SYNC) - 1;
					g_pClient->SendPackToServer((BYTE*)&sLI, sLI.m_wLength + 1);
				}
			}
			else
			{
				return 0;
			}
		}
		break;
	//UnLock
	case GOI_UNLOCKITEM: //Mo Khoa dinh' item
		if (uParam)
		{
			KUiObjAtContRegion *pObj = (KUiObjAtContRegion *)uParam;
			if (pObj->Obj.uId > 0 && pObj->Obj.uId < MAX_ITEM)
			{
				KItem*	pItem = &Item[pObj->Obj.uId];
				if (pItem)
				{
					int nIdx = pObj->Obj.uId;

					if (pItem -> GetGenre() != item_equip)
						return 0;

					if (Item[nIdx].GetLock()->nState == LOCK_STATE_UNLOCK)
						return 0;

					if (Item[nIdx].GetLock()->nState == LOCK_STATE_NORMAL)
						return 0;

					PLAYER_UNLOCK_ITEM	sLI;
					sLI.ProtocolType = c2s_playerunlockitem;
					sLI.m_wMsgID = enumC2S_PLAYERCOMMAND_ID_UNLOCKITEM;

					if (pItem->GetID() != 0)
					{
						sLI.m_lpBufUnLockItem = (void *)MAKELONG(pItem->GetID(), nParam); //TamLTM Bi then nay
					}

					sLI.m_wLength = sizeof(SHOW_MSG_SYNC) - 1;
					g_pClient->SendPackToServer((BYTE*)&sLI, sLI.m_wLength + 1);
				}
			}
			else
			{
				return 0;
			}
		}
		break;
	//end
	case GOI_SUPERSHOP:
		{
			PHONGTHAN_WIRE_HEADER sSS;
			PhongThanInitializeWireHeader(&sSS, PHONGTHAN_MSG_IBSHOP_OPEN_REQUEST,
				sizeof(sSS), PHONGTHAN_WIRE_FLAG_REQUEST, 0);
			if (g_pClient)
				g_pClient->SendPackToServer((BYTE*)&sSS, sizeof(sSS));
		}
		break;
	case GOI_SHOW_OBJ_NAME:
		NpcSet.SetShowObjFlag(nParam);
		break;
	case GOI_PICKUP_OBJ:
		Player[CLIENT_PLAYER_INDEX].PickObjectNear();
		break;
	case GOI_OPTIMAL:
		Option.SetLow((int)uParam, (BOOL)nParam);
		break;
	//TamLTM uy thac offline
	case GOI_OFFLINE:
		{
			nRet=SendClientOffline((int)uParam,nParam); // Native exit/offline confirmation

		/*	PLAYER_COMMAND	sOffL;
			sOffL.ProtocolType = c2s_playercommand;
			sOffL.m_wMsgID = enumC2S_PLAYERCOMMAND_ID_OFFLINE;
			sOffL.m_lpBuf = 0;
			sOffL.m_wLength = sizeof(SHOW_MSG_SYNC) - 1;
			g_pClient->SendPackToServer((BYTE*)&sOffL, sOffL.m_wLength + 1);*/
		}
		break;
	case GOI_MASKFEATURE:
		{
			PLAYER_COMMAND	sMF;
			sMF.ProtocolType = c2s_playercommand;
			sMF.m_wMsgID = enumC2S_PLAYERCOMMAND_ID_MASKFEATURE;
			sMF.m_wLength = sizeof(SHOW_MSG_SYNC) - 1;
			sMF.m_lpBuf = 0;
			g_pClient->SendPackToServer((BYTE*)&sMF, sMF.m_wLength + 1);
		}
		break;
	case GOI_PLAYER_SETPRICE:
		if(uParam)
		{
			KUiObjAtContRegion* pObject1 = (KUiObjAtContRegion*)uParam;
			PLAYER_TRADE_SET_COMMAND sTrade;
			sTrade.ProtocolType = c2s_playertradeset;
			sTrade.m_dwID = Item[pObject1->Obj.uId].GetID();
			sTrade.m_nPrice = nParam;
			if (g_pClient)
				g_pClient->SendPackToServer((BYTE*)&sTrade, sizeof(PLAYER_TRADE_SET_COMMAND));
		}
	break;
    case PT_GOI_NATION:
        if(uParam && g_pClient){
            const PTNationRequest* request=(const PTNationRequest*)uParam;
            if(!PTNationRequestValid(request,sizeof(*request)))return 0;
            char packet[sizeof(tagExtendProtoHeader)+sizeof(*request)];
            tagExtendProtoHeader* header=(tagExtendProtoHeader*)packet;memset(header,0,sizeof(*header));
            header->ProtocolType=c2s_extend;header->wLength=sizeof(packet)-1;
            memcpy(packet+sizeof(*header),request,sizeof(*request));g_pClient->SendPackToServer(packet,sizeof(packet));return 1;
        }return 0;
    case PT_GOI_COMMERCE_ROUTE_INFO:
        if(uParam){PTCommerceRouteInfo& info=*(PTCommerceRouteInfo*)uParam;ZeroMemory(&info,sizeof(info));
            KPlayer& player=Player[CLIENT_PLAYER_INDEX];info.Origin=player.m_cTask.GetSaveVal(333);info.Good=player.m_cTask.GetSaveVal(334);info.Quantity=player.m_cTask.GetSaveVal(335);
            if(info.Origin<1||info.Origin>9||info.Good<1||info.Good>27||info.Quantity<1)return 0;
            for(int slot=110;slot<=118;slot+=2)if(player.m_cTask.GetSaveVal(slot)==info.Good)info.Owned+=player.m_cTask.GetSaveVal(slot+1);
            info.Shop=(nParam||info.Owned>=info.Quantity)?info.Origin:PTCommerceGoodShop(info.Good);
            const PTCommerceDestination& dest=PT_COMMERCE_DESTINATIONS[info.Shop-1];info.Map=dest.Map;info.X=dest.X;info.Y=dest.Y;return 1;
        }return 0;
    case PT_GOI_CROSS_ROUTE_STATUS:
        if(uParam){PTCrossRoutePosition& pos=*(PTCrossRoutePosition*)uParam;ZeroMemory(&pos,sizeof(pos));int n=Player[CLIENT_PLAYER_INDEX].m_nIndex;
            if(n<=0||n>=MAX_NPC||Npc[n].m_SubWorldIndex<0||Npc[n].m_SubWorldIndex>=MAX_SUBWORLD)return 0;
            pos.MapId=SubWorld[Npc[n].m_SubWorldIndex].m_SubWorldID;Npc[n].GetMpsPos(&pos.X,&pos.Y);
            pos.Moving=Player[CLIENT_PLAYER_INDEX].m_cAI.m_bAutoRunMap||Player[CLIENT_PLAYER_INDEX].m_cAI.m_bAutoRunFlagMap;return 1;
        }return 0;
    case PT_GOI_CROSS_ROUTE_STEP:
        if(uParam){
            const PTCrossRouteStep& step=*(const PTCrossRouteStep*)uParam;
            int actor=Player[CLIENT_PLAYER_INDEX].m_nIndex;
            if(step.X<=0||step.Y<=0||step.X>1048575||step.Y>1048575)return 0;
            if(actor<=0||actor>=MAX_NPC)return -1; // actor can be absent while entering the next map
            int world=Npc[actor].m_SubWorldIndex;
            if(world<0||world>=MAX_SUBWORLD||SubWorld[world].m_SubWorldID!=step.MapId)return -1;
            if(Npc[actor].m_CurrentLife<=0)return 0;
            Player[CLIENT_PLAYER_INDEX].m_cAI.m_bIsActive=FALSE;
            Player[CLIENT_PLAYER_INDEX].m_cAI.m_bFollowPeople=FALSE;
            SetAutoRun(FALSE);SetFlagAutoRun(TRUE,step.X,step.Y);
            {extern void PhongThanSetRouteArrival(int);PhongThanSetRouteArrival(step.Portal?8:0);}
            g_ScenePlace.SetRouteTargetMps(step.X,step.Y);
            Npc[actor].m_nMoveToFlagMiniMapX=step.X;Npc[actor].m_nMoveToFlagMiniMapY=step.Y;
            return 1;
        }
        return 0;
	case GOI_PLAYER_DOTRADE:
		{
            // Named stall requests open; empty requests stop. The ordinary two-player
            // trade-close opcode belongs to a different session.
            PLAYER_TRADE_START_COMMAND sTrade;
			sTrade.ProtocolType = c2s_playertradestart;
            if(!uParam)return 0;
            g_StrCpyLen(sTrade.Name,(const char*)uParam,sizeof(sTrade.Name));
			if (g_pClient)
				g_pClient->SendPackToServer((BYTE*)&sTrade, sizeof(PLAYER_TRADE_START_COMMAND));
		}
		break;
	case GOI_PLAYER_BUYTRADE:
		{
			if (Player[CLIENT_PLAYER_INDEX].m_ItemList.IsLockOperation())
				return 0;
			KUiObjAtContRegion* pObject1 = (KUiObjAtContRegion*)uParam;
			if (CGOG_PLAYERSELLITEM != pObject1->Obj.uGenre)
				break;

			int nWidth, nHeight;
			ItemPos	Pos;

			nWidth = Item[pObject1->Obj.uId].GetWidth();
			nHeight = Item[pObject1->Obj.uId].GetHeight();
			if (!Player[CLIENT_PLAYER_INDEX].m_ItemList.SearchPosition(nWidth, nHeight, &Pos))
			{
				nRet = 0;
				break;
			}
			if (Pos.nPlace != pos_equiproom && Pos.nPlace != pos_equiproomex && Pos.nPlace != pos_ptbag2 && Pos.nPlace != pos_ptbag3)
			{
				nRet = 0;

				KSystemMessage	sMsg;

				strcpy(sMsg.szMessage, MSG_SHOP_NO_ROOM);
				sMsg.eType = SMT_SYSTEM;
				sMsg.byConfirmType = SMCT_CLICK;
				sMsg.byPriority = 1;
				sMsg.byParamSize = 0;
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				break;
			}

			if (Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipmentMoney() < Item[pObject1->Obj.uId].GetTradePrice())
			{
				nRet = 0;
				KSystemMessage	sMsg;

				strcpy(sMsg.szMessage, MSG_SHOP_NO_MONEY);
				sMsg.eType = SMT_SYSTEM;
				sMsg.byConfirmType = SMCT_CLICK;
				sMsg.byPriority = 1;
				sMsg.byParamSize = 0;
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				break;

			}
			PLAYER_TRADE_BUY_COMMAND sBuy;
			sBuy.ProtocolType = c2s_playertradebuy;
			sBuy.m_nIdx = g_cSellItem.FindIdx(pObject1->Obj.uId);
			sBuy.m_dwID = g_cSellItem.GetSellId();
			sBuy.m_Place = Pos.nPlace;
			sBuy.m_X = Pos.nX;
			sBuy.m_Y = Pos.nY;
			if (g_pClient)
				g_pClient->SendPackToServer((BYTE*)&sBuy, sizeof(PLAYER_TRADE_BUY_COMMAND));
			Player[CLIENT_PLAYER_INDEX].m_ItemList.LockOperation();
			nRet = 1;
		}
		break;

	case GOI_PLAYER_RLTRADE:
		{
			g_cSellItem.Release();
			BYTE btProtocol = c2s_playertradeviewend;
			g_pClient->SendPackToServer((BYTE*)&btProtocol, sizeof(BYTE));
		}
		break;
	case GOI_LOCKMOVE:
		PLAYER_LOCKMOVE	LockMove;
		LockMove.ProtocolType = c2s_lockmove;
		LockMove.m_LockMove.bLock = (BOOL)uParam;
		LockMove.m_LockMove.nPlace = (int)nParam;
		g_pClient->SendPackToServer((BYTE*)&LockMove, sizeof(PLAYER_LOCKMOVE));
		break;
	case GOI_UNLOCK:
		if(uParam)
		{
			C2S_BUF_COMMAND sUL;
			sUL.ProtocolType = c2s_unlockcommand;
			strcpy(sUL.szBuf, (char*)uParam);
			g_pClient->SendPackToServer((BYTE*)&sUL, sizeof(C2S_BUF_COMMAND));
		}
		break;
	case GOI_CHATROOM:
		if(uParam)
		{
			PLAYER_CHATROOM_DECISION_COMMAND	sRoom;
			sRoom.ProtocolType = c2s_chatroomdecision;
			sRoom.m_nDecision = (int)uParam;
			sRoom.m_bRoomList = (BOOL)nParam;
			g_pClient->SendPackToServer((BYTE*)&sRoom, sizeof(PLAYER_CHATROOM_DECISION_COMMAND));
		}
		break;

	case GOI_CP_SET_IMAGE_PLAYER:
		SendClientCPSetImageCmd(uParam);
		break;

	//TamLTM da tau
	case GOI_CP_LOAD_SCRIPT:// Da Tau nhiem vu
		SendClientDaTauCmd(uParam);
		//g_DebugLog("uParam->nScript: %d", uParam);
	break;
	//end

	//TamLTM Ma doc
//	case GOI_PLAYER_ACTIONCHAT:
	//	SendClientCPActionCheatCmd((char *)uParam);
//	break;
	//End

	//TamLTM kham nam xanh
	case GOI_ADD_UI_CMD_SCRIPT:
		if (uParam)
		{
			if (uParam == 12)
			{
				SendUiCmdScript(uParam,(char*)nParam);
			}
			else
				SendUiCmdScript(uParam,(char*)nParam);
		}
		break;
	case GOI_RECOVERY_BOX_COMMAND:
		if (uParam)
		{
			DWORD dwID;
			int nW, nH;
			PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
			if (pItem && pItem->nPlace == uParam)
			{
				dwID = Item[pItem->nIdx].GetID();
				nW = Item[pItem->nIdx].GetWidth();
				nH = Item[pItem->nIdx].GetHeight();

				SendClientRecoveryBox(dwID, nW, nH);
			}

			while(pItem)
			{
				pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
				if (pItem && pItem->nPlace == uParam)
				{
					dwID = Item[pItem->nIdx].GetID();
					nW = Item[pItem->nIdx].GetWidth();
					nH = Item[pItem->nIdx].GetHeight();

					SendClientRecoveryBox(dwID, nW, nH);
				}
			}
		}
		break;
		// end code
	//TamLTM Open Progress bar
	case GOI_CP_LOAD_SCRIPT_PROGRESS_BAR:
		SendClientOpenProgressBarCmd(uParam);
		break;
	//end
	default:
		nRet = 0;
		break;
	}
	return nRet;
}

//--------------------------------------------------------------------------
//	���ܣ����������豸�����������Ϣ
//--------------------------------------------------------------------------
void KCoreShell::ProcessInput(unsigned int uMsg, unsigned int uParam, int nParam)
{
	Player[CLIENT_PLAYER_INDEX].ProcessInputMsg(uMsg, uParam, nParam);
}

int KCoreShell::GetTargetNPC()
{
	return Player[CLIENT_PLAYER_INDEX].GetTargetNpc();
}

int KCoreShell::FindSelectNPC(int x, int y, int nRelation, bool bSelect, void* pReturn, int& nKind, bool bFind)
{
	Player[CLIENT_PLAYER_INDEX].FindSelectNpc(x, y, nRelation);
	int nT = Player[CLIENT_PLAYER_INDEX].GetTargetNpc();

	if (!bSelect)
		Player[CLIENT_PLAYER_INDEX].SetTargetNpc(0);

	if (nT > 0)
	{
		if (pReturn)
		{
			if (bFind && (Npc[nT].Name[0]==0))
				return false;

			KUiPlayerItem* p = (KUiPlayerItem*)pReturn;
			{unsigned int color=0;PhongThanParseNpcName(Npc[nT].Name,p->Name,sizeof(p->Name),color);}
			p->nIndex = Npc[nT].m_Index;
			p->uId = Npc[nT].m_dwID;
			p->nData = Npc[nT].GetMenuState();
			p->nTeamID = Npc[nT].m_nTeamServerID;

			Player[CLIENT_PLAYER_INDEX].m_cAI.m_nMovePosX = x;
			Player[CLIENT_PLAYER_INDEX].m_cAI.m_nMovePosY = y;
		//	g_DebugLog("FindSelectNPC %d %d", x, y);
		}
		nKind = Npc[nT].m_Kind;
		return true;
	}
	return false;
}

int KCoreShell::FindSelectObject(int x, int y, bool bSelect, int& nObjectIdx, int& nKind)
{
	Player[CLIENT_PLAYER_INDEX].FindSelectObject(x, y);
	int nT = Player[CLIENT_PLAYER_INDEX].GetTargetObj();

	if (!bSelect)
		Player[CLIENT_PLAYER_INDEX].SetTargetObj(0);

	if (nT > 0)
	{
		nObjectIdx = nT;
		nKind = Object[nT].m_nKind;

		return true;
	}
	return false;
}


int KCoreShell::FindSpecialNPC(char* Name, void* pReturn, int& nKind)
{
	if (Name == NULL || Name[0] == 0)
		return false;
	for (int nT = 0; nT < MAX_NPC; nT++)
	{
		if	(strcmp(Npc[nT].Name, Name) == 0)
		{
			if (pReturn)
			{
				KUiPlayerItem* p = (KUiPlayerItem*)pReturn;
				{unsigned int color=0;PhongThanParseNpcName(Npc[nT].Name,p->Name,sizeof(p->Name),color);}
				p->nIndex = Npc[nT].m_Index;
				p->uId = Npc[nT].m_dwID;
				p->nData = Npc[nT].GetMenuState();
				p->nTeamID = Npc[nT].m_nTeamServerID;
			}
			nKind = Npc[nT].m_Kind;
			return true;
		}
	}
	return false;
}

int KCoreShell::ChatSpecialPlayer(void* pPlayer, const char* pMsgBuff, unsigned short nMsgLength)
{
	KUiPlayerItem* p = (KUiPlayerItem*)pPlayer;
	if (p)
	{
		if (p->nIndex >= 0 && p->nIndex < MAX_NPC)
		{
			int nTalker = p->nIndex;
			if (Npc[nTalker].m_Kind == kind_player &&
				Npc[nTalker].m_dwID == p->uId)
			{
				Npc[nTalker].SetChatInfo(p->Name, pMsgBuff, nMsgLength);
				return true;
			}
		}
	}

	return false;
}

void KCoreShell::ApplyAddTeam(void* pPlayer)
{
	KUiPlayerItem* p = (KUiPlayerItem*)pPlayer;
	if (p)
	{
		if (p->nIndex >= 0 && p->nIndex < MAX_NPC && !Player[CLIENT_PLAYER_INDEX].CheckTrading())
		{
			Player[CLIENT_PLAYER_INDEX].ApplyAddTeam(p->nIndex);
		}
	}
}

void KCoreShell::TradeApplyStart(void* pPlayer, bool bFolkGame)
{
	KUiPlayerItem* p = (KUiPlayerItem*)pPlayer;
	if (p)
	{
		if (p->nIndex >= 0 && p->nIndex < MAX_NPC && !Player[CLIENT_PLAYER_INDEX].CheckTrading())
		{
			Player[CLIENT_PLAYER_INDEX].TradeApplyStart(p->nIndex, bFolkGame);
		}
	}
}

void KCoreShell::JoinTongReply(void* pPlayer)
{
    // Retired JX invitation protocol. Native nation requests are separate.
}

int KCoreShell::UseSkill(int x, int y, int nSkillID)
{
	if (Player[CLIENT_PLAYER_INDEX].CheckTrading())
		return 0;

	int nX = x;
	int nY = y;
	int nZ = 0;
	g_ScenePlace.ViewPortCoordToSpaceCoord(nX, nY, nZ);
	int nIndex = Player[CLIENT_PLAYER_INDEX].m_nIndex;

	if (Npc[nIndex].IsCanInput())
	{
		int nIdx = 0;

		nIdx = Npc[nIndex].m_SkillList.FindSame(nSkillID);
		Npc[nIndex].SetActiveSkill(nIdx);
	}
	else
	{
		return 0;
	}

	if (Npc[nIndex].m_ActiveSkillID > 0)
	{
		ISkill * pISkill =  g_SkillManager.GetSkill(Npc[nIndex].m_ActiveSkillID, 1);
		if (!pISkill)
            return 0;

		if (pISkill->IsAura())
			return 0;

		int nAttackRange = pISkill->GetAttackRadius();

		int nTargetIdx = 0;
		//����Object / Enemy / Ally �����ȼ��ҵ���Ҫ��Ķ���id

		if (pISkill->IsTargetAlly())
		{
			Player[CLIENT_PLAYER_INDEX].FindSelectNpc(x, y, relation_ally);
			if (Player[CLIENT_PLAYER_INDEX].GetTargetNpc())
			{
				nTargetIdx = Player[CLIENT_PLAYER_INDEX].GetTargetNpc();
			}
		}

		g_DebugLog("nTargetIdx %d", nTargetIdx);

		if (pISkill->IsTargetEnemy())
		{
			Player[CLIENT_PLAYER_INDEX].FindSelectNpc(x, y, relation_enemy);
			if (Player[CLIENT_PLAYER_INDEX].GetTargetNpc())
			{
				nTargetIdx = Player[CLIENT_PLAYER_INDEX].GetTargetNpc();
			}
		}

        if (pISkill->IsTargetEnemy() && nTargetIdx > 0 && nTargetIdx < MAX_NPC)
            SendClientCmdSummonTarget(Npc[nTargetIdx].m_dwID);
		if (pISkill->IsTargetObj())
		{
			Player[CLIENT_PLAYER_INDEX].FindSelectObject(x, y);
			if (Player[CLIENT_PLAYER_INDEX].GetTargetObj())
			{
				nTargetIdx = Player[CLIENT_PLAYER_INDEX].GetTargetObj();
			}
		}
		//������ܱ���ָ�����󣬶���ǰλ���޶���Ļ���ֱ���˳�
		if (pISkill->IsTargetOnly() && !nTargetIdx)
        {
			Npc[nIndex].m_nPeopleIdx = 0;
			Player[CLIENT_PLAYER_INDEX].SetTargetNpc(0);
			return 0;
		}

		if (nIndex == nTargetIdx)
		{
			Npc[nIndex].m_nPeopleIdx = 0;
			Player[CLIENT_PLAYER_INDEX].SetTargetNpc(0);
			return 0;
		}
		if ((!Npc[nIndex].m_SkillList.CanCast(Npc[nIndex].m_ActiveSkillID, SubWorld[Npc[nIndex].m_SubWorldIndex].m_dwCurrentTime))
			||
			(!Npc[nIndex].Cost(pISkill->GetSkillCostType(), pISkill->GetSkillCost(&Npc[nIndex]), TRUE))
			)
		{
			Npc[nIndex].m_nPeopleIdx = 0;
			Player[CLIENT_PLAYER_INDEX].SetTargetNpc(0);
			return 0;
		}

		//�޶���ֱ�ӷ�����
		if (!nTargetIdx)
		{
			//Skill thi trien
			Npc[nIndex].SendCommand(do_skill, Npc[nIndex].m_ActiveSkillID, nX, nY);
			SendClientCmdSkill(Npc[nIndex].m_ActiveSkillID, nX, nY);
		}
		else
		{
			if (pISkill->IsTargetOnly())
			{
				if (NpcSet.GetDistance(nIndex , nTargetIdx) > nAttackRange)
				{
					Player[CLIENT_PLAYER_INDEX].SetTargetNpc(nTargetIdx);
					return 0;
				}
			}
			if (nIndex == nTargetIdx && pISkill->GetSkillStyle() == SKILL_SS_Missles)
				return 0;

			if (NpcSet.GetDistance(nIndex , nTargetIdx) <= nAttackRange)
			{
				//Buff team
				Npc[nIndex].SendCommand(do_skill, Npc[nIndex].m_ActiveSkillID, -1, nTargetIdx);
				SendClientCmdSkill(Npc[nIndex].m_ActiveSkillID, -1, Npc[nTargetIdx].m_dwID);
			}
		}
	}
	Npc[nIndex].m_nPeopleIdx = 0;
	return 1;
}

int KCoreShell::LockSomeoneUseSkill(int nTargetIndex, int nSkillID)
{
	if (Player[CLIENT_PLAYER_INDEX].CheckTrading())
		return 0;

	int nIndex = Player[CLIENT_PLAYER_INDEX].m_nIndex;

	if (nTargetIndex == nIndex)
		return 0;

	if (Npc[nIndex].IsCanInput())
	{
		int nIdx = 0;

		nIdx = Npc[nIndex].m_SkillList.FindSame(nSkillID);
		Npc[nIndex].SetActiveSkill(nIdx);
	}
	else
	{
		return 0;
	}

	int nRelation = NpcSet.GetRelation(nIndex, nTargetIndex);
	if (nRelation == relation_enemy)
	{
		Npc[nIndex].m_nPeopleIdx = nTargetIndex;
		return 1;
	}

	return 0;
}

int KCoreShell::LockSomeoneAction(int nTargetIndex)
{
	if (Player[CLIENT_PLAYER_INDEX].CheckTrading())
		return 0;

	int nIndex = Player[CLIENT_PLAYER_INDEX].m_nIndex;

	if (nTargetIndex == nIndex)
		return 0;
	if (nTargetIndex <= 0 || nTargetIndex >= MAX_NPC)	//ȡ��Lock
	{
		Npc[nIndex].m_nPeopleIdx = 0;
		return 1;
	}

	int nRelation = NpcSet.GetRelation(nIndex, nTargetIndex);
	if (nRelation != relation_enemy)
	{
		Npc[nIndex].m_nPeopleIdx = nTargetIndex;
		return 1;
	}

	return 0;
}

int KCoreShell::LockObjectAction(int nTargetIndex)
{
	if (Player[CLIENT_PLAYER_INDEX].CheckTrading())
		return 0;

	int nIndex = Player[CLIENT_PLAYER_INDEX].m_nIndex;

	if (nTargetIndex <= 0)	//ȡ��Lock
		Npc[nIndex].m_nObjectIdx = 0;
	else
		Npc[nIndex].m_nObjectIdx = nTargetIndex;

	return 1;
}

void KCoreShell::GotoWhere(int x, int y, int mode)
{

	if (Player[CLIENT_PLAYER_INDEX].CheckTrading())
		return;

	if (mode < 0 || mode > 2)
		return;

	if (Player[CLIENT_PLAYER_INDEX].m_nSendMoveFrames >= defMAX_PLAYER_SEND_MOVE_FRAME)
	{
		int bRun = false;

		if ((mode == 0 && Player[CLIENT_PLAYER_INDEX].m_RunStatus) ||
			mode == 2)
			bRun = true;

		int nX = x;
		int nY = y;
		int nZ = 0;

	//	g_DebugLog("GotoWhere %d %d", x, y);

		g_ScenePlace.ViewPortCoordToSpaceCoord(nX, nY, nZ);
		int nIndex = Player[CLIENT_PLAYER_INDEX].m_nIndex;

		if (!bRun)
		{
			Npc[nIndex].SendCommand(do_walk, nX, nY);
			// Send to Server
			SendClientCmdWalk(nX, nY);
		}
		else
		{
			Npc[nIndex].SendCommand(do_run, nX, nY);
			// Send to Server
			SendClientCmdRun(nX, nY);
		}
		Player[CLIENT_PLAYER_INDEX].m_nSendMoveFrames = 0;
	}
}

void KCoreShell::GetWhere(int x, int y, void* pInfo)
{
	KPosition3* p = (KPosition3*)pInfo;
	int nX = x;
	int nY = y;
	int nZ = 0;
	g_ScenePlace.ViewPortCoordToSpaceCoordNew(p->nX, nX, nY, nZ);
	p->nY = nX;
	p->nZ = nY;
}

void KCoreShell::Goto(int nDir, int mode)
{
	if (nDir < 0 || nDir > 63)
		return;

	if (mode < 0 || mode > 2)
		return;

	int bRun = false;

	if ((mode == 0 && Player[CLIENT_PLAYER_INDEX].m_RunStatus) ||
		mode == 2)
		bRun = true;

	int nIndex = Player[CLIENT_PLAYER_INDEX].m_nIndex;
	int nSpeed;
	if (bRun)
		nSpeed = Npc[nIndex].m_CurrentRunSpeed;
	else
		nSpeed = Npc[nIndex].m_CurrentWalkSpeed;

	Player[CLIENT_PLAYER_INDEX].Walk(nDir, nSpeed);

	Player[CLIENT_PLAYER_INDEX].m_nSendMoveFrames = 0;
}

void KCoreShell::Turn(int nDir)
{
	if (nDir < 0 || nDir > 3)
		return;

	if (nDir == 0)
		Player[CLIENT_PLAYER_INDEX].TurnLeft();
	else if (nDir == 1)
		Player[CLIENT_PLAYER_INDEX].TurnRight();
	else
		Player[CLIENT_PLAYER_INDEX].TurnBack();
}

int KCoreShell::ThrowAwayItem()
{
	return Player[CLIENT_PLAYER_INDEX].ThrowAwayItem();
}

int KCoreShell::GetNPCRelation(int nIndex)
{
	return NpcSet.GetRelation(Player[CLIENT_PLAYER_INDEX].m_nIndex, nIndex);
}

BOOL KCoreShell::CheckMouse()
{
	if (Player[CLIENT_PLAYER_INDEX].m_ItemList.Hand())
		return TRUE;
	return FALSE;
}
//--------------------------------------------------------------------------
//	���ܣ�������Ϸ����
//--------------------------------------------------------------------------
void KCoreShell::DrawGameObj(unsigned int uObjGenre, unsigned int uId, int x, int y, int Width, int Height, int nParam)
{
	if (g_pRepresent)
		CoreDrawGameObj(uObjGenre, uId, x, y, Width, Height, nParam);
}

#include "../../Represent/iRepresent/iRepresentshell.h"

//--------------------------------------------------------------------------
//	���ܣ�������Ϸ����
//--------------------------------------------------------------------------
#include "PhongThanMovementTrace.inl"
void KCoreShell::DrawGameSpace()
{
	if (g_pRepresent)
	{
		// Advance display anchors and camera together before scene traversal.
        // Do not change KNpc map coordinates or the 18Hz simulation cadence.
        // IR_GetCurrentTime still describes the previous paint at this point.
        const unsigned int renderNow = timeGetTime();
        int renderNpc = 0, renderCount = 0;
        while ((renderNpc = NpcSet.GetNextIdx(renderNpc)) != 0) {
            if (Npc[renderNpc].m_RegionIndex < 0) continue;
            ++renderCount;
            Npc[renderNpc].GetNpcRes()->AdvanceRenderPosition(renderNpc, renderNow,
                renderNpc == Player[CLIENT_PLAYER_INDEX].m_nIndex);
        }
        const unsigned int prepared = timeGetTime();
        g_ScenePlace.Paint();
        PhongThanMovementFrame(renderNow, prepared, timeGetTime(), renderCount);
	}
}
void KCoreShell::DrawGameSpace1()
{
	if (g_pRepresent)
		g_ScenePlace.PaintNpcNameOverlay();
}
//--------------------------------------------------------------------------
//	���ܣ����û�ͼ�ӿ�ʵ����ָ��
//--------------------------------------------------------------------------
void KCoreShell::SetRepresentShell(struct iRepresentShell* pRepresent)
{
	g_pRepresent = pRepresent;
	g_ScenePlace.RepresentShellReset();
	if (g_pAdjustColorTab && g_ulAdjustColorCount && g_pRepresent)
		g_pRepresent->SetAdjustColorList(g_pAdjustColorTab, g_ulAdjustColorCount);
}

void KCoreShell::SetMusicInterface(void* pMusicInterface)
{
	g_pMusic = (KMusic*)pMusicInterface;
	Option.SetMusicVolume(Option.GetMusicVolume());
}

//�ճ����core���Ҫ���������򷵻�0�����򷵻ط�0ֵ
int KCoreShell::Breathe()
{
	const unsigned int movementLogicStart=timeGetTime();
	g_SubWorldSet.MessageLoop();
	g_SubWorldSet.MainLoop();
	g_ScenePlace.Breathe();
    s_PtMovementLogicMs=timeGetTime()-movementLogicStart;

//	Sleep(10); // Chay normal
	return true;
}

int KCoreShell::GetProtocolSize(BYTE byProtocol)
{
	if (byProtocol <= s2c_clientbegin || byProtocol >= s2c_end)
		return -1;
	return g_nProtocolSize[byProtocol - s2c_clientbegin - 1];
}

//Debug
#ifdef SWORDONLINE_SHOW_DBUG_INFO
extern int		g_bShowObstacle;
extern bool		g_bShowGameInfo;
#endif

int KCoreShell::Debug(unsigned int uDataId, unsigned int uParam, int nParam)
{
#ifdef SWORDONLINE_SHOW_DBUG_INFO
	switch(uDataId)
	{
	case DEBUG_SHOWINFO:
		Player[CLIENT_PLAYER_INDEX].m_bDebugMode = !Player[CLIENT_PLAYER_INDEX].m_bDebugMode;
		g_bShowGameInfo = !g_bShowGameInfo;
		break;
	case DEBUG_SHOWOBSTACLE:
		g_bShowObstacle = !g_bShowObstacle;
		break;
	}
#endif
	return 0;
}

//end code

DWORD KCoreShell::GetPing()
{
	return g_SubWorldSet.GetPing();
}

void KCoreShell::SetRepresentAreaSize(int nWidth, int nHeight)
{
	g_ScenePlace.SetRepresentAreaSize(nWidth, nHeight);
}

void KCoreShell::SetClient(LPVOID pClient)
{
	g_SetClient(pClient);
}

void KCoreShell::SendNewDataToServer(void* pData, int nLength)
{
//	if (nLength < 0 || nLength >= 255)
	//	return;
	if (g_pClient)
		g_pClient->SendPackToServer(pData, nLength);
}

//���ͼ��صĲ���
int	KCoreShell::SceneMapOperation(unsigned int uOper, unsigned int uParam, int nParam)
{
	int nRet = 0;
	switch(uOper)
	{
	case GSMOI_SCENE_TIME_INFO:
		if (uParam)
		{
			KUiSceneTimeInfo* pInfo = (KUiSceneTimeInfo*)uParam;
			g_ScenePlace.GetSceneNameAndFocus(pInfo->szSceneName, pInfo->nSceneId,
				pInfo->nScenePos0, pInfo->nScenePos1);
			pInfo->nGameSpaceTime = (SubWorld[0].m_dwCurrentTime / 100) % 1440;

		//	g_DebugLog("SubWorld[0].m_bCheckTong %s", SubWorld[0].m_szTongName);
			//TamLTM thue suat bang hoi chiem linh bang hoi
			if (SubWorld[0].m_bCheckTong)
			{
			   memcpy(pInfo->szTongName, SubWorld[0].m_szTongName,sizeof(pInfo->szTongName));
			   memcpy(pInfo->szTongNameBC, SubWorld[0].m_szTongNameBC,sizeof(pInfo->szTongNameBC));
			   pInfo->szTongT = SubWorld[0].m_nTongT;
			   pInfo->szTongVG = SubWorld[0].m_nTongVG;
			}
			else
			{
				memset(pInfo->szTongName, 0, sizeof(pInfo->szTongName));
				memset(pInfo->szTongNameBC, 0, sizeof(pInfo->szTongNameBC));
				pInfo->szTongT = 0;
                pInfo->szTongVG = 0;
			}
			//end code
		}
		break;
	case GSMOI_SCENE_TIME_INFO_OFTEN:
		if (uParam)
		{
			KUiSceneTimeInfoOften* pInfo = (KUiSceneTimeInfoOften*)uParam;
			g_ScenePlace.GetSceneNameAndFocusOften(pInfo->szSceneName, pInfo->nSceneId,
				pInfo->nScenePos0, pInfo->nScenePos1);
			pInfo->nGameSpaceTime = (SubWorld[0].m_dwCurrentTime / 100) % 1440;
		}
		break;
	case GSMOI_SCENE_MAP_INFO:
		nRet = g_ScenePlace.GetMapInfo((KSceneMapInfo*)uParam);
		break;
	case GSMOI_IS_SCENE_MAP_SHOWING:
		g_ScenePlace.SetMapParam(uParam, nParam);
		break;
	case GSMOI_PAINT_SCENE_MAP:
		g_ScenePlace.PaintMap(uParam, nParam);
		break;
	case GSMOI_SCENE_MAP_FOCUS_OFFSET:
		g_ScenePlace.SetMapFocusPositionOffset((int)uParam, nParam);
		break;
	case GSMOI_SCENE_FOLLOW_WITH_MAP:	//���ó����Ƿ����ŵ�ͼ���ƶ����ƶ�
		g_ScenePlace.FollowMapMove(nParam);
		break;
	case GSMOI_IS_SCENE_DIRECT_MAP:
		nRet = g_ScenePlace.OnDirectMap((int)uParam, nParam);
		break;
	case GSMOI_IS_SCENE_DO_DIRECT_MAP:
		g_ScenePlace.DoDirectMap((int)uParam, nParam);
		break;
	}
	return nRet;
}

//������صĲ���, uOper��ȡֵ���� GAME_TONG_OPERATION_INDEX
int	KCoreShell::TongOperation(unsigned int uOper, unsigned int uParam, int nParam)
{
    // Read-only HUD/context compatibility. Mutations use native nations.
    int actor=Player[CLIENT_PLAYER_INDEX].m_nIndex;
    if(uParam)actor=((KUiPlayerItem*)uParam)->nIndex;
    if(actor<=0||actor>=MAX_NPC)return 0;
    if(uOper==GTOI_TONG_FLAG)return Npc[actor].m_bTongFlag;
    if(uOper==GTOI_TONG_FIGURE)return Npc[actor].m_nFigure;
    return 0;
}

//�������صĲ�����uOper��ȡֵ���� GAME_TEAM_OPERATION_INDEX
int KCoreShell::TeamOperation(unsigned int uOper, unsigned int uParam, int nParam)
{
    if(uOper==PT_PEOPLE_ACTION)return uParam&&nParam==sizeof(PT_PEOPLE_COMMAND)?PhongThanRemotePeopleAction((const PT_PEOPLE_COMMAND*)uParam):0;
    if(uOper==PT_PEOPLE_GET)return uParam&&nParam==sizeof(PT_PEOPLE_RESULT)?PhongThanRemotePeopleGet((PT_PEOPLE_RESULT*)uParam):0;
    if(uOper==PT_PS_ACTION)return uParam&&nParam==sizeof(PT_PS_COMMAND)?PhongThanPlayerServiceAction((const PT_PS_COMMAND*)uParam):0;
    if(uOper==PT_PS_GET)return uParam&&nParam==sizeof(PT_PS_STATE)?PhongThanPlayerServiceGet((PT_PS_STATE*)uParam):0;
    if(uOper==PT_PS_RELEASE){if(uParam>0&&uParam<MAX_ITEM&&s_PtMailPreviews[uParam]&&Item[uParam].GetID()==0){ItemSet.Remove(uParam);s_PtMailPreviews[uParam]=false;}return 1;}
    if(uOper==PT_PS_PREVIEW){
        if(!uParam||nParam!=sizeof(PT_PS_ITEM))return 0;const PT_PS_ITEM& d=*(const PT_PS_ITEM*)uParam;
        int gen[40];memcpy(gen,d.Generator,sizeof(gen));int n=ItemSet.Add(d.Genre,d.Series,d.Level,d.Luck,d.Detail,d.Particular,gen,d.Version,d.Seed,d.Row);
        if(n<=0||n>=MAX_ITEM)return 0;
        if(!PtPsRestoreItem(Item[n],d)){ItemSet.Remove(n);return 0;}
        Item[n].SetID(0);s_PtMailPreviews[n]=true;return n;
    }
    if(uOper==PT_AUCTION_ACTION)return uParam&&nParam==sizeof(PTAuctionCommand)?PhongThanAuctionAction((const PTAuctionCommand*)uParam):0;
    if(uOper==PT_AUCTION_GET)return uParam&&nParam==sizeof(PTAuctionPage)?PhongThanAuctionGet((PTAuctionPage*)uParam):0;
    if(uOper==PT_RANK_ACTION){if(!uParam||nParam!=sizeof(PTRankingQuery))return 0;return PhongThanRankingQuery((const PTRankingQuery*)uParam);}
    if(uOper==PT_STATUS_PAGES_ACTION){if(!uParam||nParam!=sizeof(PT_STATUS_PAGES_REQUEST))return 0;return PhongThanStatusPagesAction((const PT_STATUS_PAGES_REQUEST*)uParam);}
	int nRet = 0;
	switch(uOper)
	{
    case PT_TEAM_UI_GET:
        return PhongThanGetTeamState((PT_TEAM_STATE*)uParam);
    case PT_TEAM_UI_VISIBLE:
        PhongThanSetTeamNameVisible(uParam!=0);return 1;
    case PT_TEAM_UI_NAME:
        if(uParam)PhongThanRequestTeamState((const char*)uParam);return 1;
    case TEAM_OI_PT_OPTIONS:
        return PhongThanTeamOptions(uParam,nParam!=0);
	case TEAM_OI_GD_INFO:
		if (uParam)
		{
			KUiPlayerTeam* pTeam = (KUiPlayerTeam*)uParam;
			nRet = Player[CLIENT_PLAYER_INDEX].m_cTeam.GetInfo(pTeam);
		}
		break;
	case TEAM_OI_GD_FIGURE: //TamLTM auto pt to doi
		if (Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFlag)
			nRet = Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFigure;
		else
			nRet = -1;
		break;
	case TEAM_OI_GD_MEMBER_LIST:
		nRet = g_Team[0].GetMemberInfo((KUiPlayerItem *)uParam, nParam);
		break;
	case TEAM_OI_GD_REFUSE_INVITE_STATUS:
		nRet = Player[CLIENT_PLAYER_INDEX].m_cTeam.GetAutoRefuseState();
		break;
	case TEAM_OI_COLLECT_NEARBY_LIST:
		NpcSet.GetAroundOpenCaptain(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Camp);
		break;
	case TEAM_OI_APPLY:
		if (uParam)
        {
            const KUiPlayerItem& leader=((KUiTeamItem*)uParam)->Leader;
            if(leader.nIndex>0 && leader.nIndex<MAX_NPC && Npc[leader.nIndex].m_Kind==kind_player &&
                Npc[leader.nIndex].m_RegionIndex>=0 && Npc[leader.nIndex].m_dwID==leader.uId)
                Player[CLIENT_PLAYER_INDEX].ApplyAddTeam(leader.nIndex);
        }
		break;
	case TEAM_OI_CREATE:		//
		Player[CLIENT_PLAYER_INDEX].ApplyCreateTeam();//(char*)uParam);
		break;
	case TEAM_OI_APPOINT:		//
		Player[CLIENT_PLAYER_INDEX].ApplyTeamChangeCaptain(((KUiPlayerItem*)uParam)->uId);
		break;
	case TEAM_OI_INVITE:		//
		if (uParam)
		{
			Player[CLIENT_PLAYER_INDEX].TeamInviteAdd(((KUiPlayerItem*)uParam)->uId);
		}
		break;
	case TEAM_OI_KICK:			//
		Player[CLIENT_PLAYER_INDEX].TeamKickMember(((KUiPlayerItem*)uParam)->uId);
		break;
	case TEAM_OI_LEAVE:			//
		Player[CLIENT_PLAYER_INDEX].LeaveTeam();
		break;
	case TEAM_OI_CLOSE:			//
		Player[CLIENT_PLAYER_INDEX].ApplyTeamOpenClose(nParam);
		break;
	case TEAM_OI_PKFOLLOW:			//
		Player[CLIENT_PLAYER_INDEX].ApplyTeamPKFollow(nParam);
		break;
	//TamLTM auto pt to doi
	case TEAM_OI_MODEPICK:			//
		Player[CLIENT_PLAYER_INDEX].ApplyTeamModePick(nParam);
		break;
	case TEAM_OI_REFUSE_INVITE:		//
		Player[CLIENT_PLAYER_INDEX].m_cTeam.SetAutoRefuseInvite(nParam);
		break;
	//end code
	case TEAM_OI_APPLY_RESPONSE:		//
		if (uParam)
		{
			if (nParam)
			{
				Player[CLIENT_PLAYER_INDEX].AcceptTeamMember(((KUiPlayerItem*)uParam)->uId);
			}
			else
			{
				Player[CLIENT_PLAYER_INDEX].m_cTeam.DeleteOneFromApplyList(((KUiPlayerItem*)uParam)->uId);
			}
		}
		break;
	case TEAM_OI_INVITE_RESPONSE://
		if (uParam)
			Player[CLIENT_PLAYER_INDEX].m_cTeam.ReplyInvite(((KUiPlayerItem*)uParam)->nIndex, nParam);
		break;
	case TEAM_OI_GETUID:	//TamLTM auto pt to doi / Thu nghiem
		//	nRet = ((KUiPlayerItem*)uParam)->uId;
		break;
	}
	return nRet;
}


int KCoreShell::PAIOperation(unsigned int uOper, unsigned int uParam, int nParam, int nParam1)
{
	int nRet = 0;
	switch(uOper)
	{
	//loc fix
    case PT_PET_SHORTCUT:
        PhongThanSetPetShortcut((int)uParam,nParam);break;
    case PT_AUTO_NATIVE_BUFF:
        PhongThanSetNativeBuff((int)uParam,nParam);break;
    case PT_AUTO_SUMMON:
        PhongThanSetSummonAuto((int)uParam,nParam,nParam1);break;
    case PT_AUTO_RANGE_PREVIEW:
        PhongThanSetAutoPreview(uParam!=0);break;
    case PT_AUTO_COMBAT_MODE:
        PhongThanSetAutoMode((int)uParam,nParam,nParam1!=0);break;
    case PT_AUTO_RECOVERY_ROW:
        PhongThanSetRecoveryRow((int)uParam,nParam,nParam1);break;
	case GPI_SWITCH_ACTIVE:
	{

		if (nParam == 1)
		{
			if (Player[CLIENT_PLAYER_INDEX].m_cAI.m_bIsActive == 1)
			{
				uParam = 0;
			}
			else
			{
				uParam = 1;
			}
		}

		if (uParam == 1)
		{
			Player[CLIENT_PLAYER_INDEX].m_cAI.m_bIsActive = 1;
			Player[CLIENT_PLAYER_INDEX].m_cAI.SetSortEquipment(FALSE);
		}
		else
		{
			Player[CLIENT_PLAYER_INDEX].m_cAI.m_bIsActive = 0;
			Player[CLIENT_PLAYER_INDEX].m_cAI.SetSortEquipment(FALSE);
		}
	}
	break;
	case GPI_ISACTIVE:
		nRet = Player[CLIENT_PLAYER_INDEX].m_cAI.m_bIsActive;
		break;
	// loc fix
	case GPI_PRIORITY_USE_MOUSE:
			Player[CLIENT_PLAYER_INDEX].m_cAI.m_bPriorityUseMouse = uParam;
		break;
	case GPI_HOLDING_SPACEBAR:
			Player[CLIENT_PLAYER_INDEX].m_cAI.m_bHoldSpaceBar = uParam;
		break;
	case GPI_FOLLOW_PEOPLE:
			Player[CLIENT_PLAYER_INDEX].m_cAI.m_bFollowPeople = uParam;
		break;
	case GPI_FOLLOW_PEOPLE_NAME:
			strcpy(Player[CLIENT_PLAYER_INDEX].m_cAI.m_szFollowName, (char*)uParam);
		break;
	case GPI_FOLLOW_PEOPLE_RADIUS:
			Player[CLIENT_PLAYER_INDEX].m_cAI.m_nFollowRadius = uParam;
		break;
	case GPI_FOLLOW_PEOPLE_RADIUS_IS_CHECK:
		Player[CLIENT_PLAYER_INDEX].m_cAI.m_bFollowRadius = uParam;
		break;
	case GPI_FIGHTNEAR_CHECK:
			Player[CLIENT_PLAYER_INDEX].m_cAI.m_bFightNear = uParam;
		break;
	case GPI_ATTACK_NPC_CHECK:
			Player[CLIENT_PLAYER_INDEX].m_cAI.m_bAttackNpc = uParam;
		break;
	case GPI_AUTO_PARTY:
			Player[CLIENT_PLAYER_INDEX].m_cAI.m_bAutoParty = uParam;
		break;
	case GPI_AUTO_INVITE:
			Player[CLIENT_PLAYER_INDEX].m_cAI.m_bAutoInvite = uParam;
		break;
	case GPI_AUTO_INVITE_LIST:
			Player[CLIENT_PLAYER_INDEX].m_cAI.m_bAutoInviteList = uParam;
		break;
	case GPI_SET_INVITE_LIST:
			if (nParam >= 0 && nParam < MAX_AUTO_LIST)
				strcpy(Player[CLIENT_PLAYER_INDEX].m_cAI.m_szPartyList[nParam], (char*)uParam);
		break;
	case GPI_AUTO_ACCEPT:
			Player[CLIENT_PLAYER_INDEX].m_cTeam.m_bAutoAccept = uParam;
		break;
	case GPI_AUTO_ACCEPT_WITH_NAME:
			Player[CLIENT_PLAYER_INDEX].m_cTeam.m_bAutoAcceptWithName = uParam;
		break;
	case GPI_FIGHT_AUTO:
			Player[CLIENT_PLAYER_INDEX].m_cAI.m_nDistanceAuto = uParam;
		break;
	case GPI_RANGER_AUTO:
			Player[CLIENT_PLAYER_INDEX].m_cAI.m_nRadiusAuto = uParam;
		break;
	//Distance
	case GPI_DISTANCE_AUTO:
		Player[CLIENT_PLAYER_INDEX].m_cAI.m_nRadiusQuanhDiemXY = uParam;
		break;
	//end code
	case GPI_FIGHT_USE_SB:
			Player[CLIENT_PLAYER_INDEX].m_cAI.m_bFightUseSB = uParam;
		break;
	case GPI_SUPPORT_SKILLS:
			if (uParam >= 0 && uParam < MAX_AUTO_STATESKILL && nParam < MAX_SKILL)
				Player[CLIENT_PLAYER_INDEX].m_cAI.m_nSupportSkills[uParam] = nParam;
		break;
	case GPI_FIGHT_SKILLS:
			Player[CLIENT_PLAYER_INDEX].m_cAI.m_nFightSkill = uParam;
		break;
	case GPI_FIGHTBACK:
			Player[CLIENT_PLAYER_INDEX].m_cAI.m_nFightBack = uParam;
		break;
	case GPI_FIGHTBOSS:
			Player[CLIENT_PLAYER_INDEX].m_cAI.m_nFightBoss = uParam;
		break;
	case GPI_AURA_SKILLS1:
			if (nParam >= 0 && nParam < MAX_SKILL)
				Player[CLIENT_PLAYER_INDEX].m_cAI.m_nActiveAuraID1 = uParam;
		break;
	case GPI_AURA_SKILLS2:
			if (nParam >= 0 && nParam < MAX_SKILL)
				Player[CLIENT_PLAYER_INDEX].m_cAI.m_nActiveAuraID2 = uParam;
		break;
	case GPI_OVER_TARGET:
			Player[CLIENT_PLAYER_INDEX].m_cAI.m_bOverTarget = uParam;
		break;
	case GPI_FOLLOW_TARGET:
			Player[CLIENT_PLAYER_INDEX].m_cAI.m_bFollowTarget = uParam;
		break;
	//TamLTM QuanhDiem
	case GPI_QUANH_DIEM_TARGET:
			Player[CLIENT_PLAYER_INDEX].m_cAI.m_bQuanhDiem = uParam;
		break;
	case GPI_SORT_EQUIPMENT:
			if (!Player[CLIENT_PLAYER_INDEX].m_cAI.m_bSortEquipment)
				Player[CLIENT_PLAYER_INDEX].m_cAI.SetSortEquipment(TRUE);
		break;
	case GPI_FILTER_EQUIPMENT:
			Player[CLIENT_PLAYER_INDEX].m_cAI.m_bFilterEquipment = uParam;
		break;
	case GPI_PICK_FIGHTNONE:
			Player[CLIENT_PLAYER_INDEX].m_cAI.m_bPickFightNone = uParam;
		break;
	case GPI_PICK_MONEY:
			Player[CLIENT_PLAYER_INDEX].m_cAI.m_bPickMoney = uParam;
		break;
	case GPI_PICK_NOTEQUIP:
			Player[CLIENT_PLAYER_INDEX].m_cAI.m_bPickNotEquip = uParam;
		break;
	case GPI_PICK_EQUIP:
			Player[CLIENT_PLAYER_INDEX].m_cAI.m_bPickEquip = uParam;
		break;
	case GPI_FOLLOW_PICK:
			Player[CLIENT_PLAYER_INDEX].m_cAI.m_bFollowPick = uParam;
		break;
	case GPI_PICK_KIND:
			Player[CLIENT_PLAYER_INDEX].m_cAI.m_nPickEquipKind = uParam;
		break;
	case GPI_AUTO_FILTERMAGIC:
		{
			int nIndex = (int)g_MagicDesc.String2MagicID((char*)uParam);
			if (nIndex >= magic_skill_begin && nIndex < magic_normal_end)
			{
				Player[CLIENT_PLAYER_INDEX].m_cAI.m_nFilterMagic[nParam1][0] = nIndex;
				Player[CLIENT_PLAYER_INDEX].m_cAI.m_nFilterMagic[nParam1][1] = nParam;
			}
			else
			{
				Player[CLIENT_PLAYER_INDEX].m_cAI.m_nFilterMagic[nParam1][0] = 0;
				Player[CLIENT_PLAYER_INDEX].m_cAI.m_nFilterMagic[nParam1][1] = 0;
			}
		}
		break;
	case GPI_SAVE_TRASH:
			Player[CLIENT_PLAYER_INDEX].m_cAI.m_bSaveTrash = uParam;
		break;
	case GPI_SAVE_TRASH_PRICE:
			Player[CLIENT_PLAYER_INDEX].m_cAI.m_nSaveTrashPrice = uParam;
		break;
	case GPI_SAVE_JEWELRY:
			Player[CLIENT_PLAYER_INDEX].m_cAI.m_bSaveJewelry = uParam;
		break;
	case GPI_EAT_LIFE:
			Player[CLIENT_PLAYER_INDEX].m_cAI.m_bEatLife = uParam;
		break;
	case GPI_EAT_MANA:
			Player[CLIENT_PLAYER_INDEX].m_cAI.m_bEatMana = uParam;
		break;
	case GPI_LIFE_VALUE:
			Player[CLIENT_PLAYER_INDEX].m_cAI.m_nLifeAutoV = uParam;
		break;
	case GPI_LIFE_PERCENT:
			Player[CLIENT_PLAYER_INDEX].m_cAI.m_nLifeAutoP = uParam;
		break;
	case GPI_LIFE_TIME:
			Player[CLIENT_PLAYER_INDEX].m_cAI.m_uLifeTimeUse = uParam;
		break;
	case GPI_MANA_VALUE:
			Player[CLIENT_PLAYER_INDEX].m_cAI.m_nManaAutoV = uParam;
		break;
	case GPI_MANA_PERCENT:
			Player[CLIENT_PLAYER_INDEX].m_cAI.m_nManaAutoP = uParam;
		break;
	case GPI_MANA_TIME:
			Player[CLIENT_PLAYER_INDEX].m_cAI.m_uManaTimeUse = uParam;
		break;
	case GPI_TP_LIFE:
			Player[CLIENT_PLAYER_INDEX].m_cAI.m_bTPLife = uParam;
		break;
	case GPI_TP_LIFEV:
			Player[CLIENT_PLAYER_INDEX].m_cAI.m_nTPLifeV = uParam;
		break;
	case GPI_TP_MANA:
			Player[CLIENT_PLAYER_INDEX].m_cAI.m_bTPMana = uParam;
		break;
	case GPI_TP_MANAV:
			Player[CLIENT_PLAYER_INDEX].m_cAI.m_nTPManaV = uParam;
		break;
	case GPI_TP_NOT_MEDICINEBLOOD:
			Player[CLIENT_PLAYER_INDEX].m_cAI.m_bTPNotMedicineBlood = uParam;
		break;
	case GPI_TP_NOT_MEDICINEMANA:
			Player[CLIENT_PLAYER_INDEX].m_cAI.m_bTPNotMedicineMana = uParam;
		break;
	case GPI_TP_HIGHTMONEY:
			Player[CLIENT_PLAYER_INDEX].m_cAI.m_bTPHightMoney = uParam;
		break;
	case GPI_TP_HIGHTMONEYV:
			Player[CLIENT_PLAYER_INDEX].m_cAI.m_nTPHightMoneyV = uParam;
		break;
	case GPI_TP_DAMAGEEQUIP:
			Player[CLIENT_PLAYER_INDEX].m_cAI.m_bTPDamageEquip = uParam;
		break;
	case GPI_TP_DAMAGEEQUIPV:
			Player[CLIENT_PLAYER_INDEX].m_cAI.m_nTPDamageEquipV = uParam;
		break;
	case GPI_TP_NOT_EQUIPMENT:
			Player[CLIENT_PLAYER_INDEX].m_cAI.m_bTPNotEquipment = uParam;
		break;
	case GPI_TP_NOT_EQUIPMENTV:
			Player[CLIENT_PLAYER_INDEX].m_cAI.m_nTPNotEquipmentV = uParam;
		break;
	case GPI_ANTI_POISON:
			Player[CLIENT_PLAYER_INDEX].m_cAI.m_bAntiPoison = uParam;
		break;
	case GPI_ENCHASEEXP:
			Player[CLIENT_PLAYER_INDEX].m_cAI.m_bEnchaseExp = uParam;
		break;
	case GPI_LIFEREPLENISH:
			Player[CLIENT_PLAYER_INDEX].m_cAI.m_bLifeReplenish = uParam;
		break;
	case GPI_LIFEREPLENISHP:
			Player[CLIENT_PLAYER_INDEX].m_cAI.m_nLifeReplenishP = uParam;
		break;
	case GPI_INVENTORYMONEY:
			Player[CLIENT_PLAYER_INDEX].m_cAI.m_bInventoryMoney = uParam;
		break;
	case GPI_INVENTORYITEM:
			Player[CLIENT_PLAYER_INDEX].m_cAI.m_bInventoryItem = uParam;
		break;
	case GPI_RETURNPORTAL:
			Player[CLIENT_PLAYER_INDEX].m_cAI.m_bReturnPortal = uParam;
		break;
	case GPI_RETURNPORTALSEC:
			Player[CLIENT_PLAYER_INDEX].m_cAI.m_nReturnPortalSec = uParam;
		break;
	//Code rut tien + pass ruong
	case GPI_WITHDRAWA:
		Player[CLIENT_PLAYER_INDEX].m_cAI.m_bWithDrawa = uParam; // rut tien;
		break;
	case GPI_WITHDRAWA_MONEY:
		Player[CLIENT_PLAYER_INDEX].m_cAI.m_bWithDrawaMoney = uParam; // so tien;
		break;
	case GPI_PASSREPOSITORY:
		Player[CLIENT_PLAYER_INDEX].m_cAI.m_bPassReposion = (char*)uParam;
		break;
    case GPI_OTHER_SELL_TRASH:
        Player[CLIENT_PLAYER_INDEX].m_cAI.m_bSellTrash = uParam;
        break;
    case GPI_BAN_RAC:
    {
        Player[CLIENT_PLAYER_INDEX].m_cAI.AutoBanRac(uParam);
    }
	case GPI_OPEN_MEDICINE:
			Player[CLIENT_PLAYER_INDEX].m_cAI.m_bOpenMedicine = uParam;
		break;
	case GPI_AUTO_REPAIR:
			Player[CLIENT_PLAYER_INDEX].m_cAI.m_bRepairEquip = uParam;
		break;
	case GPI_AUTO_MOVEMPS:
			Player[CLIENT_PLAYER_INDEX].m_cAI.m_bMoveMps = uParam;
		break;
	case GPI_AUTO_MOVEMPSID:
		if (uParam >= 0 && uParam < MAX_AUTO_LIST)
		{
			if(uParam == 0)
				Player[CLIENT_PLAYER_INDEX].m_cAI.m_nMoveCount = 0;

			Player[CLIENT_PLAYER_INDEX].m_cAI.m_nMoveMps[uParam][0] = nParam;
			if(nParam)
				Player[CLIENT_PLAYER_INDEX].m_cAI.m_nMoveCount++;
		}
		break;
	case GPI_AUTO_MOVEMPSX:
		if (uParam >= 0 && uParam < MAX_AUTO_LIST)
			Player[CLIENT_PLAYER_INDEX].m_cAI.m_nMoveMps[uParam][1] = nParam;
		break;
	case GPI_AUTO_MOVEMPSY:
		if (uParam >= 0 && uParam < MAX_AUTO_LIST)
			Player[CLIENT_PLAYER_INDEX].m_cAI.m_nMoveMps[uParam][2] = nParam;
		break;
	}
	return nRet;
}

// TamLTM tim pos tren map mini
void KCoreShell::DirectFindPos(unsigned int uParam, int nParam, BOOL bSync, BOOL bPaintLine)
{
//	g_DebugLog("DirectFindPos %d + %d ", uParam, nParam);
	g_ScenePlace.DirectFindPos(uParam, nParam, bSync, bPaintLine);

	//TamLTM check lam auto run move mini map
	Player[CLIENT_PLAYER_INDEX].m_cAI.m_nMapX = uParam; // auto run;
	Player[CLIENT_PLAYER_INDEX].m_cAI.m_nMapY = nParam; // auto run;
	//end code
}

//TamLTM Check Hover mini map
void KCoreShell::CheckHoverMouseMiniMap(BOOL hoverM)
{
	g_ScenePlace.CheckHoverMouseMiniMap(hoverM);
}
//end code

//Nhap box toa do thi set true
extern void PhongThanResetMapRoute();
extern void PhongThanSetRouteArrival(int);
void KCoreShell::SetAutoRun(BOOL autoRun)
{
    PhongThanSetRouteArrival(0);
    PhongThanResetMapRoute();
	Player[CLIENT_PLAYER_INDEX].m_cAI.m_bAutoRunMap = autoRun; // auto run;
//	g_DebugLog("%d ", autoRun);
}

//Cam co toa do thi set true
void KCoreShell::SetFlagAutoRun(BOOL autoRunFlag, int x, int y)
{
    PhongThanSetRouteArrival(0);
    PhongThanResetMapRoute();
	Player[CLIENT_PLAYER_INDEX].m_cAI.m_bAutoRunFlagMap = autoRunFlag; // auto run;
	Player[CLIENT_PLAYER_INDEX].m_cAI.m_nFlagMapX = x; // auto run;
	Player[CLIENT_PLAYER_INDEX].m_cAI.m_nFlagMapY = y; // auto run;
//	g_DebugLog("autoRunFlag %d ", autoRunFlag);
}
//end code

BOOL KCoreShell::GetPaintMode()
{
	return g_ScenePlace.bPaintMode;
}

// TamLTM chay khi nhap box
void KCoreShell::SetPaintMode(BOOL bFlag)
{
	g_ScenePlace.DirectFindPos(0, 0, FALSE, FALSE);
	g_ScenePlace.bPaintMode = bFlag;
	//g_DebugLog("SetPaintMode");
}

BOOL KCoreShell::GetFlagMode()
{
	return g_ScenePlace.bFlagMode;
}

void KCoreShell::SetFlagMode(BOOL bFlag)
{
	//Set cam co map mini
	g_ScenePlace.bFlagMode = bFlag;
	Player[CLIENT_PLAYER_INDEX].m_cAI.m_bAutoRunMap = FALSE; // auto run;
	Player[CLIENT_PLAYER_INDEX].m_cAI.m_bAutoRunFlagMap = FALSE;
}

void KCoreShell::GetSizeItem(unsigned int uItemId, int *nW, int *nH)
{
	if(nW && nH)
	{
		*nW = Item[uItemId].GetWidth();
		*nH = Item[uItemId].GetHeight();
	}
}

int KCoreShell::GetExtPoint()
{
	return  Player[CLIENT_PLAYER_INDEX].GetExtPoint();
}

int KCoreShell::GetItemQuality(unsigned int uItemId, unsigned int uGenre /* = CGOG_ITEM*/)
{
    int nIndex = (int)uItemId;
    KItem* pItem = NULL;
    if (uGenre == CGOG_NPCSELLITEM) {
        int shop = Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nShopIdx[Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nCurShop];
        nIndex = BuySell.GetItemIndex(shop, uItemId);
        pItem = BuySell.GetItem(nIndex);
    } else if (uGenre == CGOG_ITEM || uGenre == CGOG_PLAYERSELLITEM || uGenre == CGOG_PLAYERVIEWITEM) {
        if (nIndex <= 0 || nIndex >= MAX_ITEM) return 0;
        pItem = &Item[nIndex];
    }
    if (!pItem) return 0;
    const int quality = pItem->GetPhongThanQuality();
    // WndObjContainer consumes border nature: 0 none, 1 set green,
    // 2 Kingsoft gold, 3 forged orange, 4 purple, 5 blue. All are static.
    if (quality == 4) return 1;
    if (quality == 2) return 2;
    if (quality == 3) return 3;
    if (quality == 5) return 4; // purple border
    if (quality == 1) return 5; // blue border
    return 0;
}

int KCoreShell::GetGenreItem(unsigned int uItemId, unsigned int uGenre /* = CGOG_ITEM*/)
{
	int nIndex = uItemId;
	KItem*	pItem = NULL;
	if (uGenre == CGOG_ITEM)
		return Item[nIndex].GetGenre();
	else if (uGenre == CGOG_NPCSELLITEM)
	{
		int	nBuyIdx = Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nShopIdx[Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nCurShop];
		if (nBuyIdx != -1)
		{
			nIndex = BuySell.GetItemIndex(nBuyIdx, uItemId);
			if (nIndex >= 0)
			{
				pItem = BuySell.GetItem(nIndex);
				return pItem->GetGenre();
			}
		}
	}
	return Item[nIndex].GetGenre();
}

//Tinh Damage lam hu do ben`
int KCoreShell::IsDamage(unsigned int uItemId)
{
    if(uItemId<=0||uItemId>=MAX_ITEM||Item[uItemId].GetGenre()!=item_equip)return FALSE;
    return Item[uItemId].GetMaxDurability()>0&&Item[uItemId].GetDurability()>=0&&Item[uItemId].CanBeRepaired();
}

BOOL KCoreShell::GetLockState()
{
	return Player[CLIENT_PLAYER_INDEX].GetLockState();
}

int KCoreShell::GetStackNum(unsigned int uItemId)
{
    if(uItemId<=0 || uItemId>=MAX_ITEM)return 0;
    return Item[uItemId].IsReservoir() ? 1 : Item[uItemId].GetStackNum();
}

BOOL KCoreShell::GetSkillData(int nSkillId, int *nLevel)
{
	int nList = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.FindSame(nSkillId);
	if (nList > 0)
	{
		if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetLevel(nSkillId) > 0)
		{
			*nLevel = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetCurrentLevel(nSkillId);
			if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetAddLevelIndex(nList) > 0)
				return TRUE;
			else
				return FALSE;
		}
	}
	return FALSE;
}

void KCoreShell::GetSkillName(int nSkillId, char* szSkillName)
{
	if (nSkillId)
	{
		ISkill * pISkill = g_SkillManager.GetSkill(nSkillId, 1);
		strcpy(szSkillName, pISkill->GetSkillName());
	}
}

int KCoreShell::GetTradePrice(unsigned int uItemId)
{
	return Item[uItemId].GetTradePrice();
}

int KCoreShell::GetTradeState(int nIndex, int uId)
{
	if (nIndex || uId)
	{
		if (nIndex)
			return Npc[nIndex].m_PTrade.nTrade;
		else
		{
			int nIndex = NpcSet.SearchID(uId);
			return Npc[nIndex].m_PTrade.nTrade;
		}
	}
	else
		return Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_PTrade.nTrade;

}

int KCoreShell::GetFightState()
{
	KNpc*	pNpc = &Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex];
	return pNpc->m_FightMode;
}

void KCoreShell::BreakItem(unsigned int uParam, int nParam, BOOL bIsBreakAll)
{
    if(!uParam || nParam<0)return;
    const KUiObjAtContRegion& object=*(const KUiObjAtContRegion*)uParam;
    const int index=object.Obj.uId;
    if(object.Obj.uGenre!=CGOG_ITEM || index<=0 || index>=MAX_ITEM ||
        Player[CLIENT_PLAYER_INDEX].m_ItemList.FindSame(index)<=0 || !Item[index].IsStack())return;
    const int total=Item[index].GetStackNum();
    if(!nParam){if(bIsBreakAll)return;nParam=total/2;}
    if(nParam>0 && nParam<total)SendClientCmdBreak(Item[index].GetID(),nParam,bIsBreakAll);
}

DWORD KCoreShell::GetdwIDPlayer(unsigned int uId)
{
	int nIndex = 0;
	if (uId)
		nIndex = NpcSet.SearchID(uId);
	else
		nIndex = Player[CLIENT_PLAYER_INDEX].m_nIndex;

	if (nIndex)
		return Npc[nIndex].m_dwID;

	return 0;
}

int KCoreShell::GetOwnValue(int nMoneyUnit)
{
    if(nMoneyUnit==PT_F2_MONEY_SLOT1||nMoneyUnit==PT_F2_MONEY_SLOT2)return PhongThanF2CurrencyCount(CLIENT_PLAYER_INDEX,nMoneyUnit-PT_F2_MONEY_SLOT1+1);
	int nRet = 0;
	switch (nMoneyUnit)
	{
		case moneyunit_money:
			nRet = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetMoney(room_equipment);
			break;
        case moneyunit_ptxu:
        { extern int PhongThanXuCountForShop(int); nRet=PhongThanXuCountForShop(CLIENT_PLAYER_INDEX); break; }
		case moneyunit_extpoint:
			nRet = Player[CLIENT_PLAYER_INDEX].GetExtPoint();
			break;
		case moneyunit_fuyuan:
			nRet = Player[CLIENT_PLAYER_INDEX].m_cTask.GetSaveVal(TASKVALUE_STATTASK_FUYUAN);
			break;
		case moneyunit_repute:
			nRet = Player[CLIENT_PLAYER_INDEX].m_cTask.GetSaveVal(TASKVALUE_STATTASK_REPUTE);
			break;
		case moneyunit_accum:
			nRet = Player[CLIENT_PLAYER_INDEX].m_cTask.GetSaveVal(TASKVALUE_STATTASK_ACCUM);
			break;
		case moneyunit_honor:
			nRet = Player[CLIENT_PLAYER_INDEX].m_cTask.GetSaveVal(TASKVALUE_STATTASK_HONOR);
			break;
		case moneyunit_respect:
			nRet = Player[CLIENT_PLAYER_INDEX].m_cTask.GetSaveVal(TASKVALUE_STATTASK_RESPECT);
			break;
		default:
			break;
	}
	return nRet;
}

int KCoreShell::GetDataSuperShop(int nSaleId, unsigned int uParam, int nParam)
{
	int nRet = 0;
	int	nBuyIdx = Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nShopIdx[nSaleId];
	if (nBuyIdx == -1)
		return 0;
	if (nBuyIdx >= BuySell.GetHeight())
		return 0;
	Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nCurShop = nSaleId;
	int nIndex = 0;
	int nCount = 0;
	int nItemsPerPage = PHONGTHAN_IBSHOP_ITEMS_PER_PAGE;
	if (uParam)
	{
		KUiObjAtContRegion* pInfo = (KUiObjAtContRegion *)uParam;
		for (int i = 0; i < BuySell.GetWidth(); i++)
		{
			nIndex = BuySell.GetItemIndex(nBuyIdx, i);
			KItem* pItem = BuySell.GetItem(nIndex);

			if (nIndex >= 0 && pItem)
			{
				pInfo->Obj.uGenre = CGOG_NPCSELLITEM;
				pInfo->Obj.uId = i;
				pInfo->Region.h = 0;
				pInfo->Region.v = 0;
				pInfo->Region.Width = pItem->GetWidth();
				pInfo->Region.Height = pItem->GetHeight();
				pInfo->nContainer = nCount / nItemsPerPage;
				nCount++;
				pInfo++;
			}
		}
		nRet = nCount > 0 ? (nCount - 1) / nItemsPerPage : 0;
	}
	else
	{
		for (int i = 0; i < BuySell.GetWidth(); i++)
		{
			nIndex = BuySell.GetItemIndex(nBuyIdx, i);
			KItem* pItem = BuySell.GetItem(nIndex);

			if (nIndex >= 0 && pItem)
			{
				nCount++;
			}
		}
		nRet = nCount;
	}
	return nRet;
}


int KCoreShell::GetDataDynamicShop(int nSaleId, unsigned int uParam, int nParam)
{
	int nRet = 0;
	int	nBuyIdx = Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nShopIdx[nSaleId];
	if (nBuyIdx == -1)
		return 0;
	if (nBuyIdx >= BuySell.GetHeight())
		return 0;
	Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nCurShop = nSaleId;
	int nIndex = 0;
	int nCount = 0;
	if (uParam)
	{
		int nPage = 0;
		KUiObjAtContRegion* pInfo = (KUiObjAtContRegion *)uParam;
		if (!BuySell.m_pSShopRoom)
			return 0;
		BuySell.m_pSShopRoom->Clear();
		for (int i = 0; i < BuySell.GetWidth(); i++)
		{
			nIndex = BuySell.GetItemIndex(nBuyIdx, i);
			KItem* pItem = BuySell.GetItem(nIndex);

			if (nIndex >= 0 && pItem)
			{
				pInfo->Obj.uGenre = CGOG_NPCSELLITEM;
				pInfo->Obj.uId = i;
				POINT	Pos;
				if (BuySell.m_pSShopRoom->FindRoom(pItem->GetWidth(), pItem->GetHeight(), &Pos))
				{
					BuySell.m_pSShopRoom->PlaceItem(Pos.x, Pos.y, nIndex + 1, pItem->GetWidth(), pItem->GetHeight());
				}
				else
				{
					nPage++;
					BuySell.m_pSShopRoom->Clear();
					BuySell.m_pSShopRoom->FindRoom(pItem->GetWidth(), pItem->GetHeight(), &Pos);
					BuySell.m_pSShopRoom->PlaceItem(Pos.x, Pos.y, nIndex + 1, pItem->GetWidth(), pItem->GetHeight());
				}
				pInfo->Region.h = Pos.x;
				pInfo->Region.v = Pos.y;
				pInfo->Region.Width = pItem->GetWidth();
				pInfo->Region.Height = pItem->GetHeight();
				pInfo->nContainer = nPage;
				nCount++;
				pInfo++;
			}
		}
		nRet = nPage;
	}
	else
	{
		for (int i = 0; i < BuySell.GetWidth(); i++)
		{
			nIndex = BuySell.GetItemIndex(nBuyIdx, i);
			KItem* pItem = BuySell.GetItem(nIndex);

			if (nIndex >= 0 && pItem)
			{
				nCount++;
			}
		}
		nRet = nCount;
	}
	return nRet;
}

int KCoreShell::GetNextSkill(int nType, int nIndex)
{
	if(nType == 0)
		return Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetNextSkillState(nIndex);
	else if(nType == 1)
		return Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetNextSkillFight(nIndex);
	else if(nType == 2)
		return Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetNextSkillAura(nIndex);

	return 0;
}

/*BOOL KCoreShell::IsSkillAura(int nSkillId)
{
	if(nSkillId > 0)
	{
		KSkill * pOrdinSkill = (KSkill *)g_SkillManager.GetSkill(nSkillId, 1);
		if (!pOrdinSkill)
			return FALSE;

		return pOrdinSkill->IsAura();
	}
	return FALSE;
} */

//TamLTM kham nam xanh
int KCoreShell::GetDetailItem(unsigned int uId )
{
	return Item[uId].GetDetailType();
}

int KCoreShell::GetLevelItem(unsigned int uId )
{
	return Item[uId].GetLevel();
}

int KCoreShell::GetSeriesItem(unsigned int uId )
{
	return Item[uId].GetSeries();
}

int KCoreShell::GetItemColor(unsigned int uItemId)
{
	return Item[uItemId].GetColorItem();
}

int KCoreShell::GetParticularItem(unsigned int uId )
{
	return Item[uId].GetParticular();
}

//end code

#ifndef _SERVER
#include "PhongThanItemLinkClient.inl"
#endif
