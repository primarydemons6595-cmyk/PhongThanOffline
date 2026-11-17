PlayerIndex=1
cash=10000
level=40
morph=0
menus=0
dest=0
function GetWorldPos() return 64,1584,3262 end
function GetMorphType() return morph end
function IsPlayerInsideWeapon(p) return 0 end
function GetLevel() return level end
function GetCash() return cash end
function Pay(n) cash=cash-n end
function Say(text,count,a,b,c,d,e,f,g,h,i,j) menus=count end
function NewWorld(map,x,y) dest=map;destx=x;desty=y end
function CloseDialog() end
function Msg2Player(text) end
function random(a,b) return 1 end
function TestVienCoExit()
 main()
 assert(menus==10)
 fst()
 assert(dest==1 and destx==1536 and desty==3254 and cash==9500)
 cash=0;dest=0;fst();assert(dest==0 and cash==0)
 cash=10000;morph=364;dest=0;fst();assert(dest==0 and cash==10000)
 print('PASS original SuperTrap Lua4: menu opens from Vien Co, return to Phong Than dai, payment, insufficient cash and morph guards')
end
