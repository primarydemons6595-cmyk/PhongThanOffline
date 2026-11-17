#ifndef PHONGTHAN_NATION_STORE_H
#define PHONGTHAN_NATION_STORE_H
#include "../../../Headers/PhongThanNationProtocol.h"
#include <stdio.h>
#include <string.h>
#include "../../../Headers/PhongThanTerritoryState.h"
#include <stddef.h>

enum {PT_NATION_CAPACITY=256,PT_NATION_MEMBER_CAPACITY=4096};
struct PTNationRecord {unsigned int Id;int Money;char Name[32],Leader[32],Notice[256];};
struct PTNationMember {unsigned int NationId;int Duty;char Name[32];};
struct PTNationWallet {char Name[32];__int64 Total;};
struct PTNationState {
    unsigned int Magic,Version,Revision,NextId;
    PTNationRecord Nations[PT_NATION_CAPACITY];
    PTNationMember Members[PT_NATION_MEMBER_CAPACITY];
    PTNationWallet Wallets[PT_NATION_MEMBER_CAPACITY];
    PTNationDevelopment Development[PT_NATION_CAPACITY];
    PTTerritoryContribution Contributions[PT_NATION_MEMBER_CAPACITY];
    PTNationBook Books[PT_NATION_CAPACITY];
};
enum {PT_NATION_V2_SIZE=411664,PT_NATION_V3_SIZE=1409040};
// Native durable state. Public operations are guarded in the server adapter.
class PTNationStore {
public:
    PTNationState State;
    bool Ready,Failed;
    PTNationStore():Ready(false),Failed(false){memset(&State,0,sizeof(State));}
    int Nation(unsigned int id)const{for(int i=0;i<PT_NATION_CAPACITY;++i)if(id&&State.Nations[i].Id==id)return i;return -1;}
    int Member(const char* name)const{for(int i=0;i<PT_NATION_MEMBER_CAPACITY;++i)if(State.Members[i].NationId&&!strcmp(State.Members[i].Name,name))return i;return -1;}
    int Wallet(const char* name)const{for(int i=0;i<PT_NATION_MEMBER_CAPACITY;++i)if(State.Wallets[i].Name[0]&&!strcmp(State.Wallets[i].Name,name))return i;return -1;}
    int FreeWallet()const{for(int i=0;i<PT_NATION_MEMBER_CAPACITY;++i)if(!State.Wallets[i].Name[0])return i;return -1;}
    int FreeNation()const{for(int i=0;i<PT_NATION_CAPACITY;++i)if(!State.Nations[i].Id)return i;return -1;}
    int FreeMember()const{for(int i=0;i<PT_NATION_MEMBER_CAPACITY;++i)if(!State.Members[i].NationId)return i;return -1;}
    static unsigned int Hash(const void* p,unsigned int n){unsigned int h=2166136261u;const unsigned char* d=(const unsigned char*)p;for(unsigned int i=0;i<n;++i)h=(h^d[i])*16777619u;return h;}
    bool Valid()const{
        if(State.Magic!=0x544e5450||State.Version!=4||State.NextId<1)return false;
        for(int i=0;i<PT_NATION_CAPACITY;++i){const PTNationRecord& a=State.Nations[i];if(!a.Id)continue;
            if(a.Id>=State.NextId||a.Money<0||!a.Name[0]||!a.Leader[0]||
               !PTNationText(a.Name,sizeof(a.Name))||!PTNationText(a.Leader,sizeof(a.Leader))||!PTNationText(a.Notice,sizeof(a.Notice)))return false;
            for(int j=0;j<i;++j)if(State.Nations[j].Id&&(State.Nations[j].Id==a.Id||!strcmp(State.Nations[j].Name,a.Name)))return false;
            int leaders=0;for(int m=0;m<PT_NATION_MEMBER_CAPACITY;++m)if(State.Members[m].NationId==a.Id&&State.Members[m].Duty==PT_NATION_LORD){if(strcmp(State.Members[m].Name,a.Leader))return false;++leaders;}
            if(leaders!=1)return false;
        }
        for(int m=0;m<PT_NATION_MEMBER_CAPACITY;++m){const PTNationMember& a=State.Members[m];if(!a.NationId)continue;
            if(Nation(a.NationId)<0||a.Duty<0||a.Duty>5||!a.Name[0]||!PTNationText(a.Name,sizeof(a.Name)))return false;
            for(int n=0;n<m;++n)if(State.Members[n].NationId&&!strcmp(State.Members[n].Name,a.Name))return false;
        }
        for(int w=0;w<PT_NATION_MEMBER_CAPACITY;++w){const PTNationWallet& a=State.Wallets[w];if(!a.Name[0])continue;
            if(!PTNationText(a.Name,sizeof(a.Name))||a.Total< -((__int64)1<<60)||a.Total>((__int64)1<<60))return false;
            for(int j=0;j<w;++j)if(State.Wallets[j].Name[0]&&!strcmp(a.Name,State.Wallets[j].Name))return false;
        }
        int cities=0;
        for(int d=0;d<PT_NATION_CAPACITY;++d){const PTNationDevelopment& dev=State.Development[d];
            if(!dev.NationId)continue;
            if(!PTTerritoryDevelopmentValid(dev,State.Nations[d].Id))return false;
            if(dev.CityLevel){++cities;for(int k=0;k<d;++k)if(State.Development[k].CityLevel&&(State.Development[k].CityMapId-2100)/5==(dev.CityMapId-2100)/5)return false;}
        }
        if(cities>PT_TERRITORY_MAX_CITIES)return false;
        for(int c=0;c<PT_NATION_MEMBER_CAPACITY;++c){const PTTerritoryContribution& contribution=State.Contributions[c];if(!contribution.NationId)continue;
            if(!PTTerritoryContributionValid(contribution)||contribution.NationId!=State.Members[c].NationId||strcmp(contribution.Name,State.Members[c].Name))return false;
        }
        for(int b=0;b<PT_NATION_CAPACITY;++b){const PTNationBook& book=State.Books[b];if(book.NationId&&(book.NationId!=State.Nations[b].Id||book.Fragments[0]<0||book.Fragments[1]<0||book.Migrated<0||book.Migrated>1))return false;}
        return true;
    }
    bool Load(const char* file){
        if(Ready||Failed)return Ready;
        HANDLE f=CreateFile(file,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
        if(f==INVALID_HANDLE_VALUE){if(GetLastError()!=ERROR_FILE_NOT_FOUND){Failed=true;return false;}
            memset(&State,0,sizeof(State));State.Magic=0x544e5450;State.Version=4;State.NextId=1;Ready=true;return true;}
        unsigned int hash=0;DWORD read=0,length=GetFileSize(f,NULL);bool legacy=length==PT_NATION_V2_SIZE+sizeof(hash);
        bool v3=length==PT_NATION_V3_SIZE+sizeof(hash);unsigned int bytes=legacy?PT_NATION_V2_SIZE:v3?PT_NATION_V3_SIZE:sizeof(State);
        memset(&State,0,sizeof(State));
        bool ok=(legacy||v3||length==sizeof(State)+sizeof(hash))&&offsetof(PTNationState,Development)==PT_NATION_V2_SIZE&&
            ReadFile(f,&State,bytes,&read,NULL)&&read==bytes&&ReadFile(f,&hash,sizeof(hash),&read,NULL)&&read==sizeof(hash);
        CloseHandle(f);ok=ok&&hash==Hash(&State,bytes)&&State.Version==(legacy?2u:v3?3u:4u);
        if(ok)State.Version=4;
        Ready=ok&&Valid();Failed=!Ready;return Ready;
    }
    bool Save(const char* file){
        if(!Ready||Failed||!Valid())return false;
        char temp[MAX_PATH],backup[MAX_PATH];if(strlen(file)+5>=MAX_PATH)return false;
        sprintf(temp,"%s.new",file);sprintf(backup,"%s.bak",file);
        HANDLE f=CreateFile(temp,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
        if(f==INVALID_HANDLE_VALUE)return false;
        unsigned int hash=Hash(&State,sizeof(State));DWORD written=0;
        bool ok=WriteFile(f,&State,sizeof(State),&written,NULL)&&written==sizeof(State)&&
            WriteFile(f,&hash,sizeof(hash),&written,NULL)&&written==sizeof(hash)&&FlushFileBuffers(f);
        CloseHandle(f);if(!ok){DeleteFile(temp);return false;}
        if(GetFileAttributes(file)!=(DWORD)-1&&!CopyFile(file,backup,FALSE)){DeleteFile(temp);return false;}
        if(!MoveFileEx(temp,file,MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH)){DeleteFile(temp);return false;}
        return true;
    }
};
#endif
