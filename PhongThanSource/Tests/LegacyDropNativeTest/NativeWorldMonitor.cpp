// Debug only the explicitly launched Loose child. Logs exception addresses and
// native exit codes, never memory payloads, account names or packet contents.
#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>
#include <string.h>
static FILE* logFile;
static void Address(DWORD pid,DWORD address){
    HANDLE snap=CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,pid);MODULEENTRY32 m;ZeroMemory(&m,sizeof(m));m.dwSize=sizeof(m);
    if(snap!=INVALID_HANDLE_VALUE){BOOL ok=Module32First(snap,&m);while(ok){DWORD base=(DWORD)m.modBaseAddr;if(address>=base&&address-base<m.modBaseSize){fprintf(logFile," module=%s offset=%08lX",m.szModule,address-base);break;}ok=Module32Next(snap,&m);}CloseHandle(snap);}
}
int main(int argc,char** argv){
    if(argc!=4)return 2;char path[MAX_PATH];if(!GetFullPathNameA(argv[1],sizeof(path),path,0))return 2;
    const char* suffix="\\PhongThanRuntime-Loose\\Server\\GameServer.exe";int a=strlen(path),b=strlen(suffix);
    if(a<b||_stricmp(path+a-b,suffix))return 2;
    logFile=fopen(argv[2],"ab");if(!logFile)return 3;
    SECURITY_ATTRIBUTES sa;sa.nLength=sizeof(sa);sa.lpSecurityDescriptor=0;sa.bInheritHandle=TRUE;
    HANDLE output=CreateFileA(argv[3],GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,&sa,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
    if(output==INVALID_HANDLE_VALUE)return 4;
    STARTUPINFOA start;PROCESS_INFORMATION proc;ZeroMemory(&start,sizeof(start));ZeroMemory(&proc,sizeof(proc));start.cb=sizeof(start);
    start.dwFlags=STARTF_USESTDHANDLES;start.hStdOutput=start.hStdError=output;start.hStdInput=GetStdHandle(STD_INPUT_HANDLE);
    char command[MAX_PATH+4];sprintf(command,"\"%s\"",path);char cwd[MAX_PATH];strcpy(cwd,path);*strrchr(cwd,'\\')=0;
    SetEnvironmentVariableA("_NO_DEBUG_HEAP","1"); // Keep monitoring from adding OS debug-heap cost to a 3GB world.
    if(!CreateProcessA(path,command,0,0,TRUE,DEBUG_ONLY_THIS_PROCESS|CREATE_NO_WINDOW,0,cwd,&start,&proc)){fprintf(logFile,"START_FAILED error=%lu\n",GetLastError());fclose(logFile);CloseHandle(output);return 5;}
    fprintf(logFile,"START pid=%lu\n",proc.dwProcessId);fflush(logFile);CloseHandle(proc.hThread);CloseHandle(output);
    for(;;){DEBUG_EVENT e;ZeroMemory(&e,sizeof(e));if(!WaitForDebugEvent(&e,1000))continue;DWORD status=DBG_CONTINUE;
        if(e.dwDebugEventCode==CREATE_PROCESS_DEBUG_EVENT){if(e.u.CreateProcessInfo.hFile)CloseHandle(e.u.CreateProcessInfo.hFile);CloseHandle(e.u.CreateProcessInfo.hThread);if(e.u.CreateProcessInfo.hProcess!=proc.hProcess)CloseHandle(e.u.CreateProcessInfo.hProcess);}
        if(e.dwDebugEventCode==CREATE_THREAD_DEBUG_EVENT)CloseHandle(e.u.CreateThread.hThread);
        if(e.dwDebugEventCode==LOAD_DLL_DEBUG_EVENT&&e.u.LoadDll.hFile)CloseHandle(e.u.LoadDll.hFile);
        if(e.dwDebugEventCode==EXCEPTION_DEBUG_EVENT){DWORD code=e.u.Exception.ExceptionRecord.ExceptionCode;
            if(code!=EXCEPTION_BREAKPOINT){status=DBG_EXCEPTION_NOT_HANDLED;
                fprintf(logFile,"EXCEPTION pid=%lu first=%lu code=%08lX address=%08lX",e.dwProcessId,e.u.Exception.dwFirstChance,code,(DWORD)e.u.Exception.ExceptionRecord.ExceptionAddress);
                Address(e.dwProcessId,(DWORD)e.u.Exception.ExceptionRecord.ExceptionAddress);fprintf(logFile,"\n");
                if(!e.u.Exception.dwFirstChance){typedef HANDLE(WINAPI* OpenThreadFn)(DWORD,BOOL,DWORD);OpenThreadFn openThread=(OpenThreadFn)GetProcAddress(GetModuleHandleA("kernel32.dll"),"OpenThread");HANDLE thread=openThread?openThread(THREAD_GET_CONTEXT|THREAD_QUERY_INFORMATION,FALSE,e.dwThreadId):0;
                    if(thread){CONTEXT c;ZeroMemory(&c,sizeof(c));c.ContextFlags=CONTEXT_CONTROL;if(GetThreadContext(thread,&c)){DWORD fp=c.Ebp,previous=0;
                        for(int frame=0;frame<24&&fp>previous&&fp-previous<0x80000000UL;++frame){DWORD pair[2],read=0;if(!ReadProcessMemory(proc.hProcess,(void*)fp,pair,sizeof(pair),&read)||read!=sizeof(pair))break;
                            fprintf(logFile,"FRAME address=%08lX",pair[1]);Address(e.dwProcessId,pair[1]);fprintf(logFile,"\n");previous=fp;fp=pair[0];}}
                        CloseHandle(thread);}
                }
                fflush(logFile);
            }
        }
        if(e.dwDebugEventCode==EXIT_PROCESS_DEBUG_EVENT){fprintf(logFile,"EXIT pid=%lu code=%lu hex=%08lX\n",e.dwProcessId,e.u.ExitProcess.dwExitCode,e.u.ExitProcess.dwExitCode);fflush(logFile);ContinueDebugEvent(e.dwProcessId,e.dwThreadId,status);CloseHandle(proc.hProcess);fclose(logFile);return e.u.ExitProcess.dwExitCode==0?0:1;}
        ContinueDebugEvent(e.dwProcessId,e.dwThreadId,status);
    }
}
