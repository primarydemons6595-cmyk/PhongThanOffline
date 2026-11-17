#include "../../Headers/PhongThanTooltipWrap.h"
#include <assert.h>
#include <stdio.h>
int main(){
 char out[1024];
 const char* text="one two three four five six seven eight nine ten eleven twelve thirteen fourteen fifteen sixteen seventeen eighteen nineteen twenty";
 PtWrapTooltip(text,out,sizeof(out),48);
 int line=0,breaks=0;for(int i=0;out[i];++i){if(out[i]=='\n'){assert(line<=48);line=0;++breaks;}else ++line;}assert(line<=48&&breaks>=2);
 char restored[1024];strcpy(restored,out);for(i=0;restored[i];++i)if(restored[i]=='\n')restored[i]=' ';assert(!strcmp(restored,text));
 PtWrapTooltip("<color=green>one two three<color><enter>four five\nsix",out,sizeof(out),48);
 assert(!strcmp(out,"<color=green>one two three<color><enter>four five\nsix"));
 char vietnamese[70];memset(vietnamese,0xb5,69);vietnamese[69]=0;
 PtWrapTooltip(vietnamese,out,sizeof(out),48);assert(out[48]=='\n'&&strlen(out)==70);
 char tiny[5];PtWrapTooltip(text,tiny,sizeof(tiny),48);assert(!strcmp(tiny,"one "));
 char guarded[8];memset(guarded,'X',sizeof(guarded));PtWrapTooltip(text,guarded+1,5,48);assert(guarded[0]=='X'&&guarded[6]=='X'&&guarded[5]==0);
 puts("PASS tooltip word wrapping, TCVN3 glyphs, rich tags, explicit newlines and bounded buffers");return 0;
}
