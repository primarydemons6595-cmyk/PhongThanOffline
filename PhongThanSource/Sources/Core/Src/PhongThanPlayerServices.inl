#include "../../../Headers/PhongThanCharacterBundle.h"
#include "../../../Headers/PhongThanPlayerServices.h"
#include "../../../Headers/PhongThanMailItem.h"
#ifndef _SERVER
static PT_PS_STATE s_PtPsState;
static DWORD s_PtPsSequence=0,s_PtPsReceived=0;
int PhongThanPlayerServiceAction(const PT_PS_COMMAND* source){
 int actor=Player[CLIENT_PLAYER_INDEX].m_nIndex;
 if(!source||!g_pClient||actor<=0||actor>=MAX_NPC)return 0;
 PT_PS_COMMAND q=*source;
 PhongThanInitializeWireHeader(&q.Header,PT_PS_QUERY,sizeof(q),PHONGTHAN_WIRE_FLAG_REQUEST,++s_PtPsSequence);
 q.ActorId=Npc[actor].m_dwID;q.MapId=SubWorld[Npc[actor].m_SubWorldIndex].m_SubWorldID;
 g_pClient->SendPackToServer(&q,sizeof(q));return 1;
}
int PhongThanPlayerServiceGet(PT_PS_STATE* state){
 int actor=Player[CLIENT_PLAYER_INDEX].m_nIndex;
 if(!state||actor<=0||actor>=MAX_NPC||s_PtPsState.ActorId!=Npc[actor].m_dwID||!s_PtPsReceived)return 0;
 *state=s_PtPsState;return 1;
}
#else
#include "../../../Headers/PhongThanAccountStorage.h"
#include "../../../Headers/PhongThanCharacter.h"
#include <vector>
// Reconstructed local mailbox. Character save receipts settle attachments only
// after Goddess has atomically persisted inventory and its matching task marker.
enum {PT_PS_SEND_RECEIPT=TASKVALUE_PT_MAIL_SEND_RECEIPT,PT_PS_CLAIM_RECEIPT=TASKVALUE_PT_MAIL_CLAIM_RECEIPT};
struct PtPsMailbox {unsigned int Magic,Version,Checksum;char Account[32],Role[32];PT_PS_MAIL Mail[PT_PS_MAIL_LIMIT];};
struct PtPsJournal {unsigned int Magic,Id;char Recipient[32],Account[32];};
struct PtPsInvite {DWORD Master,Student,Time;char Name[32];};
static PtPsInvite s_PtPsInvites[MAX_PLAYER];
static DWORD s_PtPsLast[MAX_PLAYER][9],s_PtPsOwner[MAX_PLAYER];
static bool PtPsAtomic(const std::string& path,const void* data,DWORD size){
 std::string temporary=path+".tmp";HANDLE f=CreateFileA(temporary.c_str(),GENERIC_WRITE,0,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
 if(f==INVALID_HANDLE_VALUE)return false;DWORD wrote=0;
 bool ok=WriteFile(f,data,size,&wrote,0)&&wrote==size&&FlushFileBuffers(f);CloseHandle(f);
 if(ok)ok=MoveFileExA(temporary.c_str(),path.c_str(),MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH)!=0;
 if(!ok)DeleteFileA(temporary.c_str());return ok;
}
static bool PtPsRead(const std::string& path,void* data,DWORD size){
 HANDLE f=CreateFileA(path.c_str(),GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
 if(f==INVALID_HANDLE_VALUE)return false;DWORD got=0;
 bool ok=GetFileSize(f,0)==size&&ReadFile(f,data,size,&got,0)&&got==size;CloseHandle(f);return ok;
}
static int PtPsOnline(const char* name){
 for(int p=1;p<MAX_PLAYER;++p)if(Player[p].m_PlayerDBLoad&&Player[p].m_dwID&&Player[p].m_nIndex>0&&Player[p].m_nIndex<MAX_NPC&&
   Player[p].m_nNetConnectIdx>=0&&!Player[p].m_bExchangeServer&&!strcmp(Player[p].Name,name))return p;
 return 0;
}
static bool PtPsSavedReceipt(const char*,const char*,int,int&);
static bool PtPsRoleAccount(const char* role,char* account){
 int online=PtPsOnline(role);if(online){lstrcpyn(account,Player[online].AccountName,32);return true;}
 std::string name;if(!PtRoleFileName(role,name))return false;
 WIN32_FIND_DATAA found;HANDLE h=FindFirstFileA("CharacterStore\\*",&found);if(h==INVALID_HANDLE_VALUE)return false;
 int matches=0;
 do{if(!(found.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)||(found.dwFileAttributes&FILE_ATTRIBUTE_REPARSE_POINT)||found.cFileName[0]=='.')continue;
  std::string path=std::string("CharacterStore\\")+found.cFileName+"\\"+name;
  HANDLE f=CreateFileA(path.c_str(),GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);if(f==INVALID_HANDLE_VALUE)continue;
  unsigned char bytes[16+sizeof(PHONGTHAN_CHARACTER_STATE_HEADER)];DWORD got=0;
  if(ReadFile(f,bytes,sizeof(bytes),&got,0)&&got==sizeof(bytes)){
   const PHONGTHAN_CHARACTER_STATE_HEADER* state=(const PHONGTHAN_CHARACTER_STATE_HEADER*)(bytes+16);
   if(!((const char*)state->RoleName)[31]&&!((const char*)state->AccountName)[31]&&!strcmp(((const char*)state->RoleName),role)){int receipt=0;std::string component;
    if(PtAccountComponent((const char*)state->AccountName,component)&&!stricmp(component.c_str(),found.cFileName)&&PtPsSavedReceipt((const char*)state->AccountName,role,PT_PS_SEND_RECEIPT,receipt)){lstrcpyn(account,(const char*)state->AccountName,32);++matches;}}
  }CloseHandle(f);
 }while(FindNextFileA(h,&found));FindClose(h);return matches==1;
}
static bool PtPsPath(const char* account,const char* role,const char* suffix,std::string& path){
 std::string dir,name;if(!PtAccountDirectory("CharacterStore",account,dir,true)||!PtRoleFileName(role,name))return false;
 path=dir+"\\"+name+suffix;return path.size()<MAX_PATH-6;
}
static bool PtPsSavedReceipt(const char* account,const char* role,int task,int& value){
 std::string path;if(!PtPsPath(account,role,"",path))return false;
 PtCharacterBundle::Bundle bundle;if(!PtCharacterBundle::Read(path,bundle))return false;
 const PHONGTHAN_CHARACTER_STATE_HEADER* state=(const PHONGTHAN_CHARACTER_STATE_HEADER*)bundle.state.data();
 if(strcmp((const char*)state->RoleName,role)||strcmp((const char*)state->AccountName,account))return false;
 value=0;const PHONGTHAN_CHARACTER_TASK_RECORD* tasks=PhongThanCharacterTasks(state);
 for(unsigned int i=0;i<state->TaskCount;++i)if(tasks[i].TaskId==task){value=atoi((const char*)tasks[i].Value);break;}
 return true;
}
static bool PtPsSaveMailbox(PtPsMailbox& box){
 std::string path;if(!PtPsPath(box.Account,box.Role,".mail",path))return false;
 box.Checksum=0;box.Checksum=PtPsHash(&box,sizeof(box));return PtPsAtomic(path,&box,sizeof(box));
}
static bool PtPsLoadMailbox(const char* account,const char* role,PtPsMailbox& box){
 std::string path;if(!PtPsPath(account,role,".mail",path))return false;
 if(GetFileAttributesA(path.c_str())==(DWORD)-1){ZeroMemory(&box,sizeof(box));box.Magic=0x4c4d5450;box.Version=1;lstrcpyn(box.Account,account,32);lstrcpyn(box.Role,role,32);return true;}
 if(!PtPsRead(path,&box,sizeof(box)))return false;unsigned int checksum=box.Checksum;box.Checksum=0;
 if(box.Magic!=0x4c4d5450||box.Version!=1||checksum!=PtPsHash(&box,sizeof(box))||box.Account[31]||box.Role[31]||strcmp(box.Account,account)||strcmp(box.Role,role))return false;
 for(int i=0;i<PT_PS_MAIL_LIMIT;++i)if(!PtPsValidMail(box.Mail[i])||(box.Mail[i].Id&&strcmp(box.Mail[i].Recipient,role)))return false;
 return true;
}
static int PtOfflineMailSettle(PtPsMailbox&,PT_PS_MAIL&);
static bool PtPsSettle(PtPsMailbox& box){
 bool changed=false;
 for(int i=0;i<PT_PS_MAIL_LIMIT;++i){PT_PS_MAIL& m=box.Mail[i];if(m.Id&&!m.Deleted&&m.Claimed==4){int result=PtOfflineMailSettle(box,m);if(result<0)return false;if(result)changed=true;continue;}if(!m.Id||m.Deleted||(m.Claimed!=2&&m.Claimed!=3))continue;
  char account[32];const char* role=m.Claimed==2?m.Sender:box.Role;
  if(m.Claimed==2){if(!PtPsRoleAccount(role,account))continue;}else lstrcpyn(account,box.Account,32);
  int receipt=0;if(!PtPsSavedReceipt(account,role,m.Claimed==2?PT_PS_SEND_RECEIPT:PT_PS_CLAIM_RECEIPT,receipt))continue;
  if((unsigned int)receipt==m.Id){m.Claimed=m.Claimed==2?0:1;changed=true;}
  else if(!PtPsOnline(role)||Player[PtPsOnline(role)].m_cTask.GetSaveVal(m.Claimed==2?PT_PS_SEND_RECEIPT:PT_PS_CLAIM_RECEIPT)!=(int)m.Id){if(m.Claimed==2)m.Deleted=1;else m.Claimed=0;changed=true;}
 }
 return !changed||PtPsSaveMailbox(box);
}
static void PtPsCaptureItem(int index,PT_PS_ITEM& out){
 KItem& item=Item[index];ZeroMemory(&out,sizeof(out));out.Serial=item.GetID();out.Genre=item.GetGenre();out.Detail=item.GetDetailType();out.Particular=item.GetParticularType();out.Row=item.GetRow();
 out.Series=item.GetSeries();out.Level=item.GetLevel();out.Luck=item.m_GeneratorParam.nLuck;out.Version=item.m_GeneratorParam.nVersion;out.Seed=item.m_GeneratorParam.uRandomSeed;
 memcpy(out.Generator,item.m_GeneratorParam.nGeneratorLevel,sizeof(out.Generator));out.Stack=item.GetStackNum();out.Durability=item.GetDurability();out.Expire=item.GetExpireTime();out.Param=item.GetParam();out.Fortune=item.GetFortune();
 out.AddedBaseMask=item.m_CommonAttrib.nUpgradeAddedBaseMask;out.AddedRequireMask=item.m_CommonAttrib.nUpgradeAddedRequireMask;
 out.Upgrade=item.GetUpgradeState();out.Physical=item.GetBasePower();out.Magic=item.GetMagicPower();
 memcpy(out.Base,item.P0BaseAttributes(),sizeof(out.Base));memcpy(out.Require,item.TooltipRequirements(),sizeof(out.Require));memcpy(out.Attributes,item.m_aryMagicAttrib,sizeof(out.Attributes));
 lstrcpyn(out.Name,item.GetName(),80);out.Image[0]=0;
}
static int PtPsCreateItem(const PT_PS_ITEM& in){
 int generator[40];memcpy(generator,in.Generator,sizeof(generator));
 int index=ItemSet.Add(in.Genre,in.Series,in.Level,in.Luck,in.Detail,in.Particular,generator,in.Version,in.Seed,in.Row);
 if(index<=0||index>=MAX_ITEM)return 0;KItem& item=Item[index];
 if((in.Expire&&in.Expire<=KSG_GetCurSec())||!PtPsRestoreItem(item,in)){ItemSet.Remove(index);return 0;}
 return index;
}
static bool PtPsNoPendingSend(int p){
 std::string path;if(!PtPsPath(Player[p].AccountName,Player[p].Name,".mail-out",path))return false;
 PtPsJournal journal;if(GetFileAttributesA(path.c_str())==(DWORD)-1)return true;
 if(!PtPsRead(path,&journal,sizeof(journal))||journal.Magic!=0x4a4d5450||journal.Account[31]||journal.Recipient[31])return false;
 PtPsMailbox* box=new PtPsMailbox;bool ok=PtPsLoadMailbox(journal.Account,journal.Recipient,*box)&&PtPsSettle(*box),settled=false;
 if(ok)for(int i=0;i<PT_PS_MAIL_LIMIT;++i)if(box->Mail[i].Id==journal.Id)settled=box->Mail[i].Deleted||box->Mail[i].Claimed!=2;
 delete box;return ok&&settled;
}
static int PtPsSendMail(int p,const PT_PS_COMMAND& q){
 if(!q.Target[0]||!q.Subject[0]||!strcmp(q.Target,Player[p].Name)||q.Money<0||q.Money>100000000||Player[p].m_uMustSave==SAVE_DOING||!PtPsNoPendingSend(p))return -1;
 char account[32];if(!PtPsRoleAccount(q.Target,account))return -2;
 PtPsMailbox* box=new PtPsMailbox;if(!PtPsLoadMailbox(account,q.Target,*box)||!PtPsSettle(*box)){delete box;return -3;}
 int slot=-1;for(int i=0;i<PT_PS_MAIL_LIMIT;++i)if(!box->Mail[i].Id||box->Mail[i].Deleted){slot=i;break;}if(slot<0){delete box;return -4;}
 PT_PS_MAIL& mail=box->Mail[slot];ZeroMemory(&mail,sizeof(mail));int indices[9];ItemPos positions[9];
 for(i=0;i<9;++i)if(q.Items[i]){
  int index=Player[p].m_ItemList.SearchID(q.Items[i]),owned=Player[p].m_ItemList.FindSame(index);
  if(index<=0||index>=MAX_ITEM||owned<=0||owned>=MAX_PLAYER_ITEM||Item[index].GetLockTrade()||Item[index].GetLock()->IsLock()){delete box;return -5;}
  int place=Player[p].m_ItemList.m_Items[owned].nPlace;
  if(place!=pos_equiproom&&place!=pos_equiproomex&&place!=pos_ptbag2&&place!=pos_ptbag3){delete box;return -5;}
  for(int j=0;j<mail.Count;++j)if(indices[j]==index){delete box;return -5;}
  indices[mail.Count]=index;positions[mail.Count].nPlace=place;positions[mail.Count].nX=Player[p].m_ItemList.m_Items[owned].nX;positions[mail.Count].nY=Player[p].m_ItemList.m_Items[owned].nY;
  PtPsCaptureItem(index,mail.Items[mail.Count++]);
 }
 const int fee=max(0,min(1000000,(int)GetPrivateProfileIntA("Mailbox","SendFee",1000,".\\settings\\phongthan\\PlayerServices.ini")));if(Player[p].m_ItemList.GetEquipmentMoney()<q.Money+fee){delete box;return -6;}
 int id=GetPrivateProfileIntA("Mailbox","NextId",1,".\\CharacterStore\\mail_counter.ini");if(id<=0||id>=2147483646){delete box;return -3;}
 char value[24];sprintf(value,"%d",id+1);if(!WritePrivateProfileStringA("Mailbox","NextId",value,".\\CharacterStore\\mail_counter.ini")){delete box;return -3;}
 mail.Id=id;mail.Created=KSG_GetCurSec();mail.Claimed=2;mail.Money=q.Money;mail.Fee=fee;
 lstrcpyn(mail.Sender,Player[p].Name,32);lstrcpyn(mail.Recipient,q.Target,32);lstrcpyn(mail.Subject,q.Subject,32);lstrcpyn(mail.Body,q.Text,257);
 PtPsJournal journal;ZeroMemory(&journal,sizeof(journal));journal.Magic=0x4a4d5450;journal.Id=id;lstrcpyn(journal.Account,account,32);lstrcpyn(journal.Recipient,q.Target,32);
 std::string journalPath;
 if(!PtPsPath(Player[p].AccountName,Player[p].Name,".mail-out",journalPath)||!PtPsSaveMailbox(*box)||!PtPsAtomic(journalPath,&journal,sizeof(journal))){mail.Deleted=1;PtPsSaveMailbox(*box);delete box;return -3;}
 int removed=0;bool charged=Player[p].Pay(q.Money+fee)!=FALSE;bool ok=charged;
 for(i=0;i<mail.Count&&ok;++i){if(Player[p].m_ItemList.Remove(indices[i]))++removed;else ok=false;}
 if(!ok){for(i=0;i<removed;++i)Player[p].m_ItemList.Add(indices[i],positions[i].nPlace,positions[i].nX,positions[i].nY,false);
  if(charged)Player[p].Earn(q.Money+fee);
  mail.Deleted=1;PtPsSaveMailbox(*box);delete box;return -3;}
 Player[p].m_cTask.SetSaveVal(PT_PS_SEND_RECEIPT,id,TRUE);for(i=0;i<mail.Count;++i)ItemSet.Remove(indices[i]);
 Player[p].m_uMustSave=SAVE_REQUEST;Player[p].Save();delete box;return 1;
}
static int PtPsClaimMail(int p,PtPsMailbox& box,PT_PS_MAIL& mail){
 if(mail.Claimed!=0||Player[p].m_uMustSave==SAVE_DOING)return -1;
 for(int m=0;m<PT_PS_MAIL_LIMIT;++m)if(box.Mail[m].Claimed==3&&!box.Mail[m].Deleted)return -1;
 int created[9],count=0;bool ok=true;
 for(int i=0;i<mail.Count&&ok;++i){if(mail.Items[i].Expire&&mail.Items[i].Expire<=KSG_GetCurSec())continue;int item=PtPsCreateItem(mail.Items[i]);if(!item){ok=false;break;}
  ItemPos pos;if(!Player[p].m_ItemList.SearchPosition(Item[item].GetWidth(),Item[item].GetHeight(),&pos,true)||!Player[p].m_ItemList.Add(item,pos.nPlace,pos.nX,pos.nY,false)){ItemSet.Remove(item);ok=false;break;}created[count++]=item;
 }
 bool earned=false;if(ok&&mail.Money){earned=Player[p].Earn(mail.Money)!=FALSE;ok=earned;}
 if(ok){mail.Claimed=3;ok=PtPsSaveMailbox(box);}
 if(!ok){mail.Claimed=0;for(i=0;i<count;++i){Player[p].m_ItemList.Remove(created[i]);ItemSet.Remove(created[i]);}if(earned)Player[p].Pay(mail.Money);return -7;}
 Player[p].m_cTask.SetSaveVal(PT_PS_CLAIM_RECEIPT,mail.Id,TRUE);Player[p].m_uMustSave=SAVE_REQUEST;Player[p].Save();return 1;
}
static int PtPsMentor(int p,int target,int operation,int value){
 if(!target||target==p)return -2;DWORD now=GetTickCount();
 if(operation==PT_PS_INVITE_PUPIL){
  if(Npc[Player[p].m_nIndex].m_Level<=Npc[Player[target].m_nIndex].m_Level)return -8;
  PtPsInvite& invite=s_PtPsInvites[target];invite.Master=Player[p].m_dwID;invite.Student=Player[target].m_dwID;invite.Time=now;lstrcpyn(invite.Name,Player[p].Name,32);return 1;
 }
 PtPsInvite& invite=s_PtPsInvites[p];
 if(!invite.Master||invite.Master!=Player[target].m_dwID||invite.Student!=Player[p].m_dwID||now-invite.Time>60000)return -1;
 invite.Master=0;if(!value)return 1;
 if(Npc[Player[target].m_nIndex].m_Level<=Npc[Player[p].m_nIndex].m_Level||Player[p].m_cTask.GetSaveStr(TASKVALUE_PT_MASTER_NAME_BEGIN)[0])return -8;
 if(!strcmp(Player[target].m_cTask.GetSaveStr(TASKVALUE_PT_MASTER_NAME_BEGIN),Player[p].Name))return -8;
 // Use the existing persisted Lua-facing master-name fields, not a second owner.
 const char* name=Player[target].Name;int length=strlen(name);
 for(int i=0;i<3;++i){char part[12];int start=i*10,n=min(10,max(0,length-start));memcpy(part,name+min(start,length),n);part[n]=0;Player[p].m_cTask.SetSaveVal(TASKVALUE_PT_MASTER_NAME_BEGIN+i,part,TRUE);}
 int count=Player[target].m_cTask.GetSaveVal(TASKVALUE_PT_MASTER_APPRENTICE_COUNT);
 Player[target].m_cTask.SetSaveVal(TASKVALUE_PT_MASTER_APPRENTICE_COUNT,count+1,TRUE);Player[p].m_uMustSave=SAVE_REQUEST;Player[target].m_uMustSave=SAVE_REQUEST;return 1;
}
static void PhongThanPlayerServiceRequest(int p,const PT_PS_COMMAND& q){
 if(p<=0||p>=MAX_PLAYER||!Player[p].m_PlayerDBLoad||Player[p].m_nIndex<=0||Player[p].m_nIndex>=MAX_NPC)return;
 int actor=Player[p].m_nIndex,world=Npc[actor].m_SubWorldIndex;
 if(world<0||world>=MAX_SUBWORLD||q.ActorId!=Npc[actor].m_dwID||q.MapId!=(DWORD)SubWorld[world].m_SubWorldID||q.Operation<0||q.Operation>8||
   !PtPsText(q.Target,32)||!PtPsText(q.Subject,32)||!PtPsText(q.Text,257,true))return;
 DWORD now=GetTickCount();if(s_PtPsOwner[p]==Player[p].m_dwID&&now-s_PtPsLast[p][q.Operation]<350)return;s_PtPsOwner[p]=Player[p].m_dwID;s_PtPsLast[p][q.Operation]=now;
 PT_PS_STATE* state=new PT_PS_STATE;ZeroMemory(state,sizeof(*state));
 PhongThanInitializeWireHeader(&state->Header,PT_PS_REPLY,sizeof(*state),PHONGTHAN_WIRE_FLAG_RESPONSE,q.Header.Sequence);
 state->SendFee=max(0,min(1000000,(int)GetPrivateProfileIntA("Mailbox","SendFee",1000,".\\settings\\phongthan\\PlayerServices.ini")));
 state->ActorId=q.ActorId;state->MapId=q.MapId;state->Operation=q.Operation;state->Result=1;state->TargetId=q.TargetId;lstrcpyn(state->Target,q.Target,32);
 int target=PtPsOnline(q.Target);if(q.TargetId&&target&&Npc[Player[target].m_nIndex].m_dwID!=q.TargetId)target=0;
 if(q.Operation!=PT_PS_INBOX&&(Player[p].CheckTrading()||Player[p].GetLockState()))state->Result=-9;
 else if(q.Operation==PT_PS_POSITION){
  if(!target)state->Result=-2;else{int n=Player[target].m_nIndex;Npc[n].GetMpsPos(&state->X,&state->Y);state->X/=32;state->Y/=32;state->TargetMap=SubWorld[Npc[n].m_SubWorldIndex].m_SubWorldID;}
 }else if(q.Operation==PT_PS_INVITE_PUPIL||q.Operation==PT_PS_REPLY_PUPIL)state->Result=PtPsMentor(p,target,q.Operation,q.Value);
 else if(q.Operation==PT_PS_REPORT){
  if(!q.Target[0]||!q.Text[0])state->Result=-1;
  else{char account[32];if(!PtPsRoleAccount(q.Target,account))state->Result=-2;
   else{CreateDirectoryA("CharacterStore\\Reports",0);char path[160];sprintf(path,"CharacterStore\\Reports\\%08lx_%08lx_%08lx.txt",KSG_GetCurSec(),Player[p].m_dwID,q.Header.Sequence);
    char report[512];_snprintf(report,sizeof(report)-1,"Reporter: %.31s\r\nTarget: %.31s\r\nReason: %.256s\r\n",Player[p].Name,q.Target,q.Text);report[sizeof(report)-1]=0;if(!PtPsAtomic(path,report,strlen(report)))state->Result=-3;}}
 }else if(q.Operation==PT_PS_SEND_MAIL)state->Result=PtPsSendMail(p,q);
 else{
  PtPsMailbox* box=new PtPsMailbox;
  if(!PtPsLoadMailbox(Player[p].AccountName,Player[p].Name,*box)||!PtPsSettle(*box))state->Result=-3;
  else{
   PT_PS_MAIL* mail=0;for(int i=0;i<PT_PS_MAIL_LIMIT;++i)if(box->Mail[i].Id==q.MailId&&!box->Mail[i].Deleted)mail=&box->Mail[i];
   if(q.Operation==PT_PS_READ_MAIL){if(mail&&mail->Claimed!=2&&mail->Claimed!=4)state->Mail=*mail;else state->Result=-1;}
   else if(q.Operation==PT_PS_CLAIM_MAIL)state->Result=mail?PtPsClaimMail(p,*box,*mail):-1;
   else if(q.Operation==PT_PS_DELETE_MAIL){if(mail&&mail->Claimed==1){mail->Deleted=1;if(!PtPsSaveMailbox(*box))state->Result=-3;}else state->Result=-1;}
   for(i=0;i<PT_PS_MAIL_LIMIT;++i){const PT_PS_MAIL& m=box->Mail[i];if(!m.Id||m.Deleted||m.Claimed==2||m.Claimed==4)continue;
    PT_PS_MAIL_HEADER& h=state->Mails[state->Count++];h.Id=m.Id;h.Created=m.Created;h.Claimed=m.Claimed;h.Money=m.Money;h.Count=m.Count;lstrcpyn(h.Sender,m.Sender,32);lstrcpyn(h.Subject,m.Subject,32);}
  }delete box;
 }
 PtPsInvite& invite=s_PtPsInvites[p];if(invite.Master&&invite.Student==Player[p].m_dwID&&now-invite.Time<60000){state->PendingPupil=1;lstrcpyn(state->Message,invite.Name,256);}
 g_pServer->PackDataToClient(Player[p].m_nNetConnectIdx,state,sizeof(*state));delete state;
}
#include "PhongThanOfflineMail.inl"
#endif
