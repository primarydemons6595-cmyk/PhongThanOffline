#ifndef PHONGTHAN_TOOLTIP_WRAP_H
#define PHONGTHAN_TOOLTIP_WRAP_H
#include <string.h>
// Vietnamese runtime text uses single-byte TCVN3 glyphs. Keep rich-text tags
// intact and wrap at spaces; never reinterpret a Vietnamese glyph as a DBCS lead.
inline int PtWrapTooltip(const char* text,char* out,int capacity,int columns=48){
 if(!out||capacity<=0)return 0;int n=0,visible=0,lastSpace=-1,sinceSpace=0;
 if(!text)text="";
 for(int i=0;text[i]&&n<capacity-1;){
  if(text[i]=='<'){
   int end=i+1;while(text[end]&&text[end]!='>'&&end-i<64)++end;
   if(text[end]=='>'){
    int length=end-i+1;if(n+length>=capacity)break;
    memcpy(out+n,text+i,length);n+=length;
    if(length==7&&!strncmp(text+i,"<enter>",7)){visible=0;lastSpace=-1;sinceSpace=0;}
    i=end+1;continue;
   }
  }
  char c=text[i++];if(c=='\r')continue;
  if(c=='\n'){out[n++]=c;visible=0;lastSpace=-1;sinceSpace=0;continue;}
  if(visible>=columns){
   if(lastSpace>=0){out[lastSpace]='\n';visible=sinceSpace;lastSpace=-1;}
   else{if(n+1>=capacity)break;out[n++]='\n';visible=0;}
  }
  if(!visible&&c==' ')continue;
  out[n]=c;if(c==' '){lastSpace=n;sinceSpace=0;}else ++sinceSpace;
  ++n;++visible;
 }
 out[n]=0;return n;
}
#endif
