#ifndef PHONGTHAN_BAG_GRID_H
#define PHONGTHAN_BAG_GRID_H
// Shared geometry for preflight reservations; never modifies live inventory.
inline bool PtReserveBagCell(int* cells,int width,int rows,int w,int h,int value,int& x,int& y){
 if(!cells||w<1||h<1||w>width||h>rows)return false;
 for(int yy=0;yy<=rows-h;++yy)for(int xx=0;xx<=width-w;++xx){
  bool free=true;
  for(int dy=0;dy<h&&free;++dy)for(int dx=0;dx<w;++dx)
   if(cells[(yy+dy)*width+xx+dx]){free=false;break;}
  if(free){for(dy=0;dy<h;++dy)for(int dx=0;dx<w;++dx)cells[(yy+dy)*width+xx+dx]=value;
   x=xx;y=yy;return true;}
 }
 return false;
}
inline bool PtSameFabaoType(int candidateGenre,int candidateDetail,int candidateType,
 int equippedGenre,int equippedDetail,int equippedType,int fabaoDetail){
 return candidateGenre==0&&equippedGenre==0&&candidateDetail==fabaoDetail&&
  equippedDetail==fabaoDetail&&candidateType==equippedType;
}
#endif
