import fs from 'node:fs';import path from 'node:path';
const root=path.resolve(import.meta.dirname,'../../..'),iso=JSON.parse(fs.readFileSync(path.join(root,'analysis/blue_magic_p1_20261018/isolation.json'),'utf8'));
function stored(text,size){const b=Buffer.alloc(size);b.write(text,'ascii');const c=Buffer.from('Wooyue');const n=text.length;if(n>3){c[0]=b[n-1]%4;c[0]=c[0]?c[c[0]+2]:b[n-1];if(n>4){c[1]=108;if(n>5){c[2]=b[n-2];if(n>6)c[1]=b[n-3];}}}for(let p=0;p<n;p++){b[p]=(b[p]-32+255-c[c[0]%3])%223+32;c[0]=c[1];c[1]=c[2];c[2]=b[p];}return b.toString('hex').toUpperCase();}
fs.mkdirSync(path.join(iso.Client,'UserData'),{recursive:true});
fs.writeFileSync(path.join(iso.Client,'UserData/UiCommon.ini'),'[Login]\r\nLastRegionName=P1 Isolated\r\nLastGameServer=P1BlueLocal\r\nVirtualKeyboard=0\r\nLastAccount='+stored('p1blueonly',32)+'\r\nLastPassword='+stored('E10ADC3949BA59ABBE56E057F20F883E',64)+'\r\n');
fs.writeFileSync(path.join(iso.Client,'serverlist.ini'),'[List]\r\nRegionCount=1\r\nRegion_0=P1 Isolated\r\n[Region_0]\r\nCount=1\r\n0_Title=P1BlueLocal\r\n0_Address=127.0.0.1\r\n');
console.log('Prepared isolated saved-login configuration for the one P1 test account; no current Client config touched');
