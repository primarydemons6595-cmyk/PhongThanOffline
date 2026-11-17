#ifndef PT_BOT_NAME_H
#define PT_BOT_NAME_H
#include <string.h>
static int PTBotNameHex(char c){return c>='0'&&c<='9'?c-'0':c>='a'&&c<='f'?c-'a'+10:-1;}
static bool PTBotNameField(const char* field,int version,char* name){
 int length=strlen(field);if(version==2){if(length<6||length>48||length%2)return false;for(int i=0;i<length/2;++i){int a=PTBotNameHex(field[i*2]),b=PTBotNameHex(field[i*2+1]);if(a<0||b<0)return false;name[i]=(char)(a*16+b);}name[length/2]=0;length/=2;}
 else{if(length<3||length>24)return false;memcpy(name,field,length+1);}
 if(name[0]==' '||name[length-1]==' ')return false;
 static const unsigned char vietnamese[]={181,184,182,183,185,168,187,190,188,189,198,169,199,202,200,201,203,174,204,208,206,207,209,170,210,213,211,212,214,215,221,216,220,222,223,227,225,226,228,171,229,232,230,231,233,172,234,237,235,236,238,239,243,241,242,244,173,245,248,246,247,249,250,253,251,252,254,161,162,167,163,164,165,166};
 for(int j=0;j<length;++j){unsigned char c=(unsigned char)name[j];if((c>='a'&&c<='z')||(c>='A'&&c<='Z')||(c>='0'&&c<='9')||c=='_'||c=='.'||c==' ')continue;bool found=false;for(int k=0;k<sizeof(vietnamese);++k)if(c==vietnamese[k])found=true;if(!found)return false;}
 return true;
}
#endif
