#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
static DWORD tick=10;
static DWORD TestTick(){return tick;}
#define GetTickCount TestTick
#include "PhongThanCustomItemText.inl"
static int checks=0;
#define CHECK(x) do{++checks;if(!(x)){printf("FAIL %d\n",__LINE__);return 1;}}while(0)
static void Write(const char* body){FILE* f=fopen("settings\\phongthan\\custom_event_items.tsv","wb");if(!f)exit(2);fputs(body,f);fclose(f);tick+=1001;}
int main(int argc,char**argv){
 if(argc!=2||!SetCurrentDirectoryA(argv[1]))return 2;
 CreateDirectoryA("settings",0);CreateDirectoryA("settings\\phongthan",0);
 Write("771751937\t4f6c6420696e74726f\n"); // old two-column format, tag2E/recipe1
 CHECK(!PhongThanCustomItemName(0));CHECK(!PhongThanCustomItemName(1));
 CHECK(!PhongThanCustomItemName(0x2e000001));
 CHECK(!strcmp(PhongThanCustomItemTooltip(0x2e000001),"Old intro"));
 Write("771751937\t496e74726f\t3c636f6c6f723d7265643e43616b653c636f6c6f723e\n");
 const char* old=PhongThanCustomItemName(0x2e000001);
 CHECK(old&&!strcmp(old,"Cake"));
 CHECK(!strcmp(PhongThanCustomItemRichName(0x2e000001),"<color=red>Cake<color>"));
 CHECK(!PhongThanCustomItemName(0x2e000002)); // another recipe keeps template
 Write("771751937\t\t536e6f776d616e\n");
 CHECK(!strcmp(PhongThanCustomItemName(0x2e000001),"Snowman"));
 CHECK(!strcmp(old,"Cake")); // reload never leaves dangling GetName pointers
 Write("771751937\t\t+1\n");
 CHECK(!strcmp(PhongThanCustomItemName(0x2e000001),"Snowman")); // reject malformed hex
 Write("771751937\t\t4100\n");
 CHECK(!strcmp(PhongThanCustomItemName(0x2e000001),"Snowman")); // embedded NUL
 Write("1\t\t41\n");
 CHECK(!strcmp(PhongThanCustomItemName(0x2e000001),"Snowman")); // unrelated tag
 Write("771751937\t\t41\n771751937\t\t42\n");
 CHECK(!strcmp(PhongThanCustomItemName(0x2e000001),"Snowman"));
 Write("771751937\t\t\n");CHECK(!PhongThanCustomItemName(0x2e000001));
 printf("PASS custom event item text: %d checks\n",checks);return 0;
}
