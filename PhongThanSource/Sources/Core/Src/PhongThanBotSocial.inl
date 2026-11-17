// Nearby speech is delivered only after validating the actual live participants.
static bool SocialAlive(int player){return player>0&&player<MAX_PLAYER&&Player[player].m_PlayerDBLoad&&Player[player].m_nIndex>0&&Player[player].m_nIndex<MAX_NPC&&Npc[Player[player].m_nIndex].m_CurrentLife>0&&Npc[Player[player].m_nIndex].m_SubWorldIndex>=0&&Npc[Player[player].m_nIndex].m_SubWorldIndex<MAX_SUBWORLD;}
static void SocialObservers(FILE* f){
 fprintf(f,",\"SocialVersion\":1,\"Observers\":[");int total=0;
 for(int p=1;p<MAX_PLAYER&&total<32;++p)if(SocialAlive(p)&&!Player[p].m_bIsBot&&Player[p].m_nNetConnectIdx>=0){int actor=Player[p].m_nIndex;bool isNearby=false;
  for(int i=0;i<count;++i)if(bots[i].Player&&SocialAlive(bots[i].Player)&&Npc[Player[bots[i].Player].m_nIndex].m_SubWorldIndex==Npc[actor].m_SubWorldIndex&&NpcSet.GetDistance(Player[bots[i].Player].m_nIndex,actor)<=1024){isNearby=true;break;}
  std::string key;if(!isNearby||!PlayerKey(p,key))continue;char hex[65];int j=0;for(;Player[p].Name[j]&&j<32;++j)sprintf(hex+j*2,"%02x",(unsigned char)Player[p].Name[j]);hex[j*2]=0;int x=0,y=0;Npc[actor].GetMpsPos(&x,&y);
  fprintf(f,"%s{\"Key\":\"%s\",\"NameHex\":\"%s\",\"Map\":%d,\"X\":%d,\"Y\":%d}",total?",":"",key.c_str(),hex,SubWorld[Npc[actor].m_SubWorldIndex].m_SubWorldID,x,y);++total;
 }fprintf(f,"]");
}
static void SocialTick(){
 static DWORD global=0,last[Capacity];if(expires<(unsigned long)time(0)||GetTickCount()-global<3000)return;
 CreateDirectoryA("BotAIgent\\social",0);CreateDirectoryA("BotAIgent\\social\\in",0);CreateDirectoryA("BotAIgent\\social\\out",0);
 WIN32_FIND_DATAA found;HANDLE files=FindFirstFileA("BotAIgent\\social\\in\\*.say",&found);if(files==INVALID_HANDLE_VALUE)return;int limit=0;
 do{if(++limit>4)break;char id[17];if(strlen(found.cFileName)!=20)continue;memcpy(id,found.cFileName,16);id[16]=0;bool valid=true;for(int c=0;c<16;++c)if(PTBotNameHex(id[c])<0)valid=false;if(!valid)continue;
  char path[180],line[1024],magic[32],bot[33]={0},target[161]={0},kind=0,hex[511]={0};int version=0,map=0;unsigned long deadline=0;sprintf(path,"BotAIgent\\social\\in\\%s",found.cFileName);FILE* f=fopen(path,"rb");if(!f)continue;
  valid=fgets(line,sizeof(line),f)&&sscanf(line,"%31s\t%d\t%lu\t%32s\t%c\t%160s\t%d\t%510s",magic,&version,&deadline,bot,&kind,target,&map,hex)==8&&!strcmp(magic,"PT_BOT_SAY")&&version==1&&deadline>=(unsigned long)time(0)&&deadline<=(unsigned long)time(0)+20;fclose(f);DeleteFileA(path);
  int slot=-1;for(int i=0;i<count;++i)if(!strcmp(bots[i].Config.Id,bot)){slot=i;break;}
  int player=slot>=0?bots[slot].Player:0,peer=0;
  if(valid&&slot>=0&&bots[slot].Config.Enabled&&SocialAlive(player)&&GetTickCount()-last[slot]>=15000){
   if(kind=='B'){for(i=0;i<count;++i)if(!strcmp(bots[i].Config.Id,target)){peer=bots[i].Player;break;}}
   else if(kind=='P'){for(int p=1;p<MAX_PLAYER;++p)if(SocialAlive(p)&&!Player[p].m_bIsBot&&Player[p].m_nNetConnectIdx>=0){std::string key;if(PlayerKey(p,key)&&key==target){peer=p;break;}}}
   int actor=Player[player].m_nIndex,other=peer>0&&peer<MAX_PLAYER?Player[peer].m_nIndex:0;
   if(kind=='W'||kind=='T')valid=SubWorld[Npc[actor].m_SubWorldIndex].m_SubWorldID==map;
   else valid=peer!=player&&SocialAlive(peer)&&Npc[actor].m_SubWorldIndex==Npc[other].m_SubWorldIndex&&SubWorld[Npc[actor].m_SubWorldIndex].m_SubWorldID==map&&NpcSet.GetDistance(actor,other)<=768;
  }else valid=false;
  char speech[256];int length=valid?strlen(hex)/2:0;if(!length||strlen(hex)%2)valid=false;
  for(int n=0;valid&&n<length;++n){int a=PTBotNameHex(hex[n*2]),b=PTBotNameHex(hex[n*2+1]),value=a*16+b;if(a<0||b<0||value<32||value==127)valid=false;else speech[n]=(char)value;}speech[valid?length:0]=0;
  if(valid&&(kind=='W'||kind=='T')){valid=::PhongThanBotEmitPublic(player,kind=='W'?PT_CHAT_WORLD:PT_CHAT_TRADE,speech,length);if(valid){global=last[slot]=GetTickCount();Log("SOCIAL",bots[slot].Config.Id,kind=='W'?"world_conversation_spoken":"trade_advertisement_spoken");}}
  else if(valid){char packet[sizeof(tagExtendProtoHeader)+sizeof(CHAT_CHANNELCHAT_SYNC)+255];ZeroMemory(packet,sizeof(packet));int total=sizeof(tagExtendProtoHeader)+sizeof(CHAT_CHANNELCHAT_SYNC)+length;
   tagExtendProtoHeader* h=(tagExtendProtoHeader*)packet;h->ProtocolType=s2c_extendchat;h->wLength=total-1;CHAT_CHANNELCHAT_SYNC* msg=(CHAT_CHANNELCHAT_SYNC*)(h+1);msg->ProtocolType=chat_channelchat;msg->wSize=sizeof(*msg)+length-1;msg->packageID=(DWORD)-1;msg->channelid=1;msg->sentlen=(BYTE)length;lstrcpynA(msg->someone,bots[slot].Config.Name,sizeof(msg->someone));memcpy(msg+1,speech,length);
   int actor=Player[player].m_nIndex;for(int p=1;p<MAX_PLAYER;++p)if(SocialAlive(p)&&!Player[p].m_bIsBot&&Player[p].m_nNetConnectIdx>=0&&Npc[Player[p].m_nIndex].m_SubWorldIndex==Npc[actor].m_SubWorldIndex&&NpcSet.GetDistance(actor,Player[p].m_nIndex)<=1024)g_pServer->PackDataToClient(Player[p].m_nNetConnectIdx,packet,total);
   global=last[slot]=GetTickCount();Log("SOCIAL",bots[slot].Config.Id,kind=='B'?"nearby_bot_conversation_spoken":"nearby_player_greeting_spoken");
  }
  sprintf(path,"BotAIgent\\social\\out\\%s.ack",id);Atomic(path,valid?"1":"0",1);if(valid)break;
 }while(FindNextFileA(files,&found));FindClose(files);
}
