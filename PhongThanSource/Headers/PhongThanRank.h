#ifndef PHONGTHAN_RANK_H
#define PHONGTHAN_RANK_H
#include <string.h>
#include "PhongThanRanking.h"
typedef PTRankingRow PTRankEntry;
inline int PTRankCompare(const void* left,const void* right) {
    const PTRankEntry& a=*(const PTRankEntry*)left;const PTRankEntry& b=*(const PTRankEntry*)right;
    if(a.Transcendence!=b.Transcendence)return a.Transcendence>b.Transcendence?-1:1;
    if(a.Level!=b.Level)return a.Level>b.Level?-1:1;
    if(a.Experience!=b.Experience)return a.Experience>b.Experience?-1:1;
    return _stricmp(a.Name,b.Name);
}
#endif
