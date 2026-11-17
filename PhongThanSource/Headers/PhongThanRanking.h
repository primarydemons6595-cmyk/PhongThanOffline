#ifndef PHONGTHAN_RANKING_H
#define PHONGTHAN_RANKING_H
#include "PhongThanWorldProtocol.h"
#include <string.h>
enum {PT_RANK_QUERY=0x4122,PT_RANK_REPLY=0x4022,PT_RANK_GET=0x50545247,PT_RANK_ACTION=0x50545241,PT_RANK_ROWS=20};
#pragma pack(push,1)
struct PTRankingRow {
 char Name[32],Nation[32];int Level,Profession,Transcendence,Experience,Repute,Pk,Fortune,Honor,Power,Justice;
 __int64 Money;
};
struct PTRankingQuery {PHONGTHAN_WIRE_HEADER Header;unsigned int ActorId,MapId;int Category,Page;};
struct PTRankingPage {PHONGTHAN_WIRE_HEADER Header;unsigned int ActorId,MapId;int Category,Page,Count,Total,Result;PTRankingRow Rows[PT_RANK_ROWS];};
#pragma pack(pop)
inline bool PTRankingValidQuery(int category,int page){return category>=0&&category<45&&page>=0&&page<2500;}
inline bool PTRankingValidPage(const PTRankingPage& p){
 if(!PTRankingValidQuery(p.Category,p.Page)||p.Count<0||p.Count>PT_RANK_ROWS||p.Total<0||p.Total>50000||p.Count>p.Total)return false;
 for(int i=0;i<p.Count;++i)if(!memchr(p.Rows[i].Name,0,32)||!memchr(p.Rows[i].Nation,0,32))return false;
 return true;
}
#ifdef _SERVER
bool PhongThanRankingPage(int category,int page,PTRankingPage& result);
bool PhongThanNationWealthRank(PTRankingPage* result);
#else
int PhongThanRankingGet(PTRankingPage* result);
int PhongThanRankingQuery(const PTRankingQuery* query);
#endif
#endif
