// Reviewed quest graph. Website establishes objectives/levels/XP; retained
// task phases and item identities come from the available VNG Lua/tables.
const event=(id,n=1)=>[4,id,0,0,0,0,n];
const eq=(d,p,l,s=1)=>[0,d,p,l,s,0,1];
const step=(role,title,level,task,from,to,note,needs=[],gifts=[],xp=0,money=0,credit=0,extra={})=>({role,title,level,task,from,to,note,needs,gifts,xp,money,credit,...extra});
const steps=[
step('lo','Tân Thức',3,21,0,1,'Gặp Âu Thiên Hóa tại Sùng Thành Doanh.'),
step('au','Tân Thức',3,21,1,2,'Gặp Sùng Ứng Bưu.'),
step('buu','Tân Thức',3,21,2,3,'Thu thập 10 Đoản Kiếm rồi gặp Âu Thiên Hóa.'),
step('au','Tân Thức',3,21,3,4,'Gặp Sùng Hầu Hổ.',[[3,10,0,0,0,0,10]]),
step('hau','Tân Thức',3,21,4,5,'Trở lại gặp Lỗ Hùng.'),
step('lo','Tân Thức',3,21,5,6,'Gặp Sùng Hắc Hổ.'),
step('hac','Tân Thức',3,21,6,7,'Gặp Âu Thiên Hóa.'),
step('au','Tân Thức',3,21,7,8,'Trở lại gặp Lỗ Hùng nhận thưởng.'),
step('lo','Tân Thức',3,21,8,9,'Hoàn thành Tân Thức.',[],[],500,600),
// The guide names Sung Ung Loan; the old Lua put acceptance on Trieu Dien.
step('loan','Dũng Đao',7,25,0,1,'Diệt Hoàn Cẩu Vương ở Yến Sơn, lấy đủ ba bộ phận đao.'),
step('dien','Dũng Đao',7,25,1,2,'Nhờ Âu Thiên Hóa sửa Dũng Đao.', [event(21),event(22),event(23)] ,[],0,0,0,{keepNeeds:1}),
step('au','Dũng Đao',7,25,2,3,'Mang Dũng Đao về Triều Điền.',[event(21),event(22),event(23)],[event(24)]),
step('dien','Dũng Đao',7,25,3,4,'Hoàn thành Dũng Đao.',[event(24)],[eq(4,0,1,0)],0,0,10),
step('loan','Kiếm Ái',14,24,0,1,'Gặp Âu Thiên Hóa.'),
step('au','Kiếm Ái',14,24,1,2,'Diệt Hoàn Cẩu Tinh ở Yến Sơn, rồi trở về Sùng Ứng Loan.'),
step('loan','Kiếm Ái',14,24,3,4,'Hoàn thành Kiếm Ái.',[event(25)],[[5,0,0,0,0,0,5],[7,58,62,1,0,0,1]],3000,0,0,{camp:7}),
step('hau','Trung Thành',25,3,0,1,'Gặp Trịnh Luân ở Sùng Thành Doanh.'),
step('trinh','Trung Thành',25,3,1,2,'Mang Huyết Thư về Sùng Hầu Hổ.',[],[event(11)]),
step('hau','Trung Thành',25,3,2,10,'Hoàn thành Trung Thành.',[event(11)],[],20000,30000),
step('hac','Trầm Hương',35,3,10,11,'Mang Trầm Hương Mộc tới Hoàng Phi Hổ ở Triều Ca.',[],[event(45)]),
step('hoang','Trầm Hương',35,3,11,20,'Hoàn thành Trầm Hương.',[event(45)],[[7,59,128,1,0,0,1]],80000),
step('hoang','Khuyên Hàng',45,3,20,21,'Hỏi chuyện Lý Tịnh và Đặng Cửu Công.'),
step('ly','Khuyên Hàng',45,3,21,22,'Gặp Đặng Cửu Công tại Tam Sơn.'),
step('dang','Khuyên Hàng',45,3,21,23,'Gặp Lý Tịnh tại Trần Đường.'),
step('dang','Khuyên Hàng',45,3,22,24,'Trở về Hoàng Phi Hổ.'),
step('ly','Khuyên Hàng',45,3,23,24,'Trở về Hoàng Phi Hổ.'),
step('hoang','Khuyên Hàng',45,3,24,25,'Diệt Trấn Tướng Quân tại Mục Dã, lấy ấn tín Võ Thành Vương.'),
step('hoang','Khuyên Hàng',45,3,26,27,'Báo tin cho Khương Tử Nha tại Tây Kỳ.',[event(12)]),
step('khuong','Khuyên Hàng',45,3,27,30,'Hoàn thành Khuyên Hàng.',[],[eq(0,31,4)],240000),
step('duong','Thần Long',55,3,30,31,'Diệt Thần Long ở Long Cung tầng 3, lấy Râu Thần Long.'),
step('dac','Thần Long',55,3,32,33,'Mang Râu Thần Long cho Hồ Hỷ Mị.',[event(13)],[event(9)],0,0,0,{keepNeeds:1}),
step('ho','Thần Long',55,3,33,40,'Hoàn thành Thần Long.',[event(13)],[[3,46,0,0,0,0,1]],600000),
step('dac','Bình An',65,3,40,41,'Gặp Tây Vương Mẫu tại Diêu Trì.'),
step('tay','Bình An',65,3,41,42,'Diệt Cự Dã ở Bích Du Cung tầng 4, lấy An Cư Đồ.'),
step('tay','Bình An',65,3,43,50,'Hoàn thành Bình An.',[event(3)],[],1400000,0,0,{randomBook:1}),
step('nong','Thần Dụ Kính',70,3,50,51,'Diệt Trạnh Nanh ở Khổn Tiên Cung tầng 1, lấy Thần Dụ Kính.'),
step('nong','Thần Dụ Kính',70,3,52,60,'Hoàn thành Thần Dụ Kính. Giữ kính cho nhiệm vụ Tam Sách.',[event(4)],[eq(0,31,7)],2000000,0,0,{keepNeeds:1}),
step('dac','Tam Sách',75,3,60,61,'Mang hai bảng Phong Thần tới Khương Tử Nha. Giữ bảng thật cho Thiên Duyên.',[],[],0,0,0,{ensureBoard:1}),
step('khuong','Tam Sách',75,3,61,62,'Mang Thần Dụ Kính tới Võ Vương.',[event(9)]),
step('vo','Tam Sách',75,3,62,63,'Lấy ba bộ sách tại Khổn Tiên Cung tầng 2, 3 và 4.',[event(4),event(9)]),
step('vo','Tam Sách',75,3,63,70,'Hoàn thành Tam Sách.',[event(6),event(7),event(8)],[eq(4,14,1,0),eq(4,15,1,0),eq(4,16,1,0),eq(4,17,1,0)],3000000),
step('tay','Cổ Kim',80,3,70,71,'Gặp Đắc Kỷ tại Triều Ca mười năm sau.',[],[],0,0,0,{travel:[1063,198,200]}),
step('dac_future','Cổ Kim',80,3,71,72,'Gặp Nguyên Thủy Thiên Tôn tại Ngọc Hư Cung mười năm sau.',[],[],0,0,0,{travel:[1062,203,198]}),
step('nguyen','Cổ Kim',80,3,72,73,'Diệt Bộ Đầu quân Chu, lấy Trúc Giản.'),
step('nguyen','Cổ Kim',80,3,74,80,'Hoàn thành Cổ Kim. Dùng Khứ Lai Phù đến Ngọc Hư Cung mười năm trước.',[event(10)],[[3,41,0,0,0,0,1],[6,1,34,1,0,0,1]],5000000),
step('dac_young','Thiên Duyên',85,3,80,81,'Nói chuyện với Khương Tử Nha thời niên thiếu.',[event(9)]),
step('khuong_young','Thiên Duyên',85,3,81,82,'Trở lại gặp Đắc Kỷ thời niên thiếu.'),
step('dac_young','Thiên Duyên',85,3,82,85,'Diệt Bàn Cổ ở Khổn Tiên Cung tầng 5, lấy Bàn Cổ Thần Khí.'),
step('dac_young','Thiên Duyên',85,3,85,83,'Hoàn thành Thiên Duyên.',[event(175)],[eq(5,6,8)],6500000),
step('khuong_young','Thông Thiên',92,3,83,84,'Tìm Tham Lam tại Thương Nhân Đại Phong, Vô Tri tại Cửu Di Tam Sơn; dùng Mật Đồ ở Bắc Hải tìm Tàn Nhẫn.',[],[[6,1,142,1,0,0,1],[6,1,181,1,0,0,1]]),
step('merchant','Thông Thiên',92,4560,0,1,'Nhận Tham Lam.',[],[event(176)],0,0,0,{mainPhase:84}),
step('cuudi','Thông Thiên',92,4561,0,1,'Nhận Vô Tri.',[],[event(177)],0,0,0,{mainPhase:84}),
step('thong','Thông Thiên',92,3,84,86,'Dùng Khứ Lai Phù gọi Thiên Hồn, Địa Hồn và Mệnh Hồn.',[event(176),event(177),event(178)]),
// The official page shows three choices in a screenshot. The exact choices
// below are filled from that screenshot, rather than inventing a random reward.
step('khuong_young','Thông Thiên: nhận giày',92,3,89,90,'Hoàn thành Thông Thiên.',[event(174)],[eq(5,6,9)],0,0,0,{rewardChoice:1}),
step('khuong_young','Thông Thiên: nhận yêu đái',92,3,89,90,'Hoàn thành Thông Thiên.',[event(174)],[eq(6,6,9)],0,0,0,{rewardChoice:2}),
step('khuong_young','Thông Thiên: nhận mão',92,3,89,90,'Hoàn thành Thông Thiên.',[event(174)],[eq(7,6,9)],0,0,0,{rewardChoice:3}),
step('doctor_manh','Trừ Họa: Mạnh Tân',18,4550,0,1,'Diệt Hoàn Cẩu lấy ấn ký, trong 10 giây diệt Giáp Cốt lấy đủ 5 Hạp Đằng.'),
step('doctor_manh','Trừ Họa: trả Hạp Đằng',18,4550,1,2,'Đến Đại Phu tại Tam Sơn.',[event(191,5)],[],6000),
step('doctor_tam','Trừ Họa: Tam Sơn',18,4550,2,3,'Hái 10 Nguyên Sâm tại Tam Sơn. Cẩn thận Nguyên Tham Tinh.'),
step('doctor_tam','Trừ Họa: trả Nguyên Sâm',18,4550,3,4,'Đến Đại Phu tại Đồng Quan.',[event(192,10)],[],8000),
step('doctor_dong','Trừ Họa: Đồng Quan',18,4550,4,5,'Diệt Ngưu Sát, dùng Tịnh Hóa Phù rồi tịnh hóa năm Sơn Xuyên Liễu.'),
step('doctor_dong','Trừ Họa: trả Sơn Xuyên Liễu',18,4550,5,6,'Hoàn thành Trừ Họa.',[event(201,5)],[eq(2,6,2)],10000),
];
// Loose reward policy explicitly requested by the user.
for(const a of steps)if(a.task===21)a.xp=a.to===9?5000:500;
const roles={
lo:[1002,'npc_restore/1002_02.lua'],au:[1002,'npc_restore/1002_03.lua'],buu:[1002,'npc_restore/1002_04.lua'],loan:[1002,'npc_restore/1002_05.lua'],dien:[1002,'npc_restore/1002_06.lua'],hau:[1002,'npc_restore/1002_08.lua'],hac:[1002,'npc_restore/1002_09.lua'],trinh:[1002,'npc_restore/1002_10.lua'],
hoang:[1021,'npc_restore/1021_00.lua'],dac:[1021,'npc_restore/1021_01.lua'],ho:[1021,'npc_restore/1021_02.lua'],khuong:[1020,'script/Î÷áª/½ª×ÓÑÀ.lua'],duong:[1020,'npc_restore/1020_01.lua'],vo:[1020,'npc_restore/1020_02.lua'],ly:[1065,'npc_restore/1065_00.lua'],
};
// Missing quest actors get explicit authored NPC scripts/placements. Existing
// world actors are rebound when identity and location match, never duplicated.
const added={
dang:[1016,190,204,197,'Đặng Cửu Công'],tay:[1052,159,221,188,'Tây Vương Mẫu'],nong:[1064,202,194,191,'Thần Nông'],dac_future:[1063,164,198,200,'Đắc Kỷ'],nguyen:[1062,202,203,198,'Nguyên Thủy Thiên Tôn'],dac_young:[1061,164,207,203,'Đắc Kỷ'],khuong_young:[1061,202,198,201,'Khương Tử Nha'],merchant:[1034,158,200,200,'Thương Nhân'],cuudi:[1016,255,200,200,'Cửu Di'],thong:[1070,202,204,210,'Thông Thiên Giáo Chủ'],doctor_manh:[1015,149,192,212,'Đại Phu'],doctor_tam:[1016,149,204,200,'Đại Phu'],doctor_dong:[1014,149,197,210,'Đại Phu'],giao:[1002,158,207,198,'Tạp Hóa Thương Nhân'],
};
const kills=[
// template, map, task, from, to, item
[576,1007,24,2,3,25],[122,1018,3,25,26,12],[113,1039,3,31,32,13],[121,1045,3,42,43,3],[120,1047,3,51,52,4],[117,1048,3,63,63,7],[118,1049,3,63,63,8],[119,1050,3,63,63,6],[116,1062,3,73,74,10],[82,1051,3,85,85,175]
];
module.exports={steps,roles,added,kills,event,eq};
