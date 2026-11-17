#include <windows.h>
#include <stdio.h>
#include <vector>
#include "../../Sources/MultiServer/Goddess/PhongThanCharacterStore.h"
// Copies one verified white canonical record into a NEW isolated role using
// the real atomic CharacterStore writer. It never edits an existing role.
int main(int argc,char** argv){
 if(argc!=2||!strstr(argv[1],"P0-BlueMagic-Isolated\\p1_"))return 2;
 CPhongThanCharacterStore store;if(!store.Open(argv[1]))return 3;
 std::vector<BYTE> bytes(PHONGTHAN_CHARACTER_MAX_STATE_SIZE);PHONGTHAN_U32 size=0;
 if(!store.Load("TestDropRate",&bytes[0],bytes.size(),&size))return 4;
 PHONGTHAN_CHARACTER_STATE_HEADER* s=(PHONGTHAN_CHARACTER_STATE_HEADER*)&bytes[0];
 // Caller verifies ordinal 0 is serial 102, quality 0 in the bound v1 file.
 PHONGTHAN_CHARACTER_ITEM_RECORD white=PhongThanCharacterItems(s)[0];
 if(white.Genre!=0||white.DetailType!=6||white.ParticularType!=0||white.Level!=1)return 5;
 ZeroMemory(s->RoleName,sizeof(s->RoleName));strcpy((char*)s->RoleName,"P1OldWhite");s->Revision=1;s->ItemCount=1;
 PhongThanCharacterItems(s)[0]=white;s->StateSize=PhongThanCharacterExpectedStateSize(s);
 if(!store.Create(s,s->StateSize))return 6;
 printf("CREATED P1OldWhite legacy 268-byte white record; no P0 companion; state=%lu\n",s->StateSize);return 0;
}
