#ifndef PHONGTHAN_RELEASE_DIAGNOSTICS_H
#define PHONGTHAN_RELEASE_DIAGNOSTICS_H
#include "PhongThanDiagnostics.h"
// Standalone service processes do not link Engine. Preserve their Dev logs,
// but avoid creating diagnostic files in a secure release deployment.
#ifdef PT_SECURE_RELEASE_PROFILE
#include <stdio.h>
#include <string.h>
inline FILE* PtServiceReleaseFopen(const char* path,const char* mode){
 if(path&&(strstr(path,".log")||strstr(path,".jsonl")||strstr(path,"_diag")||strstr(path,"_trace")))return fopen("NUL",mode);
 return fopen(path,mode);
}
#define fopen PtServiceReleaseFopen
#else
#include "PhongThanDiagnostics.h"
#ifdef fopen
#undef fopen
#endif
#define fopen PtDiagnosticsFopen
#endif
#endif
