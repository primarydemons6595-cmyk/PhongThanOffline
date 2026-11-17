// Main Loose admission only. Companion v1 serialization remains in P0.
#ifndef PT_NATIVE_DROP_ROLLOUT_SCOPE_H
#define PT_NATIVE_DROP_ROLLOUT_SCOPE_H
#include <windows.h>
#include <string>
#include <stdio.h>
#include <string.h>
namespace NativeDropRollout {
inline bool ReadSerialCounter(const std::string& root,DWORD& end){
 DWORD fields[4];FILE*f=fopen((root+"\\serial-reservation.v1").c_str(),"rb");if(!f)return false;
 bool ok=fread(fields,1,sizeof(fields),f)==sizeof(fields)&&fgetc(f)==EOF;fclose(f);
 if(!ok||fields[0]!=0x3153444eUL||fields[1]!=1||fields[2]<100||fields[2]>=0xfffffffeUL||fields[3]!=(fields[0]^fields[1]^fields[2]^0xa55a5aa5UL))return false;end=fields[2];return true;
}
inline bool MainContext(const std::string& server,const std::string& cwd,const std::string& executable){
 const char* suffix="\\PhongThanRuntime-Loose\\Server";
 return server.size()>=strlen(suffix)&&!_stricmp(server.c_str()+server.size()-strlen(suffix),suffix)&&
  !_stricmp(server.c_str(),cwd.c_str())&&!_stricmp(executable.c_str(),"GameServer.exe");
}
inline const char* Approval(){return "NATIVE_DROP_MAIN_LOOSE_V1\nCompanionVersion=1\nCOPY_COMPATIBILITY_TESTS=PASS\n";}
inline int Root(std::string& root,std::string& error){
 char exe[MAX_PATH],cwd[MAX_PATH];
 if(!GetModuleFileNameA(0,exe,sizeof(exe))||!GetCurrentDirectoryA(sizeof(cwd),cwd)){error="ROLLOUT_CONTEXT_UNREADABLE";return -1;}
 char* slash=strrchr(exe,'\\');if(!slash)return 0;std::string name=slash+1;*slash=0;
 bool release=false;
 #if defined(KWin32_H) && !defined(_STANDALONE)
 release=PtReleaseEnabled();
 #endif
 if(release){if(_stricmp(name.c_str(),"GameServer.exe")||_stricmp(exe,cwd)){error="RELEASE_PERSISTENCE_CONTEXT";return -1;}}
 else if(!MainContext(exe,cwd,name))return 0;
 root=std::string(exe)+"\\NativeDropPersistenceV1";
 const std::string latch=std::string(exe)+"\\native_drop_persistence_v1.required";
 DWORD attributes=GetFileAttributesA(root.c_str());if(attributes==(DWORD)-1){if(GetFileAttributesA(latch.c_str())!=(DWORD)-1){error="ROLLOUT_REQUIRED_COMPANION_ROOT_MISSING";return -1;}root="";return 0;}
 if(GetFileAttributesA(latch.c_str())==(DWORD)-1){error="ROLLOUT_PERSISTENCE_LATCH_MISSING";return -1;}
 if(!(attributes&FILE_ATTRIBUTE_DIRECTORY)){error="ROLLOUT_COMPANION_ROOT_NOT_DIRECTORY";return -1;}
 // Once admitted, persistence must remain active even when blue generation is
 // OFF. A missing/corrupt approval is an error, never legacy white fallback.
 FILE* f=fopen((root+"\\rollout-v1.approved").c_str(),"rb");char data[256];size_t n=0;
 if(f){n=fread(data,1,sizeof(data),f);bool bad=ferror(f)!=0;fclose(f);if(bad)n=0;}
 if(n!=strlen(Approval())||memcmp(data,Approval(),n)){error="ROLLOUT_APPROVAL_MISSING_OR_INVALID";return -1;}
 if(!release&&GetFileAttributesA((std::string(exe)+"\\loose_only.flag").c_str())==(DWORD)-1){error="ROLLOUT_LOOSE_MARKER_REQUIRED";return -1;}
 DWORD serialEnd;if(!ReadSerialCounter(root,serialEnd)){error="ROLLOUT_SERIAL_RESERVATION_MISSING_OR_CORRUPT";return -1;}
 return 1;
}
inline bool Active(){std::string root,error;return Root(root,error)==1;}
}
#endif
