#ifndef PT_SNAPSHOT_PIXELS_H
#define PT_SNAPSHOT_PIXELS_H
inline bool PTSnapshotBgr24(const unsigned short* src,int pitch,int width,int height,bool rgb565,unsigned char* dst){
 if(!src||!dst||width<=0||height<=0||pitch<width*2)return false;
 for(int y=0;y<height;++y){const unsigned short* row=(const unsigned short*)((const unsigned char*)src+y*pitch);
  for(int x=0;x<width;++x){unsigned int v=row[x];*dst++=(unsigned char)((v&31)*255/31);*dst++=(unsigned char)(rgb565?((v>>5)&63)*255/63:((v>>5)&31)*255/31);*dst++=(unsigned char)(rgb565?((v>>11)&31)*255/31:((v>>10)&31)*255/31);}
 }return true;
}
#endif
