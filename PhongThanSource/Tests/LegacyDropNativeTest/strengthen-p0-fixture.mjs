import fs from 'node:fs';
const file=new URL('./P0PersistenceTests.cpp',import.meta.url);
let s=fs.readFileSync(file,'latin1');
function replace(old,next){if(s.includes(next))return;if(s.split(old).length!==2)throw Error('fixture anchor '+old);s=s.replace(old,next);}
replace('struct KItemList {','class KItemList {public:');
replace('Check(Item[index].SetMagicAttrib(attrs)!=0,','if(i%25==0)attrs[0].nAttribType=magic_indestructible_b;Check(Item[index].SetMagicAttrib(attrs)!=0,');
replace('player.m_ItemList.Add(index,pos_equiproom,i%10,i/10);','Item[index].SetOwner(10000+role*1000+i);Item[index].SetTradePrice(200+i);Item[index].SetFortune(i%5);Item[index].SetParam(i+1);Item[index].SetFlash(i%4);Item[index].SetLockSell(i%2);Item[index].SetLockTrade(i%3==0);Item[index].SetLockDrop(i%4==0);if(i%25)Item[index].SetDurability(5+i%10);player.m_ItemList.Add(index,pos_equiproom,i%10,i/10);');
replace('if(!legacy)Golden(root,role,expected,true);','if(legacy)for(size_t x=0;x<expected.size();++x)expected[x].serial=0;Golden(root,role,expected,true);');
replace('if(!legacy)Golden(root,role,actual,false);else for(size_t x=0;x<actual.size();++x)Check(actual[x].quality==equip_normal,"old white item remains white without companion");','if(legacy)for(size_t x=0;x<actual.size();++x){Check(actual[x].quality==equip_normal,"old white item remains white without companion");actual[x].serial=0;}Golden(root,role,actual,false);');
fs.writeFileSync(file,Buffer.from(s,'latin1'));
