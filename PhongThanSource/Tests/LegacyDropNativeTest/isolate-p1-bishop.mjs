import fs from 'node:fs';import path from 'node:path';
const root=path.resolve(import.meta.dirname,'../../..'),file=path.join(root,'PhongThanSource/Sources/MultiServer/Bishop/Application.cpp'),backup=path.join(root,'analysis/blue_magic_p1_20261018/before/Bishop.Application.cpp');
if(!fs.existsSync(backup))fs.copyFileSync(file,backup);
let s=fs.readFileSync(file,'latin1');
if(!s.includes('P1BishopMutexName')){
 const anchor='static const char szBishopClassName[] = "BishopClass";';
 const code=`${anchor}
// Keep single-instance protection, but give the isolated P1 stack its own mutex.
static const char* P1BishopMutexName()
{
 char exe[MAX_PATH];if(!GetModuleFileNameA(0,exe,sizeof(exe)))return szBishopClassName;
 char* slash=strrchr(exe,'\\\\');if(!slash)return szBishopClassName;*slash=0;
 if(!strstr(exe,"\\\\PhongThanRuntime-Loose\\\\P0-BlueMagic-Isolated\\\\p1_"))return szBishopClassName;
 slash=strrchr(exe,'\\\\');if(!slash)return szBishopClassName;*slash=0;
 if(strlen(exe)+20>=MAX_PATH)return szBishopClassName;
 strcat(exe,"\\\\p0_isolated.flag");
 return GetFileAttributesA(exe)!=(DWORD)-1?"BishopClass.P1BlueWorld":szBishopClassName;
}
`;
 if(s.split(anchor).length!==2||s.split('TRUE, szBishopClassName )').length!==3)throw Error('Bishop isolation anchors');
 s=s.replace(anchor,code).replaceAll('TRUE, szBishopClassName )','TRUE, P1BishopMutexName() )');fs.writeFileSync(file,Buffer.from(s,'latin1'));
}
