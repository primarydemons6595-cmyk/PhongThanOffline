#ifndef PT_BOT_PARTY_SPEECH_H
#define PT_BOT_PARTY_SPEECH_H
#include <string.h>
static void PTBotFoldPartySpeech(const char* raw,char* out,int size){static const unsigned char map[256]={32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,33,32,32,32,32,32,32,32,32,32,32,32,32,46,32,48,49,50,51,52,53,54,55,56,57,32,32,32,32,32,63,32,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,32,32,32,32,32,32,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,97,97,101,111,111,117,100,97,97,101,111,111,117,100,32,32,32,32,32,32,97,97,97,97,97,32,97,97,97,97,32,97,97,97,97,97,97,97,97,97,97,97,97,101,105,101,101,101,101,101,101,101,101,101,105,105,117,117,117,105,105,105,111,97,111,111,111,111,111,111,111,111,111,111,111,111,111,111,117,32,117,117,117,117,117,117,117,117,117,121,121,121,121,121,121};int i=0;for(;raw[i]&&i<size-1;++i)out[i]=map[(unsigned char)raw[i]];out[i]=0;}
// 0=status only, 1=invite speaker to bot team, 2=bot joins speaker team.
static int PTBotPartySpeechMode(const char* q){
 char clean[256];strncpy(clean,q,255);clean[255]=0;int end=strlen(clean);while(end>0&&(clean[end-1]==' '||clean[end-1]=='!'||clean[end-1]=='.'||clean[end-1]=='?'))clean[--end]=0;q=clean;
 if(strstr(q,"vao doi tui")||strstr(q,"vao doi toi")||strstr(q,"vao doi minh")||strstr(q,"vao nhom tui")||strstr(q,"vao nhom toi"))return 2;
 if(strstr(q,"moi")||strstr(q,"xin")||strstr(q,"cho ")||strstr(q,"ke ")||strstr(q,"keo ")||strstr(q,"join")||strstr(q,"nhap")||strstr(q,"vao ")||!strcmp(q,"dong y")||!strcmp(q,"ok")||!strcmp(q,"oke")||!strcmp(q,"duoc")||!strcmp(q,"pt"))return 1;
 return 0;
}
#endif
