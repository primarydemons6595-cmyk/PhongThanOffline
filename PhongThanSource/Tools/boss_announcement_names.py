"""Reviewed scheduled boss template names; keys remain native template IDs."""
BOSS_NAMES={90:'Hỗn Độn',87:'Cùng Kỳ',89:'Thao Thiết',88:'Đào Ngột',
    86:'Cửu Anh',82:'Bàn Cổ',85:'Đại Bằng',96:'Dương Tiễn',97:'Giao Long',
    98:'Ly Long',125:'Bất Nghĩa Hầu',99:'Cầu Long'}


def boss_announcement_name(template):
    return 'Boss Hoàng Kim - '+BOSS_NAMES[template]
