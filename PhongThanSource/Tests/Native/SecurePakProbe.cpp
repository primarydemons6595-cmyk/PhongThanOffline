// Build with VC6: the exact reader used in Engine, without graphics libraries.
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#define ENGINE_API
#define PT_RELEASE_IMPLEMENTATION
#include "../../Sources/Engine/Src/PtRelease.h"
#include "../../Sources/Engine/Src/PtRelease.inl"
int main(int argc,char** argv){
 if(argc<2)return 2;
 if(!PtReleaseMount()){puts("MOUNT_REJECTED");return 3;}
 FILE* f=PtReleaseFopen(argv[1],"rb");if(!f){puts("RESOURCE_REJECTED");return 4;}
 unsigned long count=0;unsigned char buffer[16384];size_t n;
 while((n=PtReleaseFread(buffer,1,sizeof(buffer),f))!=0)count+=n;
 bool error=PtReleaseFerror(f)!=0;PtReleaseFclose(f);
 if(error){puts("AUTHENTICATION_REJECTED");return 5;}
 printf("READ_OK bytes=%lu\n",count);return argc>2&&count!=strtoul(argv[2],NULL,10)?6:0;
}
