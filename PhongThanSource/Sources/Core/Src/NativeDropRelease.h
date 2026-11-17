#ifndef PT_NATIVE_DROP_RELEASE_H
#define PT_NATIVE_DROP_RELEASE_H
#include <windows.h>
#include <string.h>
namespace NativeDropRelease {
// Independent ownership latch: corrupt/off configuration must never revive
// NpcDropPolicy.ini. Companion admission remains independently enforced by P0.
inline bool Active(){
 #if defined(KWin32_H) && !defined(_STANDALONE)
 if(PtReleaseEnabled()) return PtReleaseMount();
 #endif
 char exe[MAX_PATH],cwd[MAX_PATH];
 if(!GetModuleFileNameA(0,exe,sizeof(exe))||!GetCurrentDirectoryA(sizeof(cwd),cwd))return false;
 char* slash=strrchr(exe,'\\');if(!slash||_stricmp(slash+1,"GameServer.exe"))return false;*slash=0;
 const char* suffix="\\PhongThanRuntime-Loose\\Server";int a=strlen(exe),b=strlen(suffix);
 return a>=b&&!_stricmp(exe+a-b,suffix)&&!_stricmp(exe,cwd)&&
  GetFileAttributesA("loose_only.flag")!=(DWORD)-1&&
  GetFileAttributesA("native_drop_official_v1.required")!=(DWORD)-1;
}
inline const char* ConfigPath(){return Active()?"settings\\phongthan\\NativeDrop.ini":"settings\\phongthan\\LegacyDropExperiment.ini";}
inline const char* BluePath(){return Active()?"settings\\phongthan\\NativeDropBlue.ini":"settings\\phongthan\\BlueMagicExperimental.ini";}
inline bool OwnsNormalDrop(bool official,bool eligible,bool nativeHandled){return nativeHandled||(official&&eligible);}
}
#endif
