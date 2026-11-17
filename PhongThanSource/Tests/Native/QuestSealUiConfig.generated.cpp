#include "KWin32.h"
#include "KIniFile.h"
#include "KFilePath.h"
#include "KPakFile.h"
#include <stdio.h>
#include "../../Headers/PhongThanQuestSeals.h"
static bool PtQuestSealUiHex(KIniFile& ini,const char* section,const char* field,char* out,int size){
 char hex[4096];ini.GetString(section,field,"",hex,sizeof(hex));int n=strlen(hex);if(n%2||n/2>=size)return false;
 for(int i=0;i<n;i+=2){char pair[3]={hex[i],hex[i+1],0};char* end=0;int c=strtol(pair,&end,16);if(*end||c<=0||c>255)return false;out[i/2]=(char)c;}out[n/2]=0;return true;
}
struct Fixture {KIniFile m_PtPowderIni;void Load(){KIniFile seals;if(seals.Load("\\settings\\phongthan\\QuestSeals.ini")){
        int version=0,total=0;seals.GetInteger("Main","Version",0,&version);seals.GetInteger("Main","Count",0,&total);
        if(version==1&&total>=0&&total<=PT_QUEST_SEAL_MAX)for(int seal=1;seal<=total;++seal){
            char section[32],key[32],image[256],title[256],desc[1201];sprintf(section,"Seal%d",seal);int id=0,kind=0;seals.GetInteger(section,"Id",0,&id);seals.GetInteger(section,"Kind",0,&kind);
            if(kind!=0||id<1||id>PT_QUEST_SEAL_MAX||!PtQuestSealUiHex(seals,section,"ImageHex",image,sizeof(image))||!PtQuestSealUiHex(seals,section,"NameHex",title,sizeof(title))||!PtQuestSealUiHex(seals,section,"TooltipHex",desc,sizeof(desc)))continue;
            sprintf(key,"BuffInfo_%d",PT_QUEST_SEAL_FIRST_BUFF+id-1);m_PtPowderIni.WriteString(key,"Image",image);m_PtPowderIni.WriteString(key,"Title",title);m_PtPowderIni.WriteString(key,"Desc",desc);
        }
    }}};

#define CHECK(c) do{if(!(c)){printf("FAIL seal UI config line %d %s\n",__LINE__,#c);return 1;}}while(0)
int main(int argc,char**argv){
 CHECK(argc==2&&SetCurrentDirectoryA(argv[1]));g_SetRootPath(NULL);g_SetFilePath("\\");g_SetPakFileMode(0);
 Fixture ui;ui.m_PtPowderIni.WriteString("BuffInfo_330","Title","Original buff");ui.Load();char image[256],name[256],desc[512];
 ui.m_PtPowderIni.GetString("BuffInfo_9700","Image","",image,sizeof(image));ui.m_PtPowderIni.GetString("BuffInfo_9700","Title","",name,sizeof(name));
 CHECK(!strcmp(image,"spr\\phongthan\\dialog\\answer.spr")&&name[0]);ui.m_PtPowderIni.GetString("BuffInfo_9700","Desc","",desc,sizeof(desc));CHECK(desc[0]);
 ui.m_PtPowderIni.GetString("BuffInfo_330","Title","",name,sizeof(name));CHECK(!strcmp(name,"Original buff"));
 ui.m_PtPowderIni.GetString("BuffInfo_9701","Image","",image,sizeof(image));CHECK(!image[0]);
 puts("PASS production buff UI metadata merge: timed-seal image/title/description, original buff retained, F4 seal excluded from buff bar");return 0;
}
