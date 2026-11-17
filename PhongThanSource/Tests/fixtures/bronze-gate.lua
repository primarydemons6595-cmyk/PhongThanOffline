world=52;trade=0;locked=0;delivery=0;dialogs=0;travels=0;shop=0
function GetContentWorldPos() return world end
function GetTrade() return trade end
function GetLockState() return locked end
function GetTask(k) return delivery end
function GetByte(n,k) if delivery==1 and k==1 then return 6 end return 0 end
function GetBit(n,k) return 0 end
function SetBit(n,k,v) return 128 end
function SetByte(n,k,v) return 128 end
function SetTask(k,v) assert(k==874 and v==128);delivery=2 end
function Talk(...) end
function MsgBox(...) dialogs=dialogs+1 end
function SayButtons(...) dialogs=dialogs+1 end
function CloseDialog() end
function Sale(k) assert(k==18);shop=shop+1 end
function NewWorld(m,x,y) travels=travels+1;dest={m,x,y} end
function TestBronzeGate()
 main();assert(dialogs==1)
 yes();assert(shop==1)
 delivery=1;main();assert(dialogs==1 and delivery==2)
 delivery=0;world=51;pt_bronze_enter();assert(travels==0)
 world=52;trade=1;pt_bronze_enter();assert(travels==0)
 trade=0;locked=1;pt_bronze_enter();assert(travels==0)
 locked=0;pt_bronze_enter();assert(travels==1 and dest[1]==57 and dest[2]==1608 and dest[3]==3094)
 print('PASS bronze entry: original shop/delivery preserved, map/trade/lock guards, exact destination')
end
function TestBronzeReturn()
 world=52;pt_bronze_return();assert(travels==0)
 world=57;trade=1;pt_bronze_return();assert(travels==0)
 trade=0;locked=1;pt_bronze_return();assert(travels==0)
 locked=0;pt_bronze_return();assert(travels==1 and dest[1]==52 and dest[2]==1632 and dest[3]==3067)
 print('PASS bronze return: map/trade/lock guards, exact return destination')
end
