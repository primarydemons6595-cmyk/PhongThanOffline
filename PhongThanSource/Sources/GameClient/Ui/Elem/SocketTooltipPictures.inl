// The engine text encoder only accepts system picture numbers. Resolve the
// local socket marker after encoding, using the existing custom-picture sink.
static int EncodeSocketTooltip(char* text,int size,const PT_SOCKET_VISUALS& visuals){
 int length=TEncodeText(text,size);
 struct Picture {char Path[128];WORD Id;};static Picture cache[128];static int count=0;
 for(int at=0;at+6<=length;){
  if(memcmp(text+at,"{pts",4)||text[at+4]<'0'||text[at+4]>'3'||text[at+5]!='}'){++at;continue;}
  int slot=text[at+4]-'0';const char* path=0;
  for(int s=0;s<visuals.Count&&s<4;++s)if(visuals.Slot[s]==slot)path=visuals.MenuImage[s];
  WORD id=(WORD)-1;
  if(path&&path[0]&&g_pIInlinePicSink){int p=0;for(;p<count;++p)if(!strcmp(cache[p].Path,path)){id=cache[p].Id;break;}
   if(p==count&&count<128&&SUCCEEDED(g_pIInlinePicSink->AddCustomInlinePic(id,path))){g_StrCpyLen(cache[count].Path,path,sizeof(cache[count].Path));cache[count++].Id=id;}
  }
  int used=id==(WORD)-1?0:3;memmove(text+at+used,text+at+6,length-at-6);length-=6-used;
  if(used){text[at]=KTC_INLINE_PIC;memcpy(text+at+1,&id,2);}at+=used;
 }text[length]=0;return length;
}
