#ifndef PHONGTHAN_NATION_PROTOCOL_H
#define PHONGTHAN_NATION_PROTOCOL_H

// Native Phong Than nation service, over the engine's shared extension envelope.
// No legacy guild command or master/director/manager mapping is used.
enum { PT_NATION_FAMILY=0x70, PT_NATION_VERSION=1,
       PT_GOI_NATION=0x50540032, PT_GDC_NATION=0x50544e,
       PT_NATION_ROWS=8, PT_NATION_CREATE_LEVEL=60, PT_NATION_JOIN_LEVEL=12 };
enum PT_NATION_OPERATION {
    PT_NATION_GET=1, PT_NATION_LIST, PT_NATION_CREATE, PT_NATION_APPLY,
    PT_NATION_CANCEL, PT_NATION_ACCEPT, PT_NATION_REJECT, PT_NATION_LEAVE,
    PT_NATION_KICK, PT_NATION_APPOINT, PT_NATION_TRANSFER, PT_NATION_DISSOLVE,
    PT_NATION_ANNOUNCE, PT_NATION_DEPOSIT, PT_NATION_WITHDRAW
};
enum PT_NATION_DUTY {PT_NATION_PENDING=0, PT_NATION_LORD=1,
    PT_NATION_DEPUTY=2,PT_NATION_MINISTER=3,PT_NATION_CITIZEN=4,PT_NATION_GUEST=5};
#pragma pack(push,1)
struct PTNationRequest {
    unsigned char Family,Protocol,Version,Operation;
    unsigned int RequestId,NationId;
    int Value,Page;
    char Target[32],Text[256];
};
struct PTNationRow {
    unsigned int Id;
    unsigned short Level;
    unsigned char Duty,Online;
    char Name[32],Leader[32];
};
struct PTNationResponse {
    unsigned char Family,Protocol,Version,Operation;
    unsigned int RequestId,NationId,SelfNationId,Rights;
    int Result,Duty,Money,Page,Total,Count,MemberCount;
    char Name[32],Leader[32],Notice[256],Message[160];
    PTNationRow Rows[PT_NATION_ROWS];
};
#pragma pack(pop)
inline int PTNationText(const char* p,unsigned int n) {
    if(!p||n<1)return 0;
    for(unsigned int i=0;i<n;++i){if(!p[i])return 1;if((unsigned char)p[i]<32)return 0;}
    return 0;
}
inline int PTNationRequestValid(const PTNationRequest* p,unsigned int size) {
    return p&&size==sizeof(*p)&&p->Family==PT_NATION_FAMILY&&p->Protocol==1&&
        p->Version==PT_NATION_VERSION&&p->RequestId&&
        p->Operation>=PT_NATION_GET&&p->Operation<=PT_NATION_WITHDRAW&&
        p->Page>=0&&p->Page<=4096&&PTNationText(p->Target,sizeof(p->Target))&&
        PTNationText(p->Text,sizeof(p->Text));
}
inline int PTNationResponseValid(const PTNationResponse* p,unsigned int size) {
    if(!p||size!=sizeof(*p)||p->Family!=PT_NATION_FAMILY||p->Protocol!=2||
       p->Version!=PT_NATION_VERSION||p->Operation<PT_NATION_GET||p->Operation>PT_NATION_WITHDRAW||
       p->MemberCount<0||p->MemberCount>4096||p->Count<0||p->Count>PT_NATION_ROWS||
       p->Total<0||p->Total>4096||p->Page<0||p->Page>4096||
       p->Duty<0||p->Duty>5||p->Money<0||
       !PTNationText(p->Name,sizeof(p->Name))||!PTNationText(p->Leader,sizeof(p->Leader))||
       !PTNationText(p->Notice,sizeof(p->Notice))||!PTNationText(p->Message,sizeof(p->Message)))return 0;
    for(int i=0;i<p->Count;++i)if(p->Rows[i].Duty>5||p->Rows[i].Online>1||
        !PTNationText(p->Rows[i].Name,sizeof(p->Rows[i].Name))||
        !PTNationText(p->Rows[i].Leader,sizeof(p->Rows[i].Leader)))return 0;
    return 1;
}
#endif
