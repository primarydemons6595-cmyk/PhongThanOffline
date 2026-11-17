#include <windows.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#define _ASSERT assert
class KNode{public:KNode* next;KNode():next(0){}KNode*GetNext(){return next;}};
#include "../../Sources/Core/Src/KIndexNode.h"
enum{MAX_SUBWORLD=2,MAX_OBJECT=20000,Obj_Kind_Item=1};
struct Actor{int m_SubWorldIndex,m_RegionIndex;}Npc[3];
struct List{KNode*head;KNode*GetHead(){return head;}};
struct Region{int m_nConnectRegion[8];List m_ObjList;};
struct World{int m_nTotalRegion;Region m_Region[3];}SubWorld[2];
struct Obj{int m_nKind;}Object[MAX_OBJECT];
#include "../../Sources/Core/Src/PhongThanPetNearby.inl"
int main(){ZeroMemory(SubWorld,sizeof(SubWorld));SubWorld[0].m_nTotalRegion=3;for(int r=0;r<3;++r)for(int i=0;i<8;++i)SubWorld[0].m_Region[r].m_nConnectRegion[i]=-1;
 Npc[1].m_SubWorldIndex=0;Npc[1].m_RegionIndex=0;SubWorld[0].m_Region[0].m_nConnectRegion[0]=1;SubWorld[0].m_Region[0].m_nConnectRegion[1]=1;
 KIndexNode a,b,c;a.m_nIndex=19000;b.m_nIndex=2;c.m_nIndex=3;a.next=&b;SubWorld[0].m_Region[0].m_ObjList.head=&a;SubWorld[0].m_Region[1].m_ObjList.head=&c;Object[19000].m_nKind=Object[3].m_nKind=1;
 int out[5];assert(PtPetNearbyObjects(1,out,5)==2&&out[0]==19000&&out[1]==3);assert(PtPetNearbyObjects(1,out,1)==1);Npc[1].m_RegionIndex=-1;assert(PtPetNearbyObjects(1,out,5)==0);Npc[1].m_RegionIndex=0;Npc[1].m_SubWorldIndex=2;assert(PtPetNearbyObjects(1,out,5)==0);
 puts("PASS production local object scan: high global object ID found immediately, neighbour region included once, non-items excluded, capacity and invalid worlds bounded");return 0;}
