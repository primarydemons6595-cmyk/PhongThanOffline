#include "KIndexNode.h"
// Scan the owner's nine local regions, not the 20,000-object global pool.
static int PtPetNearbyObjects(int actor,int* result,int capacity){
 int world=Npc[actor].m_SubWorldIndex,region=Npc[actor].m_RegionIndex,count=0;
 if(world<0||world>=MAX_SUBWORLD||region<0||region>=SubWorld[world].m_nTotalRegion)return 0;
 int regions[9];regions[0]=region;for(int i=0;i<8;++i)regions[i+1]=SubWorld[world].m_Region[region].m_nConnectRegion[i];
 for(int r=0;r<9&&count<capacity;++r){int at=regions[r];if(at<0||at>=SubWorld[world].m_nTotalRegion)continue;
  bool duplicate=false;for(int old=0;old<r;++old)if(regions[old]==at)duplicate=true;if(duplicate)continue;
  KIndexNode* node=(KIndexNode*)SubWorld[world].m_Region[at].m_ObjList.GetHead();int scanned=0;
  while(node&&scanned++<1024&&count<capacity){int item=node->m_nIndex;node=(KIndexNode*)node->GetNext();if(item>0&&item<MAX_OBJECT&&Object[item].m_nKind==Obj_Kind_Item)result[count++]=item;}
 }return count;
}
