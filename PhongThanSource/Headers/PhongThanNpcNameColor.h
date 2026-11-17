#ifndef PHONGTHAN_NPC_NAME_COLOR_H
#define PHONGTHAN_NPC_NAME_COLOR_H
#include <string.h>
#include <stdio.h>
// Same foreground palette as Engine/Text.cpp. Names remain in their original
// byte encoding; markup is presentation and does not contribute to centering.
inline bool PhongThanNpcColorValue(const char* value,int length,unsigned int& color){
 if(length<1||length>40)return false;char token[41];int n=0;
 while(length&&(*value==' '||*value=='\t')){++value;--length;}
 while(length&&(value[length-1]==' '||value[length-1]=='\t'))--length;
 for(int i=0;i<length;++i){unsigned char c=value[i];token[n++]=(c>='A'&&c<='Z')?c+32:c;}token[n]=0;
 struct Entry{const char* name;unsigned int rgb;};
 static const Entry palette[]={
  {"r",0xff0000},{"g",0x00ff00},{"b",0x6464ff},{"w",0xffffff},{"y",0xffff00},{"yel",0xffff00},
  {"black",0},{"white",0xffffff},{"red",0xff0000},{"green",0x00ff00},{"blue",0x6464ff},
  {"yellow",0xffff00},{"orange",0xfcab3a},{"pink",0xb500b5},{"pk",0xb500b5},{"violet",0xaa1eff},
  {"cyan",0x00ffff},{"metal",0xf6ff75},{"wood",0x00ff78},{"water",0x4e7cff},{"fire",0xff5a00},
  {"earth",0xfecfb3},{"dblue",0x787878},{"hblue",0x6464ff},{"gold",0xdee1a5},{"gray",0xbdbebd}};
 for(i=0;i<sizeof(palette)/sizeof(palette[0]);++i)if(!strcmp(token,palette[i].name)){color=0xff000000U|palette[i].rgb;return true;}
 unsigned int red,green,blue;char extra;
 if(sscanf(token,"%u,%u,%u%c",&red,&green,&blue,&extra)==3&&red<256&&green<256&&blue<256){color=0xff000000U|(red<<16)|(green<<8)|blue;return true;}
 const char* hex=token;if(*hex=='#')++hex;else if(hex[0]=='0'&&hex[1]=='x')hex+=2;else return false;
 int digits=strlen(hex);if(digits!=6&&digits!=8)return false;unsigned int rgb=0;
 for(i=0;i<digits;++i){unsigned char c=hex[i];int digit=c>='0'&&c<='9'?c-'0':c>='a'&&c<='f'?c-'a'+10:-1;if(digit<0)return false;rgb=(rgb<<4)|digit;}
 color=0xff000000U|(rgb&0xffffffU);return true;
}
inline bool PhongThanParseNpcName(const char* source,char* plain,int capacity,unsigned int& color){
 if(!plain||capacity<1)return false;if(!source)source="";int used=0;bool colored=false;
 while(*source=='#'||*source=='$')++source;
 while(*source&&used<capacity-1){
  if(*source=='<'){
   const char* start=source+1;if(*start=='/')++start;char key[8];int n=0;
   while(n<7&&start[n]&&start[n]!='='&&start[n]!='>'&&start[n]!=' '){char c=start[n];key[n]=(c>='A'&&c<='Z')?c+32:c;++n;}key[n]=0;
   bool known=!strcmp(key,"c")||!strcmp(key,"color")||!strcmp(key,"bclr");
   const char* end=strchr(source,'>');
   if(!end){
    // A network name may end in a partially truncated closing tag.
    if(known||!strcmp(key,"co")||!strcmp(key,"col")||!strcmp(key,"colo"))break;
   }else if(known){
    const char* value=start+n;while(value<end&&*value==' ')++value;
    if(*value=='='&&source[1]!='/'&&strcmp(key,"bclr")&&!colored){unsigned int parsed=color;if(PhongThanNpcColorValue(value+1,(int)(end-value-1),parsed)){color=parsed;colored=true;}}
    source=end+1;continue;
   }
  }
  unsigned char c=*source++;if(c>=32&&c!=127)plain[used++]=c;
 }plain[used]=0;return colored;
}
#endif
