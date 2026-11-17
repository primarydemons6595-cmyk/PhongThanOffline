#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "KWin32.h"
#include "KLuaScript.h"
#include "KStrBase.h"
#include "PhongThanNpcDialogRules.h"
#define MAX_ANSWERNUM 20
#define MAX_SCIRPTACTION_BUFFERNUM 4096
static int presentation,count,numeric,promptId;
static char options[20][160],prompt[4096];
static int GetPlayerIndex(Lua_State*) { return 1; }
static int LuaSelectUIWithPresentation(Lua_State* L,int mode) {
    presentation=mode;count=(int)Lua_ValueToNumber(L,2);
    numeric=Lua_IsNumber(L,1);promptId=numeric?(int)Lua_ValueToNumber(L,1):0;
    if(!numeric)g_StrCpyLen(prompt,Lua_ValueToString(L,1),sizeof(prompt));
    for(int i=0;i<count;++i)g_StrCpyLen(options[i],Lua_ValueToString(L,i+3),sizeof(options[i]));
    return 0;
}
#include "../../Sources/Core/Src/PhongThanNpcDialogLua.inl"
#define CHECK(x) do{if(!(x)){printf("FAIL line=%d %s\n",__LINE__,#x);return 1;}}while(0)
int main(){
    char eventOption[]="Chon cong thuc / thao tac/ce_recipes";
    CHECK(!strcmp(PhongThanDialogCallbackSeparator(eventOption,PHONGTHAN_DIALOG_BUTTONS)+1,"ce_recipes"));
    CHECK(!strcmp(PhongThanDialogCallbackSeparator(eventOption,0)+1," thao tac/ce_recipes"));
    KLuaScript script;CHECK(script.Init());
    TLua_Funcs api[]={{"SayTask",LuaSayTaskCompat},{"MsgBox",LuaMsgBoxCompat}};
    CHECK(script.RegisterFunctions(api,2));
    char source[]="function task() SayTask(11079,{{'hidden','skip';show=0},{'Xoay','spin';show=1},{'Tim hieu','info'},{'Chi nhan','paper';show=1}}) end\n"
        "function confirm() MsgBox('question','accept','reject') end\n"
        "function info() MsgBox(123,'close') end\n";
    CHECK(script.LoadBuffer((unsigned char*)source,strlen(source)));CHECK(script.ExecuteCode());
    CHECK(script.CallFunction("task",0,""));
    CHECK(PhongThanDialogUsesButtons(presentation,0));CHECK(numeric && promptId==11079 && count==3);
    CHECK(!strcmp(options[0],"Xoay/spin") && !strcmp(options[1],"Tim hieu/info") && !strcmp(options[2],"Chi nhan/paper"));
    CHECK(script.CallFunction("confirm",0,""));CHECK(count==2 && !numeric && !strcmp(prompt,"question"));
    CHECK(!strcmp(options[0],"OK/accept") && !strcmp(options[1],"Cancel/reject"));
    CHECK(script.CallFunction("info",0,""));CHECK(count==1 && numeric && promptId==123);
    CHECK(!PhongThanDialogUsesButtons(0,0)); // Existing Say/teleport packets remain lists.
    CHECK(!PhongThanDialogUsesButtons(PHONGTHAN_DIALOG_BUTTONS,1)); // Portrait frame field is not a layout hint.
    CHECK(!PhongThanDialogUsesButtons(5,0));
    puts("PASS NPC_DIALOG_PRESENTATION: real Lua SayTask visibility/order/resource, MsgBox callbacks, ordinary Say/portrait list compatibility.");
    return 0;
}
