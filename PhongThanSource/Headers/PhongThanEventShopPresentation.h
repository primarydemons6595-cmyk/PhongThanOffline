#ifndef PHONGTHAN_EVENT_SHOP_PRESENTATION_H
#define PHONGTHAN_EVENT_SHOP_PRESENTATION_H
#include <string.h>
enum {PT_EVENT_SHOP_TITLE_FONT=18,PT_EVENT_SHOP_GLASS_ALPHA=18};
inline int PTEventShopLabelY(int height,int font){int offset=(height-font-1)/2;return offset>0?offset:0;}
inline int PTEventShopButtonFrame(bool enabled,bool down,bool over){return !enabled?0:down?2:over?1:0;}
inline void PTEventShopGlass(KRUShadow& glass,int x,int y){
 memset(&glass,0,sizeof(glass));
 // RU_T_SHADOW takes inverse opacity 0..32, not an RGBA opacity byte.
 glass.Color.Color_b.a=PT_EVENT_SHOP_GLASS_ALPHA;
 glass.oPosition.nX=x+25;glass.oPosition.nY=y+102;
 glass.oEndPos.nX=x+615;glass.oEndPos.nY=y+496;
}
#endif
