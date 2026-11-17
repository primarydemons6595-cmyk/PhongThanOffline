#include "../../Headers/PhongThanNpcNameColor.h"
#include <assert.h>
#include <stdio.h>
static void check(const char* raw,const char* visible,unsigned int expected,bool tagged=true){
 char text[128];unsigned int color=0xffc08020;
 assert(PhongThanParseNpcName(raw,text,sizeof(text),color)==tagged);
 assert(!strcmp(text,visible));assert(color==expected);
}
int main(){
 check("<color=red>Boss<color>","Boss",0xffff0000);
 check("<c=YEL>Quai<c>","Quai",0xffffff00);
 check("<COLOR=cyan>Boss</COLOR>","Boss",0xff00ffff);
 check("<c=255,128,0>Boss<c>","Boss",0xffff8000);
 check("<color=#123ABC>Boss<color>","Boss",0xff123abc);
 check("<c=0xFF123456>Boss<c>","Boss",0xff123456);
 check("$<bclr=red><c=green>Boss<c><bclr>","Boss",0xff00ff00);
 check("<color=no_such_color>Boss<color>","Boss",0xffc08020,false);
 check("Boss","Boss",0xffc08020,false);
 check("<c=red>\xB5\xA8\xD1<c>","\xB5\xA8\xD1",0xffff0000);
 check("<c=red>Boss<colo","Boss",0xffff0000);
 check("<c=999,0,0>Boss<c>","Boss",0xffc08020,false);
 check("<c=blue>Boss<c=red>","Boss",0xff6464ff);
 char tiny[5]={'x','x','x','x','!'};unsigned int color=0;
 assert(PhongThanParseNpcName("<c=red>BossLong<c>",tiny,4,color));
 assert(!strcmp(tiny,"Bos")&&tiny[4]=='!');
 char packet[64];memset(packet,'A',63);packet[63]=0;
 memcpy(packet,"<c=red>",7);memcpy(packet+60,"<c>",3);
 char plain[64];assert(PhongThanParseNpcName(packet,plain,64,color));assert(strlen(plain)==53);
 assert(!PhongThanParseNpcName(0,plain,64,color)&&!*plain);
 assert(!PhongThanParseNpcName("Boss",0,0,color));
 puts("PASS: NPC name palette, markup, legacy bytes, truncation and 64-byte wire boundary");
 return 0;
}
