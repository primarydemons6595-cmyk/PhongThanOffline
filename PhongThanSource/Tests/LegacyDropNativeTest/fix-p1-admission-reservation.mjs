import fs from 'node:fs';import path from 'node:path';
const root=path.resolve(import.meta.dirname,'../../..'),backup=path.join(root,'analysis/blue_magic_p1_20261018/before');
const file=path.join(root,'PhongThanSource/Sources/MultiServer/Bishop/GamePlayer.cpp');
if(!fs.existsSync(path.join(backup,'GamePlayer.cpp')))fs.copyFileSync(file,path.join(backup,'GamePlayer.cpp'));
let s=fs.readFileSync(file,'latin1').replace(/\r\n/g,'\n');
const old='\t\t\tm_bAutoUnlockAccount = false;\n\n\t\t\tnNextTask = enumToNextTask;';
const next=`\t\t\t// Reserve ownership before handing the permit to the client. A load
\t\t\t// failure can then release this account without charging an ENTER event.
\t\t\tIGServer* pAdmitted = CGameServer::GetServer(m_nAttachServerID);
\t\t\tif (pAdmitted && pAdmitted->Attach(m_sAccountName.c_str(), false))
\t\t\t{
\t\t\t\tm_bAutoUnlockAccount = false;
\t\t\t\tnNextTask = enumToNextTask;
\t\t\t}
\t\t\telse pResponse->Permit = 0;`;
if(s.split(old).length!==2)throw Error('Unique admission anchor missing');s=s.replace(old,next);fs.writeFileSync(file,Buffer.from(s,'latin1'));
const world=path.join(root,'PhongThanSource/Sources/MultiServer/GameServer/KSOServer.cpp');s=fs.readFileSync(world,'latin1');
const failed='\t\t\t\t\tGameServerLoginDiag("login_data_failed id=%lu index=%d", (unsigned long)lnID, nIndex);';
const start=s.indexOf(failed),end=s.indexOf('#ifndef _STANDALONE',start);if(start<0||end<0)throw Error('Failed caller anchor missing');
let body=s.slice(start,end);body=body.replace('m_pGameStatus[lnID].nPlayerIndex = nIndex;','m_pGameStatus[lnID].nPlayerIndex = 0;');s=s.slice(0,start)+body+s.slice(end);
const clean='\t\t\tm_pCoreServerShell->PreparePlayerForLoginFailed(nPlayerIndex);';if(s.split(clean).length!==2)throw Error('Failed cleanup anchor missing');
s=s.replace(clean,clean+'\r\n\t\t\tm_pCoreServerShell->RemovePlayerLoginTimeOut(nPlayerIndex);');fs.writeFileSync(world,Buffer.from(s,'latin1'));
