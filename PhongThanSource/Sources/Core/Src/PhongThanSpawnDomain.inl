#include "PhongThanSpawnDomain.h"
#include "PhongThanVttLayout.h"
#include <map>
#include <vector>
#include <stdio.h>
#include "KPakFile.h"
namespace PtSpawnDomain {
struct Domain {int map,left,top,width,height;std::vector<unsigned char> mask;DWORD checked;FILETIME stamp;bool exists,present;Domain():map(0),left(0),top(0),width(0),height(0),checked(0),exists(false),present(false){ZeroMemory(&stamp,sizeof(stamp));}};
static std::map<int,Domain> domains;
static Domain& Load(int map){
    Domain& d=domains[map];DWORD now=GetTickCount();if(d.checked&&(DWORD)(now-d.checked)<1000)return d;d.checked=now;
    char path[160];sprintf(path,"settings\\phongthan\\spawn_domains\\%d.bin",map);WIN32_FILE_ATTRIBUTE_DATA attr;
    KPakFile resource;FILE* f=0;bool secure=PtReleaseEnabled()!=FALSE;
    if(secure){
        // Release resources are authenticated virtual files, not disk paths.
        // Mounts are immutable for the process lifetime, so cache each map once.
        if(d.present||d.exists)return d;
        char virtualPath[164];sprintf(virtualPath,"\\%s",path);
        if(!resource.Open(virtualPath)){d.exists=false;d.present=false;return d;}
        d.present=true;ZeroMemory(&attr,sizeof(attr));
    }else{
        if(!GetFileAttributesExA(path,GetFileExInfoStandard,&attr)){d.exists=false;d.present=false;d.mask.clear();return d;}d.present=true;
        if(d.exists&&!CompareFileTime(&d.stamp,&attr.ftLastWriteTime))return d;
        f=fopen(path,"rb");if(!f){d.exists=false;d.mask.clear();return d;}
    }
    d.exists=false;d.mask.clear();
    char magic[8];int fields[5];bool ok=(secure?resource.Read(magic,8)==8:fread(magic,1,8,f)==8)&&!memcmp(magic,"PTSPAWN1",8)&&(secure?resource.Read(fields,20)==20:fread(fields,4,5,f)==5);
    if(ok)ok=fields[0]==map&&fields[1]>=0&&fields[2]>=0&&fields[3]>0&&fields[4]>0&&fields[3]<=4096&&fields[4]<=4096&&fields[1]<=9375-fields[3]&&fields[2]<=9375-fields[4];
    if(ok){unsigned int size=(unsigned int)fields[3]*fields[4];d.mask.resize(size);ok=secure?(resource.Read(&d.mask[0],size)==size&&resource.Tell()==resource.Size()):(fread(&d.mask[0],1,size,f)==size&&fgetc(f)==EOF);for(unsigned int i=0;i<size&&ok;++i)if(d.mask[i]>1)ok=false;}
    if(f)fclose(f);if(!ok){d.mask.clear();return d;}
    d.map=map;d.left=fields[1];d.top=fields[2];d.width=fields[3];d.height=fields[4];d.stamp=attr.ftLastWriteTime;d.exists=true;return d;
}
}
bool PhongThanResolveMonsterSpawn(int map,int* x,int* y,bool boss){
    if(boss&&map>=1067&&map<=1070&&x&&y){const PT_VTT_LAYOUT& layout=PT_VTT_LAYOUTS[map-1067];*x=layout.bossX;*y=layout.bossY;return true;}
    if(!x||!y)return false;PtSpawnDomain::Domain& d=PtSpawnDomain::Load(map);
    if(!d.exists)return !d.present&&(map<1067||map>1070);
    int gx=*x/32-d.left,gy=*y/32-d.top;
    if(*x>=0&&*y>=0&&gx>=0&&gy>=0&&gx<d.width&&gy<d.height&&d.mask[gy*d.width+gx])return true;
    double best=1.0e100;int bx=-1,by=-1;
    for(int yy=0;yy<d.height;++yy)for(int xx=0;xx<d.width;++xx)if(d.mask[yy*d.width+xx]){
        double dx=(d.left+xx)*32+16.0-*x,dy=(d.top+yy)*32+16.0-*y,score=dx*dx+dy*dy;
        if(score<best){best=score;bx=xx;by=yy;}
    }
    if(bx<0)return false;*x=(d.left+bx)*32+16;*y=(d.top+by)*32+16;return true;
}
