#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "../Sources/Engine/Src/Text.h"
typedef int (__cdecl *Encode)(char*,int,int*);
int main(int argc,char** argv){
    if(argc!=2)return 1;
    HMODULE engine=LoadLibraryA(argv[1]);if(!engine){printf("load error=%lu\n",GetLastError());return 2;}
    Encode encode=(Encode)GetProcAddress(engine,"TEncodeText");
    if(!encode)encode=(Encode)GetProcAddress(engine,"_TEncodeText");if(!encode)return 3;
    char text[256]="<c=g>Hello<c>";
    int size=encode(text,strlen(text),0);
    const unsigned char green[]={KTC_COLOR,0,255,0,'H','e','l','l','o',KTC_COLOR_RESTORE};
    if(size!=sizeof(green)||memcmp(text,green,size))return 4;
    strcpy(text,"<color=255,128,0>x<color>");size=encode(text,strlen(text),0);
    const unsigned char rgb[]={KTC_COLOR,255,128,0,'x',KTC_COLOR_RESTORE};
    if(size!=sizeof(rgb)||memcmp(text,rgb,size))return 5;
    strcpy(text,"<c=yel>x<c>");size=encode(text,strlen(text),0);
    const unsigned char yellow[]={KTC_COLOR,255,255,0,'x',KTC_COLOR_RESTORE};
    if(size!=sizeof(yellow)||memcmp(text,yellow,size))return 6;
    strcpy(text,"a<\\n>b");size=encode(text,strlen(text),0);
    if(size!=3||text[0]!='a'||text[1]!=KTC_ENTER||text[2]!='b')return 7;
    text[0]=(char)0xb5;text[1]=(char)0xe9;text[2]=0;size=encode(text,2,0);
    if(size!=2||(unsigned char)text[0]!=0xb5||(unsigned char)text[1]!=0xe9)return 8;
    strcpy(text,"<color=");memset(text+7,'A',100);strcpy(text+107,">x");
    size=encode(text,strlen(text),0);if(size!=1||text[0]!='x')return 9;
    FreeLibrary(engine);
    puts("PASS: native short/named/RGB colors, reset, line break, TCVN3 glyphs, oversized color rejection.");return 0;
}
