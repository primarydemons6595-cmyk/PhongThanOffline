// Instance event marker survives inventory/trade/item-link persistence. Only
// marked event items use this text; original templates retain their tooltips.
#include <map>
#include <string>
static const char* PhongThanCustomItemTooltip(int param){
 int tag=param&0xFF000000;if(tag!=0x2E000000&&tag!=0x2D000000)return 0;
 static std::map<int,std::string> text;static DWORD next=0;DWORD now=GetTickCount();
 if((int)(now-next)>=0){next=now+1000;FILE* f=fopen("settings\\phongthan\\custom_event_items.tsv","rb");std::map<int,std::string> candidate;
  if(f){char line[3000];bool valid=true;while(fgets(line,sizeof(line),f)){char* end=0;long key=strtol(line,&end,10);if(end==line||*end!='\t'){if(line[0]=='\n'||line[0]=='\r')continue;valid=false;break;}char* hex=end+1;std::string value;unsigned int n=strcspn(hex,"\r\n");if(n%2||n>2400){valid=false;break;}
    for(unsigned int i=0;i<n;i+=2){char a=hex[i],b=hex[i+1];int hi=a>='0'&&a<='9'?a-'0':a>='a'&&a<='f'?a-'a'+10:a>='A'&&a<='F'?a-'A'+10:-1;int lo=b>='0'&&b<='9'?b-'0':b>='a'&&b<='f'?b-'a'+10:b>='A'&&b<='F'?b-'A'+10:-1;if(hi<0||lo<0||(!hi&&!lo)){valid=false;break;}value+=(char)(hi*16+lo);}
    if(!valid)break;candidate[(int)key]=value;
  }fclose(f);if(valid)text.swap(candidate);}else text.clear();
 }
 std::map<int,std::string>::const_iterator i=text.find(param);return i!=text.end()&&!i->second.empty()?i->second.c_str():0;
}
