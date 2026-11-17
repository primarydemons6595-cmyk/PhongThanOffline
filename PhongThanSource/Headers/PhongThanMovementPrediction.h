#ifndef PHONGTHAN_MOVEMENT_PREDICTION_H
#define PHONGTHAN_MOVEMENT_PREDICTION_H

// MOVE_REQUEST.Sequence: nonzero 16-bit command identity (zero = legacy).
// RECONCILE.Sequence: command identity in high word, simulated move calls in
// low word. No wire-size change; either old peer uses the legacy correction.
// This history never supplies a position to the server or changes move speed.
class PhongThanMovementHistory
{
    struct Sample { unsigned int sequence,step,map,actor,time; int x,y; };
    Sample samples[256];
    unsigned int next;
public:
    PhongThanMovementHistory():next(0){Clear();}
    void Clear(){for(int i=0;i<256;++i)samples[i].sequence=0;next=0;}
    void Record(unsigned int sequence,unsigned int step,unsigned int map,
        unsigned int actor,unsigned int time,int x,int y)
    {
        if(!sequence||step>=65535)return;
        Sample& s=samples[next++%256];s.sequence=sequence;s.step=step;
        s.map=map;s.actor=actor;s.time=time;s.x=x;s.y=y;
    }
    bool Matches(unsigned int stamp,unsigned int map,unsigned int actor,
        unsigned int now,int x,int y) const
    {
        const unsigned int sequence=stamp>>16,step=stamp&65535;
        if(!sequence||!step||step==65535)return false;
        for(unsigned int n=0;n<256;++n){
            const Sample& s=samples[(next-1-n)&255];
            if(s.sequence!=sequence||s.step!=step||s.map!=map||s.actor!=actor||
                (unsigned int)(now-s.time)>1500)continue;
            // Same tolerance as legacy reconcile, compared at the same move
            // step. 64-bit math avoids overflow on malformed coordinates.
            const __int64 dx=(__int64)x-s.x,dy=(__int64)y-s.y;
            return dx*dx+dy*dy<=(__int64)128*128;
        }
        return false;
    }
};

#ifndef _SERVER
unsigned int PhongThanBeginPredictedMove();
void PhongThanClearPredictedMoves();
bool PhongThanMatchesPredictedMove(unsigned int stamp,unsigned int map,
    unsigned int actor,int x,int y);
#endif
#endif
