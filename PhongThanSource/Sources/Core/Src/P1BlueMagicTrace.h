// P1 isolated-world diagnostics only. No selection/generation/persistence logic.
#ifndef PT_P1_BLUE_MAGIC_TRACE_H
#define PT_P1_BLUE_MAGIC_TRACE_H
#include "../../../Headers/PhongThanDiagnostics.h"
#include <stdio.h>
#include <string.h>
#include "NativeDropRolloutScope.h"
#ifndef _SERVER
#include "P1BlueMainClientTraceScope.h"
#endif
inline void P1BlueTrace(const char* event,KItem& item)
{
    if(PtDiagnostics::Quiet())return;
    char root[MAX_PATH];DWORD n=GetEnvironmentVariableA("PHONGTHAN_P1_BLUE_ROOT",root,sizeof(root));
    bool mainRollout=false;
#ifdef _SERVER
    if(!n){std::string admitted,error;if(NativeDropRollout::Root(admitted,error)==1&&admitted.size()<sizeof(root)){strcpy(root,admitted.c_str());n=admitted.size();mainRollout=true;}}
#else
    if(!n&&P1MainClientTraceScope(root,sizeof(root))){n=strlen(root);mainRollout=true;}
#endif
    if(!n||n>=sizeof(root))return;
    if(!mainRollout){
        for(DWORD i=0;i<n;++i)if(root[i]=='/')root[i]='\\';
        if(!strstr(root,"\\PhongThanRuntime-Loose\\P0-BlueMagic-Isolated\\p1_")||n+20>=MAX_PATH)return;
        char marker[MAX_PATH];strcpy(marker,root);strcat(marker,"\\p0_isolated.flag");
        if(GetFileAttributesA(marker)==(DWORD)-1)return;
    }
    FILE* f=fopen("p1_blue_magic.jsonl","a");if(!f)return;
    static unsigned long sequence=0;
    fprintf(f,"{\"event\":\"%s\",\"pid\":%lu,\"seq\":%lu,\"serial\":%lu,\"quality\":%d,\"level\":%d,\"durability\":%d,\"magic\":[",event,GetCurrentProcessId(),++sequence,item.GetID(),item.GetQuality(),item.GetLevel(),item.GetDurability());
    for(int a=0;a<MAX_ITEM_MAGICATTRIB;++a){KItemNormalAttrib& v=item.m_aryMagicAttrib[a];fprintf(f,"%s[%d,%d,%d,%d]",a?",":"",v.nAttribType,v.nValue[0],v.nValue[1],v.nValue[2]);}
    fprintf(f,"],\"requirements\":[");for(int r=0;r<6;++r){KItemNormalAttrib* v=(KItemNormalAttrib*)item.GetRequirement(r);fprintf(f,"%s[%d,%d,%d,%d]",r?",":"",v->nAttribType,v->nValue[0],v->nValue[1],v->nValue[2]);}fprintf(f,"]}\n");fclose(f);
}
#endif
