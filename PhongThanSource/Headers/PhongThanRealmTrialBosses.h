#ifndef PHONGTHAN_REALM_TRIAL_BOSSES_H
#define PHONGTHAN_REALM_TRIAL_BOSSES_H
// User-approved Loose trial; these are identities, never VNG loot rates.
struct PTRealmTrialBossIdentity {int type,group;unsigned long script;};
static const PTRealmTrialBossIdentity ptRealmTrialBosses[]={
{1982,0,0xC5CFAFD6UL},
{1985,0,0xC5CFAFD6UL},
{1983,1,0xA70604F0UL},
{1986,1,0xA70604F0UL},
{1984,2,0xE4A0D40AUL},
{1987,2,0xE4A0D40AUL},
{2008,3,0x8ED18DC0UL},
{2009,4,0x13B5C7C3UL},
{2010,5,0x8FFEDDC1UL},
{2039,6,0x62D74550UL},
{2040,7,0xA6FCBF32UL},
{2041,8,0x593A5FEBUL},
{2042,9,0x66EB7715UL},
{2043,10,0x2213DE7DUL},
};
inline int PhongThanRealmTrialBossGroup(int type,unsigned long script){for(int i=0;i<sizeof(ptRealmTrialBosses)/sizeof(ptRealmTrialBosses[0]);++i)if(ptRealmTrialBosses[i].type==type&&ptRealmTrialBosses[i].script==script)return ptRealmTrialBosses[i].group;return -1;}
#endif
