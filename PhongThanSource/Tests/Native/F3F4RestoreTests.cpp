#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>
#define _SERVER
#include "../../Headers/PhongThanF4Protocol.h"
#include "../../Headers/PhongThanPills.h"
#include "../../Headers/PhongThanChatLayout.h"
#include "../../Headers/PhongThanTooltipCopy.h"
#define PT_RANK_FILE_TEST
#include "../../Sources/Core/Src/PhongThanRank.inl"
typedef unsigned char BYTE;
enum {MAX_ITEM=100,MAX_PLAYER=2,EQUIPMENT_ROOM_HEIGHT=7,pos_equiproom=3,pos_equiproomex=4,pos_ptbag2=19,pos_ptbag3=20,pos_repositoryroom=5};
struct PlayerItem {int nIdx,nPlace,nX,nY;};
struct MockItem {int id,w,h,genre,detail,stack;int GetID(){return id;}int GetWidth(){return w;}int GetHeight(){return h;}int GetGenre(){return genre;}int GetDetailType(){return detail;}} Item[MAX_ITEM];
typedef MockItem KItem;
struct Links {int count;int GetNext(int i){return i<count?i+1:0;}};
struct Room {int cells[10][6];void ClearItem(){memset(cells,0,sizeof(cells));}bool PlaceItem(int x,int y,int i,int w,int h){if(x<0||y<0||x+w>6||y+h>10)return false;int yy,xx;for(yy=y;yy<y+h;++yy)for(xx=x;xx<x+w;++xx)if(cells[yy][xx])return false;for(yy=y;yy<y+h;++yy)for(xx=x;xx<x+w;++xx)cells[yy][xx]=i;return true;}};
int PositionToRoom(int place){return place==pos_equiproom?0:place==pos_equiproomex?1:place==pos_ptbag2?2:place==pos_ptbag3?3:place>=5&&place<=9?place-1:-1;}
class KItemList {public:int m_Hand,allowed[4];PlayerItem m_Items[65];Links m_UseIdx;Room m_Room[9];
    KItemList(){memset(this,0,sizeof(*this));for(int i=0;i<4;++i)allowed[i]=7;}
    int GetBagRows(int page){return allowed[page];}
    bool ApplyBagLayout(int page,const PTF4Cell* cells,int count);bool SortBag(int page);
};
#include "../../Sources/Core/Src/PhongThanF4Layout.inl"
static int checks=0;
#define CHECK(x) do{++checks;if(!(x)){printf("FAIL line %d: %s\n",__LINE__,#x);return 1;}}while(0)
int main(int argc,char** argv){
    if(argc>1){PTRankEntry role;CHECK(PTReadRankRole(argv[1],role));CHECK(role.Level>0);CHECK(role.Name[0]);}
    {PTRankEntry entries[3];memset(entries,0,sizeof(entries));strcpy(entries[0].Name,"B");strcpy(entries[1].Name,"A");strcpy(entries[2].Name,"C");
        entries[0].Level=entries[1].Level=20;entries[2].Level=12;entries[0].Experience=5;entries[1].Experience=10;
        qsort(entries,3,sizeof(entries[0]),PTRankCompare);CHECK(!strcmp(entries[0].Name,"A"));CHECK(!strcmp(entries[2].Name,"C"));}
    for(int width=800;width<=3840;width+=160)for(int height=600;height<=2160;height+=120){
        PTChatLayout r=PTMakeChatLayout(width,height,156);CHECK(r.RootY+156==height);CHECK(r.BodyY+r.BodyHeight+1==r.FilterY);CHECK(r.InputY+19==height);CHECK(r.SystemX+r.Width==width);CHECK(r.SystemY+25==r.BodyY);
        PTChatLayout dock=PTMakeChatDockLayout(width,height,156,260);CHECK(dock.RootX==width&&dock.RootY==0&&dock.Width==260);CHECK(dock.BodyHeight+45==height);CHECK(dock.SystemX==0&&dock.SystemY+dock.SystemBodyHeight+25==height);CHECK(dock.FilterY==r.FilterY&&dock.InputY==r.InputY);
        PTChatLayout restored=PTMakeChatDockLayout(width,height,156,0);CHECK(!memcmp(&r,&restored,sizeof(r)));
    }
    for(int capacity=1;capacity<=512;++capacity){
        char output[514];memset(output,'X',sizeof(output));char body[2000];memset(body,'a',sizeof(body));body[sizeof(body)-1]=0;
        PTCopyTooltipWithFooter(output,capacity,body,"\nWeight: 3\nCtrl + left click\n");CHECK(strlen(output)<(unsigned int)capacity);CHECK(output[capacity]=='X');
        if(capacity>40)CHECK(strstr(output,"Ctrl + left click")!=0);
    }
    for(int page=0;page<9;++page)for(int n=0;n<=(page>=4?60:35);++n){
        int cols=page>=4?6:5;
        KItemList bag;bag.m_UseIdx.count=n;int place=PTBagPlace(page);
        for(int i=1;i<=n;++i){Item[i].id=100+i;Item[i].w=Item[i].h=1;Item[i].genre=i%4;Item[i].detail=35-i;Item[i].stack=i+1;
            bag.m_Items[i].nIdx=i;bag.m_Items[i].nPlace=place;bag.m_Items[i].nX=(n-i)%cols;bag.m_Items[i].nY=(n-i)/cols;
            CHECK(bag.m_Room[page].PlaceItem((n-i)%cols,(n-i)/cols,i,1,1));}
        CHECK(bag.SortBag(page));
        for(i=1;i<=n;++i){CHECK(bag.m_Items[i].nIdx==i);CHECK(Item[i].stack==i+1);CHECK(bag.m_Room[page].cells[bag.m_Items[i].nY][bag.m_Items[i].nX]==i);}
        PlayerItem before[65];memcpy(before,bag.m_Items,sizeof(before));CHECK(bag.SortBag(page));CHECK(!memcmp(before,bag.m_Items,sizeof(before)));
        PTF4Cell cells[60];memset(cells,0,sizeof(cells));for(i=0;i<n;++i){cells[i].ItemId=Item[i+1].id;cells[i].X=(BYTE)bag.m_Items[i+1].nX;cells[i].Y=(BYTE)bag.m_Items[i+1].nY;}
        if(n>1){cells[1].ItemId=cells[0].ItemId;CHECK(!bag.ApplyBagLayout(page,cells,n));CHECK(!memcmp(before,bag.m_Items,sizeof(before)));}
        if(n){cells[0].X=(BYTE)cols;CHECK(!bag.ApplyBagLayout(page,cells,n));CHECK(!memcmp(before,bag.m_Items,sizeof(before)));}
        bag.m_Hand=1;CHECK(!bag.SortBag(page));CHECK(!memcmp(before,bag.m_Items,sizeof(before)));
    }
    {KItemList bag;bag.m_UseIdx.count=1;bag.m_Items[1].nIdx=1;bag.m_Items[1].nPlace=pos_ptbag2;bag.allowed[2]=1;Item[1].id=51;Item[1].w=2;Item[1].h=2;CHECK(!bag.SortBag(2));CHECK(bag.m_Items[1].nY==0);}
    {PTF4Command c;memset(&c,0,sizeof(c));PhongThanInitializeWireHeader(&c.Header,PT_F4_COMMAND,sizeof(c),PHONGTHAN_WIRE_FLAG_REQUEST,0);c.MapId=1;c.EntityId=2;c.Action=PT_F4_SORT;
        for(int p=0;p<9;++p){c.Page=(BYTE)p;CHECK(PTValidateF4Command(&c,sizeof(c)));}c.Page=9;CHECK(!PTValidateF4Command(&c,sizeof(c)));c.Page=1;c.Action=PT_F4_UNLOCK;c.ExpectedRows=7;CHECK(!PTValidateF4Command(&c,sizeof(c)));c.ExpectedRows=6;CHECK(PTValidateF4Command(&c,sizeof(c)));CHECK(!PTValidateF4Command(&c,sizeof(c)-1));c.Reserved=1;CHECK(!PTValidateF4Command(&c,sizeof(c)));}
    {PTPillsState s;memset(&s,0,sizeof(s));int now=1800000000;s.Armed=1;s.LastSeen=now-7200;PTPillsRestore(s,now,7200);CHECK(s.NewCount==1);CHECK(s.Count==0);CHECK(!s.Armed);CHECK(s.MadeToday==1);
        s.Armed=1;s.LastSeen=now-86400;PTPillsRestore(s,now,7200);CHECK(s.NewCount==12);CHECK(s.MadeToday==12);
        s.Count=3;s.Until=now-1200;s.Bonus=50;PTPillsExpire(s,now);CHECK(s.Count==0);CHECK(s.Until==now+600);CHECK(s.Bonus==50);PTPillsExpire(s,now+600);CHECK(!s.Until);CHECK(!s.Bonus);}
    {PTPillsState s;memset(&s,0,sizeof(s));int now=1800000000;
        s.Armed=1;s.LastSeen=now-3599;PTPillsRestore(s,now,3600);CHECK(s.NewCount==0);CHECK(s.Seconds==3599);
        s.Armed=1;s.LastSeen=now-1;PTPillsRestore(s,now,3600);CHECK(s.NewCount==1);CHECK(s.Seconds==0);
        s.Armed=1;s.LastSeen=now-86400;PTPillsRestore(s,now,3600);CHECK(s.NewCount==12);CHECK(s.MadeToday==12);
    }
    printf("PASS F3/F4: %d assertions (production bag layout/sort, malformed packet rejection, offline pill clock)\n",checks);return 0;
}
