#ifndef PHONGTHAN_OVERHEAD_DRAW_H
#define PHONGTHAN_OVERHEAD_DRAW_H
#include "PhongThanOverheadRules.h"
// Exact original assets, screen coordinates throughout. Source paths and
// hashes: analysis/monster_ui_20261017/visual_asset_provenance.json.
inline void PhongThanOverheadSprite(iRepresentShell* rep, const char* path,
    int x, int y, int frame)
{
    KRUImage image;
    ZeroMemory(&image, sizeof(image));
    image.nType = ISI_T_SPR;
    image.Color.Color_b.a = 255;
    image.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
    image.nISPosition = IMAGE_IS_POSITION_INIT;
    image.nFrame = (short)frame;
    strcpy(image.szImage, path);
    image.oPosition.nX = x;
    image.oPosition.nY = y;
    rep->DrawPrimitives(1, &image, RU_T_IMAGE, TRUE);
}
// Exact default advertisement style from original ADD210DA.ini [0].
inline void PhongThanDrawStallFrame(iRepresentShell* rep,int centerX,int top,int textWidth){
    const int inner=textWidth+8,left=centerX-inner/2;
    KRUShadow fill;ZeroMemory(&fill,sizeof(fill));fill.Color.Color_dw=0x0a000000;
    fill.oPosition.nX=left;fill.oPosition.nY=top+1;
    fill.oEndPos.nX=left+inner;fill.oEndPos.nY=top+18;
    rep->DrawPrimitives(1,&fill,RU_T_SHADOW,TRUE);
    PhongThanOverheadSprite(rep,"\\spr\\phongthan\\stall\\name_left.spr",left-13,top,0);
    KRUImageStretch mid;ZeroMemory(&mid,sizeof(mid));mid.nType=ISI_T_SPR;
    mid.Color.Color_b.a=255;mid.bRenderStyle=IMAGE_RENDER_STYLE_ALPHA;mid.nISPosition=IMAGE_IS_POSITION_INIT;
    strcpy(mid.szImage,"\\spr\\phongthan\\stall\\name_mid.spr");
    mid.oPosition.nX=left;mid.oPosition.nY=top;mid.oEndPos.nX=left+inner;mid.oEndPos.nY=top+19;
    rep->DrawPrimitives(1,&mid,RU_T_IMAGE_STRETCH,TRUE);
    PhongThanOverheadSprite(rep,"\\spr\\phongthan\\stall\\name_right.spr",left+inner,top,0);
}
// This flag is a screen overlay drawn after every overhead name, never a
// reused weapon/world primitive. Crop bounds keep its bottom above all labels.
inline void PhongThanDrawTeamFlag(iRepresentShell* rep,const char* path,int centerX,int highestTextY){
    if(!rep||!path||!path[0])return;
    KImageParam info;ZeroMemory(&info,sizeof(info));
    if(!rep->GetImageParam(path,&info,ISI_T_SPR)||info.nNumFrames<=0)return;
    int frame=(GetTickCount()/max(1,info.nInterval))%info.nNumFrames;
    KRPosition2 offset,size;
    if(!rep->GetImageFrameParam(path,frame,&offset,&size,ISI_T_SPR)||size.nX<=0||size.nY<=0)return;
    KRUImage flag;ZeroMemory(&flag,sizeof(flag));flag.nType=ISI_T_SPR;
    flag.Color.Color_dw=0xffffffff;flag.bRenderStyle=IMAGE_RENDER_STYLE_ALPHA;
    flag.nISPosition=IMAGE_IS_POSITION_INIT;flag.bRenderFlag=RUIMAGE_RENDER_FLAG_FRAME_DRAW;
    flag.nFrame=frame;lstrcpyn(flag.szImage,path,sizeof(flag.szImage));
    flag.oPosition.nX=centerX-size.nX/2;flag.oPosition.nY=highestTextY-size.nY-4;
    rep->DrawPrimitives(1,&flag,RU_T_IMAGE,TRUE);
}
inline void PhongThanDrawLevel(iRepresentShell* rep, int badgeLeft, int badgeTop,
    int level, int observerLevel)
{
    if (PhongThanShowSkull(level, observerLevel)) {
        PhongThanOverheadSprite(rep, "\\spr\\phongthan\\overhead\\skull.spr",
            badgeLeft + 2, badgeTop + 1, 0);
        return;
    }
    char number[16];
    sprintf(number, "%d", level > 0 ? level : 0);
    int x = PhongThanLevelLeft(badgeLeft, level);
    for (const char* digit = number; *digit; ++digit, x += 7)
        PhongThanOverheadSprite(rep, "\\spr\\phongthan\\overhead\\digits.spr",
            x, badgeTop + 2, *digit - '0');
}
inline void PhongThanDrawLife(iRepresentShell* rep, int centerX, int top,
    int level, int observerLevel, int percent, int type)
{
    type = type == 1 ? 1 : 0;
    const int width = type ? 228 : 78;
    const int lifeWidth = type ? 200 : 50;
    const int lifeHeight = type ? 8 : 4;
    const int left = centerX - width / 2;
    PhongThanOverheadSprite(rep, type ?
        "\\spr\\phongthan\\overhead\\background_1.spr" :
        "\\spr\\phongthan\\overhead\\background_0.spr", left, top, 0);
    if (percent > 0) {
        if (percent > 100) percent = 100;
        KRUImagePart fill;
        ZeroMemory(&fill, sizeof(fill));
        fill.nType = ISI_T_SPR;
        fill.Color.Color_b.a = 255;
        fill.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
        fill.nISPosition = IMAGE_IS_POSITION_INIT;
        fill.nFrame = percent >= 50 ? 0 : (percent >= 25 ? 1 : 2);
        strcpy(fill.szImage, type ? "\\spr\\phongthan\\overhead\\life_1.spr" :
            "\\spr\\phongthan\\overhead\\life_0.spr");
        fill.oPosition.nX = left + 25;
        fill.oPosition.nY = top + (type ? 3 : 5);
        fill.oImgRBPos.nX = lifeWidth * percent / 100;
        fill.oImgRBPos.nY = lifeHeight;
        rep->DrawPrimitives(1, &fill, RU_T_IMAGE_PART, TRUE);
    }
    PhongThanDrawLevel(rep, left, top, level, observerLevel);
}
inline void PhongThanDrawLevelBadge(iRepresentShell* rep, int left, int top,
    int level, int observerLevel)
{
    KRUImagePart badge;
    ZeroMemory(&badge, sizeof(badge));
    badge.nType = ISI_T_SPR;
    badge.Color.Color_b.a = 255;
    badge.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
    badge.nISPosition = IMAGE_IS_POSITION_INIT;
    strcpy(badge.szImage, "\\spr\\phongthan\\overhead\\background_0.spr");
    badge.oPosition.nX = left;
    badge.oPosition.nY = top;
    badge.oImgRBPos.nX = 24;
    badge.oImgRBPos.nY = 14;
    rep->DrawPrimitives(1, &badge, RU_T_IMAGE_PART, TRUE);
    badge.oImgLTPos.nX=77;badge.oImgRBPos.nX=78;
    badge.oPosition.nX=left+23;
    rep->DrawPrimitives(1,&badge,RU_T_IMAGE_PART,TRUE);
    PhongThanDrawLevel(rep, left, top, level, observerLevel);
}
// Original ui4 team member badges include a profession icon and a level field.
inline void PhongThanDrawTeamBadge(iRepresentShell* rep,int left,int top,
    int level,int observerLevel,int profession)
{
    const char* badge=profession==0?"\\spr\\phongthan\\team\\career_jiashi.spr":
        profession==1?"\\spr\\phongthan\\team\\career_daoshi.spr":
        profession==2?"\\spr\\phongthan\\team\\career_yiren.spr":0;
    if(!badge){PhongThanDrawLevelBadge(rep,left,top+2,level,observerLevel);return;}
    PhongThanOverheadSprite(rep,badge,left,top,0);
    PhongThanDrawLevel(rep,left+17,top+2,level,observerLevel);
}
#endif
