#ifndef PHONGTHAN_RECOVERY_CATALOG_H
#define PHONGTHAN_RECOVERY_CATALOG_H
struct PTAutoPoolDef {int key,detail,part,kind;};
static const PTAutoPoolDef ptAutoPools[]={
    {32796,28,3,1},
    {32797,29,4,2},
    {32930,162,3,1},
    {32931,163,4,2},
    {32966,198,3,1},
    {32967,199,4,2},
    {33149,381,3,1},
    {33150,382,4,2},
    {33151,383,3,1},
    {33152,384,4,2},
    {33269,501,3,1},
    {33503,735,3,1},
    {33504,736,4,2},
    {33547,779,3,1},
    {33548,780,4,2},
    {33822,1054,3,1},
    {33823,1055,4,2},
    {33992,1224,3,1},
    {34052,1284,3,1},
    {34053,1285,4,2},
    {34054,1286,3,1},
    {34055,1287,4,2},
    {34069,1301,3,1},
    {34121,1353,3,1},
    {34122,1354,4,2},
    {41497,8729,3,1},
    {41498,8730,4,2},
};
inline const PTAutoPoolDef* PhongThanAutoPool(int key){for(int i=0;i<sizeof(ptAutoPools)/sizeof(ptAutoPools[0]);++i)if(ptAutoPools[i].key==key)return &ptAutoPools[i];return 0;}
#endif
