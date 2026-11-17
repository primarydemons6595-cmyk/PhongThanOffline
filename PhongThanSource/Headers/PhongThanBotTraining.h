#ifndef PHONGTHAN_BOT_TRAINING_H
#define PHONGTHAN_BOT_TRAINING_H
#include "PhongThanBotRoutes.h"
#include <stdlib.h>
inline int PTBotConservativeCeiling(int level){return level<5?5:level;}
inline bool PTBotShouldReturnForTraining(unsigned int idle,unsigned int retry,int map,int home,bool companion,bool quest,int scrolls,bool busy){return idle>=30000&&retry>=60000&&map!=home&&!companion&&!quest&&scrolls>0&&!busy;}
inline int PTBotDecisionBatch(int population){return population>0?(population+17)/18:0;}
inline int PTBotRouteWithout(int from,int to,int blocked,int* first=0){
 if(first)*first=-1;if(from<1000||from>=1256||to<1000||to>=1256)return -1;
 int previous[256],edge[256],queue[256],head=0,tail=0;for(int i=0;i<256;++i){previous[i]=-1;edge[i]=-1;}previous[from-1000]=from;queue[tail++]=from;
 while(head<tail){int map=queue[head++];if(map==to)break;for(int e=0;e<sizeof(PTBotEdges)/sizeof(PTBotEdges[0]);++e){const PTBotEdge& p=PTBotEdges[e];if(e==blocked||p.From!=map||p.To<1000||p.To>=1256||previous[p.To-1000]>=0)continue;previous[p.To-1000]=map;edge[p.To-1000]=e;queue[tail++]=p.To;}}
 if(previous[to-1000]<0)return -1;int hops=0,choice=-1,node=to;while(node!=from){choice=edge[node-1000];node=previous[node-1000];++hops;}if(first)*first=choice;return hops;
}
inline int PTBotRoute(int from,int to,int* first=0){
 if(from<1000||from>=1256||to<1000||to>=1256)return -1;
 // Authored world edges are immutable. Share resolved routes across all bots
 // instead of repeating BFS for every monster considered by every decision.
 static int cachedHops[256][256],cachedFirst[256][256];static bool ready=false;
 if(!ready){for(int f=0;f<256;++f)for(int t=0;t<256;++t){cachedHops[f][t]=-2;cachedFirst[f][t]=-1;}ready=true;}
 int& cached=cachedHops[from-1000][to-1000];int& cachedEdge=cachedFirst[from-1000][to-1000];
 if(cached!=-2){if(first)*first=cachedEdge;return cached;}
 int previous[256],via[256],queue[256],head=0,tail=0;
 for(int i=0;i<256;++i){previous[i]=-1;via[i]=-1;}
 previous[from-1000]=from;queue[tail++]=from;
 while(head<tail){int map=queue[head++];if(map==to)break;
  for(int e=0;e<sizeof(PTBotEdges)/sizeof(PTBotEdges[0]);++e){const PTBotEdge& edge=PTBotEdges[e];
   if(edge.From!=map||edge.To<1000||edge.To>=1256||previous[edge.To-1000]>=0)continue;
   previous[edge.To-1000]=map;via[edge.To-1000]=e;queue[tail++]=edge.To;
  }
 }
 if(previous[to-1000]<0){cached=-1;if(first)*first=-1;return -1;}
 int hops=0,node=to,index=-1;while(node!=from){index=via[node-1000];node=previous[node-1000];++hops;}
 cached=hops;cachedEdge=index;if(first)*first=index;return hops;
}
// Score actual live monsters, not a fixed level->map lookup. -1 rejects danger/unreachable areas.
inline int PTBotTrainScore(int level,int monster,int hops,int distance,int penalty,bool questTarget=false){
 // Authored starter monsters begin at level 5, while new players begin at 1.
 // Keep that real novice band usable instead of reserving the world's lone level-1 mob.
 int ceiling=level<=5?level+4:level+2;
 if(level<1||monster<1||monster>ceiling||(!questTarget&&monster<(level>5?level-5:1))||hops<0||hops>255)return -1;
 return abs(monster-level)*400+hops*250+distance/128+penalty;
}
#endif
