import fs from 'node:fs';import path from 'node:path';
const root=path.resolve(import.meta.dirname,'../../..'),p=path.join(root,'PhongThanSource/Sources/Core/Src/KPlayer.cpp');let s=fs.readFileSync(p,'latin1');
if(!s.includes('P1BlueWorldTest.h')){const anchor='#include "KPlayer.h"';if(s.split(anchor).length!==2)throw Error('Player include');s=s.replace(anchor,anchor+'\n#include "P1BlueWorldTest.h"');}
if(!s.includes('P1BlueWorldTick(*this);')){const anchor='\tm_TimerTask.Activate(g_PlayerTimerCallBackFun);';if(s.split(anchor).length!==2)throw Error('Player tick');s=s.replace(anchor,anchor+'\n\tP1BlueWorldTick(*this);');}
fs.writeFileSync(p,Buffer.from(s,'latin1'));
