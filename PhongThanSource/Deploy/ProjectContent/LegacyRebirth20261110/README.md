# Công thức chuyển sinh Loose

Hai công thức 1500 và 1502 là **công thức thay thế được người dùng đồng ý ngày 10/11/2026**, không phải số VNG đã tìm lại. Chỉnh `formulas.json`, sau đó chạy:

```powershell
python PhongThanSource/Tools/build_rebirth_formulas.py --config PhongThanSource/Deploy/ProjectContent/LegacyRebirth20261110/formulas.json --skills PhongThanRuntime-Loose/Server/settings/Skills.txt --output PhongThanSource/Deploy/ProjectContent/LegacyRebirth20261110/generated --old-control-source PhongThanRuntime-Loose/Server/script/skill/jiashi/hua_di_wei_lao.lua
```

Đóng client, lưu/dừng world bằng công cụ điều khiển thông thường, cài đồng thời những file trong `generated/manifest.json` vào **cả Client và Server**, rồi khởi động lại. Không cập nhật riêng một phía: skill đã nạp được cache theo cấp trong mỗi tiến trình. Không cần sửa dữ liệu nhân vật hay cấp lại skill đã học.

1516/1517 là Họa Địa Vi Lao học trực tiếp, giữ nguyên ID và đạn 223. Chúng dùng bản sao bộ điều khiển làm chậm cùng họ còn lưu trong dữ liệu gốc; đây không phải bằng chứng đã tìm lại đúng công thức cũ. 1560 là bản Thần Kỹ, dùng đạn 532 và đường kích hoạt sách riêng. Không hợp nhất hai kỹ năng.

Buff cấp cao thay thế đúng đóng góp của cấp cũ và cập nhật cấp/thời gian hiển thị, không cộng dồn qua lần sử dụng. Kiểm thử native bao phủ cấp 1–10, áp dụng lại, nâng cấp và gỡ hiệu ứng.
