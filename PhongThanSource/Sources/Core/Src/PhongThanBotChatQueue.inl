// Included inside PtBotAIgent; only intent classification leaves the world thread.
struct ChatPending {int From,Bot,Length,Scope;DWORD Until;BYTE Ticket[PHONGTHAN_SESSION_TICKET_SIZE];char Id[17],Text[256],Speaker[160];};
static ChatPending chats[32];static DWORD chatSerial=0;static int forcedIntent=0,forcedStyle=0,forcedPartyAction=0,forcedCommand=0,forcedSemantic=0,activeChat=-1,chatScope=0;
static char generatedReply[256]={0},forcedArgument[256]={0};
static int HexDigit(char c){return c>='0'&&c<='9'?c-'0':c>='a'&&c<='f'?c-'a'+10:-1;}
static void CancelQueuedCompanionChats(int from,int bot){
 for(int i=0;i<32;++i)if(i!=activeChat&&chats[i].From==from&&chats[i].Bot==bot){char path[160];sprintf(path,"BotAIgent\\chat\\in\\%s.req",chats[i].Id);DeleteFileA(path);sprintf(path,"BotAIgent\\chat\\out\\%s.reply",chats[i].Id);DeleteFileA(path);chats[i].From=0;}
}
static bool QueueChat(int from,int bot,const char* text,int length){
 int slot=-1;for(int i=0;i<32;++i)if(!chats[i].From){slot=i;break;}if(slot<0)return false;
 std::string speaker;if(!PlayerKey(from,speaker)||speaker.size()>=160)return false;
 CreateDirectoryA("BotAIgent\\chat",0);CreateDirectoryA("BotAIgent\\chat\\in",0);CreateDirectoryA("BotAIgent\\chat\\out",0);
 ChatPending& pending=chats[slot];ZeroMemory(&pending,sizeof(pending));pending.From=from;pending.Bot=bot;pending.Length=length;pending.Until=GetTickCount()+30000;
 pending.Scope=chatScope;
 lstrcpynA(pending.Speaker,speaker.c_str(),sizeof(pending.Speaker));
 memcpy(pending.Ticket,Player[from].m_SessionTicket,sizeof(pending.Ticket));memcpy(pending.Text,text,length);sprintf(pending.Id,"%08lx%08lx",GetTickCount(),++chatSerial);
 char path[160],data[1024];sprintf(path,"BotAIgent\\chat\\in\\%s.req",pending.Id);
 int n=sprintf(data,"PT_BOT_CHAT\t1\t%lu\t%s\t",(unsigned long)time(0)+28,bots[bot].Config.Id);
 for(i=0;i<length;++i)n+=sprintf(data+n,"%02x",(unsigned char)text[i]);n+=sprintf(data+n,"\t%s\n",pending.Speaker);
 if(!Atomic(path,data,n)){pending.From=0;return false;}return true;
}
static void PollChat(){
 for(int i=0;i<32;++i)if(chats[i].From){ChatPending& p=chats[i];char path[160],magic[32],line[1400],hex[511],argument[511];hex[0]=argument[0]=0;generatedReply[0]=forcedArgument[0]=0;sprintf(path,"BotAIgent\\chat\\out\\%s.reply",p.Id);
  int version=0,intent=0,style=0,partyAction=0,command=0,fields=0,semantic=0;FILE* f=fopen(path,"rb");if(f){bool ok=fgets(line,sizeof(line),f)&&(fields=sscanf(line,"%31s\t%d\t%d\t%d\t%510s\t%d\t%d\t%510s",magic,&version,&intent,&style,hex,&partyAction,&command,argument))>=3&&!strcmp(magic,"PT_BOT_REPLY")&&(version==1||version==2)&&intent>=1&&intent<=5;fclose(f);DeleteFileA(path);if(!ok)intent=1;if(style<0||style>2)style=0;if(!ok||intent!=5||partyAction<1||partyAction>3)partyAction=0;
   semantic=ok&&version==2&&fields>=7&&command>=0&&command<=PTBC_DISCARD;
   if(!ok||version!=2||fields<7||command<PTBC_FOLLOW||command>PTBC_DISCARD)command=0;
   if((command==PTBC_SHOP||command==PTBC_BOSS||command==PTBC_FIGHT||command>=PTBC_PLAYER)&&fields>=8){int count=strlen(argument)/2;bool valid=count>0&&count<=255&&strlen(argument)%2==0;for(int a=0;valid&&a<count;++a){int hi=HexDigit(argument[a*2]),lo=HexDigit(argument[a*2+1]),value=hi*16+lo;if(hi<0||lo<0||value<32||value==127)valid=false;else forcedArgument[a]=(char)value;}forcedArgument[valid?count:0]=0;}
   int bytes=strlen(hex)/2;if(ok&&(intent==1||intent==2)&&bytes>0&&strlen(hex)%2==0){bool valid=true;for(int n=0;n<bytes;++n){int a=HexDigit(hex[n*2]),b=HexDigit(hex[n*2+1]);int value=a*16+b;if(a<0||b<0||value<32||value==127){valid=false;break;}generatedReply[n]=(char)value;}generatedReply[valid?bytes:0]=0;}
  }
  if(!f&&(int)(GetTickCount()-p.Until)<0)continue;if(!intent)intent=1;
  int from=p.From;p.From=0;
  if(from>0&&from<MAX_PLAYER&&Player[from].m_PlayerDBLoad&&Player[from].m_nNetConnectIdx>=0&&!memcmp(Player[from].m_SessionTicket,p.Ticket,sizeof(p.Ticket))&&p.Bot>=0&&p.Bot<count&&bots[p.Bot].Player){
   forcedIntent=intent;forcedCommand=command;forcedSemantic=semantic;forcedStyle=style;forcedPartyAction=partyAction;activeChat=i;chatScope=p.Scope;::PhongThanBotWhisper(from,bots[p.Bot].Config.Name,p.Text,p.Length);forcedIntent=forcedStyle=forcedPartyAction=forcedCommand=forcedSemantic=0;forcedArgument[0]=0;activeChat=-1;chatScope=0;generatedReply[0]=0;
  }
  sprintf(path,"BotAIgent\\chat\\in\\%s.req",p.Id);DeleteFileA(path);
 }
}
static bool ChatLetter(char c){return (c>='a'&&c<='z')||(c>='A'&&c<='Z');}
static void AdaptChatStyle(char* answer){
 if(!forcedStyle)return;char result[256];int out=0;
 for(int i=0;answer[i]&&out<255;){const char* word=0;int length=0;
  if(i==0||!ChatLetter(answer[i-1])){
   if(!strncmp(answer+i,"Minh",4)&&!ChatLetter(answer[i+4])){word=forcedStyle==1?"Tui":"Em";length=4;}
   else if(!strncmp(answer+i,"minh",4)&&!ChatLetter(answer[i+4])){word=forcedStyle==1?"tui":"em";length=4;}
   else if(!strncmp(answer+i,"Ban",3)&&!ChatLetter(answer[i+3])){word=forcedStyle==1?"Ong":"Anh";length=3;}
   else if(!strncmp(answer+i,"ban",3)&&!ChatLetter(answer[i+3])){word=forcedStyle==1?"ong":"anh";length=3;}
  }
  if(word){while(*word&&out<255)result[out++]=*word++;i+=length;}else result[out++]=answer[i++];
 }result[out]=0;lstrcpyA(answer,result);
}
static void RememberChatTurn(const char* raw,int length,const char* answer){
 if(activeChat<0||activeChat>=32)return;ChatPending& pending=chats[activeChat];char path[160],data[2048];sprintf(path,"BotAIgent\\chat\\in\\%s.turn",pending.Id);
 int n=sprintf(data,"PT_BOT_TURN\t1\t%lu\t%s\t%s\t",(unsigned long)time(0),bots[pending.Bot].Config.Id,pending.Speaker);
 for(int i=0;i<length;++i)n+=sprintf(data+n,"%02x",(unsigned char)raw[i]);data[n++]='\t';
 for(i=0;answer[i];++i)n+=sprintf(data+n,"%02x",(unsigned char)answer[i]);n+=sprintf(data+n,"\t%d\n",forcedIntent);Atomic(path,data,n);
}

static void RememberDirectTurn(Runtime& r,int from,const char* raw,const char* answer){
 std::string peer;if(!PlayerKey(from,peer))return;char path[180],data[2048];sprintf(path,"BotAIgent\\chat\\in\\%08lx%08lx.turn",GetTickCount(),++chatSerial);CreateDirectoryA("BotAIgent\\chat",0);CreateDirectoryA("BotAIgent\\chat\\in",0);int n=sprintf(data,"PT_BOT_TURN\t1\t%lu\t%s\t%s\t",(unsigned long)time(0),r.Config.Id,peer.c_str());for(int i=0;raw[i]&&i<255;++i)n+=sprintf(data+n,"%02x",(unsigned char)raw[i]);data[n++]='\t';for(i=0;answer[i]&&i<255;++i)n+=sprintf(data+n,"%02x",(unsigned char)answer[i]);n+=sprintf(data+n,"\t2\n");Atomic(path,data,n);
}
