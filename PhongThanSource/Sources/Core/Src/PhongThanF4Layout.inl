#include "../../../Headers/PhongThanF4Protocol.h"
static int PTBagPlace(int page) {return page==0?pos_equiproom:page==1?pos_equiproomex:page==2?pos_ptbag2:page==3?pos_ptbag3:page>=4 && page<=8?pos_repositoryroom+page-4:-1;}
bool KItemList::ApplyBagLayout(int page,const PTF4Cell* cells,int count)
{
    if(page<0 || page>8 || count<0 || count>(page>=4?60:35) || !cells || m_Hand)return false;
    int place=PTBagPlace(page),room=PositionToRoom(place),rows=page>=4?10:GetBagRows(page),columns=page>=4?6:5;
    int list[60],indexes[60],oldCount=0;
    for(int slot=m_UseIdx.GetNext(0);slot;slot=m_UseIdx.GetNext(slot))
        if(m_Items[slot].nPlace==place) {
            if(oldCount==(page>=4?60:35))return false;
            list[oldCount++]=slot;
        }
    if(oldCount!=count)return false;
    bool occupied[10][6],seen[60];memset(occupied,0,sizeof(occupied));memset(seen,0,sizeof(seen));
    for(int i=0;i<count;++i) {
        int found=-1;
        for(int j=0;j<count;++j) {
            int candidate=m_Items[list[j]].nIdx;
            if(candidate<=0 || candidate>=MAX_ITEM)return false;
            if(Item[candidate].GetID()==cells[i].ItemId)found=j;
        }
        if(found<0 || seen[found])return false;
        seen[found]=true;indexes[i]=list[found];int index=m_Items[indexes[i]].nIdx;
        if(index<=0 || index>=MAX_ITEM)return false;
        int w=Item[index].GetWidth(),h=Item[index].GetHeight();
        if(w<1 || h<1 || w>columns || h>rows || cells[i].X+w>columns || cells[i].Y+h>rows)return false;
        for(int y=cells[i].Y;y<cells[i].Y+h;++y)for(int x=cells[i].X;x<cells[i].X+w;++x) {
            if(occupied[y][x])return false;occupied[y][x]=true;
        }
    }
    // All identities and rectangles validated before mutating the existing room.
    m_Room[room].ClearItem();
    for(i=0;i<count;++i) {
        PlayerItem& item=m_Items[indexes[i]];item.nX=cells[i].X;item.nY=cells[i].Y;
        m_Room[room].PlaceItem(item.nX,item.nY,item.nIdx,Item[item.nIdx].GetWidth(),Item[item.nIdx].GetHeight());
    }
#ifndef _SERVER
    CoreDataChanged(PT_GDCNI_F4_LAYOUT,0,page);
#endif
    return true;
}
#ifdef _SERVER
bool KItemList::SortBag(int page)
{
    if(page<0 || page>8 || m_Hand)return false;
    if(page>=4 && !PhongThanBankAccess(m_PlayerIdx,page-3))return false;
    int indexes[60],count=0,place=PTBagPlace(page),rows=page>=4?10:GetBagRows(page),columns=page>=4?6:5;
    if(!rows)return false;
    for(int slot=m_UseIdx.GetNext(0);slot;slot=m_UseIdx.GetNext(slot))if(m_Items[slot].nPlace==place) {
        if(count==(page>=4?60:35))return false;
        int index=m_Items[slot].nIdx;
        if(index<=0 || index>=MAX_ITEM || !Item[index].GetID())return false;
        indexes[count++]=index;
    }
    // Stable category ordering, large rectangles first; a failed packing is a no-op.
    for(int i=1;i<count;++i) {
        int value=indexes[i],j=i-1;
        while(j>=0) {
            KItem& a=Item[indexes[j]];KItem& b=Item[value];
            int areaA=a.GetWidth()*a.GetHeight(),areaB=b.GetWidth()*b.GetHeight();
            bool before=areaB>areaA || (areaB==areaA && (b.GetGenre()<a.GetGenre() ||
                (b.GetGenre()==a.GetGenre() && b.GetDetailType()<a.GetDetailType())));
            if(!before)break;indexes[j+1]=indexes[j];--j;
        }
        indexes[j+1]=value;
    }
    PTF4Cell cells[60];memset(cells,0,sizeof(cells));bool occupied[10][6];memset(occupied,0,sizeof(occupied));
    for(i=0;i<count;++i) {
        int index=indexes[i],w=Item[index].GetWidth(),h=Item[index].GetHeight();bool placed=false;
        if(w<1 || h<1 || w>columns || h>rows)return false;
        for(int y=0;y+h<=rows && !placed;++y)for(int x=0;x+w<=columns && !placed;++x) {
            bool free=true;
            int yy,xx;
            for(yy=y;yy<y+h;++yy)for(xx=x;xx<x+w;++xx)if(occupied[yy][xx])free=false;
            if(!free)continue;
            for(yy=y;yy<y+h;++yy)for(xx=x;xx<x+w;++xx)occupied[yy][xx]=true;
            cells[i].ItemId=Item[index].GetID();cells[i].X=(BYTE)x;cells[i].Y=(BYTE)y;placed=true;
        }
        if(!placed)return false;
    }
    return ApplyBagLayout(page,cells,count);
}
#endif
