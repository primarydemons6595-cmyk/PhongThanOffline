#include <stdio.h>
#include "../../Headers/PhongThanThuongChuPolicy.h"
static int checks=0;
#define CHECK(x) do{++checks;if(!(x)){printf("FAIL %d %s\n",__LINE__,#x);return 1;}}while(0)
int main(){PTThuongChuMatch m;
 for(int mode=1;mode<=3;++mode){m.Open(100,600,1800,mode);CHECK(m.mode==mode&&m.phase==1&&m.fightAt==700&&m.endAt==2500);CHECK(m.World()==(mode==1?1071:1064));CHECK(!m.Begin(699,5,5));CHECK(m.Begin(700,5,5)==1);CHECK(!m.Admit(1,5,5));}
 m.Open(100,600,1800,3);CHECK(m.Admit(1,9,0));CHECK(!m.Admit(1,10,0));CHECK(!m.Admit(2,100,100));CHECK(!m.Admit(3,0,0));CHECK(m.Begin(700,4,5)==-1&&m.winner==0);
 m.Open(100,600,1800,3);CHECK(m.Begin(700,5,5)==1);CHECK(m.DeliverFlag(1,false,701)&&m.flags[1]==0);CHECK(m.DeliverFlag(1,true,702)&&m.phase==2);CHECK(m.DeliverFlag(1,true,703)&&m.phase==2);CHECK(m.DeliverFlag(1,true,704)&&m.phase==3&&m.winner==1);CHECK(!m.DeliverFlag(2,true,705));
 m.Open(100,600,1800,1);CHECK(m.Begin(700,5,5)==1);CHECK(m.Capture(0,1,true,1300));CHECK(!m.Capture(0,1,false,1301));CHECK(m.Capture(1,1,true,1302));CHECK(m.Capture(2,1,true,1303)&&m.phase==3&&m.winner==1);
 CHECK(m.Points(990,100)==1090&&m.Points(0,-1)==0&&m.Points(1999999999,100)==2000000000);
 CHECK(m.QuestGoal(1)==10&&m.QuestGoal(2)==10&&m.QuestGoal(3)==10&&m.QuestGoal(4)==3&&m.QuestGoal(0)==0);
 CHECK(m.QuestCopper(0)==0&&m.QuestCopper(2)==0&&m.QuestCopper(3)==3&&m.QuestCopper(9)==3);CHECK(m.CanTakeQuest(9)&&!m.CanTakeQuest(10));
 printf("PASS %d TGPT session mode, minimum five, commander flag victory, quest limits and currency policy checks\n",checks);return 0;
}
