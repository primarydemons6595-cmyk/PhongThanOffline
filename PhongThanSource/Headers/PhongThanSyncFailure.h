#ifndef PHONGTHAN_SYNC_FAILURE_H
#define PHONGTHAN_SYNC_FAILURE_H
#include <windows.h>
#include <stdio.h>
// Keep real socket failures visible with context, without flooding the console.
inline void PhongThanSyncFailure(const char* kind,int client,unsigned long entity,unsigned int size,HRESULT hr){
 static DWORD last=0,count=0;static bool reported=false;++count;DWORD now=GetTickCount();
 if(reported&&now-last<5000)return;
 printf("[SYNC_SEND_FAILED] kind=%s client=%d entity=%lu bytes=%u hr=0x%08lX failures=%lu\n",kind,client,entity,size,(unsigned long)hr,count);
 last=now;reported=true;count=0;
}
#endif
