#ifndef PHONGTHAN_SKILL_BOOK_KEY_H
#define PHONGTHAN_SKILL_BOOK_KEY_H
// Goods and older NPC Lua retain six previous skill IDs. The winning original
// item/001/skillbook.txt keeps the same book details but moves these skill IDs.
// Do not reinterpret any other detail/skill pair.
inline int PhongThanCurrentSkillBookKey(int detail,int skill)
{
    const int aliases[6][3]={{84,1559,1696},{85,1560,1701},{86,1561,1706},
        {92,1567,1736},{93,1568,1741},{94,1569,1746}};
    for(int i=0;i<6;++i)if(detail==aliases[i][0] && skill==aliases[i][1])return aliases[i][2];
    return skill;
}
#endif
