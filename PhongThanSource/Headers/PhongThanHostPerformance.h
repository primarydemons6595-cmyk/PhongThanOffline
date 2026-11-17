#ifndef PHONGTHAN_HOST_PERFORMANCE_H
#define PHONGTHAN_HOST_PERFORMANCE_H
#include <windows.h>
#include <stdio.h>
#include <string.h>
enum PTHostPart {PTHOST_MESSAGES,PTHOST_GATEWAY,PTHOST_DATABASE,PTHOST_RELAY,PTHOST_CLIENTS,PTHOST_MAIN,PTHOST_SAVES,PTHOST_LOGOUT,PTHOST_TRANSFER,PTHOST_CORE,PTHOST_FRAME_WORK,PTHOST_FRAME_GAP,PTHOST_PARTS};
struct PTHostStat {
 unsigned long count,bins[257];double total,maximum;
 void Clear(){memset(this,0,sizeof(*this));}
 void Add(double ms){++count;total+=ms;if(ms>maximum)maximum=ms;int bin=(int)ms;if(bin<0)bin=0;if(bin>256)bin=256;++bins[bin];}
 double Percentile(unsigned int percent){unsigned long goal=(count*percent+99)/100,seen=0;if(!goal)return 0;for(int i=0;i<257;++i){seen+=bins[i];if(seen>=goal)return i==256?maximum:i+1.0;}return maximum;}
};
struct PTHostPerformance {
 LARGE_INTEGER frequency;PTHostStat parts[PTHOST_PARTS];DWORD started;double pendingInput,lastFrame;
 unsigned long packets,yields,late;
 PTHostPerformance():pendingInput(0),lastFrame(0),packets(0),yields(0),late(0){QueryPerformanceFrequency(&frequency);started=GetTickCount();for(int i=0;i<PTHOST_PARTS;++i)parts[i].Clear();}
 double Now(){LARGE_INTEGER at;QueryPerformanceCounter(&at);return at.QuadPart*1000.0/frequency.QuadPart;}
 void Add(int part,double elapsed){parts[part].Add(elapsed);if(part==PTHOST_MESSAGES)pendingInput+=elapsed;}
 void FinishFrame(){double now=Now();double work=pendingInput;pendingInput=0;parts[PTHOST_FRAME_WORK].Add(work);if(work>1000.0/18.0)++late;if(lastFrame)parts[PTHOST_FRAME_GAP].Add(now-lastFrame);lastFrame=now;}
 void Publish(){DWORD now=GetTickCount(),elapsed=now-started;if(elapsed<10000)return;
  FILE* f=fopen("server_host_performance.json.tmp","wb");if(f){
   static const char* names[]={"message_loop","gateway","database","relay","client_input","main_loop","save_requests","logout","transfers","core_breathe","frame_work","frame_gap"};
   fprintf(f,"{\"Version\":1,\"Pid\":%lu,\"Milliseconds\":%lu,\"ClientPackets\":%lu,\"InputBudgetYields\":%lu,\"OverBudgetFrames\":%lu,\"Parts\":[",GetCurrentProcessId(),elapsed,packets,yields,late);
   for(int i=0;i<PTHOST_PARTS;++i){PTHostStat& s=parts[i];fprintf(f,"%s{\"Name\":\"%s\",\"Calls\":%lu,\"TotalMs\":%.3f,\"AverageMs\":%.3f,\"MaximumMs\":%.3f,\"P50UpperMs\":%.3f,\"P95UpperMs\":%.3f,\"P99UpperMs\":%.3f}",i?",":"",names[i],s.count,s.total,s.count?s.total/s.count:0,s.maximum,s.Percentile(50),s.Percentile(95),s.Percentile(99));}
   fprintf(f,"]}");bool ok=!ferror(f);if(fclose(f))ok=false;if(ok)MoveFileExA("server_host_performance.json.tmp","server_host_performance.json",MOVEFILE_REPLACE_EXISTING);
  }
  for(int i=0;i<PTHOST_PARTS;++i)parts[i].Clear();started=now;packets=yields=late=0;
 }
};
static PTHostPerformance ptHostPerformance;
class PTHostScope {
 int part;double began;
public:PTHostScope(int p):part(p),began(ptHostPerformance.Now()){}
 ~PTHostScope(){double elapsed=ptHostPerformance.Now()-began;ptHostPerformance.Add(part,elapsed);if(part==PTHOST_MAIN)ptHostPerformance.pendingInput+=elapsed;}
};
#endif
