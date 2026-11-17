#ifndef PHONGTHAN_AVATAR_LAYOUT_H
#define PHONGTHAN_AVATAR_LAYOUT_H
#include <string.h>
// Coordinates measured against the original three avatar sprites. Image2 has
// three tracks; Image3 has four. The ordinary sprite/coordinates stay intact.
struct PTAvatarBarLayout {int X,Y;bool Visible;};
inline PTAvatarBarLayout PhongThanAvatarBarLayout(int skin,const char* section,int originalX,int originalY){
 PTAvatarBarLayout p={originalX,originalY,true};
 if(!strcmp(section,"NewExp")){p.Visible=false;return p;}
 if(skin<=0){if(!strcmp(section,"JEExp"))p.Visible=false;return p;}
 p.X=80;
 if(!strcmp(section,"Life"))p.Y=28;
 else if(!strcmp(section,"Mana"))p.Y=38;
 else if(!strcmp(section,"Exp")){p.Y=48;p.Visible=skin==2;}
 else if(!strcmp(section,"JEExp"))p.Y=skin==2?58:48;
 else {p.X=originalX;p.Y=originalY;}
 return p;
}
#endif
