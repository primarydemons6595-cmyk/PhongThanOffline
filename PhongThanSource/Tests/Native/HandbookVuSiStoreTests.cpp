#include <windows.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <vector>
#include "../../Sources/MultiServer/Goddess/PhongThanCharacterStore.h"
#include "../../Headers/PhongThanWorldProtocol.h"
#include "../../Headers/PhongThanPlayerProtocol.h"
int main(int argc,char** argv){
    assert(argc==2);CPhongThanCharacterStore store;assert(store.Open(argv[1]));
    PHONGTHAN_CHARACTER_STATE_HEADER role;memset(&role,0,sizeof(role));
    role.SchemaVersion=PHONGTHAN_CHARACTER_SCHEMA_VERSION;role.StateSize=sizeof(role);role.Revision=1;
    strcpy((char*)role.AccountName,"HandbookFixture");strcpy((char*)role.RoleName,"VuSiFixture");role.Profession=3;
    assert(store.Create(&role,sizeof(role)));
    unsigned char restored[PHONGTHAN_CHARACTER_MAX_STATE_SIZE];PHONGTHAN_U32 size=0;
    assert(store.Load("VuSiFixture",restored,sizeof(restored),&size));
    assert(size==sizeof(role) && ((PHONGTHAN_CHARACTER_STATE_HEADER*)restored)->Profession==3);
    PHONGTHAN_CHARACTER_SUMMARY summary;assert(store.List("HandbookFixture",&summary,1)==1 && summary.Profession==3);
    PHONGTHAN_PLAYER_SNAPSHOT snapshot;memset(&snapshot,0,sizeof(snapshot));
    PhongThanInitializeWireHeader(&snapshot.Header,PHONGTHAN_MSG_WORLD_PLAYER_SNAPSHOT,sizeof(snapshot),PHONGTHAN_WIRE_FLAG_RESPONSE,0);
    snapshot.MapId=snapshot.EntityId=1;snapshot.Profession=3;
    assert(PhongThanValidatePlayerSnapshot(&snapshot,sizeof(snapshot)));
    snapshot.Profession=4;assert(!PhongThanValidatePlayerSnapshot(&snapshot,sizeof(snapshot)));
    PHONGTHAN_PLAYER_EVENT event;memset(&event,0,sizeof(event));
    PhongThanInitializeWireHeader(&event.Header,PHONGTHAN_MSG_UI_PLAYER_EVENT,sizeof(event),PHONGTHAN_WIRE_FLAG_RESPONSE,0);
    event.MapId=event.EntityId=1;event.Operation=PHONGTHAN_PLAYER_PROFESSION;event.Value=3;
    assert(PhongThanValidatePlayerEventPacket(&event,sizeof(event)));
    event.Value=4;assert(!PhongThanValidatePlayerEventPacket(&event,sizeof(event)));
    event.Value=3;event.Header.Flags=PHONGTHAN_WIRE_FLAG_REQUEST;assert(!PhongThanValidatePlayerEventPacket(&event,sizeof(event)));
    assert(PHONGTHAN_CREATABLE_PROFESSION_COUNT==3);
    assert(store.Delete("HandbookFixture","VuSiFixture"));
    puts("PASS Vu Si canonical persistence list reload world_wire validation creation_stays_three");return 0;
}
