#include <windows.h>
#include <stdio.h>
#include "../../Sources/MultiServer/Goddess/PhongThanCharacterStore.h"

// Only the test character created by PhongThanLoginProbe may be positioned.
// Invoke while staging services are stopped; user gameplay characters untouched.
int main(int argc, char** argv)
{
    if (argc != 2 && argc != 3) return 2;
    CPhongThanCharacterStore store;
    BYTE bytes[PHONGTHAN_CHARACTER_MAX_STATE_SIZE];
    PHONGTHAN_U32 size=0;
    if (!store.Open(argv[1]) || !store.Load("PhongThanNpc",bytes,sizeof(bytes),&size)) return 3;
    PHONGTHAN_CHARACTER_STATE_HEADER* role=(PHONGTHAN_CHARACTER_STATE_HEADER*)bytes;
    if (strcmp((const char*)role->AccountName,"123456") || strcmp((const char*)role->RoleName,"PhongThanNpc")) return 4;
    role->EnterMapId=1052; role->EnterX=50824; role->EnterY=96855;
    if(argc==3){
        if(!strcmp(argv[2],"authored-dieutri")){
            role->EnterMapId=1052;role->EnterX=48256;role->EnterY=102624;
        }else if(!strcmp(argv[2],"viewport-ngochu")){
            // Chuyen Sinh Lao Lao is 992 MPS below this point: within the
            // projected viewport, outside the obsolete 800-MPS circle.
            role->EnterMapId=1003;role->EnterX=54400;role->EnterY=99616;
        }else if(!strcmp(argv[2],"gate-supertrap")){
            // Exact center of the authored 9x9 SuperTrap on PT content map 1.
            // Script coordinates are converted to MPS with the engine's x32 scale.
            role->EnterMapId=1001;role->EnterX=1563*32;role->EnterY=3232*32;
            if(role->Money<100000)role->Money=100000;
        }else if(!strcmp(argv[2],"map-exit-forward")){
            // One cell north of the authentic PT 2 corridor mouth.
            role->EnterMapId=1002;role->EnterX=1531*32;role->EnterY=3273*32;
        }else if(!strcmp(argv[2],"map-exit-reverse")){
            // One cell outside the authentic PT 5 corridor.
            role->EnterMapId=1005;role->EnterX=1823*32;role->EnterY=2943*32;
        }else if(!strcmp(argv[2],"map-exit-pt3-pt8")){
            // Approach the authentic corridor through its open north mouth.
            // The west edge is blocked by authored collision and cannot enter it.
            role->EnterMapId=1003;role->EnterX=1784*32;role->EnterY=3260*32;
        }else if(!strcmp(argv[2],"map-exit-pt52-pt47")){
            role->EnterMapId=1052;role->EnterX=1585*32;role->EnterY=2958*32;
        }else if(!strcmp(argv[2],"map-exit-pt2-pt6")){
            role->EnterMapId=1002;role->EnterX=1544*32;role->EnterY=3100*32;
		}else if(!strcmp(argv[2],"map-exit-pt6-pt2")){
			// Exact PT 6 return corridor is x=1922 and x=1925..1927 at y=2783.
			// Start one cell south so the probe enters it using a normal move packet.
			role->EnterMapId=1006;role->EnterX=1922*32;role->EnterY=2784*32;
        }else return 6;
    }
    role->UseRevive=0;
    if (!store.Save(role,size)) return 5;
    printf("Positioned diagnostic PhongThanNpc map=%u x=%d y=%d; other characters unchanged\n",role->EnterMapId,role->EnterX,role->EnterY);
    return 0;
}
