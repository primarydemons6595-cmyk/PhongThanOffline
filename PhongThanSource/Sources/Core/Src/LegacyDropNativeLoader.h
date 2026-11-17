#ifndef PT_LEGACY_DROP_NATIVE_LOADER_H
#define PT_LEGACY_DROP_NATIVE_LOADER_H
#include "KPakFile.h"
#include "LegacyDropEngine.h"
class LegacyDropNativeLoader {
public:
    bool Load(const std::string& path,std::string& bytes,std::string& error) {
        bytes="";KPakFile file;if(!file.Open(path.c_str())){error="MISSING_TABLE";return false;}
        DWORD length=file.Size();if(length>2*1024*1024){error="INPUT_TOO_LARGE";return false;}
        if(!length){error="EMPTY_TABLE";return false;}
        std::vector<char> data(length);if(file.Read(&data[0],length)!=length){error="SHORT_READ";return false;}
        bytes.assign(&data[0],length);error="";return true;
    }
};
#endif
