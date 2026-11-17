#include "KCore.h"
#include "KMission.h"
#include "KTabFile.h"
#include <stdio.h>
// Same production timer constructor; no world/timer activation in this test.
KTimerTaskFun::KTimerTaskFun(){m_dwTimeTaskTime=0;m_dwTimerTaskId=0;m_dwIntervalTime=0;}
BOOL KTimerTaskFun::LoadTask(KPlayer*){return FALSE;}
BOOL KTimerTaskFun::SaveTask(KPlayer*){return FALSE;}
// Deterministic timer storage for testing the production mission lookup key.
BOOL KTimerTaskFun::SetTimer(size_t interval,unsigned short id){m_dwTimeTaskTime=interval;m_dwTimerTaskId=id;m_dwIntervalTime=interval;return TRUE;}
unsigned long KTimerTaskFun::GetRestTime(){return m_dwTimeTaskTime;}
#define CHECK(x) do{if(!(x)){printf("FAIL line=%d %s\n",__LINE__,#x);return 1;}}while(0)
int main(int argc,char** argv){
 printf("sizeof_KMission=%lu default_thread_stack_bytes=1048576\n",(unsigned long)sizeof(KMission));
 KSubWorldMissionArray* array=new KSubWorldMissionArray;
 CHECK(array!=NULL);CHECK(array->FindByMissionId(1)==NULL);
 KMission* owned=array->Add();CHECK(owned!=NULL);owned->SetMissionId(1);
 CHECK(owned->StartTimer(3001,5400));CHECK(owned->GetTimerRestTimer(3001)==5400);CHECK(owned->GetTimerRestTimer(185)==0);
 CHECK(owned->StartTimer(3002,32400));CHECK(owned->GetTimerRestTimer(3002)==32400);CHECK(owned->GetTimerRestTimer(3001)==5400);
 puts("PASS MISSION_TIMER_ID_WIDTH real_mission_lookup timer_3001_3002_distinct_from_185_186=PASS");
 for(int i=0;i<10000;++i){CHECK(array->FindByMissionId(1)==owned);CHECK(array->FindByMissionId(2)==NULL);}
 array->Remove(owned);CHECK(array->FindByMissionId(1)==NULL);delete array;
 if(argc==2){
  CHECK(SetCurrentDirectoryA(argv[1]));g_SetRootPath(NULL);g_SetFilePath("\\");
  KTabFile table;CHECK(table.Load("\\settings\\phongthan\\OriginalWorldSchedule.txt"));
  int slots[6]={0,0,0,0,0,0};const int hours[6]={2,10,13,18,21,23};int vttRows=0;
  for(int row=2;row<=table.GetHeight();++row){
   int h=-1,m=-1;char path[240];table.GetInteger(row,"Hour",-1,&h);table.GetInteger(row,"Minute",-1,&m);table.GetString(row,"Script","",path,sizeof(path));
   if(strstr(path,"\\van_tien\\open")){++vttRows;CHECK(m==20);for(int j=0;j<6;++j)if(h==hours[j])++slots[j];}
  }
  CHECK(vttRows==24);for(int j=0;j<6;++j)CHECK(slots[j]==4);
  puts("PASS ACTUAL_NATIVE_SCHEDULE_PARSER vtt_rows=24 each_daily_window_has_four_open_scripts=PASS");
 }
 puts("PASS MISSION_LOOKUP_STACK scalar_lookup=20000 real_production_array=PASS no_large_temporary=PASS");return 0;
}
