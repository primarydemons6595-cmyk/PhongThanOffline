#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>
#include "../../Headers/PhongThanUiAttention.h"
#include "../../Headers/PhongThanNpcQuestMark.h"
enum {UI_NOTEINFO=1};
struct KPhongThanScriptAction {int View,NumberArgument,ServerOwned,MapId,ContentLength;char Key[128],Content[4096];};
static DWORD now=1000;
static DWORD Clock(){return now;}
#define GetTickCount Clock
#include "../../Sources/Core/Src/PhongThanNpcQuestMarks.inl"
#undef GetTickCount
int main(){
    PTUiAttention a;assert(!a.quest&&!a.friendAdded);
    a.quest=true;assert(a.Lit(a.quest,0)&&!a.Lit(a.quest,400)&&a.Lit(a.quest,800));
    a.friendAdded=true;a.quest=false;assert(a.friendAdded);a.Clear();assert(!a.friendAdded&&!a.quest);
    KPhongThanScriptAction p;memset(&p,0,sizeof(p));p.View=UI_NOTEINFO;p.NumberArgument=PT_NPC_QUEST_MARK_NOTE;
    p.ServerOwned=1;p.MapId=1002;strcpy(p.Key,PT_NPC_QUEST_MARK_KEY);
    strcpy(p.Content,"1:0:50000:100000;2:1:60000:120000;");p.ContentLength=strlen(p.Content);
    assert(PhongThanReceiveNpcQuestMarks(p));int x=0,y=0;
    assert(PhongThanGetMapQuestMark(0,1002,x,y)==0&&x==50000&&y==100000);
    assert(PhongThanGetMapQuestMark(1,1002,x,y)==1&&x==60000);
    assert(PhongThanGetNpcQuestMark(2,1002)==1);
    assert(PhongThanGetMapQuestMark(0,1003,x,y)==-1);
    const char* invalid[]={"1:1:-1:0;","1:1:10000001:0;","1:8:20:20;","1:0:20:20;1:1:30:30;","1:0:1:1","1:0;"};
    for(int i=0;i<6;++i){strcpy(p.Content,invalid[i]);p.ContentLength=strlen(p.Content);assert(PhongThanReceiveNpcQuestMarks(p));assert(PhongThanGetNpcQuestMark(2,1002)==1);}
    now+=6001;assert(PhongThanGetMapQuestMark(0,1002,x,y)==-1);
    p.ContentLength=0;p.Content[0]=0;assert(PhongThanReceiveNpcQuestMarks(p));assert(s_PTQuestMarkCount==0);
    puts("PASS quest map v2: positions/category, map scope, malformed/duplicate/coordinate rejection, atomic update/expiry; independent blinking flags and reset.");return 0;
}
