#ifndef PT_EMBEDDED_H
#define PT_EMBEDDED_H

// Phong Than OFFLINE mode: the whole server cluster is hosted inside the
// game client process. The client keeps talking the exact same wire
// protocol; only the transports change:
//
//   client gateway IClient  --memory-->  CPtGatewayEmbed (Bishop's job)
//   client world   IClient  --memory-->  CPtMemoryServer (GameServer's IServer)
//   GameServer "Goddess" IClient ------>  CPtStoreClient (answers from the
//                                          local .pthc character store)
//   GameServer "Bishop"  IClient ------>  CPtGatewayLink (bidirectional pipe
//                                          to CPtGatewayEmbed; the attach
//                                          handshake flows through it exactly
//                                          like it did over TCP)
//
// Everything runs on the client main thread: PtEmbedded_Pump() is called at
// the top of KNetConnectAgent::Breathe(), so no extra thread and no lock
// discipline beyond what the standalone server already had.
//
// Enabled by Config.ini [Offline] Enabled=1 or env PHONGTHAN_OFFLINE=1.

#include "IClient.h"
#include "IServer.h"

#include <deque>
#include <string>
#include <vector>

// ---------------------------------------------------------------------------
// Mode / host lifecycle (PtEmbeddedHost.cpp)
// ---------------------------------------------------------------------------
BOOL PtEmbedded_IsEnabled();
IClientFactory* PtEmbedded_CreateClientFactory();
BOOL PtEmbedded_EnsureHostStarted();
void PtEmbedded_Pump();
void PtEmbedded_Shutdown();
const char* PtEmbedded_GetServerDataDir();

// ---------------------------------------------------------------------------
// Packet FIFO shared by every memory transport.
// One queue item == one delivered buffer. The gateway/session channel
// expects exactly one wire packet per item; the world channel accepts a
// whole flushed sink (possibly several packets) per item, exactly like
// the TCP transports behaved.
// ---------------------------------------------------------------------------
class CPtPacketQueue
{
public:
	void Push(const void* pData, size_t nSize)
	{
		std::vector<unsigned char> item;
		if (nSize)
		{
			item.resize(nSize);
			memcpy(&item[0], pData, nSize);
		}
		m_Queue.push_back(item);
	}
	bool Pop(std::vector<unsigned char>& out)
	{
		if (m_Queue.empty())
			return false;
		out = m_Queue.front();
		m_Queue.pop_front();
		return true;
	}
	bool Empty() const { return m_Queue.empty(); }
	void Clear() { m_Queue.clear(); }

private:
	std::deque<std::vector<unsigned char> > m_Queue;
};

// ---------------------------------------------------------------------------
// Client-side transport (replaces Rainbow.dll in offline mode).
// Two instances exist, exactly like online: gateway + world. ConnectTo()
// routes by port: 5622 -> embedded gateway, anything else -> memory server.
// ---------------------------------------------------------------------------
class CPtMemoryServer;
class CPtGatewayEmbed;

class CPtVirtualClient : public IClient
{
public:
	CPtVirtualClient();
	~CPtVirtualClient();

	// IUnknown
	STDMETHODIMP QueryInterface(REFIID riid, void** ppv);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();

	// IClient
	STDMETHODIMP Startup();
	STDMETHODIMP Cleanup();
	STDMETHODIMP ConnectTo(const char* const& pAddress, unsigned short usPort);
	STDMETHODIMP RegisterMsgFilter(LPVOID lpParam, CALLBACK_CLIENT_EVENT pfnEventNotify);
	STDMETHODIMP SendPackToServer(const void* const pData, const size_t& datalength);
	STDMETHODIMP_(const void*) GetPackFromServer(size_t& datalength);
	STDMETHODIMP Shutdown();

	// Transport plumbing (not COM): called by the peer ends.
	void Deliver(const void* pData, size_t nSize) { m_Inbound.Push(pData, nSize); }
	void NotifyClosedByServer();

private:
	LONG m_nRef;
	CALLBACK_CLIENT_EVENT m_pfnEvent;
	LPVOID m_lpParam;
	int m_nPort;              // port passed to ConnectTo (5622 gateway / 6666 world)
	int m_nLnID;              // world channel: slot inside CPtMemoryServer
	bool m_bConnected;
	CPtPacketQueue m_Inbound; // peer -> client
	std::vector<unsigned char> m_Current; // buffer behind GetPackFromServer
};

// ---------------------------------------------------------------------------
// Server-side player channel (replaces heaven.dll IServer for KSOServer).
// ---------------------------------------------------------------------------
class CPtMemoryServer : public IServer
{
public:
	CPtMemoryServer(int nMaxPlayer);
	~CPtMemoryServer();

	// IUnknown
	STDMETHODIMP QueryInterface(REFIID riid, void** ppv);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();

	// IServer
	STDMETHODIMP Startup();
	STDMETHODIMP Cleanup();
	STDMETHODIMP OpenService(const unsigned long& ulnAddress, const unsigned short& usnPort);
	STDMETHODIMP CloseService();
	STDMETHODIMP RegisterMsgFilter(LPVOID lpParam, CALLBACK_SERVER_EVENT pfnEventNotify);
	STDMETHODIMP RegisterMsgFilter(const unsigned long ulnClientID, IMessageProcess* pfnMsgNotify);
	STDMETHODIMP PreparePackSink();
	STDMETHODIMP PackDataToClient(const unsigned long& ulnClientID, const void* const pData, const size_t& datalength);
	STDMETHODIMP SendPackToClient(const unsigned long& ulnClientID);
	STDMETHODIMP SendData(const unsigned long& ulnClientID, const void* const pData, const size_t& datalength);
	STDMETHODIMP_(const void*) GetPackFromClient(const unsigned long& ulnClientID, size_t& datalength);
	STDMETHODIMP ShutdownClient(const unsigned long& ulnClientID);
	STDMETHODIMP_(size_t) GetClientCount();
	STDMETHODIMP_(const char*) GetClientInfo(const unsigned long& ulnClientID);

	// Plumbing used by CPtVirtualClient (world channel).
	int AttachClient(CPtVirtualClient* pClient);
	void DetachClient(int nLnID, bool bNotifyServer);
	void ReceiveFromClient(int nLnID, const void* pData, size_t nSize);

private:
	struct Slot
	{
		Slot() : pClient(NULL) {}
		CPtVirtualClient* pClient;
		CPtPacketQueue ToServer;              // client -> server packets
		std::vector<unsigned char> Sink;      // PackDataToClient accumulator
		std::vector<unsigned char> Current;   // buffer behind GetPackFromClient
		char szInfo[32];                      // behind GetClientInfo
	};
	void FlushSlot(int nLnID);

	LONG m_nRef;
	CALLBACK_SERVER_EVENT m_pfnEvent;
	LPVOID m_lpParam;
	std::vector<Slot> m_Slots;
	size_t m_nCount;
};

CPtMemoryServer* PtEmbedded_GetMemoryServer();

// ---------------------------------------------------------------------------
// "Goddess" channel for KSOServer: answers CHARACTER_SAVE / CHARACTER_LOCK
// straight from the local character store, mirroring Goddess' responses.
// ---------------------------------------------------------------------------
class CPtStoreClient : public IClient
{
public:
	CPtStoreClient();
	~CPtStoreClient();

	STDMETHODIMP QueryInterface(REFIID riid, void** ppv);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();

	STDMETHODIMP Startup();
	STDMETHODIMP Cleanup();
	STDMETHODIMP ConnectTo(const char* const& pAddress, unsigned short usPort);
	STDMETHODIMP RegisterMsgFilter(LPVOID lpParam, CALLBACK_CLIENT_EVENT pfnEventNotify);
	STDMETHODIMP SendPackToServer(const void* const pData, const size_t& datalength);
	STDMETHODIMP_(const void*) GetPackFromServer(size_t& datalength);
	STDMETHODIMP Shutdown();

private:
	void HandleSave(const void* pData, size_t nSize);

	LONG m_nRef;
	CALLBACK_CLIENT_EVENT m_pfnEvent;
	LPVOID m_lpParam;
	CPtPacketQueue m_Inbound; // responses -> KSOServer
	std::vector<unsigned char> m_Current;
};

// ---------------------------------------------------------------------------
// "Bishop" channel for KSOServer: a pipe whose other end is CPtGatewayEmbed.
// KSOServer sends attach permits / world session events into it; the
// gateway injects WORLD_ATTACH_CHARACTER packets into it.
// Singleton: Release() never destroys it (the gateway keeps using it).
// ---------------------------------------------------------------------------
class CPtGatewayLink : public IClient
{
public:
	CPtGatewayLink();
	~CPtGatewayLink();

	STDMETHODIMP QueryInterface(REFIID riid, void** ppv);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();

	STDMETHODIMP Startup();
	STDMETHODIMP Cleanup();
	STDMETHODIMP ConnectTo(const char* const& pAddress, unsigned short usPort);
	STDMETHODIMP RegisterMsgFilter(LPVOID lpParam, CALLBACK_CLIENT_EVENT pfnEventNotify);
	STDMETHODIMP SendPackToServer(const void* const pData, const size_t& datalength);
	STDMETHODIMP_(const void*) GetPackFromServer(size_t& datalength);
	STDMETHODIMP Shutdown();

	// Gateway side of the pipe.
	void InjectToServer(const void* pData, size_t nSize) { m_ToServer.Push(pData, nSize); }
	bool PollFromServer(std::vector<unsigned char>& out) { return m_FromServer.Pop(out); }

private:
	LONG m_nRef;
	CALLBACK_CLIENT_EVENT m_pfnEvent;
	LPVOID m_lpParam;
	CPtPacketQueue m_ToServer;   // gateway -> KSOServer
	CPtPacketQueue m_FromServer; // KSOServer -> gateway
	std::vector<unsigned char> m_Current;
};

CPtGatewayLink* PtEmbedded_GetGatewayLink();

// ---------------------------------------------------------------------------
// Embedded session gateway: does Bishop's client-facing job in-process.
// Auth (always succeeds offline), character list/create/delete straight
// against the store, select -> attach via the gateway link -> ENTER_WORLD.
// ---------------------------------------------------------------------------
class CPtGatewayEmbed
{
public:
	CPtGatewayEmbed();

	void Attach(CPtVirtualClient* pClient);
	void Detach(CPtVirtualClient* pClient);
	void ReceiveFromClient(const void* pData, size_t nSize);
	void PumpIn();
	void PumpOut();

private:
	void HandlePacket(const unsigned char* pData, size_t nSize);
	void SendToClient(const void* pData, size_t nSize);
	void SendCharacterList();
	void StartAttach(const char* pRoleName);
	bool BuildCreateRequest(const void* pSessionRequest,
		void* pServiceRequestOut); // PHONGTHAN_SERVICE_CHARACTER_CREATE_REQUEST
	int CreateCharacter(const void* pServiceRequest); // PHONGTHAN_CHARACTER_OPERATION_*
	void MakeTicket(unsigned char* pTicketOut);

	CPtVirtualClient* m_pClient;
	CPtPacketQueue m_InPackets;
	std::string m_sAccount;
	bool m_bHasPendingAttach;
	unsigned char m_PendingTicket[16];
	char m_szPendingRole[32];
	unsigned int m_nAttachSequence;
	unsigned int m_nTicketCounter;
};

CPtGatewayEmbed* PtEmbedded_GetGateway();

#endif // PT_EMBEDDED_H
