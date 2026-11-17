import fs from 'node:fs';import path from 'node:path';
const root=path.resolve(import.meta.dirname,'../../..'),file=path.join(root,'PhongThanSource/Sources/MultiServer/GameServer/KSOServer.cpp'),backup=path.join(root,'analysis/blue_magic_p1_20261018/before/KSOServer.cpp');
if(!fs.existsSync(backup))fs.copyFileSync(file,backup);let s=fs.readFileSync(file,'latin1');
if(!s.includes('P1: release the admitted session before failed-load cleanup')){
 const anchor='\t\t\tm_pCoreServerShell->PreparePlayerForLoginFailed(nPlayerIndex);';
 const next=`\t\t\t// P1: release the admitted session before failed-load cleanup.
\t\t\t// No item state is saved here; the last committed role stays authoritative.
\t\t\tNotifyGatewayWorldSession(nPlayerIndex,
\t\t\t\tPHONGTHAN_WORLD_SESSION_LEAVING,
\t\t\t\tPHONGTHAN_WORLD_SESSION_REASON_DISCONNECTED, TRUE, 0, 0);
\t\t\tUnbindPhongThanRelaySession(m_RelayClient,nPlayerIndex,
\t\t\t\tPHONGTHAN_RELAY_SESSION_END_DISCONNECTED);
${anchor}
\t\t\tm_pGameStatus[lnID].nPlayerIndex=0;
\t\t\tm_pGameStatus[lnID].nGameStatus=enumPlayerBegin;`;
 if(s.split(anchor).length!==2)throw Error('Unique failed-load cleanup anchor missing');s=s.replace(anchor,next);fs.writeFileSync(file,Buffer.from(s,'latin1'));
}
