#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "PhongThanNpcVitalRules.h"
#include "PhongThanWorldProtocol.h"
#include "PhongThanIbItemEffect.h"
#include "../Sources/Represent/iRepresent/PhongThanSpriteAnchor.h"

static int checks = 0;
#define CHECK(test) do { ++checks; if (!(test)) { printf("FAIL line=%d: %s\n", __LINE__, #test); return 1; } } while (0)

int main()
{
	CHECK(PhongThanClampLife(40, 100) == 40);
	CHECK(PhongThanClampLife(140, 100) == 100);
	CHECK(PhongThanClampLife(-1, 100) == 0);
	CHECK(PhongThanLifePercent(50, 100, 1000) == 500);
	CHECK(PhongThanLifePercent(2000000000, 2000000000, 1000) == 1000);
	CHECK(PhongThanLifePercent(1, 0, 1000) == 0);
	CHECK(PhongThanApplyNpcLifeRegen(50, 100, 10, 0, 1000, 1) == 60);
	CHECK(PhongThanApplyNpcLifeRegen(50, 100, 10, 500, 1000, 1) == 65);
	CHECK(PhongThanApplyNpcLifeRegen(95, 100, 10, 0, 1000, 1) == 100);
	CHECK(PhongThanApplyNpcLifeRegen(0, 100, 10, 0, 1000, 1) == 0);
	CHECK(PhongThanApplyNpcLifeRegen(50, 100, 10, 0, 1000, 0) == 50);
	CHECK(PhongThanVisualAnchorHeight(340, 96) == 340);
	CHECK(PhongThanVisualAnchorHeight(0, 96) == 96);
	CHECK(PhongThanSpriteVisualAnchorHeight(16, 128, 96, 80) == 112);
	CHECK(PhongThanSpriteVisualAnchorHeight(16, 128, 0, 80) == 144);
	CHECK(PhongThanSpriteVisualAnchorHeight(16, 0, 0, 80) == 16);
	CHECK(PhongThanUsesActorCanvas("\\spr\\npcres\\human\\xia001_st.spr", 510, 510));
	CHECK(PhongThanUsesNpcBodyCanvas("\\spr\\npcres\\monster\\ani001_st.spr", 510, 510));
	CHECK(!PhongThanUsesActorCanvas("\\spr\\npcres\\monster\\ani001_st.spr", 510, 510));
	CHECK(!PhongThanUsesNpcBodyCanvas("\\spr\\effect\\magic.spr", 510, 510));
	CHECK(PhongThanActorReferenceY("\\spr\\npcres\\human\\xia001_st.spr", 0, 0) == 293);
	CHECK(PhongThanActorReferenceY("\\spr\\npcres\\passerby\\npc.spr", 255, 293) == 293);
	CHECK(PhongThanActorReferenceY("\\spr\\npcres\\human\\xia001_st.spr", 0, 0) != 510);
	CHECK(PhongThanAdaptIbEffectId(153) == 148);
	CHECK(PhongThanAdaptIbEffectId(154) == 149);
	CHECK(PhongThanAdaptIbEffectId(85) == 85);
	int x1 = 0, y1 = 0, x2 = 0, y2 = 0;
	PhongThanProjectWorldAnchor(1000, 2000, 160, 900, 1800, 1024, 768, x1, y1);
	PhongThanProjectWorldAnchor(1032, 2064, 160, 900, 1800, 1024, 768, x2, y2);
	CHECK(x2 - x1 == 32);
	CHECK(y2 - y1 == 32);

	PHONGTHAN_NPC_SNAPSHOT snapshot;
	ZeroMemory(&snapshot, sizeof(snapshot));
	PhongThanInitializeWireHeader(&snapshot.Header, PHONGTHAN_MSG_WORLD_NPC_SNAPSHOT,
		sizeof(snapshot), PHONGTHAN_WIRE_FLAG_RESPONSE, 1);
	snapshot.MapId = 1007; snapshot.EntityId = 1; snapshot.TemplateId = 1;
	snapshot.Action = PHONGTHAN_ACTION_STAND;
	snapshot.X = 1000; snapshot.Y = 1000; snapshot.MaxLife = 100; snapshot.Life = 50;
	CHECK(PhongThanValidateNpcSnapshot(&snapshot, sizeof(snapshot)));
	snapshot.Life = 101;
	CHECK(!PhongThanValidateNpcSnapshot(&snapshot, sizeof(snapshot)));
	snapshot.Life = -1;
	CHECK(!PhongThanValidateNpcSnapshot(&snapshot, sizeof(snapshot)));

	PHONGTHAN_NPC_UPDATE update;
	ZeroMemory(&update, sizeof(update));
	PhongThanInitializeWireHeader(&update.Header, PHONGTHAN_MSG_WORLD_NPC_UPDATE,
		sizeof(update), PHONGTHAN_WIRE_FLAG_RESPONSE, 2);
	update.MapId = 1007; update.EntityId = 1; update.X = 1000; update.Y = 1000;
	update.Action = PHONGTHAN_ACTION_STAND;
	update.MaxLife = 100; update.Life = 0;
	CHECK(PhongThanValidateNpcUpdate(&update, sizeof(update)));
	update.MaxLife = 0; update.Life = 1;
	CHECK(!PhongThanValidateNpcUpdate(&update, sizeof(update)));

	printf("PASS small runtime hotfix checks=%d\n", checks);
	return 0;
}
