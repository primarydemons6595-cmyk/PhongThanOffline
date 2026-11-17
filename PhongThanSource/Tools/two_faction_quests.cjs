// Main quest phases and item keys retained from the audited VNG Lua.
// XP/coin rewards follow the official Tam Gioi guides where the old Lua differs.
const base=require('./giapsi/quests.cjs');
const e=base.event,eq=base.eq;
const steps=[];
function a(f,role,title,level,from,to,note,needs=[],gifts=[],xp=0,money=0,extra={}){
 steps.push({f,role,title,level,task:f===1?1:2,from,to,note,needs,gifts,xp,money,credit:0,...extra});
}
a(1,'hoanglong','Chinh Đồ',25,0,1,'Mang thư tiến cử đến Khương Tử Nha, Tây Kỳ (158,189).',[],[e(0)]);
a(1,'khuong','Chinh Đồ',25,1,2,'Hoàn thành Chinh Đồ.',[e(0)],[],20000,30000);
a(1,'lei_tay','Thế Sở',35,2,10,'Lần lượt gặp Hoàng Phi Hổ, Lý Tịnh và Đặng Cửu Công.');
for(let p=10;p<17;p++)for(const [role,bit,name] of [['hoang',1,'Hoàng Phi Hổ'],['ly',2,'Lý Tịnh'],['dang',4,'Đặng Cửu Công']])
 if(!((p-10)&bit))a(1,role,'Thế Sở',35,p,p+bit,'Đã hỏi chuyện '+name+'. Gặp đủ ba tướng rồi về Lôi Chấn Tử.');
a(1,'lei_tay','Thế Sở',35,17,20,'Hoàn thành Thế Sở.',[],[[7,59,128,1,0,0,1]],80000);
a(1,'khuong','Thần Oanh',45,20,21,'Gặp Hoàng Thiên Hóa, Triều Ca (214,184).');
a(1,'hoangthien','Thần Oanh',45,21,22,'Diệt Thần Oanh ở Bắc Hải (213,204), (209,200), (228,203).');
a(1,'thohanh','Thần Oanh',45,23,24,'Mang Thần Oanh về Hoàng Thiên Hóa.',[e(1)]);
a(1,'hoangthien','Thần Oanh',45,24,30,'Hoàn thành Thần Oanh.',[],[eq(0,33,4,1)],240000);
a(1,'thohanh','Thần Mộc',55,30,31,'Hỏi Hồ Hỷ Mị, Triều Ca (205,184).');
a(1,'ho','Thần Mộc',55,31,32,'Diệt Bất Tử Thần Mộc ở Sa Mạc Phong Thần (212,181).');
a(1,'ho','Thần Mộc',55,33,40,'Hoàn thành Thần Mộc.',[e(2)],[[3,46,0,0,0,0,1]],600000);
a(2,'hinh','Mao Lư',25,0,1,'Gặp Hồ Hỷ Mị, Triều Ca (205,184) nhận thiếp mời.');
a(2,'ho','Mao Lư',25,1,2,'Mang thiếp mời về Hình Thiên, Xi Vưu Mộ (194,200).',[],[e(15)]);
a(2,'hinh','Mao Lư',25,2,10,'Hoàn thành Mao Lư.',[e(15)],[],20000,30000);
a(2,'phonglam','Bất Túy',35,10,11,'Gặp Ngô Long ở Mạnh Tân (209,195).');
a(2,'ngolong','Bất Túy',35,11,12,'Gặp chủ tửu điếm, Triều Ca (227,195) lấy thuốc tỉnh rượu.');
a(2,'wine','Bất Túy',35,12,13,'Mang thuốc tỉnh rượu về Ngô Long.',[],[e(16)]);
a(2,'ngolong','Bất Túy',35,13,20,'Hoàn thành Bất Túy.',[e(16)],[[7,59,128,1,0,0,1]],80000);
a(2,'thuonghao','Cổ Đao',45,20,21,'Diệt Hiệu Úy Thương ở Mạnh Tân (209,195), cứu tộc nhân.');
a(2,'thuonghao','Cổ Đao',45,22,30,'Hoàn thành Cổ Đao. Giữ chiếc nhẫn cho nhiệm vụ Ma Huyết.',[],[e(17),eq(0,34,4,2)],240000);
a(2,'van','Ma Huyết',55,30,31,'Đến Tam Sơn diệt Hỏa Thú (180,211), lấy Kim Quán.',[e(17)]);
a(2,'dabao','Ma Huyết',55,32,33,'Mang Ma Huyết về Văn Thái Sư, Đồng Quan (193,207).',[e(18)],[e(19)]);
a(2,'van','Ma Huyết',55,33,34,'Gặp Hồ Hỷ Mị ở Triều Ca (205,184).',[e(19)]);
a(2,'ho','Ma Huyết',55,34,40,'Hoàn thành Ma Huyết.',[],[[3,46,0,0,0,0,1]],600000);
for(const f of [1,2])for(const old of base.steps){
 if(!((old.task===3&&old.from>=40)||old.mainPhase===84))continue;
 const q=JSON.parse(JSON.stringify(old));q.f=f;if(q.task===3)q.task=f===1?1:2;
 if(q.role==='nong')q.role=f===1?'hienvien':'xivuu';
 for(const g of q.gifts)if(g[0]===0&&g[1]!==4){g[4]=f;if(g[1]===0)g[2]=f===1?33:34;else g[2]=f===1?7:8;}
 if(q.from===63){q.gifts=[];q.randomFabao70=1;}
 steps.push(q);
}
// These auxiliary task flags are character-local, shared only by the single
// active profession on that character. No Giap Si Lua is rewritten.
for(const f of [1,2])for(const old of base.steps)if(old.task===4550){
 const q=JSON.parse(JSON.stringify(old));q.f=f;
 for(const g of q.gifts)if(g[0]===0){g[2]=f===1?7:8;g[4]=f;}
 steps.push(q);
}
function n(f,role,title,level,task,from,to,note,needs=[],gifts=[],xp=0,money=0,extra={}){
 steps.push({f,role,title,level,task,from,to,note,needs,gifts,xp,money,credit:0,...extra});
}
const m=(id,count=1)=>[3,id,0,0,0,0,count];
const medicine=[[1,0,1,1,0,0,3],[1,3,1,1,0,0,3]];
for(const f of [1,2]){
 const task=f===1?10:30,title=f===1?'Bách Lý':'Khai Trí',root=f===1?'tuhang':'thieuhao';
 n(f,root,title,1,task,0,1,f===1?'Lần lượt gặp Xích Tinh Tử, Linh Bảo Đại Pháp Sư và Nam Cực Tiên Ông.':'Lần lượt gặp Phong Bá, Chúc Dung và Khoa Phụ.');
 const actors=f===1?[['xich',8],['linhbao',2],['namcuc',4]]:[['phongba',2],['chucdung',8],['khoaphu',4]];
 for(let phase=1;phase<15;phase+=2)for(const [role,bit] of actors)if(!((phase-1)&bit))
  n(f,role,title,1,task,phase,phase+bit,'Nghe chỉ dẫn của ba vị rồi trở về phục mệnh.');
 n(f,root,title,1,task,15,16,'Hoàn thành '+title+'.',[],medicine);
}
n(1,'namcuc','Ngũ Thất',3,11,0,1,'Gặp Hoàng Long Chân Nhân, Ngọc Hư Cung (204,195).');
n(1,'hoanglong','Ngũ Thất',3,11,1,2,'Đánh Tuyết Quái quanh chân núi Côn Lôn, thu thập 10 Băng Cơ về Nam Cực Tiên Ông.');
n(1,'namcuc','Ngũ Thất',3,11,2,3,'Gặp Nhiên Đăng Đạo Nhân, Ngọc Hư Cung (206,192).',[m(13,10)]);
n(1,'nhiendang','Ngũ Thất',3,11,3,4,'Mang Không Trung Hỏa về Nam Cực Tiên Ông.',[],[e(20)]);
n(1,'namcuc','Ngũ Thất',3,11,4,5,'Hoàn thành Ngũ Thất.',[e(20)],[],500,600);
n(1,'vantrung','Khảo Nghiệm',7,15,0,1,'Đến Linh Bảo Đại Pháp Sư trả lời ba câu hỏi: 2 - 1 - 3.');
// Keep the original question text and answer callbacks on Linh Bao. The
// original successful answer writes phase 2, which rejoins this graph.
n(1,'vantrung','Khảo Nghiệm',7,15,2,5,'Tiêu diệt Yếm Hỏa ở Thủ Dương Sơn (168,192), rồi về Vân Trung Tử.');
n(1,'vantrung','Khảo Nghiệm',7,15,6,7,'Hoàn thành Khảo Nghiệm.',[],[],0,0,{credit:10,randomFabao10:1});
n(1,'nhiendang','Linh Lực',14,14,0,1,'Đánh Băng Lang quanh chân núi, thu thập 10 Ngọc Cốt về Nhiên Đăng Đạo Nhân.');
n(1,'nhiendang','Linh Lực',14,14,1,2,'Hoàn thành Linh Lực.',[m(9,10)],[[7,58,62,1,0,0,1],[5,0,0,0,0,0,5]],3000,0,{camp:7});
n(2,'hautho','Tân Thức',3,31,0,1,'Gặp Cao Giác, Xi Vưu Mộ (205,197).');
n(2,'caogiac','Tân Thức',3,31,1,2,'Gặp Cao Minh, Xi Vưu Mộ (206,198).');
n(2,'cao','Tân Thức',3,31,2,3,'Mang mật thư đến Hình Thiên, Xi Vưu Mộ (194,200).',[],[e(29)]);
n(2,'hinh','Tân Thức',3,31,3,4,'Trở về gặp Cao Minh.',[e(29)]);
n(2,'cao','Tân Thức',3,31,4,5,'Diệt Hỏa Diện quanh Xi Vưu Mộ, lấy 10 Mặt Quỷ về Hậu Thổ.');
n(2,'hautho','Tân Thức',3,31,5,6,'Hoàn thành Tân Thức.',[m(12,10)],[],500,600);
n(2,'chucdung','Thần Khí',7,35,0,1,'Gặp Cộng Công, Xi Vưu Mộ (192,204).');
n(2,'congcong','Thần Khí',7,35,1,2,'Gặp Cao Minh, Xi Vưu Mộ (206,198).');
n(2,'cao','Thần Khí',7,35,2,3,'Diệt Thảo Tiên Bà Bà ở Miêu Cương (199,206), (187,197), (219,209), lấy ba mảnh thần khí về Cộng Công.');
n(2,'congcong','Thần Khí',7,35,6,7,'Mang Xi Vưu Thần Khí về Chúc Dung.',[e(28,3)],[e(27)]);
n(2,'chucdung','Thần Khí',7,35,7,8,'Hoàn thành Thần Khí.',[e(27)],[],0,0,{credit:10,randomFabao10:1});
n(2,'phongba','Cứu Tế',14,34,0,1,'Gặp Hình Thiên, Xi Vưu Mộ (194,200).');
n(2,'hinh','Cứu Tế',14,34,1,2,'Đến Cự Lộc tìm Đầu Lĩnh (239,192).');
n(2,'tocnhan','Cứu Tế',14,34,2,3,'Diệt Độc Lam Quái ở Cự Lộc, lấy lại lương thực cho tộc nhân.');
n(2,'tocnhan','Cứu Tế',14,34,4,5,'Mang lương thực về Hình Thiên.',[e(30)],[],0,0,{keepNeeds:1});
n(2,'hinh','Cứu Tế',14,34,5,6,'Trở về Phong Bá nhận sách kỹ năng.',[e(30)]);
n(2,'phongba','Cứu Tế',14,34,6,7,'Hoàn thành Cứu Tế.',[],[[7,58,62,1,0,0,1],[5,0,0,0,0,0,5]],2000,0,{camp:7});
n(1,'namcuc','Băng Lang',14,4770,0,1,'Đến Tây Côn Lôn đánh Băng Lang, đủ 20 con sẽ dụ được Băng Lang Vương. Diệt đủ 3 Băng Lang Vương rồi về Nam Cực Tiên Ông.');
n(1,'namcuc','Băng Lang',14,4770,2,3,'Hoàn thành Băng Lang.',[],[],1500,0,{randomTransform:1});
// Quest armour uses VNG set identities 6..10 / 11..15. Profession
// 1/2 is not an elemental Series and makes Gen_Equipment fail.
for(const q of steps)for(const g of q.gifts)
 if(g[0]===0&&[2,5,6,7].includes(g[1])&&[7,8].includes(g[2]))
  g[4]=(g[2]===7?6:11)+Math.floor((g[3]-1)/2);
// Loose reward policy for the matching level-three starter chains.
for(const a of steps)if(a.task===11||a.task===31)a.xp=a.to===(a.task===11?5:6)?5000:500;
module.exports={steps};
