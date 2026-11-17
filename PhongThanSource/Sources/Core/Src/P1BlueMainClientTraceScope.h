// Diagnostics only: permits the already accepted P1 trace on paired Main Loose.
#ifndef PT_P1_MAIN_CLIENT_TRACE_SCOPE_H
#define PT_P1_MAIN_CLIENT_TRACE_SCOPE_H
inline bool P1MainClientTraceScope(char* root,size_t capacity){
 char exe[MAX_PATH],cwd[MAX_PATH];if(!GetModuleFileNameA(0,exe,sizeof(exe))||!GetCurrentDirectoryA(sizeof(cwd),cwd))return false;
 char* slash=strrchr(exe,'\\');if(!slash||_stricmp(slash+1,"Game.exe"))return false;*slash=0;
 const char*suffix="\\PhongThanRuntime-Loose\\Client";size_t size=strlen(exe),tail=strlen(suffix);
 if(size<tail||_stricmp(exe+size-tail,suffix)||_stricmp(exe,cwd))return false;
 char server[MAX_PATH],file[MAX_PATH];if(size+80>=MAX_PATH||size+40>=capacity)return false;
 strcpy(server,exe);server[size-7]=0;strcat(server,"\\Server");strcpy(root,server);strcat(root,"\\NativeDropPersistenceV1");
 sprintf(file,"%s\\native_drop_persistence_v1.required",server);if(GetFileAttributesA(file)==(DWORD)-1)return false;
 sprintf(file,"%s\\rollout-v1.approved",root);FILE*f=fopen(file,"rb");char bytes[256];size_t n=0;if(f){n=fread(bytes,1,sizeof(bytes),f);fclose(f);}
 const char*expected="NATIVE_DROP_MAIN_LOOSE_V1\nCompanionVersion=1\nCOPY_COMPATIBILITY_TESTS=PASS\n";
 if(n!=strlen(expected)||memcmp(bytes,expected,n))return false;
 sprintf(file,"%s\\serial-reservation.v1",root);f=fopen(file,"rb");if(!f)return false;DWORD v[4];bool ok=fread(v,1,sizeof(v),f)==sizeof(v)&&fgetc(f)==EOF;fclose(f);
 return ok&&v[0]==0x3153444eUL&&v[1]==1&&v[2]>=100&&v[2]<0xfffffffeUL&&v[3]==(v[0]^v[1]^v[2]^0xa55a5aa5UL);
}
#endif
