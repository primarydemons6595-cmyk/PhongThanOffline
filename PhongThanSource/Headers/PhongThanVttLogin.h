#ifndef PHONGTHAN_VTT_LOGIN_H
#define PHONGTHAN_VTT_LOGIN_H
// Same outside destinations as the four restored mission EndMission scripts.
// Coordinates here are native MPS, rather than the Lua 32-pixel grid.
inline bool PhongThanVttOutside(int arenaMap,int& map,int& x,int& y){
    static const int destinations[4][3]={{1002,1650,3159},{1004,1548,3232},{1003,1709,3105},{1001,1510,3290}};
    if(arenaMap<1067||arenaMap>1070)return false;
    const int* p=destinations[arenaMap-1067];map=p[0];x=p[1]*32;y=p[2]*32;return true;
}
#endif
