#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "KWin32.h"
#include "Text.h"
int main(){char text[256]="<color=green>Tri hoa: Tam Son (da xong)<color>";int raw=strlen(text),encoded=TEncodeText(text,raw);if(encoded>=raw)return 1;int plain=TRemoveCtrlInEncodedText(text,encoded);text[plain]=0;if(strcmp(text,"Tri hoa: Tam Son (da xong)"))return 2;
 strcpy(text,"<c=yellow>Hoan Cau Tinh<c>");raw=strlen(text);encoded=TEncodeText(text,raw);if(encoded>=raw)return 3;plain=TRemoveCtrlInEncodedText(text,encoded);text[plain]=0;if(strcmp(text,"Hoan Cau Tinh"))return 4;
 puts("PASS actual Engine text codec: positive-length native NPC list text carries colour controls and preserves words");return 0;}
