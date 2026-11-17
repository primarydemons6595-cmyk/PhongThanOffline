#include "../../Headers/PhongThanDiagnostics.h"
#include <assert.h>
static bool Exists(const char* p){return GetFileAttributesA(p)!=(DWORD)-1;}
int main(int argc,char** argv){
 assert(argc==2);bool enabled=!strcmp(argv[1],"on");assert(PtDiagnostics::Current().scoped&&PtDiagnostics::Current().enabled==enabled);
 const char* traces[]={"revive_identity_diag.log","loose_missing_files.log","movement_trace_123.jsonl","normal_shop_loader_diag.csv","ui_action_diag.log"};
 for(int i=0;i<5;++i){FILE* f=PtDiagnosticsFopen(traces[i],"ab");assert(f);fputs("trace\n",f);fclose(f);assert(Exists(traces[i])==enabled);}
 FILE* ready=PtDiagnosticsFopen("native_world_ready.log","ab");assert(ready);fputs("READY\n",ready);fclose(ready);assert(Exists("native_world_ready.log"));
 assert(CreateDirectoryA("CharacterStore",0));FILE* data=PtDiagnosticsFopen("CharacterStore\\transaction.log","wb");assert(data);fputs("persistent-data",data);fclose(data);assert(Exists("CharacterStore\\transaction.log"));
 data=PtDiagnosticsFopen("role.pthc","wb");assert(data);fputs("character-data",data);fclose(data);assert(Exists("role.pthc"));
 assert(CreateDirectoryA("NativeDropPersistenceV1",0));data=PtDiagnosticsFopen("NativeDropPersistenceV1\\internal.log","wb");assert(data);fputs("state",data);fclose(data);assert(Exists("NativeDropPersistenceV1\\internal.log"));
 // A debug-file read remains a real read even while writes are suppressed.
 FILE* raw=fopen("old_trace.log","wb");assert(raw);fputs("old",raw);fclose(raw);raw=PtDiagnosticsFopen("old_trace.log","rb");assert(raw&&fgetc(raw)=='o');fclose(raw);
 char block[4096];memset(block,'x',sizeof(block));for(i=0;i<100;++i){FILE* f=PtDiagnosticsFopen("runtime_errors.log","ab");assert(f);assert(fwrite(block,1,sizeof(block),f)==sizeof(block));fclose(f);}
 WIN32_FILE_ATTRIBUTE_DATA a,b;assert(GetFileAttributesExA("runtime_errors.log",GetFileExInfoStandard,&a)&&GetFileAttributesExA("runtime_errors.log.1",GetFileExInfoStandard,&b));
 assert(a.nFileSizeLow<=65536&&b.nFileSizeLow<=65536&&!Exists("runtime_errors.log.2"));
 printf("PASS quiet=%d: trace files %s, caller stream compatibility, read paths and character/companion data preserved, critical error log bounded to two 64KB files\n",!enabled,enabled?"enabled explicitly":"not created");return 0;
}
