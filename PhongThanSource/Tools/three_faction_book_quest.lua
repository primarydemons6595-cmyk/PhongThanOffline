-- Cần Mẫn: bounded reconstruction from the hash-pinned reference scripts.
-- Their original saved slots are retained. Showing the book does not consume it.
function PT3BookAllowed()
 return GetPlayerType()==PT3_BOOK.f and GetContentWorldPos()+1000==PT3_BOOK.map and GetLevel()>=PT3_BOOK.level
end
function PT3BookHave()
 -- Legacy rewards store level 0; shop books store level 1. The native query
 -- uses a negative level as a wildcard while keeping detail/skill exact.
 return HaveNormalItem(7,PT3_BOOK.detail,PT3_BOOK.skill,-1)>0 or IsSkillActived(PT3_BOOK.skill)>0
end
function PT3BookNote()
 if not PT3BookAllowed() then return end
 local phase=GetTask(PT3_BOOK.task)
 if phase==1 then AddNote(PT3_BOOK.id,1,PT3_BOOK.note,1)
 elseif phase==2 then AddNote(PT3_BOOK.id,2," ",2) end
end
function pt3_book_close() CloseDialog() end
function pt3_book_other() if PT3_BOOK_OLD_MAIN then return PT3_BOOK_OLD_MAIN() end CloseDialog() end
function pt3_book_view()
 if not PT3BookAllowed() then return end
 local phase=GetTask(PT3_BOOK.task)
 if phase==0 then SayButtons(PT3_BOOK.note,2,"Nhận nhiệm vụ/pt3_book_take","Quay lại/main")
 elseif phase==1 then
  if PT3BookHave() then SayButtons(PT3_BOOK.note,2,"Hoàn thành/pt3_book_claim","Quay lại/main")
  else SayButtons(PT3_BOOK.note,1,"Quay lại/main") end
 end
end
function pt3_book_take()
 if not PT3BookAllowed() or GetTrade()~=0 or GetLockState()~=0 then return end
 if QuestExchange(PT3_BOOK.task,0,1,{},{},1)~=1 then return end
 PT3BookNote();pt3_book_view()
end
function pt3_book_claim()
 if not PT3BookAllowed() or GetTrade()~=0 or GetLockState()~=0 or not PT3BookHave() then return end
 -- No yielding calls between checking possession and committing the phase.
 if QuestExchange(PT3_BOOK.task,1,2,{},PT3_BOOK.gifts,1)~=1 then return end
 AddOwnExp(400);PT3BookNote();Msg2Player("Cần Mẫn: nhận 400 kinh nghiệm.")
end
PT3_BOOK_OLD_MAIN=main
PT3_BOOK_OLD_MARK=GetPlayerTaskState
function main(sel)
 PT3BookNote()
 if PT3BookAllowed() and GetTask(PT3_BOOK.task)<2 then
  return SayButtons(PT3_BOOK.note,2,"Cần Mẫn/pt3_book_view","Nhiệm vụ khác/pt3_book_other")
 end
 if PT3_BOOK_OLD_MAIN then return PT3_BOOK_OLD_MAIN(sel) end CloseDialog()
end
function GetPlayerTaskState()
 local state,level=0,0
 if PT3_BOOK_OLD_MARK then state,level=PT3_BOOK_OLD_MARK() end
 state=state or 0
 if PT3BookAllowed() then
  local phase=GetTask(PT3_BOOK.task);local own=0
  if phase==0 then own=1 elseif phase==1 then own=PT3BookHave() and 3 or 2 end
  if own>state then return own,PT3_BOOK.level end
 end
 return state,level
end
