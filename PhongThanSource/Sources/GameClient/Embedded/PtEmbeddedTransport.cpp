// Memory transports for Phong Than offline mode: CPtVirtualClient (client
// side, replaces Rainbow.dll) and CPtMemoryServer (server side, replaces
// heaven.dll). See PtEmbedded.h for the architecture overview.
#include "KWin32.h"
#include "PtEmbedded.h"
#include "../../MultiServer/GameServer/PtEmbeddedServerHooks.h"

#include <objbase.h>

// ===========================================================================
// Registry: the memory server created by KSOServer::Init() (through the
// hook below) is found here by world-channel virtual clients.
// ===========================================================================
static CPtMemoryServer* g_pPtMemoryServer = NULL;

CPtMemoryServer* PtEmbedded_GetMemoryServer()
{
	return g_pPtMemoryServer;
}

// ===========================================================================
// KSOServer hooks (real implementations; the standalone GameServer build
// links the stubs in PtEmbeddedServerHooks.cpp instead).
// ===========================================================================
IServer* PtEmbedded_CreateMemoryServer(int nMaxPlayer)
{
	CPtMemoryServer* pServer = new CPtMemoryServer(nMaxPlayer);
	g_pPtMemoryServer = pServer;
	return pServer;
}

IClient* PtEmbedded_CreateStoreClient()
{
	return new CPtStoreClient();
}

IClient* PtEmbedded_CreateGatewayLink()
{
	CPtGatewayLink* pLink = PtEmbedded_GetGatewayLink();
	pLink->AddRef();
	return pLink;
}

// ===========================================================================
// CPtClientFactory
// ===========================================================================
class CPtClientFactory : public IClientFactory
{
public:
	CPtClientFactory() : m_nRef(1) {}

	STDMETHODIMP QueryInterface(REFIID riid, void** ppv)
	{
		if (!ppv)
			return E_POINTER;
		*ppv = NULL;
		if (riid == IID_IUnknown || riid == IID_IClientFactory)
		{
			*ppv = static_cast<IClientFactory*>(this);
			AddRef();
			return S_OK;
		}
		return E_NOINTERFACE;
	}
	STDMETHODIMP_(ULONG) AddRef() { return ++m_nRef; }
	STDMETHODIMP_(ULONG) Release()
	{
		LONG n = --m_nRef;
		if (n <= 0)
			delete this;
		return n;
	}
	STDMETHODIMP SetEnvironment(const size_t& /*bufferSize*/) { return S_OK; }
	STDMETHODIMP CreateClientInterface(REFIID riid, void** ppv)
	{
		if (!ppv)
			return E_POINTER;
		*ppv = NULL;
		if (riid != IID_IESClient && riid != IID_IUnknown)
			return E_NOINTERFACE;
		CPtVirtualClient* pClient = new CPtVirtualClient();
		if (!pClient)
			return E_OUTOFMEMORY;
		*ppv = pClient;
		return S_OK;
	}

private:
	LONG m_nRef;
};

IClientFactory* PtEmbedded_CreateClientFactory()
{
	return new CPtClientFactory();
}

// ===========================================================================
// CPtVirtualClient
// ===========================================================================
CPtVirtualClient::CPtVirtualClient()
	: m_nRef(1), m_pfnEvent(NULL), m_lpParam(NULL),
	  m_nPort(0), m_nLnID(-1), m_bConnected(false)
{
}

CPtVirtualClient::~CPtVirtualClient()
{
}

STDMETHODIMP CPtVirtualClient::QueryInterface(REFIID riid, void** ppv)
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

STDMETHODIMP_(ULONG) CPtVirtualClient::AddRef()
{
	return ++m_nRef;
}

STDMETHODIMP_(ULONG) CPtVirtualClient::Release()
{
	LONG n = --m_nRef;
	if (n <= 0)
		delete this;
	return n;
}

STDMETHODIMP CPtVirtualClient::Startup()
{
	return S_OK;
}

STDMETHODIMP CPtVirtualClient::Cleanup()
{
	// Silent teardown; the client asked for it, so no Close event.
	if (m_bConnected)
	{
		if (m_nPort == 5622)
			PtEmbedded_GetGateway()->Detach(this);
		else if (m_nLnID >= 0 && PtEmbedded_GetMemoryServer())
			PtEmbedded_GetMemoryServer()->DetachClient(m_nLnID, false);
		m_bConnected = false;
		m_nLnID = -1;
	}
	m_Inbound.Clear();
	return S_OK;
}

STDMETHODIMP CPtVirtualClient::ConnectTo(const char* const& /*pAddress*/,
	unsigned short usPort)
{
	m_nPort = usPort;
	if (usPort == 5622)
	{
		// The embedded host (character store + KSOServer) starts lazily on
		// the first gateway connection, i.e. when the player logs in.
		if (!PtEmbedded_EnsureHostStarted())
			return E_FAIL;
		PtEmbedded_GetGateway()->Attach(this);
	}
	else
	{
		CPtMemoryServer* pServer = PtEmbedded_GetMemoryServer();
		if (!pServer)
			return E_FAIL;
		m_nLnID = pServer->AttachClient(this);
		if (m_nLnID < 0)
			return E_FAIL;
	}
	m_bConnected = true;
	if (m_pfnEvent)
		m_pfnEvent(m_lpParam, enumServerConnectCreate);
	return S_OK;
}

STDMETHODIMP CPtVirtualClient::RegisterMsgFilter(LPVOID lpParam,
	CALLBACK_CLIENT_EVENT pfnEventNotify)
{
	m_lpParam = lpParam;
	m_pfnEvent = pfnEventNotify;
	return S_OK;
}

STDMETHODIMP CPtVirtualClient::SendPackToServer(const void* const pData,
	const size_t& datalength)
{
	if (!m_bConnected || !pData || datalength == 0)
		return E_FAIL;
	if (m_nPort == 5622)
	{
		PtEmbedded_GetGateway()->ReceiveFromClient(pData, datalength);
		return S_OK;
	}
	CPtMemoryServer* pServer = PtEmbedded_GetMemoryServer();
	if (!pServer || m_nLnID < 0)
		return E_FAIL;
	pServer->ReceiveFromClient(m_nLnID, pData, datalength);
	return S_OK;
}

STDMETHODIMP_(const void*) CPtVirtualClient::GetPackFromServer(size_t& datalength)
{
	if (!m_Inbound.Pop(m_Current))
	{
		datalength = 0;
		return NULL;
	}
	datalength = m_Current.size();
	return m_Current.empty() ? (const void*)"" : (const void*)&m_Current[0];
}

STDMETHODIMP CPtVirtualClient::Shutdown()
{
	if (m_bConnected)
	{
		if (m_nPort == 5622)
			PtEmbedded_GetGateway()->Detach(this);
		else if (m_nLnID >= 0 && PtEmbedded_GetMemoryServer())
			// The server must see a normal disconnect so it logs the
			// character out and saves it.
			PtEmbedded_GetMemoryServer()->DetachClient(m_nLnID, true);
		m_bConnected = false;
		m_nLnID = -1;
	}
	return S_OK;
}

void CPtVirtualClient::NotifyClosedByServer()
{
	if (!m_bConnected)
		return;
	m_bConnected = false;
	m_nLnID = -1;
	if (m_pfnEvent)
		m_pfnEvent(m_lpParam, enumServerConnectClose);
}

// ===========================================================================
// CPtMemoryServer
// ===========================================================================
CPtMemoryServer::CPtMemoryServer(int nMaxPlayer)
	: m_nRef(1), m_pfnEvent(NULL), m_lpParam(NULL), m_nCount(0)
{
	if (nMaxPlayer < 1)
		nMaxPlayer = 1;
	m_Slots.resize((size_t)nMaxPlayer);
	for (size_t i = 0; i < m_Slots.size(); ++i)
		strcpy(m_Slots[i].szInfo, "127.0.0.1");
}

CPtMemoryServer::~CPtMemoryServer()
{
	if (g_pPtMemoryServer == this)
		g_pPtMemoryServer = NULL;
}

STDMETHODIMP CPtMemoryServer::QueryInterface(REFIID riid, void** ppv)
{
	if (!ppv)
		return E_POINTER;
	*ppv = NULL;
	if (riid == IID_IUnknown || riid == IID_IIOCPServer)
	{
		*ppv = static_cast<IServer*>(this);
		AddRef();
		return S_OK;
	}
	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) CPtMemoryServer::AddRef()
{
	return ++m_nRef;
}

STDMETHODIMP_(ULONG) CPtMemoryServer::Release()
{
	LONG n = --m_nRef;
	if (n <= 0)
		delete this;
	return n;
}

STDMETHODIMP CPtMemoryServer::Startup()
{
	return S_OK;
}

STDMETHODIMP CPtMemoryServer::Cleanup()
{
	for (size_t i = 0; i < m_Slots.size(); ++i)
	{
		m_Slots[i].pClient = NULL;
		m_Slots[i].ToServer.Clear();
		m_Slots[i].Sink.clear();
	}
	m_nCount = 0;
	return S_OK;
}

STDMETHODIMP CPtMemoryServer::OpenService(const unsigned long& /*ulnAddress*/,
	const unsigned short& /*usnPort*/)
{
	return S_OK;
}

STDMETHODIMP CPtMemoryServer::CloseService()
{
	return S_OK;
}

STDMETHODIMP CPtMemoryServer::RegisterMsgFilter(LPVOID lpParam,
	CALLBACK_SERVER_EVENT pfnEventNotify)
{
	m_lpParam = lpParam;
	m_pfnEvent = pfnEventNotify;
	return S_OK;
}

STDMETHODIMP CPtMemoryServer::RegisterMsgFilter(const unsigned long /*ulnClientID*/,
	IMessageProcess* /*pfnMsgNotify*/)
{
	// The per-client message-process filter is not used by KSOServer/Core.
	return S_OK;
}

int CPtMemoryServer::AttachClient(CPtVirtualClient* pClient)
{
	for (size_t i = 0; i < m_Slots.size(); ++i)
	{
		if (!m_Slots[i].pClient)
		{
			m_Slots[i].pClient = pClient;
			m_Slots[i].ToServer.Clear();
			m_Slots[i].Sink.clear();
			++m_nCount;
			if (m_pfnEvent)
				m_pfnEvent(m_lpParam, (unsigned long)i, enumClientConnectCreate);
			return (int)i;
		}
	}
	return -1;
}

void CPtMemoryServer::DetachClient(int nLnID, bool bNotifyServer)
{
	if (nLnID < 0 || (size_t)nLnID >= m_Slots.size())
		return;
	if (!m_Slots[nLnID].pClient)
		return;
	m_Slots[nLnID].pClient = NULL;
	m_Slots[nLnID].ToServer.Clear();
	m_Slots[nLnID].Sink.clear();
	if (m_nCount)
		--m_nCount;
	if (bNotifyServer && m_pfnEvent)
		m_pfnEvent(m_lpParam, (unsigned long)nLnID, enumClientConnectClose);
}

void CPtMemoryServer::ReceiveFromClient(int nLnID, const void* pData, size_t nSize)
{
	if (nLnID < 0 || (size_t)nLnID >= m_Slots.size())
		return;
	if (m_Slots[nLnID].pClient)
		m_Slots[nLnID].ToServer.Push(pData, nSize);
}

STDMETHODIMP CPtMemoryServer::PreparePackSink()
{
	return S_OK;
}

STDMETHODIMP CPtMemoryServer::PackDataToClient(const unsigned long& ulnClientID,
	const void* const pData, const size_t& datalength)
{
	if (ulnClientID >= m_Slots.size() || !pData || datalength == 0)
		return E_FAIL;
	Slot& slot = m_Slots[ulnClientID];
	if (!slot.pClient)
		return E_FAIL;
	const unsigned char* pBytes = (const unsigned char*)pData;
	slot.Sink.insert(slot.Sink.end(), pBytes, pBytes + datalength);
	return S_OK;
}

void CPtMemoryServer::FlushSlot(int nLnID)
{
	Slot& slot = m_Slots[nLnID];
	if (slot.Sink.empty())
		return;
	if (slot.pClient)
		slot.pClient->Deliver(&slot.Sink[0], slot.Sink.size());
	slot.Sink.clear();
}

STDMETHODIMP CPtMemoryServer::SendPackToClient(const unsigned long& ulnClientID)
{
	if (ulnClientID == (unsigned long)-1)
	{
		for (size_t i = 0; i < m_Slots.size(); ++i)
			if (m_Slots[i].pClient)
				FlushSlot((int)i);
		return S_OK;
	}
	if (ulnClientID >= m_Slots.size() || !m_Slots[ulnClientID].pClient)
		return E_FAIL;
	FlushSlot((int)ulnClientID);
	return S_OK;
}

STDMETHODIMP CPtMemoryServer::SendData(const unsigned long& ulnClientID,
	const void* const pData, const size_t& datalength)
{
	if (!pData || datalength == 0)
		return E_FAIL;
	if (ulnClientID == (unsigned long)-1)
	{
		for (size_t i = 0; i < m_Slots.size(); ++i)
			if (m_Slots[i].pClient)
				m_Slots[i].pClient->Deliver(pData, datalength);
		return S_OK;
	}
	if (ulnClientID >= m_Slots.size() || !m_Slots[ulnClientID].pClient)
		return E_FAIL;
	m_Slots[ulnClientID].pClient->Deliver(pData, datalength);
	return S_OK;
}

STDMETHODIMP_(const void*) CPtMemoryServer::GetPackFromClient(
	const unsigned long& ulnClientID, size_t& datalength)
{
	datalength = 0;
	if (ulnClientID >= m_Slots.size())
		return NULL;
	Slot& slot = m_Slots[ulnClientID];
	if (!slot.pClient || !slot.ToServer.Pop(slot.Current))
		return NULL;
	datalength = slot.Current.size();
	return slot.Current.empty() ? (const void*)"" : (const void*)&slot.Current[0];
}

STDMETHODIMP CPtMemoryServer::ShutdownClient(const unsigned long& ulnClientID)
{
	if (ulnClientID >= m_Slots.size())
		return E_FAIL;
	CPtVirtualClient* pClient = m_Slots[ulnClientID].pClient;
	if (!pClient)
		return E_FAIL;
	// Server-initiated close: free the slot without a server event, then
	// tell the client end, mirroring a TCP close from heaven.dll.
	m_Slots[ulnClientID].pClient = NULL;
	m_Slots[ulnClientID].ToServer.Clear();
	m_Slots[ulnClientID].Sink.clear();
	if (m_nCount)
		--m_nCount;
	pClient->NotifyClosedByServer();
	return S_OK;
}

STDMETHODIMP_(size_t) CPtMemoryServer::GetClientCount()
{
	return m_nCount;
}

STDMETHODIMP_(const char*) CPtMemoryServer::GetClientInfo(
	const unsigned long& ulnClientID)
{
	if (ulnClientID >= m_Slots.size())
		return "127.0.0.1";
	return m_Slots[ulnClientID].szInfo;
}
