#include "PhongThanUiSpriteCache.inl"
#include "../../../Headers/PhongThanEquippedSocketLayout.h"
static void PhongThanPaintSocketSprite(const char* path,int cx,int cy,int x,int y,int width,int height){
 if(!path||!path[0]||!g_pRepresent)return;
 PtUiSpriteEntry* entry=PtUiSprite(path);if(!entry)return;
 const KImageParam& info=entry->Info;
 KRUImagePart image;ZeroMemory(&image,sizeof(image));image.nType=ISI_T_SPR;image.nISPosition=IMAGE_IS_POSITION_INIT;image.bRenderStyle=IMAGE_RENDER_STYLE_ALPHA;image.Color.Color_b.a=255;image.bRenderFlag=RUIMAGE_RENDER_FLAG_FRAME_DRAW;g_StrCpyLen(image.szImage,path,sizeof(image.szImage));
 image.nFrame=(GetTickCount()/(info.nInterval>0?info.nInterval:100))%info.nNumFrames;
 KRPosition2 offset,size;if(!PtUiSpriteFrameInfo(path,image.nFrame,offset,size)||size.nX<=0||size.nY<=0)return;
 // Retain the full sprite canvas and cropped frame offsets through animation.
 int left=cx-info.nWidth/2+offset.nX,top=cy-info.nHeight/2+offset.nY;
 int clipLeft=left>x?left:x,clipTop=top>y?top:y;
 int right=left+size.nX<x+width?left+size.nX:x+width,bottom=top+size.nY<y+height?top+size.nY:y+height;
 if(right<=clipLeft||bottom<=clipTop)return;
 image.oPosition.nX=clipLeft;image.oPosition.nY=clipTop;image.oImgLTPos.nX=clipLeft-left;image.oImgLTPos.nY=clipTop-top;image.oImgRBPos.nX=right-left;image.oImgRBPos.nY=bottom-top;
 g_pRepresent->DrawPrimitives(1,&image,RU_T_IMAGE_PART,TRUE);
}
static void PhongThanPaintEquippedSockets(KItem& item,int x,int y,int width,int height){
 if(item.GetGenre()!=item_equip||!PtSocketParam(item.GetParam()))return;
 PT_SOCKET_STATE state;if(!PtSocketRead(item.GetParam(),state)){PtSocketRequest(item.GetParam());return;}
 PT_SOCKET_VISUALS visuals;if(!PtSocketVisuals(item,visuals))return;
 for(int at=0;at<visuals.Count&&at<4;++at){int cx,cy;PhongThanEquippedSocketCenter(item.GetDetailType(),x,y,width,height,visuals.X[at],visuals.Y[at],cx,cy);
  PhongThanPaintSocketSprite(visuals.HoleImage[at],cx,cy,x,y,width,height);
  if(strcmp(visuals.Image[at],visuals.HoleImage[at]))PhongThanPaintSocketSprite(visuals.Image[at],cx,cy,x,y,width,height);
 }
}
