#include <assert.h>
#include <stdio.h>
#include "PhongThanSnapshotPixels.h"
int main(){unsigned short src[]={0xf800,0x07e0,0x001f,0x1234,0xffff,0,0x07e0,0x9999};unsigned char out[18];
 assert(PTSnapshotBgr24(src,8,3,2,true,out));assert(out[0]==0&&out[1]==0&&out[2]==255);assert(out[3]==0&&out[4]==255&&out[5]==0);assert(out[6]==255&&out[7]==0&&out[8]==0);assert(out[9]==255&&out[10]==255&&out[11]==255);assert(out[12]==0&&out[13]==0&&out[14]==0);
 src[0]=0x7c00;assert(PTSnapshotBgr24(src,8,3,1,false,out)&&out[2]==255);assert(!PTSnapshotBgr24(src,4,3,2,true,out));
 puts("PASS native snapshot conversion: colour channels, padded stride, two rows, RGB555/565 and invalid bounds");return 0;}
