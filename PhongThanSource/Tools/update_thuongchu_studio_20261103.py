from pathlib import Path
ROOT=Path(__file__).resolve().parents[2]
p=ROOT/'PhongThanRuntime-Loose/ManagerStudio/Wpf/PhongThanStudio.App/EventManagerView.cs'
d=p.read_text('utf-8-sig');a=d.index('if (section == EventManagementSection.ThuongChu)');start=d.index('Text="',a)+len('Text="');end=d.index('",TextWrapping=',start)
d=d[:start]+'Theo luật Tam Giới: báo danh 12:00 và 20:00 trong 10 phút, thi đấu 30 phút; tối thiểu 5 người mỗi phe. Mỗi phiên chọn ngẫu nhiên một trong ba dạng trận dùng chung cho toàn bộ người chơi và bot. Bot đủ cấp tự đến Bia Thần bằng Dịch Chuyển Tiên, trả phí bình thường; lệnh đi cùng người chơi được ưu tiên. Mở ngay bắt đầu báo danh.'+d[end:];p.write_text(d,'utf8')
p=ROOT/'PhongThanSource/Sources/Core/Src/PhongThanBotTraining.inl';d=p.read_bytes();a=b'&&strcmp(r.State,"boss_travel"))continue;';assert d.count(a)==1;extra=''.join('&&strcmp(r.State,"'+name+'")' for name in ['thuong_chu_registering','thuong_chu_entering','thuong_chu_returning_flag','thuong_chu_retreating','thuong_chu_fighting']).encode();d=d.replace(a,b'&&strcmp(r.State,"boss_travel")'+extra+b')continue;');p.write_bytes(d)
