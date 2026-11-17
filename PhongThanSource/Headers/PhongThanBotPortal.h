#ifndef PT_BOT_PORTAL_H
#define PT_BOT_PORTAL_H
// UI route markers are approximate. Choose an actual traversable trigger cell,
// with no direct ChangeWorld or fabricated passage through blocked terrain.
template<class Grid> bool PTBotPortalCell(Grid& grid,int markerX,int markerY,int startX,int startY,int& x,int& y){
 int best=2147483647;bool found=false;for(int dy=-20;dy<=20;++dy)for(int dx=-20;dx<=20;++dx){int cx=markerX+dx,cy=markerY+dy;if(!grid.Trap(cx,cy)||!grid.Free(cx,cy))continue;int score=(abs(dx)+abs(dy))*8+abs(cx-startX)+abs(cy-startY);if(score<best){best=score;x=cx;y=cy;found=true;}}return found;
}
#endif
