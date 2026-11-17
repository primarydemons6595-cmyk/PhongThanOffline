#ifndef PT_PK_PUNISHMENT_H
#define PT_PK_PUNISHMENT_H
namespace PtPkPunishment {
enum {Island=1072,Sentence=4762,Minutes=4763,Seconds=4764};
inline int Raw(int points){static const int upper[]={0,10,33,87,215,517,1230,3000,4000,5000,6000};return upper[points<0?0:points>10?10:points];}
inline int Points(int minutes){if(minutes<=0)return 0;for(int p=1;p<=10;++p)if(minutes<=Raw(p))return p;return 10;}
inline bool MustStay(int map,int points,int sentenced){return map==Island&&points>0&&(points>6||sentenced!=0);}
inline bool DeathDrops(int camp,int mode,int npcMode,int sparMode,int noPunish,int battleMode,int tournamentMode){return camp==8&&mode!=sparMode&&mode!=noPunish&&mode!=battleMode&&mode!=tournamentMode;}
}
#endif
