#ifndef PT_SERVER_PERFORMANCE_H
#define PT_SERVER_PERFORMANCE_H
#ifdef _SERVER
#include <windows.h>
#include <stdio.h>
#include <string.h>
enum PTPerfPart {PTPERF_TICK,PTPERF_WORLD,PTPERF_BOTS,PTPERF_ROUTE,PTPERF_TARGET,PTPERF_SAVE,PTPERF_DIAGNOSTIC,PTPERF_QUEST_MARKS,PTPERF_PLAYERS,PTPERF_RANKING,PTPERF_BOT_MOVEMENT,PTPERF_BOT_CONTROL,PTPERF_BOT_SOCIAL,PTPERF_BOT_INDEXES,PTPERF_BOT_CHAT,PTPERF_BOT_DECISIONS,PTPERF_BOT_STATUS,PTPERF_BOT_ALL_SAVES,PTPERF_CLIENT_INPUT,PTPERF_NPC_DIALOG,PTPERF_NPC_CHOICE,PTPERF_ALL_SAVES,PTPERF_ITEM_STAGE,PTPERF_LUA,PTPERF_PARTS};
struct PTServerPerformance {
 LARGE_INTEGER frequency;DWORD started;double sum[PTPERF_PARTS],maximum[PTPERF_PARTS];unsigned long count[PTPERF_PARTS],late;
 PTServerPerformance(){QueryPerformanceFrequency(&frequency);started=GetTickCount();Reset();}
 void Reset(){memset(sum,0,sizeof(sum));memset(maximum,0,sizeof(maximum));memset(count,0,sizeof(count));late=0;}
 void Add(int part,double value){sum[part]+=value;if(value>maximum[part])maximum[part]=value;++count[part];if(part==PTPERF_TICK&&value>1000.0/18.0)++late;}
 void Publish(){DWORD now=GetTickCount(),elapsed=now-started;if(elapsed<10000)return;FILE* f=fopen("server_performance.json.tmp","wb");if(f){static const char* names[]={"tick","world","bots","routes","target_selection","bot_saves","combat_diagnostics","quest_marks_ui","player_logic","ranking_refresh","bot_movement","bot_control","bot_social","bot_indexes","bot_chat","bot_decisions","bot_status","bot_all_saves","client_input","npc_dialog","npc_choice","all_character_saves","item_stage","lua"};fprintf(f,"{\"Version\":1,\"Pid\":%lu,\"SampleMilliseconds\":%lu,\"Ticks\":%lu,\"EffectiveFps\":%.3f,\"TargetFps\":18,\"OverBudgetTicks\":%lu,\"Parts\":[",GetCurrentProcessId(),elapsed,count[0],elapsed?count[0]*1000.0/elapsed:0.0,late);for(int i=0;i<PTPERF_PARTS;++i)fprintf(f,"%s{\"Name\":\"%s\",\"Calls\":%lu,\"TotalMs\":%.3f,\"AverageMs\":%.3f,\"MaximumMs\":%.3f}",i?",":"",names[i],count[i],sum[i],count[i]?sum[i]/count[i]:0.0,maximum[i]);fprintf(f,"]}");bool ok=!ferror(f);if(fclose(f))ok=false;if(ok)MoveFileExA("server_performance.json.tmp","server_performance.json",MOVEFILE_REPLACE_EXISTING);}Reset();started=now;}
};
extern PTServerPerformance g_PTServerPerformance;
class PTPerfScope {
 int part;LARGE_INTEGER start;
public:
 PTPerfScope(int p):part(p){QueryPerformanceCounter(&start);}
 ~PTPerfScope(){if(part<0)return;LARGE_INTEGER end;QueryPerformanceCounter(&end);g_PTServerPerformance.Add(part,(end.QuadPart-start.QuadPart)*1000.0/g_PTServerPerformance.frequency.QuadPart);if(part==PTPERF_TICK)g_PTServerPerformance.Publish();}
};
#endif
#endif
