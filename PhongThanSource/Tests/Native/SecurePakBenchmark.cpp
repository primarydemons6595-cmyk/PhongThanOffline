#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>
#define ENGINE_API __declspec(dllimport)
#include "../../Sources/Engine/Src/PtRelease.h"
struct Memory {DWORD cb,faults,peakWorking,working,peakPaged,paged,peakNonpaged,nonpaged,pagefile,peakPagefile;};
typedef BOOL (WINAPI *GetMemory)(HANDLE,Memory*,DWORD);
static double Millis(LARGE_INTEGER a,LARGE_INTEGER b,LARGE_INTEGER frequency){return (double)(b.QuadPart-a.QuadPart)*1000/frequency.QuadPart;}
int main(int argc,char** argv){
 SetErrorMode(SEM_FAILCRITICALERRORS|SEM_NOGPFAULTERRORBOX);if(argc<2)return 2;
 // Read the external benchmark list through the caller's own CRT before mount.
 FILE* list=fopen(argv[1],"rb");if(!list)return 3;std::vector<std::string> paths;char line[512];while(fgets(line,sizeof(line),list)){size_t n=strlen(line);while(n&&(line[n-1]=='\n'||line[n-1]=='\r'))line[--n]=0;if(n)paths.push_back(line);}fclose(list);
 LARGE_INTEGER frequency,start,mounted,done;QueryPerformanceFrequency(&frequency);QueryPerformanceCounter(&start);
 if(!PtReleaseMount())return 4;QueryPerformanceCounter(&mounted);unsigned bytes=0,failures=0;char buffer[4096];
 for(unsigned i=0;i<paths.size();++i){FILE* file=fopen(paths[i].c_str(),"rb");if(!file){++failures;continue;}bytes+=fread(buffer,1,sizeof(buffer),file);if(ferror(file))++failures;fclose(file);}
 QueryPerformanceCounter(&done);Memory memory;memset(&memory,0,sizeof(memory));memory.cb=sizeof(memory);HMODULE psapi=LoadLibraryA("psapi.dll");if(psapi){GetMemory read=(GetMemory)GetProcAddress(psapi,"GetProcessMemoryInfo");if(read)read(GetCurrentProcess(),&memory,sizeof(memory));}
 printf("{\"mount_ms\":%.3f,\"read_ms\":%.3f,\"files\":%u,\"bytes\":%u,\"failures\":%u,\"peak_working_bytes\":%lu}\n",Millis(start,mounted,frequency),Millis(mounted,done,frequency),paths.size(),bytes,failures,memory.peakWorking);
 return failures?5:0;
}
