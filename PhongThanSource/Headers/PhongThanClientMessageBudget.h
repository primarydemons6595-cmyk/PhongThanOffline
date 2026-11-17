#ifndef PHONGTHAN_CLIENT_MESSAGE_BUDGET_H
#define PHONGTHAN_CLIENT_MESSAGE_BUDGET_H
// Consume only the packet that will be executed now. Unread packets remain in
// the transport FIFO. Atomic handlers are never interrupted or replayed.
struct PTClientDrainResult {
 int visited,packets;bool exhausted;
 PTClientDrainResult():visited(0),packets(0),exhausted(false){}
};
template<class Adapter>
PTClientDrainResult PTDrainClientMessages(Adapter& adapter,int& cursor,int clients,int quantum,double milliseconds){
 PTClientDrainResult result;if(clients<=0){cursor=0;return result;}
 if(cursor<0||cursor>=clients)cursor=0;
 double start=adapter.NowMilliseconds();
 while(result.visited<clients){
  if(result.visited&&milliseconds>0&&adapter.NowMilliseconds()-start>=milliseconds){result.exhausted=true;break;}
  int client=cursor;cursor=(cursor+1)%clients;++result.visited;
  if(!adapter.Connected(client))continue;
  int taken=0;
  while(quantum<=0||taken<quantum){
   if(result.packets&&milliseconds>0&&adapter.NowMilliseconds()-start>=milliseconds){result.exhausted=true;return result;}
   if(!adapter.Connected(client))break;
   unsigned int size=0;const char* packet=adapter.Pop(client,size);
   if(!packet||!size)break;
   adapter.Handle(client,packet,size);++result.packets;++taken;
  }
 }
 return result;
}
#endif
