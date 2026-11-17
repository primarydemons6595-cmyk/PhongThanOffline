#include "../../Headers/PhongThanSkillLearning.h"
#include <stdio.h>
#include <stdlib.h>
struct Skills {int values[65536]; Skills(){for(int i=0;i<65536;i++)values[i]=0;} int GetLevel(int id){return values[id];}};
static int checks=0;
void check(bool ok){++checks;if(!ok){printf("FAIL check=%d\n",checks);exit(1);}}
struct Row {int id;const char* rule;int level;};
Row rows[]={
{3,"0:",4},
{4,"0:",6},
{5,"0:",8},
{6,"0:",10},
{7,"0:",14},
{8,"1:4&4",18},
{9,"0:",22},
{10,"1:6&4",26},
{11,"1:3&4",30},
{12,"0:",34},
{13,"1:5&4",38},
{14,"0:",42},
{15,"1:11&4",46},
{16,"1:8&4",50},
{17,"0:",54},
{18,"1:10&4",58},
{19,"0:",62},
{20,"1:16&4",66},
{21,"1:13&4",70},
{22,"0:",74},
{23,"1:15&4",78},
{24,"1:20&4",82},
{25,"1:21&4",86},
{26,"1:18&4",90},
{27,"0:",6},
{28,"0:",12},
{29,"0:",18},
{30,"1:27&4",24},
{31,"1:29&4",30},
{32,"1:30&4",36},
{33,"0:",42},
{34,"0:",42},
{35,"1:31&4",48},
{36,"1:32&4",54},
{37,"1:35&4",60},
{38,"1:36&4",66},
{39,"1:37&4",72},
{40,"0:",78},
{41,"1:38&4",84},
{42,"1:39&4",90},
{43,"0:",10},
{44,"0:",20},
{45,"0:",30},
{46,"0:",40},
{47,"0:",50},
{48,"0:",60},
{49,"0:",70},
{50,"0:",80},
{51,"0:",90},
{450,"0:",5},
{451,"1:450&4",15},
{452,"1:451&4",25},
{453,"1:452&4",35},
{454,"1:453&4",45},
{455,"1:454&4",55},
{456,"1:455&4",65},
{457,"1:456&4",75},
{458,"1:457&4",85},
};
int main(){Skills s;int edges=0;
for(int r=0;r<sizeof(rows)/sizeof(rows[0]);++r){
 const char* rule=rows[r].rule;int n=atoi(rule);
 check(PhongThanSkillParentsAllow(rule,s)==(n==0));
 if(n){int id=0,lvl=0;check(sscanf(rule,"1:%d&%d",&id,&lvl)==2);++edges;
 s.values[id]=lvl-1;check(!PhongThanSkillParentsAllow(rule,s));
 s.values[id]=lvl;check(PhongThanSkillParentsAllow(rule,s));
 s.values[id]=lvl+1;check(PhongThanSkillParentsAllow(rule,s));s.values[id]=0;
 }}
check(!PhongThanSkillParentsAllow("",s));check(!PhongThanSkillParentsAllow("1:27&0",s));
check(!PhongThanSkillParentsAllow("1:27&4,trailing",s));check(!PhongThanSkillParentsAllow("0:27&4",s));
s.values[27]=4;s.values[29]=3;check(!PhongThanSkillParentsAllow("2:27&4,29&4",s));
s.values[29]=4;check(PhongThanSkillParentsAllow("2:27&4,29&4",s));
check(!PhongThanSkillParentsAllow("2:27&4",s));check(!PhongThanSkillParentsAllow("1:9999999999999999999&4",s));
check(PhongThanCanLearnBook(24,27,0,10,0));check(!PhongThanCanLearnBook(24,27,0,5,0));
check(!PhongThanCanLearnBook(24,27,1,10,0));check(!PhongThanCanLearnBook(24,27,0,10,1));
printf("PASS checks=%d original_skill_rows=%d parent_edges=%d; original PAK rules, below/exact/above boundaries, multi-parent, malformed, book eligibility\n",checks,sizeof(rows)/sizeof(rows[0]),edges);return 0;}
