#include <stdio.h>
#include <string.h>
#include "../../Sources/Core/Src/PhongThanScriptWire.h"
#define CHECK(x) do{if(!(x)){printf("FAIL line=%d\n",__LINE__);return 1;}}while(0)
int main(){
    KPhongThanScriptAction action,decoded;unsigned char bytes[8192];
    memset(&action,0,sizeof(action));action.Operation=PHONGTHAN_SCRIPT_SHOW;action.View=UI_ROULETTE;
    action.ServerOwned=1;action.MapId=1021;action.DialogToken=67;
    for(int i=0;i<12;++i){
        action.NumberArgument=i;unsigned int size=PhongThanBuildScriptPacket(action,bytes,sizeof(bytes));CHECK(size>0);
        CHECK(PhongThanReadScriptPacket(bytes,size,&decoded));CHECK(decoded.View==UI_ROULETTE && decoded.NumberArgument==i && decoded.DialogToken==67 && decoded.MapId==1021);
    }
    action.NumberArgument=12;CHECK(!PhongThanBuildScriptPacket(action,bytes,sizeof(bytes)));
    action.NumberArgument=-1;CHECK(!PhongThanBuildScriptPacket(action,bytes,sizeof(bytes)));
    action.NumberArgument=0;action.DialogToken=0;CHECK(!PhongThanBuildScriptPacket(action,bytes,sizeof(bytes)));
    action.DialogToken=67;action.OptionCount=1;CHECK(!PhongThanBuildScriptPacket(action,bytes,sizeof(bytes)));
    puts("PASS ROULETTE_WIRE outcomes=12 roundtrip=PASS malformed_outcome_missing_token_options=REJECTED");return 0;
}
