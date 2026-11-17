#ifndef PHONGTHAN_STATUS_TEXT_H
#define PHONGTHAN_STATUS_TEXT_H
#include <string.h>
// Preserve original description bytes; adapt only VNG's display markup.
inline void PtStatusDisplayText(char* out,int capacity,const char* input){
 if(!out||capacity<1)return;int used=0;if(!input)input="";
 if(*input=='#'||*input=='$')++input;
 while(*input&&used<capacity-1){
  const char* replacement=0;int consumed=0;char color[48];
  if(!strncmp(input,"<enter>",7)){replacement="\n";consumed=7;}
  else if(!strncmp(input,"<\\n>",4)){replacement="\n";consumed=4;}
  else if(!strncmp(input,"<c>",3)){replacement="<color>";consumed=3;}
  else if(!strncmp(input,"<c=",3)){
   const char* end=strchr(input+3,'>');int size=end?(int)(end-input-3):0;
   if(size>0&&size<24){char token[24];memcpy(token,input+3,size);token[size]=0;const char* name=token;
    if(!strcmp(token,"r"))name="red";else if(!strcmp(token,"g"))name="green";else if(!strcmp(token,"b"))name="blue";else if(!strcmp(token,"y"))name="yellow";else if(!strcmp(token,"pk"))name="pink";
    strcpy(color,"<color=");strcat(color,name);strcat(color,">");replacement=color;consumed=(int)(end-input+1);
   }
  }
  if(replacement){int size=strlen(replacement);if(size>capacity-used-1)break;memcpy(out+used,replacement,size);used+=size;input+=consumed;}
  else out[used++]=*input++;
 }out[used]=0;
}
#endif
