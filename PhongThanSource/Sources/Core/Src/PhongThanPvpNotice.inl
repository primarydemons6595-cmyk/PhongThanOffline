#define PT_PVP_KILLED_TEXT "\074\143\157\154\157\162\075\157\162\141\156\147\145\076\045\163\074\143\157\154\157\162\076\040\164\271\151\040\074\143\157\154\157\162\075\171\145\154\154\157\167\076\045\163\074\143\157\154\157\162\076\040\143\150\320\155\040\156\147\267\040\074\143\157\154\157\162\075\142\154\165\145\076\045\163\074\143\157\154\157\162\076\056"
// Guild assistance uses the original guild chat channel, never nation chat.
struct PTPvpNoticeWindow {DWORD role,lastDeath;};
static PTPvpNoticeWindow ptPvpNotices[MAX_PLAYER];
static bool PTPvpNoticePlayer(int p){
 return p>0&&p<MAX_PLAYER&&Player[p].m_PlayerDBLoad&&!Player[p].m_bIsBot&&!Player[p].m_bExchangeServer&&Player[p].m_dwID;
}
static void PTPvpSafeName(char* out,int size,const char* in){
 int n=0;while(in&&*in&&n<size-1){unsigned char c=(unsigned char)*in++;if(c>=32&&c!='<'&&c!='>'&&c!=127)out[n++]=c;}out[n]=0;
}
static void PTPvpNoticePacket(DWORD channel,const char* text,int length,DWORD firstTong,DWORD secondTong,int world){
 char packet[sizeof(tagExtendProtoHeader)+sizeof(CHAT_CHANNELCHAT_SYNC)+255];ZeroMemory(packet,sizeof(packet));
 int size=sizeof(tagExtendProtoHeader)+sizeof(CHAT_CHANNELCHAT_SYNC)+length;
 tagExtendProtoHeader* h=(tagExtendProtoHeader*)packet;h->ProtocolType=s2c_extendchat;h->wLength=size-1;
 CHAT_CHANNELCHAT_SYNC* msg=(CHAT_CHANNELCHAT_SYNC*)(h+1);msg->ProtocolType=chat_channelchat;msg->wSize=sizeof(*msg)+length-1;
 msg->channelid=channel;msg->packageID=(DWORD)-1;msg->sentlen=(BYTE)length;strcpy(msg->someone,"PK");memcpy(msg+1,text,length);
 for(int to=PlayerSet.GetFirstPlayer();to;to=PlayerSet.GetNextPlayer()){
  if(!PTPublicSubscribed(to,channel))continue;
  bool eligible=false;
  if(channel==PT_CHAT_TONG){DWORD tong=PTPublicTong(to);eligible=tong&&(tong==firstTong||tong==secondTong);}
  else if(channel==PT_CHAT_AREA)eligible=Npc[Player[to].m_nIndex].m_SubWorldIndex==world;
  if(eligible)g_pServer->PackDataToClient(Player[to].m_nNetConnectIdx,packet,size);
 }
}
void PhongThanPvpNotice(int victimNpc,int attackerNpc){
 if(victimNpc<=0||victimNpc>=MAX_NPC||attackerNpc<=0||attackerNpc>=MAX_NPC||victimNpc==attackerNpc)return;
 KNpc& v=Npc[victimNpc];KNpc& a=Npc[attackerNpc];if(!v.IsPlayer()||!a.IsPlayer()||!v.m_FightMode||!a.m_FightMode)return;
 int vp=v.GetPlayerIdx(),ap=a.GetPlayerIdx();if(!PTPvpNoticePlayer(vp)||!PTPvpNoticePlayer(ap))return;
 if(v.m_SubWorldIndex<0||v.m_SubWorldIndex>=MAX_SUBWORLD||v.m_SubWorldIndex!=a.m_SubWorldIndex)return;
 if(v.m_nCurPKPunishState==enumDEATH_MODE_PKBATTLE_PUNISH||v.m_nCurPKPunishState==enumDEATH_MODE_TOURNAMENTS_PUNISH)return;
 if(Player[vp].m_cPK.GetExercisePKAim()==ap||Player[ap].m_cPK.GetExercisePKAim()==vp||Player[vp].m_cPK.IsEnmitySpar()||Player[ap].m_cPK.IsEnmitySpar())return;
 DWORD vt=Player[vp].m_cTong.m_nFlag?Player[vp].m_cTong.m_dwTongNameID:0;
 DWORD at=Player[ap].m_cTong.m_nFlag?Player[ap].m_cTong.m_dwTongNameID:0;if(!vt&&!at)return;
 PTPvpNoticeWindow& window=ptPvpNotices[vp];if(window.role!=Player[vp].m_dwID){ZeroMemory(&window,sizeof(window));window.role=Player[vp].m_dwID;}
 DWORD now=GetTickCount();DWORD& last=window.lastDeath;
 if(last&&now-last<3000u)return;last=now?now:1;
 static KIniFile maps;static bool loaded=false;if(!loaded){maps.Load("\\settings\\WorldSet.ini");loaded=true;}
 char key[32],mapRaw[80],map[65],victim[33],attacker[33];sprintf(key,"%d_name",SubWorld[v.m_SubWorldIndex].m_SubWorldID);
 maps.GetString("List",key,key,mapRaw,sizeof(mapRaw));PTPvpSafeName(map,sizeof(map),mapRaw);PTPvpSafeName(victim,sizeof(victim),v.Name);PTPvpSafeName(attacker,sizeof(attacker),a.Name);
 char text[768];
 // Encoded native Vietnamese formats are defined immediately below.
 sprintf(text,PT_PVP_KILLED_TEXT,attacker,map,victim);
 int length=TEncodeText(text,strlen(text));if(length<=0||length>255)return;
 PTPvpNoticePacket(PT_CHAT_TONG,text,length,vt,at,v.m_SubWorldIndex);
 PTPvpNoticePacket(PT_CHAT_AREA,text,length,vt,at,v.m_SubWorldIndex);
}
