#include <windows.h>
#include <stdio.h>
#include <vector>
#include "KWin32.h"
#include "KLuaScript.h"
int main(int argc,char** argv){if(argc!=2)return 2;FILE* list=fopen(argv[1],"r");if(!list)return 3;char path[1024];int count=0;
while(fgets(path,sizeof(path),list)){int length=strlen(path);while(length&&(path[length-1]=='\r'||path[length-1]=='\n'))path[--length]=0;if(!length)continue;
FILE* f=fopen(path,"rb");if(!f){printf("MISSING %s\n",path);return 4;}fseek(f,0,SEEK_END);int size=ftell(f);rewind(f);std::vector<unsigned char> bytes(size);if(fread(&bytes[0],1,size,f)!=(unsigned)size)return 5;fclose(f);
KLuaScript s;if(!s.Init()||!s.LoadBuffer(&bytes[0],size)||!s.ExecuteCode()){printf("FAIL %s\n",path);return 6;}++count;}
fclose(list);printf("PASS LUA4_STAGED_SYNTAX files=%d actual_payloads=1\n",count);return 0;}
