// Part of the existing native mailbox. Claim state 4 is hidden until the
// character's offline receipt is durable. No second inbox or claim UI.
#include "PhongThanOfflineMail.h"
#include "../../../Headers/PhongThanOfflineContract.h"
struct PtOfflineMailJournal {unsigned int Magic,Checksum,MailId;int Before,After;char Account[32],Role[32];};
static bool PtOfflineJournalValid(PtOfflineMailJournal& j,const char* account,const char* role){
 unsigned int hash=j.Checksum;j.Checksum=0;bool ok=j.Magic==0x4f4d5450&&hash==PtPsHash(&j,sizeof(j))&&j.MailId&&j.Before>=0&&j.Before<1000000000&&j.After==j.Before+1&&j.Account[31]==0&&j.Role[31]==0&&!strcmp(j.Account,account)&&!strcmp(j.Role,role);j.Checksum=hash;return ok;
}
static bool PtOfflineMailCounter(int next){
 char value[24];sprintf(value,"%d",next);const char* path=".\\CharacterStore\\mail_counter.ini";
 if(!WritePrivateProfileStringA("Mailbox","NextId",value,path))return false;
 // The all-null profile flush has no useful success result on all Windows
 // versions. Flush the actual file handle before reserving a visible mail ID.
 WritePrivateProfileStringA(0,0,0,path);HANDLE f=CreateFileA(path,GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
 if(f==INVALID_HANDLE_VALUE)return false;bool ok=FlushFileBuffers(f)!=0;CloseHandle(f);return ok;
}
static int PtOfflineMailSettle(PtPsMailbox& box,PT_PS_MAIL& mail){
 std::string path;PtOfflineMailJournal journal;if(!PtPsPath(box.Account,box.Role,".offline-mail",path)||!PtPsRead(path,&journal,sizeof(journal))||!PtOfflineJournalValid(journal,box.Account,box.Role)||journal.MailId!=mail.Id)return -1;
 int saved=0;if(!PtPsSavedReceipt(box.Account,box.Role,PtOfflineTask::Receipt,saved))return 0;
 if(saved>=journal.After){mail.Claimed=0;return 1;}
 int p=PtPsOnline(box.Role);if(!p||Player[p].m_cTask.GetSaveVal(PtOfflineTask::Receipt)==journal.Before){mail.Deleted=1;return 1;}
 return 0;
}
bool PhongThanOfflineMailReconcile(int p){
 if(p<1||p>=MAX_PLAYER||!Player[p].m_PlayerDBLoad)return false;std::string path;if(!PtPsPath(Player[p].AccountName,Player[p].Name,".offline-mail",path))return false;
 if(GetFileAttributesA(path.c_str())==(DWORD)-1)return GetLastError()==ERROR_FILE_NOT_FOUND;
 PtOfflineMailJournal journal;if(!PtPsRead(path,&journal,sizeof(journal))||!PtOfflineJournalValid(journal,Player[p].AccountName,Player[p].Name))return false;
 PtPsMailbox* box=new PtPsMailbox;bool ok=PtPsLoadMailbox(Player[p].AccountName,Player[p].Name,*box)&&PtPsSettle(*box),settled=false;
 if(ok)for(int i=0;i<PT_PS_MAIL_LIMIT;++i)if(box->Mail[i].Id==journal.MailId)settled=box->Mail[i].Deleted||box->Mail[i].Claimed!=4;
 delete box;return ok&&settled&&DeleteFileA(path.c_str());
}
bool PhongThanOfflineMailPrepare(int p,int before,int after,int detail){
 if(p<1||p>=MAX_PLAYER||!Player[p].m_PlayerDBLoad||(detail!=78&&detail!=80)||before<0||before>=1000000000||after!=before+1||Player[p].m_cTask.GetSaveVal(PtOfflineTask::Receipt)!=before||!PhongThanOfflineMailReconcile(p))return false;
 PtPsMailbox* box=new PtPsMailbox;if(!PtPsLoadMailbox(Player[p].AccountName,Player[p].Name,*box)||!PtPsSettle(*box)){delete box;return false;}
 int slot=-1;for(int i=0;i<PT_PS_MAIL_LIMIT;++i)if(!box->Mail[i].Id||box->Mail[i].Deleted){slot=i;break;}if(slot<0){delete box;return false;}
 int levels[MAX_ITEM_MAGICLEVEL];ZeroMemory(levels,sizeof(levels));int item=ItemSet.Add(3,0,1,0,detail,0,levels,g_SubWorldSet.GetGameVersion());if(item<=0||item>=MAX_ITEM){delete box;return false;}
 PT_PS_MAIL& mail=box->Mail[slot];ZeroMemory(&mail,sizeof(mail));mail.Count=1;PtPsCaptureItem(item,mail.Items[0]);ItemSet.Remove(item);
 int id=GetPrivateProfileIntA("Mailbox","NextId",1,".\\CharacterStore\\mail_counter.ini");for(int check=0;check<PT_PS_MAIL_LIMIT;++check)if(box->Mail[check].Id>=(unsigned int)id&&box->Mail[check].Id<2147483646U)id=box->Mail[check].Id+1;if(id<1||id>=2147483646||!PtOfflineMailCounter(id+1)){delete box;return false;}
 mail.Id=id;mail.Created=KSG_GetCurSec();mail.Claimed=4;lstrcpyn(mail.Sender,"He thong",32);lstrcpyn(mail.Recipient,Player[p].Name,32);lstrcpyn(mail.Subject,"Uy thac roi mang",32);lstrcpyn(mail.Body,"Vat pham uy thac duoc gui vao thu vi hanh trang da day.",257);
 PtOfflineMailJournal journal;ZeroMemory(&journal,sizeof(journal));journal.Magic=0x4f4d5450;journal.MailId=id;journal.Before=before;journal.After=after;lstrcpyn(journal.Account,box->Account,32);lstrcpyn(journal.Role,box->Role,32);journal.Checksum=PtPsHash(&journal,sizeof(journal));std::string path;
 bool ok=PtPsPath(box->Account,box->Role,".offline-mail",path)&&PtPsAtomic(path,&journal,sizeof(journal));
 if(ok&&!PtPsSaveMailbox(*box)){DeleteFileA(path.c_str());ok=false;}delete box;return ok;
}
