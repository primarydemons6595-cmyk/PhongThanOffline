#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "PhongThanMapIdentity.h"

#define CHECK(x) do { if (!(x)) { printf("FAIL line=%d: %s\n", __LINE__, #x); return 1; } } while (0)

static void JoinPath(char* output, const char* root, const char* relative)
{
	sprintf(output, "%s\\%s", root, relative);
}

int main(int argc, char** argv)
{
	CHECK(argc == 2);
	char activePath[MAX_PATH], metadataPath[MAX_PATH];
	JoinPath(activePath, argv[1], "maps\\WorldSet.ini");
	JoinPath(metadataPath, argv[1], "settings\\WorldSet.ini");

	const int activeCount = GetPrivateProfileIntA("Init", "Count", -1, activePath);
	CHECK(activeCount == 102);
	bool active[1200];
	memset(active, 0, sizeof(active));
	for (int i = 0; i < activeCount; ++i)
	{
		char key[32];
		sprintf(key, "World%03d", i);
		const int runtimeMap = GetPrivateProfileIntA("World", key, 0, activePath);
		CHECK(runtimeMap > 0 && runtimeMap < 1200);
		CHECK(!active[runtimeMap]);
		active[runtimeMap] = true;
		PHONGTHAN_MAP_RESOLUTION resolved;
		CHECK(PhongThanResolveRuntimeMap((unsigned int)runtimeMap, &resolved));
		CHECK(resolved.RuntimeMapId == (unsigned int)runtimeMap);
	}

	CHECK(PhongThanCanonicalMapIdentityCount() == 118);
	for (unsigned int mapIndex = 0; mapIndex < PhongThanCanonicalMapIdentityCount(); ++mapIndex)
	{
		const PHONGTHAN_MAP_IDENTITY& entry = g_PhongThanCanonicalMapIdentities[mapIndex];
		CHECK(entry.Evidence == PHONGTHAN_MAP_CONFIRMED_FROM_EXISTING_RUNTIME);
		for (unsigned int j = mapIndex + 1; j < PhongThanCanonicalMapIdentityCount(); ++j)
		{
			CHECK(entry.ContentMapId != g_PhongThanCanonicalMapIdentities[j].ContentMapId);
			CHECK(entry.RuntimeMapId != g_PhongThanCanonicalMapIdentities[j].RuntimeMapId);
		}
		char key[32], label[256], sourceText[32];
		sprintf(key, "%u_name", entry.RuntimeMapId);
		sprintf(sourceText, "Phong Than %u", entry.ContentMapId);
		CHECK(GetPrivateProfileStringA("List", key, "", label, sizeof(label), metadataPath) > 0);
		CHECK(strstr(label, sourceText) != NULL);
	}

	PHONGTHAN_MAP_RESOLUTION result;
	CHECK(PhongThanResolveContentMap(1, &result));
	CHECK(result.RuntimeMapId == 1001);

	for (unsigned int collision = 103; collision <= 118; ++collision)
		CHECK(!PhongThanResolveContentMap(collision, &result));

	const unsigned int highContent[] = {
		60010, 60011, 60012, 60013, 60014, 60015,
		60020, 60021, 60022, 60023, 60024,
		60025, 60026, 60027, 60028, 60029
	};
	for (unsigned int high = 0; high < sizeof(highContent) / sizeof(highContent[0]); ++high)
	{
		CHECK(PhongThanResolveContentMap(highContent[high], &result));
		CHECK(result.RuntimeMapId == 1103 + high);
	}

	CHECK(PhongThanResolveContentMap(58, &result));
	CHECK(result.RuntimeMapId == 1058 && !active[result.RuntimeMapId]);

	unsigned int currentRuntimeMap = 1001;
	CHECK(!PhongThanResolveContentMap(77777, &result));
	CHECK(result.ContentMapId == 0 && result.RuntimeMapId == 0 &&
		result.Evidence == PHONGTHAN_MAP_UNKNOWN);
	CHECK(currentRuntimeMap == 1001);

	// Persistence stores the runtime domain.  A reload consumes 1103 directly;
	// reverse lookup identifies PT 60010 and a round trip returns exactly 1103.
	const unsigned int persistedMap = 1103;
	CHECK(PhongThanResolveRuntimeMap(persistedMap, &result));
	CHECK(result.ContentMapId == 60010);
	PHONGTHAN_MAP_RESOLUTION roundTrip;
	CHECK(PhongThanResolveContentMap(result.ContentMapId, &roundTrip));
	CHECK(roundTrip.RuntimeMapId == persistedMap);

	puts("PASS canonical map resolver: 118 explicit identities, 102 active worlds, collisions rejected, high maps preserved, unknown safe, persistence stable");
	return 0;
}
