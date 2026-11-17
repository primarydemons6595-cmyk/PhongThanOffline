#ifndef PT_EMBEDDED_SERVER_HOOKS_H
#define PT_EMBEDDED_SERVER_HOOKS_H

// Offline (embedded server) hooks for KSOServer.
//
// When g_bPtEmbeddedMode is TRUE, KSOServer::Init() obtains its three
// transports from these factories instead of heaven.dll / rainbow.dll:
//   - PtEmbedded_CreateMemoryServer : IServer  - player channel, in memory
//   - PtEmbedded_CreateStoreClient  : IClient  - "Goddess" channel, answered
//                                     by the local character store
//   - PtEmbedded_CreateGatewayLink  : IClient  - "Bishop" channel, answered
//                                     by the embedded session gateway
// The standalone GameServer build links PtEmbeddedServerHooks.cpp, whose
// stubs return NULL; the flag is never set there. The GameClient build
// links the real implementations in Sources/GameClient/Embedded/ and does
// NOT link the stub file.

#include "IServer.h"
#include "IClient.h"

IServer* PtEmbedded_CreateMemoryServer(int nMaxPlayer);
IClient* PtEmbedded_CreateStoreClient();
IClient* PtEmbedded_CreateGatewayLink();

#endif // PT_EMBEDDED_SERVER_HOOKS_H
