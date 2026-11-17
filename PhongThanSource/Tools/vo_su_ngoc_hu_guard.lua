-- Keep the original teacher quest/dialogue and Sale(9) inventory.
-- Original IsMaster is the runtime's persistent IsMantleMaster service.
IsMaster=IsMantleMaster
PTVS_OriginalMain=main
PTVS_Contacts={}
function PTVSNear()
 local contact=PTVS_Contacts[PlayerIndex or 0]
 if not contact or contact.owner~=PlayerId or GetNpcID(contact.npc)~=contact.id then return nil end
 if GetNpcTemplateID(contact.npc)~=154 then return nil end
 local world,x,y=PTVS_RuntimePos()
 local map,nx,ny=GetNpcPos(contact.npc)
 return world==1003 and map==world and (x-nx)*(x-nx)+(y-ny)*(y-ny)<=144
end
function main(npc)
 if not npc or npc<=0 or GetNpcTemplateID(npc)~=154 then return end
 PTVS_Contacts[PlayerIndex or 0]={npc=npc,id=GetNpcID(npc),owner=PlayerId}
 if not PTVSNear() then return end
 return PTVS_OriginalMain(npc)
end
function jineng()
 if not PTVSNear() then return end
 if GetPlayerType()~=1 then Talk(1,"no",10554);return end
 MsgBox(10553,"yes_2","no")
end
function yes_2()
 if not PTVSNear() or GetPlayerType()~=1 or GetTrade()~=0 or GetLockState()~=0 then return end
 CloseDialog();Sale(9)
end
