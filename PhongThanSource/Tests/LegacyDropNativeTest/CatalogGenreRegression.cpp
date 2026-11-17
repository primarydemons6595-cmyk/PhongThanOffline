#include "KCore.h"
#include "KPakList.h"
#include "KBasPropTbl.h"
#include <stdio.h>
KPakList* g_pPakList=0;
int main(int argc,char** argv){
    if(argc!=3)return 2;g_SetRootPath(argv[1]);g_SetFilePath("\\");KPakList packs;if(!packs.Open("\\package.ini"))return 3;g_pPakList=&packs;g_SetPakFileMode(1);
    KLibOfBPT catalog;if(!catalog.Init())return 4;FILE* file=fopen(argv[2],"wb");if(!file)return 5;
    char binaryPath[2048];sprintf(binaryPath,"%s.bin",argv[2]);FILE* binary=fopen(binaryPath,"wb");if(!binary)return 7;
    char rawPath[256];if(!BuildActiveItemTablePath("magicscript.txt",rawPath,sizeof(rawPath)))return 8;KTabFile raw;if(!raw.Load(rawPath))return 9;
    fprintf(file,"row,genre,detail,name_hex,other_fields_fnv,raw_anchor,raw_name_hex\n");int count=catalog.GetMagicScriptRecordNumber();
    for(int row=0;row<count;++row){const KBASICPROP_MAGICSCRIPT* item=catalog.MagicScriptAt(row);if(!item)return 6;
        KBASICPROP_MAGICSCRIPT rest=*item;rest.m_nItemGenre=0;memset(rest.m_szName,0,sizeof(rest.m_szName));
        unsigned long fingerprint=2166136261UL;const unsigned char* bytes=(const unsigned char*)&rest;for(size_t i=0;i<sizeof(rest);++i)fingerprint=(fingerprint^bytes[i])*16777619UL;
        char rawName[80],cell[80];int anchor=0;raw.GetString(row+2,1,"",rawName,sizeof(rawName));for(int col=3;col<=12;++col){raw.GetString(row+2,col,"",cell,sizeof(cell));if(strstr(cell,".spr")||strstr(cell,".SPR")){anchor=col;break;}}
        fprintf(file,"%d,%d,%d,",row,item->m_nItemGenre,item->m_nDetailType);for(i=0;i<strlen(item->m_szName);++i)fprintf(file,"%02X",(unsigned char)item->m_szName[i]);fprintf(file,",%lu,%d,",fingerprint,anchor);for(i=0;i<strlen(rawName);++i)fprintf(file,"%02X",(unsigned char)rawName[i]);fprintf(file,"\n");
        if(fwrite(item,sizeof(*item),1,binary)!=1)return 10;
    }fclose(binary);fclose(file);printf("GENRE_CATALOG_DUMP records=%d record_size=%d\n",count,sizeof(KBASICPROP_MAGICSCRIPT));g_pPakList=0;return 0;
}
