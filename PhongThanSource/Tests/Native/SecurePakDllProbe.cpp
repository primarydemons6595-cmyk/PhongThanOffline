// /MT caller against the real /MD Engine.dll; validates CRT ownership too.
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#define ENGINE_API __declspec(dllimport)
#include "../../Sources/Engine/Src/PtRelease.h"
static int Fault(EXCEPTION_POINTERS* p){fprintf(stderr,"FAULT address=%p engine=%p code=%08lX\n",p->ExceptionRecord->ExceptionAddress,GetModuleHandleA("Engine.dll"),p->ExceptionRecord->ExceptionCode);return EXCEPTION_EXECUTE_HANDLER;}
static bool Mount(){__try{return PtReleaseMount();}__except(Fault(GetExceptionInformation())){return false;}}
int main(int argc,char** argv){
 SetErrorMode(SEM_FAILCRITICALERRORS|SEM_NOGPFAULTERRORBOX);
 __try {
 if(argc<2)return 2;if(!Mount()){puts("MOUNT_REJECTED");return 3;}
 FILE* f=fopen(argv[1],"rb");if(!f){puts("RESOURCE_REJECTED");return 4;}
 unsigned count=0;unsigned char bytes[32768];size_t n;
 while((n=fread(bytes,1,sizeof(bytes),f))!=0)count+=n;
 bool bad=ferror(f)!=0;fclose(f);if(bad){puts("AUTHENTICATION_REJECTED");return 5;}
 printf("READ_OK bytes=%u\n",count);
 if(argc>2){FILE* real=fopen(argv[2],"rb");if(!real)return 6;bool virtualStream=PtReleaseIsVirtual(real);int first=fgetc(real);fclose(real);if(virtualStream||first==EOF)return 7;puts("CALLER_CRT_OK");}
 return 0;
 } __except(Fault(GetExceptionInformation())) { return 10; }
}
