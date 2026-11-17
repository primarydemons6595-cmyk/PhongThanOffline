function pt_off_close() CloseDialog() end
function main()
 PT_OFF_MODE=PtOfflineState(3)
 local text="<color=yellow>Ủy thác rời mạng<color><enter>EXP: "..PtOfflineState(0).." giờ chờ; Lam Dương: "..PtOfflineState(1).." giờ chờ; Thái Cực: "..PtOfflineState(2).." giờ chờ.<enter>Nhật Nguyệt Đơn phải được kích hoạt bằng chuột phải và còn hạn khi đăng nhập. Thái Cực/Lam Dương chỉ trừ khi nhận thưởng thành công. Mỗi phiên tính tối đa 4 giờ trọn vẹn."
 SayButtons(text,7,"Chọn nhận EXP/pt_off_exp","Chọn luyện kỹ năng/pt_off_skill","Chọn luyện bảo thạch/pt_off_gem","Áp dụng lựa chọn/pt_off_apply","Nhận thưởng đang chờ/pt_off_rewards","Tắt Ủy thác/pt_off_disable","Kết thúc/pt_off_close")
end
function pt_off_has(bit) return mod(floor(PT_OFF_MODE/bit),2)==1 end
function pt_off_toggle(bit) if pt_off_has(bit) then PT_OFF_MODE=PT_OFF_MODE-bit else PT_OFF_MODE=PT_OFF_MODE+bit end;pt_off_choice() end
function pt_off_exp() pt_off_toggle(1) end
function pt_off_skill() pt_off_toggle(4) end
function pt_off_gem() pt_off_toggle(2) end
function pt_off_choice()
 local text="<color=yellow>Lựa chọn Ủy thác<color><enter>EXP: "..(pt_off_has(1) and "Bật" or "Tắt").."<enter>Kỹ năng chuột phải: "..(pt_off_has(4) and "Bật" or "Tắt").."<enter>Bảo thạch: "..(pt_off_has(2) and "Bật" or "Tắt").."<enter>Hãy dùng lựa chọn Áp dụng trước khi thoát bình thường. Kỹ năng được ghi nhận lúc mở Ủy thác từ ESC."
 SayButtons(text,5,"Đổi EXP/pt_off_exp","Đổi kỹ năng/pt_off_skill","Đổi bảo thạch/pt_off_gem","Áp dụng/pt_off_apply","Kết thúc/pt_off_close")
end
function pt_off_apply()
 if PtOfflineConfigure(PT_OFF_MODE)==1 then Talk(1,"pt_off_close","Đã lưu lựa chọn. Hãy thoát game bình thường để bắt đầu tính giờ. Đăng nhập lại rồi mở ESC - Ủy thác để nhận thưởng.")
 else Talk(1,"pt_off_close","Chưa đặt được Ủy thác. Nhận hết giờ đang chờ trước; EXP cần Nhật Nguyệt Đơn còn hạn đã kích hoạt, kỹ năng cần chọn bên chuột phải và dưới cấp 10. Kết thúc khóa/giao dịch rồi thử lại.") end
end
function pt_off_disable() PT_OFF_MODE=0;pt_off_apply() end
function pt_off_rewards() SayButtons("Chọn từng giờ cần nhận. Phần không nhận được do thiếu đơn, hết hạn hoặc đầy túi vẫn được giữ, không tiêu hao vật phẩm. EXP tối đa 4 giờ/ngày và không dùng để vượt cấp 200. Kỹ năng tối đa cấp 10.",5,"Nhận 1 giờ EXP/pt_off_claim_exp","Nhận 1 giờ bảo thạch/pt_off_claim_gem","Nhận 1 giờ kỹ năng/pt_off_claim_skill","Bỏ giờ chờ không dùng/pt_off_discard_confirm","Kết thúc/pt_off_close") end
function pt_off_discard_confirm() SayButtons("Bỏ toàn bộ giờ thưởng đang chờ? Không trừ vật phẩm, nhưng số giờ này không thể lấy lại.",2,"Đồng ý bỏ/pt_off_discard","Giữ lại/pt_off_rewards") end
function pt_off_discard() if PtOfflineDiscard()==1 then Talk(1,"pt_off_close","Đã bỏ giờ thưởng đang chờ.") else Talk(1,"pt_off_close","Hãy kết thúc khóa/giao dịch rồi thử lại.") end end
function pt_off_result(result) if result==1 then Talk(1,"pt_off_rewards","Đã nhận 1 giờ Ủy thác.") else Talk(1,"pt_off_rewards","Không nhận được giờ này. Kiểm tra giờ đang chờ, hạn Nhật Nguyệt Đơn, số đơn trong bốn túi, kỹ năng và ô trống.") end end
function pt_off_claim_exp() pt_off_result(PtOfflineClaim(0)) end
function pt_off_claim_gem() pt_off_result(PtOfflineClaim(1)) end
function pt_off_claim_skill() pt_off_result(PtOfflineClaim(2)) end
