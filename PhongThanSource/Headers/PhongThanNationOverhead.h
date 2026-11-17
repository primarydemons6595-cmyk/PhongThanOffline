#ifndef PHONGTHAN_NATION_OVERHEAD_H
#define PHONGTHAN_NATION_OVERHEAD_H
// Original Phong Than duty IDs; no legacy JX rank mapping.
inline const char* PhongThanNationDutyCaption(int duty){
    switch(duty){
    case 1:return "[L\267nh ch\363a]";
    case 2:return "[B\346 Ch\335nh]";
    case 3:return "[\247\271i th\307n]";
    case 4:return "[D\251n ch\363ng]";
    case 5:return "[Kh\270ch m\352i]";
    default:return "";
    }
}
#endif
