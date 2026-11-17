#include <windows.h>
#include <map>
#include <string>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#define CLIENT_PLAYER_INDEX 1
#define MAX_NPC 4
struct TestPlayer { int m_nIndex; } Player[2];
struct TestNpc { char Name[32]; } Npc[MAX_NPC];
static std::map<std::string,int> saved;
class KIniFile {
    std::string path;int value;
public:
    KIniFile():value(0){}
    bool Load(const char* file){path=file;value=saved.count(path)?saved[path]:0;return saved.count(path)!=0;}
    void GetInteger(const char*,const char*,int fallback,int* result){*result=saved.count(path)?value:fallback;}
    void WriteInteger(const char*,const char*,int number){value=number;}
    bool Save(const char* file){saved[file]=value;return true;}
};
#include "TeamOptionsProduction.inl"
int main(){
    Player[1].m_nIndex=1;strcpy(Npc[1].Name,"Alice");
    assert(PhongThanTeamOptions(0,false)==0);
    assert(PhongThanTeamOptions(1,true)==1);
    assert(PhongThanTeamOptions(0,false)==1);
    // A refuse setting overrides an inconsistent accept bit.
    assert(PhongThanTeamOptions(3,true)==2);
    assert(PhongThanTeamOptions(0xff,true)==14);
    assert(PhongThanTeamOptions(8,true)==8);assert(PhongThanTeamOptions(14,true)==14);
    strcpy(Npc[1].Name,"Bob");assert(PhongThanTeamOptions(0,false)==0);
    assert(PhongThanTeamOptions(5,true)==5);
    strcpy(Npc[1].Name,"Alice");assert(PhongThanTeamOptions(0,false)==14);
    assert(saved.size()==2);
    assert(saved["\\userdata\\team_416C696365.ini"]==14);
    Player[1].m_nIndex=0;assert(PhongThanTeamOptions(1,true)==0);
    Player[1].m_nIndex=MAX_NPC;assert(PhongThanTeamOptions(0,false)==0);
    Player[1].m_nIndex=1;Npc[1].Name[0]=0;assert(PhongThanTeamOptions(0,false)==0);
    puts("PASS: production team options, persistence, role isolation, conflict normalization and invalid actor");
    return 0;
}
