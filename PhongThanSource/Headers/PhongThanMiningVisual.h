#ifndef PHONGTHAN_MINING_VISUAL_H
#define PHONGTHAN_MINING_VISUAL_H
// A native alpha halo, independent of the daytime scene-light map.
// Emit narrow world-space strips in concentric ellipses for a soft edge.
template<class Sink> inline void PhongThanMiningHalo(unsigned long tick,Sink& sink){
    int phase=(int)(tick%2400);if(phase>1200)phase=2400-phase;
    for(int ring=8;ring>=1;--ring){
        int rx=12*ring+phase/160,ry=8*ring+phase/240;
        for(int y=-ry;y<ry;y+=4){
            double unit=(double)(y+2)/ry;if(unit>1||unit< -1)continue;
            int width=(int)(rx*sqrt(1-unit*unit));if(width<1)continue;
            sink.Strip(-width,y,width,y+4,5+phase/200);
        }
    }
}
inline bool PhongThanMiningAcceptOwnPose(int value){return value==62;}
#endif
