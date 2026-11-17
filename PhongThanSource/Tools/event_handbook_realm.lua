-- Additive Event Handbook folder. The server validates every operation.
EBRealmText={
 "Tiên Ma","Chọn hệ","Chọn cấp độ Tiên Ma","Chọn danh vọng",
 "Học tối đa kỹ năng Tiên Ma","Tiên","Ma","Trở về",
 "Nhập cấp Tiên Ma (1–90)","Nhập danh vọng (0–1.000.000.000)",
 "Đã cập nhật Tiên Ma.","Chưa cập nhật. Hãy chọn hệ trước; kiểm tra khóa, giao dịch và chỗ trống túi khi đổi bộ trang bị.",
 "Giá trị không hợp lệ.","Danh vọng Tiên","Danh vọng Ma",
 "Chưa học kỹ năng: bộ dữ liệu hiện tại còn thiếu công thức kỹ năng gốc của hệ phái này. Không thay đổi các kỹ năng đang có."
}
function EBRealmMenu()
 EBShow(EBRealmText[1],{
  EBRealmText[2].."/EBRealmSide",
  EBRealmText[3].."/EBRealmLevel",
  EBRealmText[4].."/EBRealmReputation",
  EBRealmText[5].."/EBRealmApply(24)",
  EBRealmText[8].."/EBMenu(1,1)"})
end
function EBRealmSide()
 EBShow(EBRealmText[2],{EBRealmText[6].."/EBRealmApply(20,1)",EBRealmText[7].."/EBRealmApply(20,2)",EBRealmText[8].."/EBRealmMenu"})
end
function EBRealmLevel()
 InputDialog(EBRealmText[9],2,"OK/EBRealmLevelDone","Cancel/EBRealmMenu")
end
function EBRealmResult(ok)
 if ok==-9 then EBShow(EBRealmText[16],{EBRealmText[8].."/EBRealmMenu"});return end
 EBShow(EBRealmText[ok>0 and 11 or 12],{EBRealmText[8].."/EBRealmMenu"})
end
function EBRealmApply(operation,value)
 local result
 if operation==24 then result=EventHandbookAction(24)
 else result=EventHandbookAction(operation,value) end
 EBRealmResult(result)
end
function EBRealmLevelDone(value)
 local n=tonumber(value)
 if not n or n~=floor(n) or n<1 or n>90 then EBShow(EBRealmText[13],{EBRealmText[8].."/EBRealmLevel"});return end
 EBRealmApply(21,n)
end
function EBRealmReputation()
 EBShow(EBRealmText[4],{EBRealmText[14].."/EBRealmReputationInput(22)",EBRealmText[15].."/EBRealmReputationInput(23)",EBRealmText[8].."/EBRealmMenu"})
end
function EBRealmReputationInput(operation)
 if operation~=22 and operation~=23 then return end
 EBRealmReputationOperation=operation
 InputDialog(EBRealmText[10],2,"OK/EBRealmReputationDone","Cancel/EBRealmMenu")
end
function EBRealmReputationDone(value)
 local n=tonumber(value)
 if not n or n~=floor(n) or n<0 or n>1000000000 then EBShow(EBRealmText[13],{EBRealmText[8].."/EBRealmReputation"});return end
 if EBRealmReputationOperation~=22 and EBRealmReputationOperation~=23 then return end
 EBRealmApply(EBRealmReputationOperation,n)
end
if EBNodes and EBNodes[1] then
 local found=0
 for i=1,getn(EBNodes[1][3]) do if EBNodes[1][3][i][2]=="EBRealmMenu" then found=1 end end
 if found==0 then tinsert(EBNodes[1][3],1,{EBRealmText[1],"EBRealmMenu",""}) end
end
