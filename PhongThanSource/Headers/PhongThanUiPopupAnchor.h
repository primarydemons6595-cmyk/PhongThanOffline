#ifndef PHONGTHAN_UI_POPUP_ANCHOR_H
#define PHONGTHAN_UI_POPUP_ANCHOR_H
// Keep the menu's top edge at the click whenever it fits. The old tooltip
// locator flips above every click in the bottom half, even with room below.
inline void PtPopupAtClick(int& x,int& y,int width,int height,int screenW,int screenH){
 int maxX=screenW-width,maxY=screenH-height;
 if(maxX<0)maxX=0;if(maxY<0)maxY=0;
 if(x<0)x=0;else if(x>maxX)x=maxX;
 if(y<0)y=0;else if(y>maxY)y=maxY;
}
#endif
