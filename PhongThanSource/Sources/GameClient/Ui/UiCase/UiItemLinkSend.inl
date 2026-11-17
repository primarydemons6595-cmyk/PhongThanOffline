#include "../../../../Headers/PhongThanItemLink.h"
static bool PTSendItemLink(iCoreShell* core,const char* item,DWORD channel,const char* target,const char* text,int length,BYTE flags)
{
    if(!core || !item || strncmp(item,"PTI:",4) || !text || length<1 || length>255)return false;
    char* end=0;unsigned id=strtoul(item+4,&end,10);if(!end || *end || !id)return false;
    PTItemLinkRequest request;ZeroMemory(&request,sizeof(request));
    PhongThanInitializeWireHeader(&request.Header,PT_ITEM_LINK_REQUEST,sizeof(request),PHONGTHAN_WIRE_FLAG_REQUEST,0);
    request.ItemId=id;request.Channel=channel;request.TextSize=length;request.Flags=flags;
    if(target)strncpy(request.Target,target,sizeof(request.Target)-1);
    memcpy(request.Text,text,length);
    if(!PTValidateItemLinkRequest(&request,sizeof(request)))return false;
    core->SendNewDataToServer(&request,sizeof(request));return true;
}
