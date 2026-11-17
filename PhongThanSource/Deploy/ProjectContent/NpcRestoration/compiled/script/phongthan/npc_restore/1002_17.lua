-- AUTHORED NPC 1002_17; not a claim of original appearance.
function pt_guard()
    local w = GetWorldPos()
    if w ~= 1002 then CloseDialog(); return 0 end
    return 1
end
function main()
    if pt_guard() == 0 then return end
    Say("Ho Tro Tan Thu - Sung Thanh doanh (210/200)", 5, "Gia Nh\203p Giang H\229/pt_join_world", "Vai tro va huong dan/pt_about", "NPC lien quan/pt_routes", "Trang thai chuc nang/pt_status", "Dong/pt_close")
end
-- STAGING TEST ONLY, user-authorized; not an original quest/reward branch.
function pt_join_world()
    if pt_guard() == 0 then return end
    if GetTeamSize() > 0 then Msg2Player("[TEST] Hay roi to doi truoc khi thoat Tan thu."); return end
    if GetCamp() ~= 0 or GetCurCamp() ~= 0 then Msg2Player("[TEST] Ban khong con o trang thai Tan thu."); return end
    -- SetCurCamp retains the existing native camp lock. Check before SetCamp.
    SetCurCamp(7)
    if GetCurCamp() ~= 7 then Msg2Player("[TEST] Doi camp bi khoa; khong thay doi trang thai Tan thu."); return end
    SetCamp(7)
    Msg2Player("[TEST] Da Gia Nhap Giang Ho: camp 7. Khong doi nhiem vu, vat pham hay FightMode.")
    CloseDialog()
end
function pt_about()
    if pt_guard() == 0 then return end
    Say("Ho tro thao tac: mo F4 de kiem tra vat pham, click phai de su dung hoac trang bi. Danh sach trong map giup tim nguoi giao nhiem vu.", 2, "Quay lai/main", "Dong/pt_close")
end
function pt_routes()
    if pt_guard() == 0 then return end
    Say("Pham Nghia (210/200); Sung Ung Buu (211/198); Sung Ung Loan (212/198); Sung Hac Ho (213/200); Tap Hoa Thuong Nhan (207/198)", 2, "Quay lai/main", "Dong/pt_close")
end
function pt_status()
    if pt_guard() == 0 then return end
    Say("Dang mo: doi thoai, huong dan va tra cuu. Nhan/tra nhiem vu, doi thuong va giao dich chua mo khi chua du ma du lieu.", 2, "Quay lai/main", "Dong/pt_close")
end
function pt_close()
    CloseDialog()
end
