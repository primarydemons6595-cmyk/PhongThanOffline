#ifndef PHONGTHAN_ROLE_INI_H
#define PHONGTHAN_ROLE_INI_H
// Character templates must use the authenticated resource reader in Release.
// Keep the existing Windows profile reader for the developer deployment.
#ifdef PT_SECURE_RELEASE_PROFILE
#include "../Sources/Engine/Src/KWin32.h"
#include "../Sources/Engine/Src/KIniFile.h"
#include "../Sources/Engine/Src/KFilePath.h"
class PhongThanRoleIni {
    KIniFile ini;
public:
    bool SetFile(const char* path) { return ini.Load(path) != FALSE; }
    int ReadInteger(const char* section,const char* key,int fallback) {
        int value=fallback; ini.GetInteger(section,key,fallback,&value); return value;
    }
    void ReadSections(OnlineGameLib::Win32::CIniFile::_VETSTR& sections) {
        char current[256]="", next[256];
        while(ini.GetNextSection(current,next)) {
            sections.push_back(next[0]=='[' ? std::string(next+1,strlen(next)-2) : std::string(next)); strcpy(current,next);
        }
    }
};
#else
typedef OnlineGameLib::Win32::CIniFile PhongThanRoleIni;
#endif
#endif
