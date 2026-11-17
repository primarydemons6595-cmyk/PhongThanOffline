#ifndef PT_LEGACY_DROP_DEBUG_LOGGER_H
#define PT_LEGACY_DROP_DEBUG_LOGGER_H
#include "../../../Headers/PhongThanDiagnostics.h"
#include "LegacyDropEngine.h"
#include "NativeDropRelease.h"
#include <windows.h>
#ifndef INVALID_FILE_ATTRIBUTES
#define INVALID_FILE_ATTRIBUTES ((DWORD)-1)
#endif
// Fixed destination, bounded rotation, byte-preserving JSON. Never formats a
// name as a printf format and never records account credentials/names.
class LegacyDropDebugLogger {
    std::string path;
    unsigned long limit;
    unsigned long sequence;
    CRITICAL_SECTION lock;
    LegacyDropDebugLogger(const LegacyDropDebugLogger&);
    LegacyDropDebugLogger& operator=(const LegacyDropDebugLogger&);
    struct Guard {CRITICAL_SECTION* lock;Guard(CRITICAL_SECTION* p):lock(p){EnterCriticalSection(lock);}~Guard(){LeaveCriticalSection(lock);}};
public:
    LegacyDropDebugLogger(const char* file="legacy_drop_debug.jsonl",unsigned long bytes=10*1024*1024):path(file),limit(bytes),sequence(0){InitializeCriticalSection(&lock);}
    ~LegacyDropDebugLogger(){DeleteCriticalSection(&lock);}
    static std::string Quote(const std::string& s){
        const char* hex="0123456789abcdef";std::string out="\"";
        for(size_t i=0;i<s.size();++i){unsigned char c=(unsigned char)s[i];
            if(c=='"'||c=='\\'){out+='\\';out+=(char)c;}
            else if(c<32||c>=127){out+="\\u00";out+=hex[c>>4];out+=hex[c&15];}
            else out+=(char)c;
        }return out+'"';
    }
    bool Write(const char* event,const std::string& death,const LegacyDrop::Fields& fields){
        if(PtDiagnostics::Quiet())return true;
        try {
        Guard guard(&lock);
        WIN32_FILE_ATTRIBUTE_DATA a;
        if(GetFileAttributesExA(path.c_str(),GetFileExInfoStandard,&a)&&
            (a.nFileSizeHigh||a.nFileSizeLow>=limit)){
            std::string last=path+".4";if(GetFileAttributesA(last.c_str())!=INVALID_FILE_ATTRIBUTES&&!DeleteFileA(last.c_str()))return false;
            for(int n=3;n>=1;--n){char from[8],to[8];sprintf(from,".%d",n);sprintf(to,".%d",n+1);
                std::string src=path+from,dst=path+to;
                if(GetFileAttributesA(src.c_str())!=INVALID_FILE_ATTRIBUTES&&!MoveFileA(src.c_str(),dst.c_str()))return false;}
            if(!MoveFileA(path.c_str(),(path+".1").c_str()))return false;
        }
        SYSTEMTIME utc;GetSystemTime(&utc);char stamp[48],pid[32],seq[32];sprintf(stamp,"%04d-%02d-%02dT%02d:%02d:%02d.%03dZ",utc.wYear,utc.wMonth,utc.wDay,utc.wHour,utc.wMinute,utc.wSecond,utc.wMilliseconds);sprintf(pid,"%lu",GetCurrentProcessId());sprintf(seq,"%lu",++sequence);
        std::string line="{\"event\":"+Quote(event)+",\"death_id\":"+Quote(death)+",\"utc\":"+Quote(stamp)+",\"pid\":"+Quote(pid)+",\"log_seq\":"+Quote(seq)+",\"test_kind\":\"FUNCTIONAL_TEST\",\"parity\":\"KINGSOFT_PARITY_UNVERIFIED\",\"policy_status\":\"EXPERIMENTAL\"";
        if(NativeDropRelease::Active())line="{\"event\":"+Quote(event)+",\"death_id\":"+Quote(death)+",\"utc\":"+Quote(stamp)+",\"pid\":"+Quote(pid)+",\"log_seq\":"+Quote(seq)+",\"engine\":\"LOOSE_NATIVE_DROP_V1\",\"test_kind\":\"RUNTIME\",\"parity\":\"NOT_CLAIMED\",\"policy_status\":\"OFFICIAL\"";
        for(LegacyDrop::Fields::const_iterator i=fields.begin();i!=fields.end();++i)line+=","+Quote(i->first)+":"+Quote(i->second);
        line+="}\n";
        FILE* f=fopen(path.c_str(),"ab");if(!f)return false;
        bool ok=fwrite(line.data(),1,line.size(),f)==line.size();if(fflush(f))ok=false;if(fclose(f))ok=false;return ok;
        } catch(...) {return false;}
    }
};
#endif
