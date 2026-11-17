#ifndef PHONGTHAN_MAP_CELLS_H
#define PHONGTHAN_MAP_CELLS_H
#include <string.h>
// Lossless RLE storage for an idle region's barriers/traps. Entity lists and
// occupancy grids are deliberately retained; waking never respawns entities.
struct PTMapCellData {long obstacle[16][32];unsigned long trap[16][32];};
class PTMapCells {
    PTMapCellData* live;unsigned char* packed;int packedSize;
    PTMapCells(const PTMapCells&);PTMapCells& operator=(const PTMapCells&);
public:
    PTMapCells():live(0),packed(0),packedSize(0){}
    ~PTMapCells(){delete live;delete[] packed;}
    PTMapCellData* Get(){
        if(!live){live=new PTMapCellData;memset(live,0,sizeof(*live));
            if(packed){unsigned char* out=(unsigned char*)live;int at=0,done=0;
                while(at<packedSize){int count=packed[at++];unsigned char value=packed[at++];memset(out+done,value,count);done+=count;}
                delete[] packed;packed=0;packedSize=0;
            }
        }return live;
    }
    void Sleep(){
        if(!live)return;const unsigned char* src=(const unsigned char*)live;int size=sizeof(*live),need=0;
        for(int i=0;i<size;){unsigned char value=src[i];int n=1;while(i+n<size&&n<255&&src[i+n]==value)++n;need+=2;i+=n;}
        if(need+16>=size)return;
        unsigned char* next=new unsigned char[need];int at=0;
        for(int j=0;j<size;){unsigned char value=src[j];int n=1;while(j+n<size&&n<255&&src[j+n]==value)++n;next[at++]=(unsigned char)n;next[at++]=value;j+=n;}
        packed=next;packedSize=need;delete live;live=0;
    }
    unsigned long Bytes()const{return live?sizeof(PTMapCellData):(unsigned long)packedSize;}
    bool Compressed()const{return packed!=0;}
};
#endif
