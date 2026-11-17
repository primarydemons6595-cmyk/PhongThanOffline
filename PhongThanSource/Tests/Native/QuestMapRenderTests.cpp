#include <windows.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "../../Headers/PhongThanNpcQuestMark.h"
#include "../../Headers/PhongThanQuestMarkSprites.h"
#include "../../Sources/Represent/iRepresent/KRepresentUnit.h"
enum {UI_NOTEINFO=1};
struct KPhongThanScriptAction {int View,NumberArgument,ServerOwned,MapId,ContentLength;char Key[128],Content[4096];};
static DWORD now=1000;
static DWORD Clock(){return now;}
#define GetTickCount Clock
#include "../../Sources/Core/Src/PhongThanNpcQuestMarks.inl"
#undef GetTickCount
struct Recorder {int count;KRUImage books[128];void DrawPrimitives(int n,KRUImage* b,int type,bool screen){assert(n==1&&type==RU_T_IMAGE_STRETCH&&screen);books[count++]=*b;}};
Recorder recorder;Recorder* g_pRepresent=&recorder;
#include "../../Sources/Core/Src/Scene/PhongThanMapQuestMarks.inl"
int main(){
 KPhongThanScriptAction a;memset(&a,0,sizeof(a));a.View=UI_NOTEINFO;a.NumberArgument=PT_NPC_QUEST_MARK_NOTE;a.ServerOwned=1;a.MapId=1002;strcpy(a.Key,PT_NPC_QUEST_MARK_KEY);
 // Main, daily, already active, reward (main and side), and a remote available quest.
 strcpy(a.Content,"1:0:16000:64000;2:1:17600:67200;3:2:17600:67200;4:3:17600:67200;5:1:32000:128000;6:4:18000:68000;");a.ContentLength=strlen(a.Content);assert(PhongThanReceiveNpcQuestMarks(a));
 RECT miniView={950,1950,1250,2250};PhongThanPaintMapQuestMarks(1200,20,1002,miniView,16,32);
 assert(recorder.count==5);assert(!strcmp(recorder.books[0].szImage,s_PTQuestMarkSprites[0]));assert(!strcmp(recorder.books[1].szImage,s_PTQuestMarkSprites[1]));assert(recorder.books[0].oPosition.nX==1242&&recorder.books[0].oPosition.nY==62);
 assert(recorder.books[0].oEndPos.nX-recorder.books[0].oPosition.nX==16&&recorder.books[0].oEndPos.nY-recorder.books[0].oPosition.nY==16);
 assert(!strcmp(recorder.books[2].szImage,s_PTQuestMarkSprites[2]));
 assert(!strcmp(recorder.books[3].szImage,s_PTQuestMarkSprites[3]));
 assert(!strcmp(recorder.books[4].szImage,s_PTQuestMarkSprites[4]));
 RECT expanded={900,1900,2100,4100};recorder.count=0;PhongThanPaintMapQuestMarks(20,20,1002,expanded,16,32);assert(recorder.count==6);
 recorder.count=0;PhongThanPaintMapQuestMarks(20,20,1003,expanded,16,32);assert(!recorder.count);now+=6001;PhongThanPaintMapQuestMarks(20,20,1002,expanded,16,32);assert(!recorder.count);
 puts("PASS actual map painter + network cache: mini/expanded viewports, all five available/accepted-in-progress/ready-to-turn-in variants, gold main/blue daily-side, remote NPC positions, current-map scope, clipping and expiry.");return 0;
}
