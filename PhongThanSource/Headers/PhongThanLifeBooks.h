#ifndef PHONGTHAN_LIFE_BOOKS_H
#define PHONGTHAN_LIFE_BOOKS_H
struct PT_LIFE_RECIPE_BOOK {int detail,recipe,skill,level;};
static const PT_LIFE_RECIPE_BOOK PT_LIFE_BOOKS[]={
{693,101,5,8},
{703,111,7,6},
{696,104,5,10},
{702,110,6,10},
{701,109,6,10},
{699,107,6,10},
{700,108,6,10},
{698,106,5,10},
{692,100,5,7},
{695,103,5,10},
{720,124,5,6},
{721,125,5,6},
{719,123,5,6},
{722,126,5,6},
{697,105,5,10},
{694,102,5,10},
 };
inline const PT_LIFE_RECIPE_BOOK* PhongThanLifeRecipeBook(int detail){for(int i=0;i<sizeof(PT_LIFE_BOOKS)/sizeof(PT_LIFE_BOOKS[0]);++i)if(PT_LIFE_BOOKS[i].detail==detail)return &PT_LIFE_BOOKS[i];return 0;}
#endif
