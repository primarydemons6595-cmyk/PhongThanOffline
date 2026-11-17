#ifndef PT_TEST_PAK_FILE_H
#define PT_TEST_PAK_FILE_H
#include <windows.h>
#include <stdio.h>
extern const char* g_RouteTestRoot;
// Filesystem boundary for testing the production Region_C reader against
// recovered native files. The map's raw path bytes use the loose Latin-1 names.
class KPakFile {
    HANDLE file;
public:
    KPakFile():file(INVALID_HANDLE_VALUE){}
    ~KPakFile(){Close();}
    BOOL Open(const char* path){
        char full[1024];sprintf(full,"%s%s",g_RouteTestRoot,path);
        wchar_t wide[1024];MultiByteToWideChar(1252,0,full,-1,wide,1024);
        file=CreateFileW(wide,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
        return file!=INVALID_HANDLE_VALUE;
    }
    void Close(){if(file!=INVALID_HANDLE_VALUE){CloseHandle(file);file=INVALID_HANDLE_VALUE;}}
    DWORD Size(){return GetFileSize(file,NULL);}
    DWORD Read(void* out,unsigned int size){DWORD n=0;ReadFile(file,out,size,&n,NULL);return n;}
    DWORD Seek(int offset,unsigned int method){return SetFilePointer(file,offset,NULL,method);}
};
#endif
