const e=(id,n=1)=>[4,id,0,0,0,0,n],m=(id,n=1)=>[3,id,0,0,0,0,n];
const a=(role,title,level,task,from,to,note,needs=[],gifts=[],extra={})=>({role,title,level,task,from,to,note,needs,gifts,...extra});
const actions=[
 a('west','Phi Tiên',39,51,0,1,'Gặp Thầy Tướng Số, Tây Kỳ (174,188).'),
 a('west_boi','Phi Tiên',39,51,1,2,'Mang Bá Lạc Nhãn đến hỏi Cao Minh, Xi Vưu Mộ (206,198).',[],[m(29)],{cost:1000}),
 a('cao','Phi Tiên',39,51,2,3,'Diệt Trư Tinh ở Hoang Mạc: (187,186), (183,192), (183,188), (187,189). Lấy Băng Tinh về Người Tây Vực, Tây Kỳ (185,192).'),
 a('west','Phi Tiên',39,51,4,5,'Đã mở hợp thành thú cưỡi. Có thể tìm thêm Băng Tinh để tiếp tục hợp thành.',[e(37)],[],{finish:1}),
 a('tieu','Vi Lao',57,50,0,1,'Hỏi Ông Lão Hái Thuốc, Tây Kỳ (178,192).'),
 a('herbalist','Vi Lao',57,50,1,2,'Diệt Cự Thạch ở Thủ Dương Sơn (151,175), (134,185), (134,204), lấy Huyên Thảo về Võ Cát, Tây Kỳ (181,195).'),
 a('vocat','Vi Lao',57,50,3,4,'Đến Xích Tinh Tử, Ngọc Hư Cung (213,197).',[e(38)]),
 a('xich','Vi Lao',57,50,4,5,'Tìm 1 Liễu Mộc, 1 Côn Lôn Kính, 2 Bất Diệt Thần Đăng. Thi Thú: Bích Du Cung tầng 2 (212,211), (196,213), (190,219). Mua đèn ở Cao Giác, Xi Vưu Mộ (205,197).'),
 a('xich','Vi Lao',57,50,5,6,'Mang bảo vật đến Võ Cát, Tây Kỳ (181,195).',[e(39),e(40),e(41,2)],[],{keep:1}),
 a('vocat','Vi Lao',57,50,6,7,'Trở lại Xích Tinh Tử, Ngọc Hư Cung (213,197), nhận phần thưởng.',[e(39),e(40),e(41,2)]),
 a('xich','Vi Lao',57,50,7,8,'Nhận 5000 kinh nghiệm, 3-5 Hồng Thủy Tinh và 1 đèn còn thừa. Võ Cát có thể nhờ giúp lần nữa.',[],[e(41)],{xp:5000,crystals:1,finish:1}),
 a('vocat','Vi Lao',57,50,8,4,'Đến Xích Tinh Tử, Ngọc Hư Cung (213,197), giúp Võ Cát lần nữa.'),
 a('nham_tau','Phu Thê',27,92,0,1,'Gặp Dương Tiễn, Tây Kỳ (165,185), xin Biến Thân Phù.'),
 a('duong','Phu Thê',27,92,1,2,'Dùng Biến Thân Phù thành Phi Thử và Ngọc Nữ, lần lượt gặp Nhậm Đại Ca, Tây Kỳ (170,195).',[],[[6,1,11,1,0,0,3]]),
 a('nham_ca','Phu Thê',27,92,2,3,'Biến thành Ngọc Nữ, gặp Nhậm Đại Ca (170,195).',[],[],{morph:50}),
 a('nham_ca','Phu Thê',27,92,2,4,'Biến thành Phi Thử, gặp Nhậm Đại Ca (170,195).',[],[],{morph:34}),
 a('nham_ca','Phu Thê',27,92,3,5,'Biến thành biểu tượng tình yêu, gặp Nhậm Đại Tẩu (170,195).',[],[],{morph:34}),
 a('nham_ca','Phu Thê',27,92,4,5,'Biến thành biểu tượng tình yêu, gặp Nhậm Đại Tẩu (170,195).',[],[],{morph:50}),
 a('nham_tau','Phu Thê',27,92,5,6,'Hoàn thành: 20 sức lực và 5000 kinh nghiệm.',[],[],{morph:249,weight:20,xp:5000,finish:1}),
 a('tong','Yên Phúc',23,91,0,1,'Hỏi Thầy Tướng Số, Triều Ca (214,195), về tấm quẻ.'),
 a('east_boi','Yên Phúc',23,91,1,2,'Trở lại Tống Dị Nhân, Triều Ca (191,189), chọn nói thật hoặc mua quẻ.'),
 a('tong','Yên Phúc',23,91,2,5,'Nói thật, nhận 5000 lượng.',[],[],{money:5000,finish:1,label:'Nói thật'}),
 a('tong','Yên Phúc',23,91,2,3,'Mua quẻ giá 300 lượng, mang đến Thầy Tướng Số, Triều Ca (214,195).',[],[[6,1,12,1,0,0,1]],{cost:300,label:'Mua quẻ'}),
 a('east_boi','Yên Phúc',23,91,3,4,'Quẻ không thuộc về ngươi. Bị trừ 1 danh vọng.',[[6,1,12,1,0,0,1]],[],{credit:-1,finish:1}),
 a('gambler','Minh Châu',43,41,0,1,'Hỏi Chủ Tiệm Cầm Đồ, Triều Ca (209,198), về Định Hải Bảo Châu.'),
 a('pawn','Minh Châu',43,41,1,2,'Mang Bảo Châu đến Võ Sư ở Phong Thần Đài (182,203).',[],[e(33)]),
 a('library','Minh Châu',43,41,2,3,'Gặp Cù Lưu Tôn, Triều Ca (206,197).',[e(33)],[],{keep:1}),
 a('culuu','Minh Châu',43,41,3,20,'Diệt ba cá ở Đông Hải: Chúc Ngư (225,207), Thố Ngư (222,179), Thể Ngư (204,177). Mang ba ngư huyết, 3 Sơn Thạch và 10 Đồng Thau về Cù Lưu Tôn (206,197).',[e(33)],[],{keep:1}),
 a('pawn','Minh Châu',43,41,10,11,'Mang Bảo Châu về Cù Lưu Tôn, Triều Ca (206,197), khôi phục linh lực lần nữa.',[],[e(33)]),
 a('culuu','Minh Châu',43,41,11,20,'Thu thập ba ngư huyết ở Đông Hải, 3 Sơn Thạch và 10 Đồng Thau, mang về Cù Lưu Tôn (206,197).',[e(33)],[],{keep:1}),
 a('culuu','Minh Châu',43,41,27,10,'Nhận 5000 lượng, có cơ hội nhận Hồng Bảo Thạch. Có thể nhận Bảo Châu mới ở Tiệm Cầm Đồ.',[e(33),e(34),e(35),e(36),m(82,3),m(3,10)],[],{money:5000,ruby:1,finish:1}),
 a('gambler','Minh Châu',43,41,0,0,'Đã cho con bạc vay 100 lượng. Muốn tìm hiểu Bảo Châu, hãy từ chối cho vay ở lần nói chuyện tiếp theo.',[],[],{cost:100,label:'Cho vay',loan:1})
];
const roles={west:[1020,'npc_restore/1020_03.lua'],west_boi:[1020,'npc_restore/1020_04.lua'],cao:[1004,'npc_restore/1004_07.lua'],tieu:[1020,'npc_restore/1020_14.lua'],herbalist:[1020,'npc_recovered_batch/1020_c70224a3.lua'],vocat:[1020,'npc_restore/1020_05.lua'],xich:[1003,'script/ÓñÐé¹¬/³à¾«×Ó.lua'],nham_ca:[1020,'npc_restore/1020_12.lua'],nham_tau:[1020,'npc_restore/1020_13.lua'],duong:[1020,'npc_restore/1020_01.lua'],tong:[1021,'npc_restore/1021_06.lua'],east_boi:[1021,'npc_restore/1021_03.lua'],gambler:[1021,'npc_restore/1021_04.lua'],pawn:[1021,'npc_restore/1021_05.lua'],culuu:[1021,'npc_restore/1021_08.lua'],library:[1001,'npc_recovered_batch/1001_1a469ad5.lua'],store_west:[1020,'npc_recovered_batch/1020_1cbd6c1b.lua'],store_east:[1021,'npc_recovered_batch/1021_517a52ae.lua'],caogiac:[1004,'npc_restore/1004_06.lua']};
module.exports={actions,roles};
