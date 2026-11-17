#ifndef PHONGTHAN_SKILL_LEARNING_H
#define PHONGTHAN_SKILL_LEARNING_H
#include <stdlib.h>
#include <errno.h>

// Exact native SkillParent grammar: count:id&learnedLevel,... . Bonus levels
// from equipment must not satisfy a prerequisite for learning a new skill.
template<class Skills> inline bool PhongThanSkillParentsAllow(const char* text,
    Skills& skills, int* missing = 0, int* required = 0)
{
    if(missing)*missing=0;if(required)*required=0;
    if(!text||!*text)return false;
    char* end;errno=0;long count=strtol(text,&end,10);
    if(errno||end==text||*end!=':'||count<0||count>16)return false;
    const char* cursor=end+1;bool allow=true;
    for(int i=0;i<count;++i){
        const char* start=cursor;errno=0;long id=strtol(start,&end,10);
        if(errno||end==start||id<=0||id>65535||*end!='&')return false;
        start=end+1;errno=0;long level=strtol(start,&end,10);
        if(errno||end==start||level<1||level>100)return false;
        if(skills.GetLevel((int)id)<level){allow=false;if(missing&&!*missing)*missing=(int)id;if(required&&!*required)*required=(int)level;}
        if(i+1<count){if(*end!=',')return false;cursor=end+1;}else cursor=end;
    }
    return !*cursor&&allow;
}

// Native playerskills.ini: Knight=1, Enchanter=2, Monstrous=135.
inline int PhongThanDefaultPhysicalSkill(int profession)
{
    return profession == 3 ? 1691 : (profession == 1 ? 2 : (profession == 2 ? 135 : 1));
}

// Original item/001/skillbook.txt rows 1..58: detail, skill ID and the
// character level printed in each book's native description. This is not
// derived from price, item level, icon or the character's current skill list.
inline int PhongThanSkillBookLevel(int detail, int skill)
{
    static const int levels[] = {
        4,6,8,10,14,18,22,26,30,34,38,42,46,50,54,58,62,66,70,74,78,82,86,90,
        6,12,18,24,30,36,42,42,48,54,60,66,72,78,84,90,
        10,20,30,40,50,60,70,80,90,
        5,15,25,35,45,55,65,75,85
    };
    if (detail < 0 || detail >= sizeof(levels)/sizeof(levels[0])) return 0;
    int expected = detail < 49 ? detail + 3 : 450 + detail - 49;
    return skill == expected ? levels[detail] : 0;
}

inline int PhongThanSkillBookProfession(int skill)
{
    if (skill >= 3 && skill <= 26) return 1;
    if (skill >= 27 && skill <= 42) return 0;
    if ((skill >= 43 && skill <= 51) || (skill >= 450 && skill <= 458)) return 2;
    return -1;
}

inline bool PhongThanCanLearnBook(int detail, int skill, int profession,
    int characterLevel, int learnedLevel)
{
    int required = PhongThanSkillBookLevel(detail, skill);
    return required > 0 && learnedLevel <= 0 && characterLevel >= required &&
        profession == PhongThanSkillBookProfession(skill);
}
#endif
