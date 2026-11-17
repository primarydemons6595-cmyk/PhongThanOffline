#ifndef PHONGTHAN_EQUIPPED_SOCKET_LAYOUT_H
#define PHONGTHAN_EQUIPPED_SOCKET_LAYOUT_H
// Original large-item artwork planes. Coordinates in the client-only socket
// table are pixels in these planes, not percentages of every F4 rectangle.
inline int PhongThanEquipmentSocketPlaneHeight(int detail){return detail==7?64:detail==6?43:96;}
inline void PhongThanEquippedSocketCenter(int detail,int x,int y,int width,int height,int authoredX,int authoredY,int& cx,int& cy){
 cx=x+(width-64)/2+authoredX;
 cy=y+(height-PhongThanEquipmentSocketPlaneHeight(detail))/2+authoredY;
}
#endif
