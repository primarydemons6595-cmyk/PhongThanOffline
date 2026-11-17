#include <time.h>
static time_t fixtureTime=0;
static time_t FixtureTime(time_t* out){if(out)*out=fixtureTime;return fixtureTime;}
#define time FixtureTime
#define main ExistingShopTests
#include "CustomEventShopSnapshotTests.cpp"
#undef main
#undef time
int main(){int checks=0;using namespace PtCustomEvents;
 PTCustomEventSchedule schedule;schedule.startDay=20261230;schedule.startMinute=510;schedule.endDay=20270103;schedule.endMinute=1320;schedule.yearly=true;
 CHECK(schedule.Valid());CHECK(!schedule.Active(20261230,509));CHECK(schedule.Active(20261230,510));CHECK(schedule.Active(20270103,1319));CHECK(!schedule.Active(20270103,1320));CHECK(schedule.Active(20280101,0));CHECK(!schedule.Active(20260601,0));CHECK(!schedule.Active(20260101,0));CHECK(!schedule.Active(20280601,0));
 schedule.yearly=false;CHECK(!schedule.Active(20280101,0));schedule.endDay=20261229;CHECK(!schedule.Valid());schedule.endDay=20270229;CHECK(!schedule.Valid());schedule.endDay=20280229;CHECK(schedule.Valid());
 schedule.yearly=true;CHECK(!schedule.Valid());schedule.startDay=20261230;schedule.endDay=20271230;schedule.endMinute=510;CHECK(!schedule.Valid());
 // UTC 2026-11-03 00:59 / 01:00 / 01:01 = Vietnam 07:59 / 08:00 / 08:01.
 fixtureTime=1793667540;
 fixtureSnapshot="PT_CUSTOM_EVENTS\t1\t1\nE\t1\t1\t1\t54657374\t20261103\t480\t20261103\t481\t0\nN\t1\t1\t261\t1\t100\t100\t1\t1\t54657374\tscript\\phongthan\\custom_events\\e1\\v1\\npc1.lua\nE\t2\t1\t1\t4D616E75616C\nN\t2\t1\t261\t1\t200\t200\t1\t1\t54657374\tscript\\phongthan\\custom_events\\e2\\v1\\npc1.lua\n";
 PhongThanCustomEventsTick();CHECK(revision==1);CHECK(!events[1].enabled&&events[2].enabled);CHECK(spawns.size()==1&&fixtureSpawned==1);
 fixtureTime+=60;PhongThanCustomEventsTick();CHECK(events[1].enabled);CHECK(spawns.size()==2&&fixtureSpawned==2);
 ++fixtureTime;PhongThanCustomEventsTick();CHECK(spawns.size()==2&&fixtureSpawned==2);
 fixtureTime+=59;PhongThanCustomEventsTick();CHECK(!events[1].enabled);CHECK(spawns.size()==1&&fixtureRemoved==1&&events[2].enabled);
 // A world restart inside the window recomputes state from the saved snapshot.
 fixtureTime-=30;for(unsigned int i=0;i<spawns.size();++i)Remove(spawns[i]);spawns.clear();events.clear();revision=0;next=0;PhongThanCustomEventsTick();CHECK(events[1].enabled&&spawns.size()==2);
 // Bad schedules fail atomically without replacing the active configuration.
 CHECK(!Load("PT_CUSTOM_EVENTS\t1\t2\nE\t1\t1\t1\t54657374\t20260230\t0\t20260301\t0\t0\n"));CHECK(revision==1&&events[1].enabled);
 CHECK(Load("PT_CUSTOM_EVENTS\t1\t2\nE\t1\t0\t1\t54657374\t20261103\t480\t20261103\t481\t0\n"));CHECK(!events[1].enabled&&spawns.empty());
 printf("PASS CUSTOM_EVENT_SCHEDULE checks=%d production_parser_and_tick=1\n",checks);return 0;
}
