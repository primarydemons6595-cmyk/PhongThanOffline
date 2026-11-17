#ifndef PHONGTHAN_RENDER_MOTION_H
#define PHONGTHAN_RENDER_MOTION_H

// Presentation only: authoritative map/cell coordinates are never written here.
// The original renderer has separate display and target anchors. Interpolate
// those anchors over one 18Hz tick, using elapsed time rather than paint count
// so 30/54 FPS and irregular paints retain the same motion speed.
class PhongThanRenderMotion {
    int from[3],target[3];
    unsigned int started;
    bool initialized;
public:
    PhongThanRenderMotion(){Reset();}
    void Reset(){initialized=false;started=0;for(int i=0;i<3;++i)from[i]=target[i]=0;}
    bool IsInitialized()const{return initialized;}
    void Sample(unsigned int now,int& x,int& y,int& z)const{
        unsigned int elapsed=now-started;
        if(elapsed>55)elapsed=55;
        int* result[3]={&x,&y,&z};
        for(int i=0;i<3;++i)*result[i]=from[i]+(target[i]-from[i])*(int)elapsed/55;
    }
    void SetTarget(int x,int y,int z,unsigned int now,bool smooth){
        int next[3]={x,y,z};
        bool snap=!initialized||!smooth||now-started>250;
        for(int i=0;i<3;++i){
            int delta=next[i]-target[i];
            // Match the original's small-motion envelope; map changes,
            // teleports and large corrections must not slide across terrain.
            if(delta>=60||delta<=-60)snap=true;
        }
        if(snap){for(int i=0;i<3;++i)from[i]=target[i]=next[i];started=now;initialized=true;return;}
        if(x==target[0]&&y==target[1]&&z==target[2])return;
        int current[3];Sample(now,current[0],current[1],current[2]);
        for(int j=0;j<3;++j){from[j]=current[j];target[j]=next[j];}
        started=now;
    }
};
#endif
