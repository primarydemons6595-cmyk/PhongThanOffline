// Embedded session gateway + server-side service channels for offline mode.
// CPtGatewayEmbed does Bishop's client-facing job in-process; CPtStoreClient
// does the slice of Goddess that GameServer talks to (save/lock), answered
// by the local character store; CPtGatewayLink is the pipe between
// KSOServer and the gateway. See PtEmbedded.h.
#include "KWin32.h"
#include "PtEmbedded.h"

#include "../../../Headers/PhongThanProtocol.h"
#include "../../../Headers/PhongThanCharacter.h"
#include "../../../Headers/PhongThanStarterSpawn.h"
#include "../../../Headers/PhongThanStarterWeapon.h"
#include "../../MultiServer/Goddess/PhongThanCharacterStore.h"

#include <objbase.h>
#include <stdio.h>

// ===========================================================================
// CPtStoreClient - KSOServer's "Goddess" channel
// ===========================================================================
CPtStoreClient::CPtStoreClient()
	: m_nRef(1), m_pfnEvent(NULL), m_lpParam(NULL)
{
}

CPtStoreClient::~CPtStoreClient()
{
}

STDMETHODIMP CPtStoreClient::QueryInterface(REFIID riid, void** ppv)
{
	if (!ppv)
		return E_POINTER;
	*ppv = NULL;
	if (riid == IID_IUnknown || riid == IID_IESClient)
	{
		*ppv = static_cast<IClient*>(this);
		AddRef();
		return S_OK;
	}
	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) CPtStoreClient::AddRef() { return ++m_nRef; }

STDMETHODIMP_(ULONG) CPtStoreClient::Release()
{
	LONG n = --m_nRef;
	if (n <= 0)
		delete this;
	return n;
}

STDMETHODIMP CPtStoreClient::Startup() { return S_OK; }
STDMETHODIMP CPtStoreClient::Cleanup() { return S_OK; }

STDMETHODIMP CPtStoreClient::ConnectTo(const char* const& /*pAddress*/,
	unsigned short /*usPort*/)
{
	if (m_pfnEvent)
		m_pfnEvent(m_lpParam, enumServerConnectCreate);
	return S_OK;
}

STDMETHODIMP CPtStoreClient::RegisterMsgFilter(LPVOID lpParam,
	CALLBACK_CLIENT_EVENT pfnEventNotify)
{
	m_lpParam = lpParam;
	m_pfnEvent = pfnEventNotify;
	return S_OK;
}

void CPtStoreClient::HandleSave(const void* pData, size_t nSize)
{
	// Mirrors CClientNode::_SaveRoleInfo in Goddess, minus the lock table:
	// in one process the only writer is this server, so a save is never
	// blocked by another lock holder.
	if (nSize < sizeof(PHONGTHAN_SERVICE_CHARACTER_SAVE_REQUEST_HEADER))
		return;
	const PHONGTHAN_SERVICE_CHARACTER_SAVE_REQUEST_HEADER* pRequest =
		(const PHONGTHAN_SERVICE_CHARACTER_SAVE_REQUEST_HEADER*)pData;
	if (!PhongThanValidateWireHeader(&pRequest->Header, (PHONGTHAN_U32)nSize) ||
		pRequest->Header.MessageType != PHONGTHAN_MSG_SERVICE_CHARACTER_SAVE ||
		pRequest->Header.Flags != PHONGTHAN_WIRE_FLAG_REQUEST ||
		pRequest->Header.PacketSize != nSize ||
		pRequest->StateSize != nSize - sizeof(*pRequest) ||
		pRequest->StateSize < sizeof(PHONGTHAN_CHARACTER_STATE_HEADER))
		return;

	const PHONGTHAN_CHARACTER_STATE_HEADER* pState =
		(const PHONGTHAN_CHARACTER_STATE_HEADER*)
			((const BYTE*)pData + sizeof(*pRequest));
	if (!PhongThanValidateCharacterState(pState, pRequest->StateSize))
		return;

	PHONGTHAN_SERVICE_CHARACTER_SAVE_RESPONSE Response;
	ZeroMemory(&Response, sizeof(Response));
	PhongThanInitializeWireHeader(&Response.Header,
		PHONGTHAN_MSG_SERVICE_CHARACTER_SAVE, sizeof(Response),
		PHONGTHAN_WIRE_FLAG_RESPONSE, pRequest->Header.Sequence);
	Response.RequestId = pRequest->RequestId;
	Response.Result = g_PhongThanCharacterStore.Save(pState, pRequest->StateSize) ?
		PHONGTHAN_CHARACTER_OPERATION_SUCCESS :
		PHONGTHAN_CHARACTER_OPERATION_SERVICE_UNAVAILABLE;
	strncpy((char*)Response.RoleName, (const char*)pState->RoleName,
		sizeof(Response.RoleName) - 1);
	m_Inbound.Push(&Response, sizeof(Response));
}

STDMETHODIMP CPtStoreClient::SendPackToServer(const void* const pData,
	const size_t& datalength)
{
	if (!pData || datalength == 0)
		return E_FAIL;
	if (PhongThanIsWirePacket(pData, (PHONGTHAN_U32)datalength))
	{
		const PHONGTHAN_WIRE_HEADER* pHeader =
			(const PHONGTHAN_WIRE_HEADER*)pData;
		if (pHeader->MessageType == PHONGTHAN_MSG_SERVICE_CHARACTER_SAVE)
			HandleSave(pData, datalength);
		// CHARACTER_LOCK and everything else: accepted, no response -
		// exactly what KSOServer expects (fire and forget).
	}
	// Non-wire legacy packets (the startup gamestatistic) are swallowed.
	return S_OK;
}

STDMETHODIMP_(const void*) CPtStoreClient::GetPackFromServer(size_t& datalength)
{
	if (!m_Inbound.Pop(m_Current))
	{
		datalength = 0;
		return NULL;
	}
	datalength = m_Current.size();
	return m_Current.empty() ? (const void*)"" : (const void*)&m_Current[0];
}

STDMETHODIMP CPtStoreClient::Shutdown() { return S_OK; }

// ===========================================================================
// CPtGatewayLink - KSOServer's "Bishop" channel (singleton)
// ===========================================================================
static CPtGatewayLink g_PtGatewayLink;

CPtGatewayLink* PtEmbedded_GetGatewayLink()
{
	return &g_PtGatewayLink;
}

CPtGatewayLink::CPtGatewayLink()
	: m_nRef(1), m_pfnEvent(NULL), m_lpParam(NULL)
{
}

CPtGatewayLink::~CPtGatewayLink()
{
}

STDMETHODIMP CPtGatewayLink::QueryInterface(REFIID riid, void** ppv)
{
	if (!ppv)
		return E_POINTER;
	*ppv = NULL;
	if (riid == IID_IUnknown || riid == IID_IESClient)
	{
		*ppv = static_cast<IClient*>(this);
		AddRef();
		return S_OK;
	}
	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) CPtGatewayLink::AddRef() { return ++m_nRef; }

STDMETHODIMP_(ULONG) CPtGatewayLink::Release()
{
	// Singleton: the gateway keeps the raw pointer for the whole process
	// lifetime, so the refcount never destroys the object.
	if (m_nRef > 1)
		--m_nRef;
	return m_nRef;
}

STDMETHODIMP CPtGatewayLink::Startup() { return S_OK; }
STDMETHODIMP CPtGatewayLink::Cleanup() { return S_OK; }

STDMETHODIMP CPtGatewayLink::ConnectTo(const char* const& /*pAddress*/,
	unsigned short /*usPort*/)
{
	if (m_pfnEvent)
		m_pfnEvent(m_lpParam, enumServerConnectCreate);
	return S_OK;
}

STDMETHODIMP CPtGatewayLink::RegisterMsgFilter(LPVOID lpParam,
	CALLBACK_CLIENT_EVENT pfnEventNotify)
{
	m_lpParam = lpParam;
	m_pfnEvent = pfnEventNotify;
	return S_OK;
}

STDMETHODIMP CPtGatewayLink::SendPackToServer(const void* const pData,
	const size_t& datalength)
{
	// Called BY KSOServer: permits and world-session events for the gateway.
	if (!pData || datalength == 0)
		return E_FAIL;
	m_FromServer.Push(pData, datalength);
	return S_OK;
}

STDMETHODIMP_(const void*) CPtGatewayLink::GetPackFromServer(size_t& datalength)
{
	// Called BY KSOServer's MessageLoop: packets the gateway injected.
	if (!m_ToServer.Pop(m_Current))
	{
		datalength = 0;
		return NULL;
	}
	datalength = m_Current.size();
	return m_Current.empty() ? (const void*)"" : (const void*)&m_Current[0];
}

STDMETHODIMP CPtGatewayLink::Shutdown() { return S_OK; }

// ===========================================================================
// CPtGatewayEmbed
// ===========================================================================
static CPtGatewayEmbed g_PtGatewayEmbed;

CPtGatewayEmbed* PtEmbedded_GetGateway()
{
	return &g_PtGatewayEmbed;
}

CPtGatewayEmbed::CPtGatewayEmbed()
	: m_pClient(NULL), m_bHasPendingAttach(false),
	  m_nAttachSequence(0), m_nTicketCounter(0)
{
	ZeroMemory(m_PendingTicket, sizeof(m_PendingTicket));
	ZeroMemory(m_szPendingRole, sizeof(m_szPendingRole));
}

void CPtGatewayEmbed::Attach(CPtVirtualClient* pClient)
{
	m_pClient = pClient;
	m_sAccount.clear();
	m_bHasPendingAttach = false;
	m_InPackets.Clear();
}

void CPtGatewayEmbed::Detach(CPtVirtualClient* pClient)
{
	if (m_pClient == pClient)
	{
		m_pClient = NULL;
		m_sAccount.clear();
		m_bHasPendingAttach = false;
		m_InPackets.Clear();
	}
}

void CPtGatewayEmbed::ReceiveFromClient(const void* pData, size_t nSize)
{
	m_InPackets.Push(pData, nSize);
}

void CPtGatewayEmbed::SendToClient(const void* pData, size_t nSize)
{
	if (m_pClient)
		m_pClient->Deliver(pData, nSize);
}

void CPtGatewayEmbed::PumpIn()
{
	std::vector<unsigned char> packet;
	while (m_InPackets.Pop(packet))
	{
		if (!packet.empty())
			HandlePacket(&packet[0], packet.size());
	}
}

void CPtGatewayEmbed::PumpOut()
{
	std::vector<unsigned char> packet;
	CPtGatewayLink* pLink = PtEmbedded_GetGatewayLink();
	while (pLink->PollFromServer(packet))
	{
		if (packet.size() < sizeof(PHONGTHAN_WIRE_HEADER))
			continue;
		const PHONGTHAN_WIRE_HEADER* pHeader =
			(const PHONGTHAN_WIRE_HEADER*)&packet[0];
		if (pHeader->MessageType != PHONGTHAN_MSG_SESSION_ENTER_WORLD ||
			packet.size() != sizeof(PHONGTHAN_SESSION_ENTER_WORLD_RESPONSE))
			continue; // world-session events etc. need no offline bookkeeping
		const PHONGTHAN_SESSION_ENTER_WORLD_RESPONSE* pPermit =
			(const PHONGTHAN_SESSION_ENTER_WORLD_RESPONSE*)&packet[0];
		if (!m_bHasPendingAttach)
			continue;
		if (memcmp(pPermit->SessionTicket, m_PendingTicket,
				PHONGTHAN_SESSION_TICKET_SIZE) != 0)
			continue;

		PHONGTHAN_SESSION_ENTER_WORLD_RESPONSE Response;
		ZeroMemory(&Response, sizeof(Response));
		PhongThanInitializeWireHeader(&Response.Header,
			PHONGTHAN_MSG_SESSION_ENTER_WORLD, sizeof(Response),
			PHONGTHAN_WIRE_FLAG_RESPONSE, 0);
		memcpy(Response.SessionTicket, m_PendingTicket,
			PHONGTHAN_SESSION_TICKET_SIZE);
		// 127.0.0.1 in the wire's a|(b<<8)|(c<<16)|(d<<24) layout; the
		// virtual world client ignores the address anyway.
		Response.ServerAddressV4 = 0x0100007F;
		Response.ServerPort = 6666;
		Response.Permit = pPermit->Permit;
		strncpy((char*)Response.AccountName, m_sAccount.c_str(),
			sizeof(Response.AccountName) - 1);
		strncpy((char*)Response.RoleName, m_szPendingRole,
			sizeof(Response.RoleName) - 1);
		m_bHasPendingAttach = false;
		SendToClient(&Response, sizeof(Response));
	}
}

void CPtGatewayEmbed::SendCharacterList()
{
	PHONGTHAN_SESSION_CHARACTER_LIST_RESPONSE Response;
	ZeroMemory(&Response, sizeof(Response));
	PhongThanInitializeWireHeader(&Response.Header,
		PHONGTHAN_MSG_SESSION_CHARACTER_LIST, sizeof(Response),
		PHONGTHAN_WIRE_FLAG_RESPONSE, 0);
	Response.Result = PHONGTHAN_CHARACTER_LIST_SUCCESS;
	const int nCount = g_PhongThanCharacterStore.List(m_sAccount.c_str(),
		Response.Characters, PHONGTHAN_CHARACTER_LIMIT);
	Response.CharacterCount = (PHONGTHAN_U8)(nCount > 0 ? nCount : 0);
	Response.RecommendedIndex = 0;
	SendToClient(&Response, sizeof(Response));
}

void CPtGatewayEmbed::MakeTicket(unsigned char* pTicketOut)
{
	static bool bSeeded = false;
	if (!bSeeded)
	{
		srand(GetTickCount() ^ GetCurrentProcessId());
		bSeeded = true;
	}
	++m_nTicketCounter;
	unsigned int nWords[4];
	nWords[0] = ((unsigned int)rand() << 16) ^ (unsigned int)rand();
	nWords[1] = GetTickCount();
	nWords[2] = m_nTicketCounter * 2654435761u;
	nWords[3] = ((unsigned int)rand() << 16) ^ GetCurrentProcessId();
	memcpy(pTicketOut, nWords, PHONGTHAN_SESSION_TICKET_SIZE);
}

void CPtGatewayEmbed::StartAttach(const char* pRoleName)
{
	BYTE stateBuffer[PHONGTHAN_CHARACTER_MAX_STATE_SIZE];
	PHONGTHAN_U32 nStateSize = 0;
	if (!g_PhongThanCharacterStore.Load(pRoleName, stateBuffer,
			sizeof(stateBuffer), &nStateSize))
	{
		// Mirror a refused enter-world so the client falls back cleanly.
		PHONGTHAN_SESSION_ENTER_WORLD_RESPONSE Response;
		ZeroMemory(&Response, sizeof(Response));
		PhongThanInitializeWireHeader(&Response.Header,
			PHONGTHAN_MSG_SESSION_ENTER_WORLD, sizeof(Response),
			PHONGTHAN_WIRE_FLAG_RESPONSE, 0);
		strncpy((char*)Response.RoleName, pRoleName,
			sizeof(Response.RoleName) - 1);
		strncpy((char*)Response.AccountName, m_sAccount.c_str(),
			sizeof(Response.AccountName) - 1);
		SendToClient(&Response, sizeof(Response));
		return;
	}
	const PHONGTHAN_CHARACTER_STATE_HEADER* pState =
		(const PHONGTHAN_CHARACTER_STATE_HEADER*)stateBuffer;
	if (!PhongThanValidateCharacterState(pState, nStateSize) ||
		m_sAccount.compare((const char*)pState->AccountName) != 0)
		return;

	MakeTicket(m_PendingTicket);
	strncpy(m_szPendingRole, pRoleName, sizeof(m_szPendingRole) - 1);
	m_szPendingRole[sizeof(m_szPendingRole) - 1] = 0;
	m_bHasPendingAttach = true;

	const PHONGTHAN_U32 nPacketSize =
		(PHONGTHAN_U32)sizeof(PHONGTHAN_WORLD_ATTACH_CHARACTER_HEADER) +
		nStateSize;
	std::vector<unsigned char> packet(nPacketSize);
	PHONGTHAN_WORLD_ATTACH_CHARACTER_HEADER* pAttach =
		(PHONGTHAN_WORLD_ATTACH_CHARACTER_HEADER*)&packet[0];
	ZeroMemory(pAttach, sizeof(*pAttach));
	PhongThanInitializeWireHeader(&pAttach->Header,
		PHONGTHAN_MSG_WORLD_ATTACH_CHARACTER, nPacketSize,
		PHONGTHAN_WIRE_FLAG_REQUEST, ++m_nAttachSequence);
	memcpy(pAttach->SessionTicket, m_PendingTicket,
		PHONGTHAN_SESSION_TICKET_SIZE);
	pAttach->ExtensionPoint = 0;
	pAttach->ChangedExtensionPoint = 0;
	pAttach->StateSize = nStateSize;
	memcpy(&packet[sizeof(*pAttach)], stateBuffer, nStateSize);
	PtEmbedded_GetGatewayLink()->InjectToServer(&packet[0], nPacketSize);
}

// Reads one starter template (settings\npc\player\newplayeriniNN.ini from
// the ServerData tree) with plain profile APIs and fills the service-level
// create request, mirroring CPlayerCreator + CGamePlayer::
// _CreateNewPlayer_ToDBRole in Bishop.
bool CPtGatewayEmbed::BuildCreateRequest(const void* pSessionRequest,
	void* pServiceRequestOut)
{
	const PHONGTHAN_SESSION_CREATE_CHARACTER_REQUEST* pCreate =
		(const PHONGTHAN_SESSION_CREATE_CHARACTER_REQUEST*)pSessionRequest;
	PHONGTHAN_SERVICE_CHARACTER_CREATE_REQUEST* pRequest =
		(PHONGTHAN_SERVICE_CHARACTER_CREATE_REQUEST*)pServiceRequestOut;
	ZeroMemory(pRequest, sizeof(*pRequest));

	char szIniPath[MAX_PATH];
	_snprintf(szIniPath, sizeof(szIniPath) - 1,
		"%s\\settings\\npc\\player\\newplayerini%02d.ini",
		PtEmbedded_GetServerDataDir(),
		(int)(pCreate->Profession * PHONGTHAN_GENDER_COUNT + pCreate->Gender));
	szIniPath[sizeof(szIniPath) - 1] = 0;

	struct IniReader
	{
		const char* path;
		int Role(const char* key, int def) const
			{ return GetPrivateProfileIntA("ROLE", key, def, path); }
		int Skill(const char* key, int def) const
			{ return GetPrivateProfileIntA("FSKILLS", key, def, path); }
	};
	IniReader ini;
	ini.path = szIniPath;
	if (GetFileAttributesA(szIniPath) == 0xFFFFFFFF)
		return false;

	PhongThanInitializeWireHeader(&pRequest->Header,
		PHONGTHAN_MSG_SERVICE_CHARACTER_CREATE, sizeof(*pRequest),
		PHONGTHAN_WIRE_FLAG_REQUEST, 0);
	strncpy((char*)pRequest->AccountName, m_sAccount.c_str(),
		sizeof(pRequest->AccountName) - 1);
	strncpy((char*)pRequest->RoleName, (const char*)pCreate->RoleName,
		sizeof(pRequest->RoleName) - 1);
	pRequest->Gender = pCreate->Gender;
	pRequest->Profession = pCreate->Profession;
	pRequest->NativePlaceId = pCreate->NativePlaceId;
	pRequest->Portrait = pCreate->Portrait;

	int nSpawnMap = 0, nSpawnX = 0, nSpawnY = 0;
	PhongThanStarterSpawn(pCreate->Profession, nSpawnMap, nSpawnX, nSpawnY);
	pRequest->SpawnMapId = nSpawnMap;
	pRequest->SpawnX = nSpawnX;
	pRequest->SpawnY = nSpawnY;

	pRequest->FightLevel = (PHONGTHAN_U16)ini.Role("ifightlevel", 0);
	pRequest->FightExperience = (PHONGTHAN_U32)ini.Role("ifightexp", 0);
	pRequest->Money = ini.Role("imoney", 0);
	pRequest->BankMoney = ini.Role("isavemoney", 0);
	pRequest->Power = ini.Role("ipower", 0);
	pRequest->Agility = ini.Role("iagility", 0);
	pRequest->Physique = ini.Role("iouter", 0);
	pRequest->Wisdom = ini.Role("iinside", 0);
	pRequest->Luck = ini.Role("iluck", 0);
	pRequest->MaxLife = ini.Role("imaxlife", 0);
	pRequest->MaxStamina = ini.Role("imaxstamina", 0);
	pRequest->MaxMana = ini.Role("imaxinner", 0);
	pRequest->RemainingAttributePoints = ini.Role("ileftprop", 0);
	pRequest->RemainingSkillPoints = ini.Role("ileftfight", 0);
	pRequest->LeadershipLevel = (PHONGTHAN_U16)ini.Role("ileadlevel", 0);
	pRequest->LeadershipExperience = (PHONGTHAN_U32)ini.Role("ileadexp", 0);

	int nSkillCount = ini.Skill("COUNT", 0);
	if (nSkillCount < 0)
		nSkillCount = 0;
	if (nSkillCount > PHONGTHAN_STARTER_SKILL_LIMIT)
		nSkillCount = PHONGTHAN_STARTER_SKILL_LIMIT;
	pRequest->SkillCount = (PHONGTHAN_U16)nSkillCount;
	for (int i = 0; i < nSkillCount; ++i)
	{
		char szId[32], szLevel[32];
		sprintf(szId, "S%d", i + 1);
		sprintf(szLevel, "L%d", i + 1);
		pRequest->Skills[i].SkillId = ini.Skill(szId, 0);
		pRequest->Skills[i].SkillLevel = ini.Skill(szLevel, 0);
		pRequest->Skills[i].SkillValue = 0;
	}
	return true;
}

// Mirrors CClientNode::_CreateRole in Goddess (state build + store Create),
// without the network response. Returns the PHONGTHAN_CHARACTER_OPERATION_*
// result code.
int CPtGatewayEmbed::CreateCharacter(const void* pServiceRequest)
{
	const PHONGTHAN_SERVICE_CHARACTER_CREATE_REQUEST* pRequest =
		(const PHONGTHAN_SERVICE_CHARACTER_CREATE_REQUEST*)pServiceRequest;

	// Offline name policy: the cluster build runs the role-name filter in
	// Goddess; here a plain sanity check takes its place.
	const char* pName = (const char*)pRequest->RoleName;
	bool bNameOk = pName[0] != 0;
	for (const unsigned char* p = (const unsigned char*)pName;
		bNameOk && *p; ++p)
	{
		if (*p < 0x20 || *p == '\\' || *p == '/' || *p == ':')
			bNameOk = false;
	}
	if (!bNameOk)
		return PHONGTHAN_CHARACTER_OPERATION_INVALID_NAME;

	PHONGTHAN_CHARACTER_STATE_HEADER SizeProbe;
	ZeroMemory(&SizeProbe, sizeof(SizeProbe));
	SizeProbe.FightSkillCount = pRequest->SkillCount;
	SizeProbe.ItemCount = 1;
	const PHONGTHAN_U32 nStateSize =
		PhongThanCharacterExpectedStateSize(&SizeProbe);
	BYTE* pStateBuffer = new BYTE[nStateSize];
	ZeroMemory(pStateBuffer, nStateSize);
	PHONGTHAN_CHARACTER_STATE_HEADER* pState =
		(PHONGTHAN_CHARACTER_STATE_HEADER*)pStateBuffer;
	pState->SchemaVersion = PHONGTHAN_CHARACTER_SCHEMA_VERSION;
	pState->StateSize = nStateSize;
	pState->Revision = 1;
	strncpy((char*)pState->AccountName, (const char*)pRequest->AccountName,
		sizeof(pState->AccountName) - 1);
	strncpy((char*)pState->RoleName, (const char*)pRequest->RoleName,
		sizeof(pState->RoleName) - 1);
	pState->Gender = pRequest->Gender;
	pState->HelmResource = pRequest->Portrait;
	pState->Profession = pRequest->Profession;
	pState->UseRevive = 1;
	int starterMap = 0, starterX = 0, starterY = 0;
	PhongThanStarterSpawn(pState->Profession, starterMap, starterX, starterY);
	pState->ReviveMapId = starterMap;
	pState->ReviveX = starterX;
	pState->ReviveY = starterY;
	pState->EnterMapId = starterMap;
	pState->EnterX = starterX;
	pState->EnterY = starterY;
	pState->FightLevel = pRequest->FightLevel;
	pState->FightExperience = pRequest->FightExperience;
	pState->Money = pRequest->Money;
	pState->BankMoney = pRequest->BankMoney;
	pState->Power = pRequest->Power;
	pState->Agility = pRequest->Agility;
	pState->Physique = pRequest->Physique;
	pState->Wisdom = pRequest->Wisdom;
	pState->Luck = pRequest->Luck;
	pState->MaxLife = pRequest->MaxLife;
	pState->MaxStamina = pRequest->MaxStamina;
	pState->MaxMana = pRequest->MaxMana;
	pState->CurrentLife = pRequest->MaxLife;
	pState->CurrentStamina = pRequest->MaxStamina;
	pState->CurrentMana = pRequest->MaxMana;
	pState->RemainingAttributePoints = pRequest->RemainingAttributePoints;
	pState->RemainingSkillPoints = pRequest->RemainingSkillPoints;
	pState->LeadershipLevel = pRequest->LeadershipLevel;
	pState->LeadershipExperience = pRequest->LeadershipExperience;
	pState->FightSkillCount = pRequest->SkillCount;
	pState->ItemCount = 1;
	PhongThanInitializeStarterWeapon(*PhongThanCharacterItems(pState),
		pState->Profession);
	PHONGTHAN_CHARACTER_SKILL_RECORD* pSkills =
		(PHONGTHAN_CHARACTER_SKILL_RECORD*)(pState + 1);
	for (PHONGTHAN_U32 i = 0; i < pState->FightSkillCount; ++i)
	{
		pSkills[i].SkillId = pRequest->Skills[i].SkillId;
		pSkills[i].Level = pRequest->Skills[i].SkillLevel;
		pSkills[i].Value = pRequest->Skills[i].SkillValue;
	}
	const bool bCreated = PhongThanValidateCharacterState(pState, nStateSize) &&
		g_PhongThanCharacterStore.Create(pState, nStateSize);
	delete [] pStateBuffer;
	return bCreated ? PHONGTHAN_CHARACTER_OPERATION_SUCCESS :
		PHONGTHAN_CHARACTER_OPERATION_NAME_IN_USE;
}

void CPtGatewayEmbed::HandlePacket(const unsigned char* pData, size_t nSize)
{
	if (!PhongThanIsWirePacket(pData, (PHONGTHAN_U32)nSize))
		return;
	const PHONGTHAN_WIRE_HEADER* pHeader =
		(const PHONGTHAN_WIRE_HEADER*)pData;
	if (!PhongThanValidateWireHeader(pHeader, (PHONGTHAN_U32)nSize) ||
		pHeader->PacketSize != nSize ||
		pHeader->Flags != PHONGTHAN_WIRE_FLAG_REQUEST)
		return;

	switch (pHeader->MessageType)
	{
	case PHONGTHAN_MSG_SESSION_AUTHENTICATE:
	{
		if (nSize != sizeof(PHONGTHAN_SESSION_AUTHENTICATE_REQUEST))
			return;
		const PHONGTHAN_SESSION_AUTHENTICATE_REQUEST* pRequest =
			(const PHONGTHAN_SESSION_AUTHENTICATE_REQUEST*)pData;
		if (!memchr(pRequest->AccountName, 0, sizeof(pRequest->AccountName)) ||
			!memchr(pRequest->PasswordProof, 0, sizeof(pRequest->PasswordProof)) ||
			!pRequest->AccountName[0] || !pRequest->PasswordProof[0])
			return;
		// Offline: any account name / password is accepted; the account
		// name is simply the local profile (its CharacterStore folder).
		m_sAccount = (const char*)pRequest->AccountName;

		PHONGTHAN_SESSION_AUTHENTICATE_RESPONSE Response;
		ZeroMemory(&Response, sizeof(Response));
		PhongThanInitializeWireHeader(&Response.Header,
			PHONGTHAN_MSG_SESSION_AUTHENTICATE, sizeof(Response),
			PHONGTHAN_WIRE_FLAG_RESPONSE, 0);
		Response.Result = PHONGTHAN_AUTH_SUCCESS;
		Response.RemainingTime = 0;
		strncpy((char*)Response.AccountName, m_sAccount.c_str(),
			sizeof(Response.AccountName) - 1);
		SendToClient(&Response, sizeof(Response));
		// Bishop pushes the character list right after a successful auth;
		// the client never asks for it.
		SendCharacterList();
		break;
	}

	case PHONGTHAN_MSG_SESSION_CREATE_CHARACTER:
	{
		if (nSize != sizeof(PHONGTHAN_SESSION_CREATE_CHARACTER_REQUEST) ||
			m_sAccount.empty())
			return;
		const PHONGTHAN_SESSION_CREATE_CHARACTER_REQUEST* pCreate =
			(const PHONGTHAN_SESSION_CREATE_CHARACTER_REQUEST*)pData;
		PHONGTHAN_SESSION_CREATE_CHARACTER_RESPONSE Response;
		ZeroMemory(&Response, sizeof(Response));
		PhongThanInitializeWireHeader(&Response.Header,
			PHONGTHAN_MSG_SESSION_CREATE_CHARACTER, sizeof(Response),
			PHONGTHAN_WIRE_FLAG_RESPONSE, 0);
		strncpy((char*)Response.RoleName, (const char*)pCreate->RoleName,
			sizeof(Response.RoleName) - 1);
		Response.Result = PHONGTHAN_CHARACTER_OPERATION_SERVICE_UNAVAILABLE;
		if (memchr(pCreate->RoleName, 0, sizeof(pCreate->RoleName)) &&
			pCreate->RoleName[0] &&
			pCreate->Gender < PHONGTHAN_GENDER_COUNT &&
			pCreate->Profession < PHONGTHAN_CREATABLE_PROFESSION_COUNT &&
			pCreate->Portrait >= 1 && pCreate->Portrait <= 3)
		{
			PHONGTHAN_SERVICE_CHARACTER_CREATE_REQUEST ServiceRequest;
			if (BuildCreateRequest(pCreate, &ServiceRequest))
			{
				Response.Result = CreateCharacter(&ServiceRequest);
			}
		}
		else
		{
			Response.Result = PHONGTHAN_CHARACTER_OPERATION_INVALID_NAME;
		}
		SendToClient(&Response, sizeof(Response));
		if (Response.Result == PHONGTHAN_CHARACTER_OPERATION_SUCCESS)
			StartAttach((const char*)pCreate->RoleName);
		break;
	}

	case PHONGTHAN_MSG_SESSION_DELETE_CHARACTER:
	{
		if (nSize != sizeof(PHONGTHAN_SESSION_DELETE_CHARACTER_REQUEST) ||
			m_sAccount.empty())
			return;
		const PHONGTHAN_SESSION_DELETE_CHARACTER_REQUEST* pDelete =
			(const PHONGTHAN_SESSION_DELETE_CHARACTER_REQUEST*)pData;
		if (!memchr(pDelete->RoleName, 0, sizeof(pDelete->RoleName)) ||
			!pDelete->RoleName[0])
			return;
		// Offline: the super-password check is skipped; the account's own
		// store folder is the only authority.
		PHONGTHAN_SESSION_DELETE_CHARACTER_RESPONSE Response;
		ZeroMemory(&Response, sizeof(Response));
		PhongThanInitializeWireHeader(&Response.Header,
			PHONGTHAN_MSG_SESSION_DELETE_CHARACTER, sizeof(Response),
			PHONGTHAN_WIRE_FLAG_RESPONSE, 0);
		Response.Result = g_PhongThanCharacterStore.Delete(
			m_sAccount.c_str(), (const char*)pDelete->RoleName) ?
			PHONGTHAN_CHARACTER_OPERATION_SUCCESS :
			PHONGTHAN_CHARACTER_OPERATION_INVALID_CREDENTIALS;
		strncpy((char*)Response.RoleName, (const char*)pDelete->RoleName,
			sizeof(Response.RoleName) - 1);
		SendToClient(&Response, sizeof(Response));
		break;
	}

	case PHONGTHAN_MSG_SESSION_SELECT_CHARACTER:
	{
		if (nSize != sizeof(PHONGTHAN_SESSION_SELECT_CHARACTER_REQUEST) ||
			m_sAccount.empty())
			return;
		const PHONGTHAN_SESSION_SELECT_CHARACTER_REQUEST* pSelect =
			(const PHONGTHAN_SESSION_SELECT_CHARACTER_REQUEST*)pData;
		if (!memchr(pSelect->RoleName, 0, sizeof(pSelect->RoleName)) ||
			!pSelect->RoleName[0])
			return;
		StartAttach((const char*)pSelect->RoleName);
		break;
	}

	default:
		break;
	}
}
