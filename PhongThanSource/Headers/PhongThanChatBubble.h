#ifndef PHONGTHAN_CHAT_BUBBLE_H
#define PHONGTHAN_CHAT_BUBBLE_H
// Original PC nine-slice art from ui/head-icons.ini [ChatImage].
// Provenance: analysis/status_pet_20261023/compose_followup/resource_provenance.json.
inline void PhongThanChatPiece(iRepresentShell* rep,const char* part,int x,int y,int w,int h) {
    KRUImage image;ZeroMemory(&image,sizeof(image));
    image.nType=ISI_T_SPR;image.Color.Color_b.a=255;
    image.bRenderStyle=IMAGE_RENDER_STYLE_ALPHA;image.nISPosition=IMAGE_IS_POSITION_INIT;
    sprintf(image.szImage,"\\spr\\pt_chat\\bubble_%s.spr",part);
    // These are native 1-pixel edge tiles, not pictures to aspect-fit.
    // RU_T_IMAGE_STRETCH in Represent2 fits a sprite while preserving aspect
    // ratio; a 1x11 top tile consequently stays only one pixel wide.
    // Tile the exact original pixels through the normal sprite path instead.
    int tileW=w,tileH=h;
    if(!strcmp(part,"imagetop") || !strcmp(part,"imagebottom"))tileW=1;
    if(!strcmp(part,"imageleft") || !strcmp(part,"imageright"))tileH=1;
    KRUImage tiles[256];int count=0;
    for(int yy=0;yy<h;yy+=tileH)for(int xx=0;xx<w;xx+=tileW){
        tiles[count]=image;tiles[count].oPosition.nX=x+xx;tiles[count].oPosition.nY=y+yy;
        if(++count==256){rep->DrawPrimitives(count,tiles,RU_T_IMAGE,TRUE);count=0;}
    }
    if(count)rep->DrawPrimitives(count,tiles,RU_T_IMAGE,TRUE);
}
inline void PhongThanDrawChatBubble(iRepresentShell* rep,int center,int nameTop,
    const char* text,int bytes,int columns,int lines) {
    if(!rep || !text || bytes<=0 || columns<=0 || lines<=0)return;
    const int font=14;
    if(lines>2)lines=2;
    int textWidth=columns*font/2;if(textWidth<14)textWidth=14;
    const int w=textWidth+22,h=lines*(font+1)+20;
    const int left=center-w/2,bottom=nameTop-26,top=bottom-h;
    KRUShadow fill;ZeroMemory(&fill,sizeof(fill));
    fill.oPosition.nX=left+7;fill.oPosition.nY=top+7;
    fill.oEndPos.nX=left+w-7;fill.oEndPos.nY=bottom-7;
    // RU_T_SHADOW uses inverse 0..32 alpha, already converted by UI callers.
    fill.Color.Color_dw=0x0a000000;rep->DrawPrimitives(1,&fill,RU_T_SHADOW,TRUE);
    PhongThanChatPiece(rep,"imagelt",left,top,11,11);
    PhongThanChatPiece(rep,"imagetop",left+11,top,w-22,11);
    PhongThanChatPiece(rep,"imagert",left+w-11,top,11,11);
    PhongThanChatPiece(rep,"imageleft",left,top+11,11,h-22);
    PhongThanChatPiece(rep,"imageright",left+w-11,top+11,11,h-22);
    PhongThanChatPiece(rep,"imagelb",left,bottom-11,11,11);
    PhongThanChatPiece(rep,"imagebottom",left+11,bottom-11,w-22,11);
    PhongThanChatPiece(rep,"imagerb",left+w-11,bottom-11,11,11);
    PhongThanChatPiece(rep,"imagetail",center-14,bottom-10,28,26);
    KOutputTextParam param;ZeroMemory(&param,sizeof(param));
    param.nX=left+11;param.nY=top+10;param.nZ=TEXT_IN_SINGLE_PLANE_COORD;
    param.Color=0xffd6d6d6;param.BorderColor=0xff000000;param.nNumLine=lines;
    param.bPicPackInSingleLine=true;
    rep->OutputRichText(font,&param,text,bytes,textWidth);
}
#endif
