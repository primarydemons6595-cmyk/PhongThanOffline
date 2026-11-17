// Stub implementations for the standalone GameServer build.
// The real embedded transports live in the GameClient build
// (Sources/GameClient/Embedded/). g_bPtEmbeddedMode is never set in the
// standalone server, so these are never called there; they exist only so
// KSOServer.obj links without the client sources.
#include "PtEmbeddedServerHooks.h"

IServer* PtEmbedded_CreateMemoryServer(int /*nMaxPlayer*/)
{
	return NULL;
}

IClient* PtEmbedded_CreateStoreClient()
{
	return NULL;
}

IClient* PtEmbedded_CreateGatewayLink()
{
	return NULL;
}
