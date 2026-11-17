-- Each merchant VM binds these original callbacks once after its original Lua.
PTTradeOriginalMain=main
PTTradeOriginalBuy=buyover
PTTradeOriginalSell=dosell
PTTradeOriginalComplete=complete_task
PTTradeOriginalCamel=yes_2
PTTradeOriginalCancel=cancel_task
PTTradeWorldPos=GetWorldPos
GetWorldPos=GetContentWorldPos
SubWorldIdx2ID=SubWorldIdx2ContentID
Include("\\script\\phongthan\\daily\\reward.lua")

PTTradeContacts={}
function PTTradeNear()
 local map,x,y=PTTradeWorldPos()
 local contact=PTTradeContacts[PlayerIndex or 0]
 local npc=contact and contact.npc or 0
 if not contact or contact.owner~=PlayerId or GetNpcID(npc)~=contact.id then return nil end
 if map~=PT_TRADE_WORLD or npc<=0 or GetNpcTemplateID(npc)~=158 then return nil end
 local nm,nx,ny=GetNpcPos(npc)
 return nm==map and (x-nx)*(x-nx)+(y-ny)*(y-ny)<=144
end
function pt_trade_active()
 return PTTradeNear() and GetLevel()>=PTTradeMinLevel(PT_TRADE_SHOP) and GetTask(60)>=1 and GetTask(60)<=9 and GetTask(332)>=1 and GetTask(332)<=5 and HaveIBBuff(160)==1
end
function PTTradeMinLevel(shop)
 if shop==6 then return 40 elseif shop==7 then return 50 elseif shop==8 then return 60 elseif shop==9 then return 70 end
 return 20
end
function PTTradeJournal()
 local good=GetTask(334)
 if GetTask(333)<=0 or good<1 or good>27 then return end
 DailyJournal(1,1,"Mua "..GetGoodsName(good).." x"..GetTask(335).." t¹i Th­¬ng §iÕm, råi trë vÒ n¬i nhËn nhiÖm vô. NhÊn liªn kÕt t×m ®­êng ®Ó ®i ®Õn ®óng NPC.")
end
function main(npc)
 if not npc or npc<=0 or GetNpcTemplateID(npc)~=158 then CloseDialog();return end
 PTTradeContacts[PlayerIndex or 0]={npc=npc,id=GetNpcID(npc),owner=PlayerId}
 if not PTTradeNear() then CloseDialog();return end
 PTTradeJournal()
 return PTTradeOriginalMain()
end
function buyover(slot,good,quantity)
 local first=(PT_TRADE_SHOP-1)*3+1
 if not pt_trade_active() or quantity~=floor(quantity) or quantity<1 or quantity>1000 or slot<110 or slot>118 or mod(slot,2)~=0 or good<first or good>first+2 then return end
 local result=PTTradeOriginalBuy(slot,good,quantity)
 PTTradeJournal();return result
end
function dosell(slot,quantity)
 if not pt_trade_active() or quantity~=floor(quantity) or quantity<1 or quantity>1000 or slot<110 or slot>118 or mod(slot,2)~=0 then return end
 local result=PTTradeOriginalSell(slot,quantity)
 PTTradeJournal();return result
end
function complete_task()
 if not pt_trade_active() or GetTask(333)~=PT_TRADE_SHOP or GetTask(334)<1 or GetTask(334)>27 or GetTask(335)<1 or GetTask(335)>1000 then return end
 local old=GetTask(803)
 local result=PTTradeOriginalComplete()
 if GetTask(803)==old+1 and GetTask(333)==0 then
  DailyJournal(1,0,"§· giao hµng vµ hoµn thµnh Bµo Th­¬ng.")
  PTDailyHope(1,PTDailyNextReceipt(1))
 end
 return result
end
function cancel_task()
 if not PTTradeNear() then return end
 local result=PTTradeOriginalCancel()
 if GetTask(333)==0 then DailyJournal(1,0,"§· hñy Bµo Th­¬ng.") end
 return result
end
function yes_2(kind)
 if not PTTradeNear() or GetTask(60)~=0 or kind<1 or kind>10 or kind~=floor(kind) then return end
 if HaveIBBuff(160)==0 and GetIBBuffCount()>=16 then Msg2Player("H·y gi¶m bít tr¹ng th¸i tr­íc khi thuª l¹c ®µ.");return end
 return PTTradeOriginalCamel(kind)
end
function paoshangrenwu()
 if not pt_trade_active() or GetTask(333)~=0 or GetLevel()<25 then return end
 local count=GetTask(766)
 if count<0 or count>=20 then return end
 local available={}
 for shop=1,9 do
  local minlevel=25
  if shop==6 then minlevel=40 elseif shop==7 then minlevel=50 elseif shop==8 then minlevel=60 elseif shop==9 then minlevel=70 end
  if shop~=PT_TRADE_SHOP and GetLevel()>=minlevel then
   for good=(shop-1)*3+1,shop*3 do tinsert(available,good) end
  end
 end
 if getn(available)==0 then return end
 if count>=5 then local item=FindAValidIBItem(8,137,2,0);if item<=0 or CostIBItem(item)~=1 then return end end
 local good=available[random(1,getn(available))]
 local kind=mod(good-1,3)
 local quantity=1
 if kind==0 then quantity=20 elseif kind==1 then quantity=2 end
 SetTask(333,PT_TRADE_SHOP);SetTask(334,good);SetTask(335,quantity)
 if count==0 then SetTask(769,SystemTime()) end
 PTTradeJournal()
 MsgBox("H·y mua "..GetGoodsName(good).." ("..quantity..") t¹i Th­¬ng §iÕm t­¬ng øng råi mang vÒ giao cho ta.","no")
end
