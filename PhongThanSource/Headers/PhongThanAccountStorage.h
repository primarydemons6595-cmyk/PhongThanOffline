#ifndef PHONGTHAN_ACCOUNT_STORAGE_H
#define PHONGTHAN_ACCOUNT_STORAGE_H
#include <windows.h>
#include <string>
#include <stdio.h>
#include <string.h>
// One safe, case-insensitive directory per account. Ordinary account IDs retain
// their readable spelling. Encode reserved/unsafe Windows components entirely.
inline bool PtAccountComponent(const char* account,std::string& out){
    out="";if(!account||!account[0]||strlen(account)>=32)return false;
    bool plain=true;std::string hex;
    for(size_t i=0;account[i];++i){unsigned char c=(unsigned char)account[i];if(c>='A'&&c<='Z')c+='a'-'A';
        char h[3];sprintf(h,"%02x",c);hex+=h;
        if(!((c>='a'&&c<='z')||(c>='0'&&c<='9')||c=='_'||c=='-'))plain=false;out+=(char)c;}
    if(out=="con"||out=="prn"||out=="aux"||out=="nul"||out=="consumablebuffs"||
       (out.size()==4&&(out.substr(0,3)=="com"||out.substr(0,3)=="lpt")&&out[3]>='0'&&out[3]<='9'))plain=false;
    if(!plain)out="~"+hex;return true;
}
inline bool PtRoleFileName(const char* role,std::string& out){
    out="role_";if(!role||!role[0]||strlen(role)>=32)return false;
    for(size_t i=0;role[i];++i){unsigned char c=(unsigned char)role[i];if(c>='A'&&c<='Z')c+='a'-'A';char h[3];sprintf(h,"%02x",c);out+=h;}
    out+=".pthc";return true;
}
inline bool PtAccountDirectory(const char* root,const char* account,std::string& out,bool create=false){
    std::string component;if(!root||!root[0]||!PtAccountComponent(account,component))return false;
    out=std::string(root)+"\\"+component;if(out.size()>=MAX_PATH-80)return false;
    DWORD attr=GetFileAttributesA(out.c_str());
    if(attr!=(DWORD)-1)return (attr&FILE_ATTRIBUTE_DIRECTORY)&&!(attr&FILE_ATTRIBUTE_REPARSE_POINT);
    return !create||CreateDirectoryA(out.c_str(),0)!=0||GetLastError()==ERROR_ALREADY_EXISTS;
}
#endif
