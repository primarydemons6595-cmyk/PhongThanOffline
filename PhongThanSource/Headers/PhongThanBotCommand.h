#ifndef PT_BOT_COMMAND_H
#define PT_BOT_COMMAND_H
#include <string.h>
// Input is already folded from the client's original Vietnamese glyph map.
enum PTBotCommand {PTBC_NONE,PTBC_FOLLOW,PTBC_CONFIRM,PTBC_WAIT,PTBC_FIGHT,PTBC_BOSS,PTBC_LOOT,PTBC_SHOP,PTBC_CANCEL,PTBC_GIFT,PTBC_INVITE,PTBC_PLAYER,PTBC_RAID_SUPPLY,PTBC_RAID_ESCORT,PTBC_SALE_QUOTE,PTBC_SALE_SHOW,PTBC_SALE_MEET,PTBC_SALE_ACCEPT,PTBC_RPS,PTBC_PARTY,PTBC_VTT,PTBC_GOTO,PTBC_MARKET_BUY,PTBC_DECLUTTER,PTBC_TRADE,PTBC_DISCARD};
static bool PTBotCombatCommand(int mode){return mode==PTBC_FIGHT||mode==PTBC_BOSS||(mode>=PTBC_PLAYER&&mode<=PTBC_RAID_ESCORT);}
static bool PTBotReleaseCommand(const char* q){
 if(strstr(q,"khong can nua")||strstr(q,"ko can nua")||strstr(q,"khong can ong")||strstr(q,"ko can ong")||strstr(q,"khong can theo")||strstr(q,"khong can cho")||strstr(q,"khoi cho")||strstr(q,"khoi doi")||strstr(q,"khoi theo")||strstr(q,"dung cho nua")||strstr(q,"di train tiep")||strstr(q,"tu di train")||strstr(q,"tu lam nhiem vu")||strstr(q,"xong roi")||strstr(q,"di ve di")||strstr(q,"lam viec cua ong")||strstr(q,"lam gi thi lam")||strstr(q,"huy lenh"))return true;
 char clean[256];strncpy(clean,q,255);clean[255]=0;int end=strlen(clean);while(end>0&&(clean[end-1]==' '||clean[end-1]=='!'||clean[end-1]=='.'||clean[end-1]=='?'))clean[--end]=0;
 return !strcmp(clean,"thoi")||!strcmp(clean,"thoi nhe")||!strcmp(clean,"thoi nha")||!strcmp(clean,"khong can")||!strcmp(clean,"ko can")||!strcmp(clean,"xong")||!strcmp(clean,"ve di");
}
static int PTBotCommandMode(const char* q){
 if(PTBotReleaseCommand(q))return PTBC_CANCEL;
 if(strstr(q,"vut bot")||strstr(q,"bo bot do"))return PTBC_DISCARD;
 if(strstr(q,"ban bot")||strstr(q,"don tui")||strstr(q,"don hanh trang"))return PTBC_DECLUTTER;
 if(strstr(q,"di train tiep")||strstr(q,"xong roi")||strstr(q,"di ve di")||strstr(q,"lam viec cua ong")||strstr(q,"lam gi thi lam"))return PTBC_CANCEL;
 if(strstr(q,"ban cho ong")||strstr(q,"ban cho bot")||strstr(q,"ong can mua")||strstr(q,"mua do cua toi"))return PTBC_MARKET_BUY;
 if(strstr(q,"oan tu ti")||strstr(q,"oan tu xi")||!strncmp(q,"ott",3)||strstr(q,"do den"))return PTBC_RPS;
 if(!strcmp(q,"pt")||!strncmp(q,"pt ",3)||strstr(q,"to doi")||strstr(q,"lap nhom")||strstr(q,"tao nhom")||strstr(q,"tao mom"))return PTBC_PARTY;
 if(strstr(q,"vtt")||strstr(q,"van tien tran"))return PTBC_VTT;
 if(strstr(q,"xem lai do")||strstr(q,"xem lai mon")||strstr(q,"post lai")||strstr(q,"gui lai link"))return PTBC_SALE_SHOW;
 if(strstr(q,"ban bao nhieu")||strstr(q,"gia bao nhieu")||strstr(q,"hoi mua")||strstr(q,"doi do")||strstr(q,"ban ko")||strstr(q,"ban khong"))return PTBC_SALE_QUOTE;
 if(strstr(q,"khong di")||strstr(q,"khong danh")||strstr(q,"dung danh")||strstr(q,"khong nhat")||strstr(q,"dung nhat")||strstr(q,"khong mua")||strstr(q,"dung mua")||strstr(q,"thoi khoi")||strstr(q,"dung theo")||strstr(q,"huy lenh")||strstr(q,"di viec cua")||strstr(q,"toi ban")||strstr(q,"tui ban"))return PTBC_CANCEL;
 // Questions about the bot's existing activity are not orders to start it.
 if(strstr(q,"dang ")||strstr(q,"tai sao")||strstr(q,"sao ong")||strstr(q,"sao ban")||strstr(q,"o dau"))return PTBC_NONE;
 if(strstr(q,"dung cho")||strstr(q,"dung yen")||strstr(q,"cho o day")||strstr(q,"dung lai")||strstr(q,"doi o day"))return PTBC_WAIT;
 if(strstr(q,"cuop luong"))return PTBC_RAID_SUPPLY;
 if(strstr(q,"cuop tieu"))return PTBC_RAID_ESCORT;
 if(strstr(q,"danh nguoi")||strstr(q,"danh thang")||strstr(q,"danh con nho"))return PTBC_PLAYER;
 if(strstr(q,"danh boss")||strstr(q,"danh con boss")||strstr(q,"san boss")||strstr(q,"di boss"))return PTBC_BOSS;
 if(strstr(q,"danh quai")||strstr(q,"danh con quai")||strstr(q,"diet quai")||strstr(q,"di train")||strstr(q,"luyen cap"))return PTBC_FIGHT;
 if(strstr(q,"nhat do")||strstr(q,"nhat vat pham")||strstr(q,"nhat tien"))return PTBC_LOOT;
 if(strstr(q,"di mua")||strstr(q,"mua giup")||strstr(q,"mua ho"))return PTBC_SHOP;
 if(strstr(q,"nhan qua")||strstr(q,"dua tien")||strstr(q,"cho ong tien")||strstr(q,"cho ban tien")||strstr(q,"giao dich"))return PTBC_GIFT;
 if(strstr(q,"theo sau")||strstr(q,"theo toi")||strstr(q,"theo tui")||strstr(q,"theo minh")||strstr(q,"di theo"))return PTBC_FOLLOW;
 if(strstr(q,"di cung")||strstr(q,"di voi toi")||strstr(q,"di voi tui"))return PTBC_INVITE;
 char clean[256];strncpy(clean,q,255);clean[255]=0;int end=strlen(clean);while(end>0&&(clean[end-1]==' '||clean[end-1]=='!'||clean[end-1]=='.'||clean[end-1]=='?'))clean[--end]=0;
 if(!strcmp(clean,"ok")||!strcmp(clean,"oke")||!strcmp(clean,"dong y")||!strcmp(clean,"duoc")||!strcmp(clean,"di thoi")||!strcmp(clean,"theo di"))return PTBC_CONFIRM;
 return PTBC_NONE;
}
#endif
