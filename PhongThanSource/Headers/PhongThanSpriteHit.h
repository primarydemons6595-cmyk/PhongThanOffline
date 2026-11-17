#ifndef PHONGTHAN_SPRITE_HIT_H
#define PHONGTHAN_SPRITE_HIT_H
// Coordinates come from the same projection and SPR reference used to draw.
// Cropped frames may extend below the world anchor (notably escort carts).
inline bool PtSpriteFrameHit(int x,int y,int ax,int ay,int cx,int cy,
    int ox,int oy,int width,int height)
{
    if(width<=0 || height<=0)return false;
    int left=ax+ox-cx,top=ay+2*(oy-cy);
    return x>=left && x<left+width && y>=top && y<top+2*height;
}
#endif
