#include <assert.h>
#include <stdio.h>
#include "../../Headers/PhongThanItemLink.h"
int main(){char label[67];
 assert(PTFormatItemChatLabel(label,sizeof(label),"#<color=yellow>Real Sword<color>"));
 assert(!strcmp(label,"<Real Sword>"));
 char ad[256];sprintf(ad,"Co %s khong dung, ai can pm nhe.",label);
 // Client creates exactly this bracketed plain-name label for its item button.
 char clientName[64];PTPlainItemName(clientName,sizeof(clientName),"#<color=yellow>Real Sword<color>");char clientLabel[67];sprintf(clientLabel,"<%s>",clientName);
 assert(PTLocateItemLabel(ad,strlen(ad),clientLabel,strlen(clientLabel))>=0);
 const char* old="Co Real Sword khong dung, ai can pm nhe.";
 assert(PTLocateItemLabel(old,strlen(old),clientLabel,strlen(clientLabel))==-1);
 assert(PTFormatItemChatLabel(label,sizeof(label),"$$#<c=red>Long bow<c>"));assert(!strcmp(label,"<Long bow>"));
 assert(!PTFormatItemChatLabel(label,5,"Sword"));assert(!label[0]);
 assert(!PTFormatItemChatLabel(label,sizeof(label),"#<color=yellow><color>"));
 assert(!PTFormatItemChatLabel(label,sizeof(label),0));
 char longName[100];memset(longName,'A',99);longName[99]=0;
 assert(PTFormatItemChatLabel(label,sizeof(label),longName));assert(strlen(label)==65&&label[0]=='<'&&label[64]=='>');
 PTPlainItemName(clientName,sizeof(clientName),longName);sprintf(clientLabel,"<%s>",clientName);assert(!strcmp(label,clientLabel));
 PTItemLinkMessage item;memset(&item,0,sizeof(item));PhongThanInitializeWireHeader(&item.Header,PT_ITEM_LINK_MESSAGE,sizeof(item),PHONGTHAN_WIRE_FLAG_RESPONSE,1);item.Token=1;strcpy(item.Sender,"Bot");item.Stack=1;item.TextSize=1;item.Text[0]='x';item.Magic[0].Type=387;
 assert(PTValidateItemLinkMessage(&item,sizeof(item)));item.Magic[0].Type=388;assert(!PTValidateItemLinkMessage(&item,sizeof(item)));
 printf("PASS item label matches actual client lookup; old unbracketed advert reproduced as unclickable\n");return 0;
}
