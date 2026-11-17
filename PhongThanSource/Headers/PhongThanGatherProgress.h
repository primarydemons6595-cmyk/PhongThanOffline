#ifndef PHONGTHAN_GATHER_PROGRESS_H
#define PHONGTHAN_GATHER_PROGRESS_H
// Exact original life-skill SPR artwork. Crop the track, never synthesize pixels.
// Original ProduceStop/Processing_Image: track at (30,39), 235x12.
template<class Renderer>
inline void PhongThanDrawGatherProgress(Renderer* rep,int center,int top,int remaining,const char* caption=0)
{
 if(!rep||remaining<0||remaining>100||(!remaining&&!caption))return;
 int done=100-remaining;
 KRUImagePart image;memset(&image,0,sizeof(image));
 image.nType=ISI_T_SPR;image.nISPosition=IMAGE_IS_POSITION_INIT;
 image.bRenderStyle=IMAGE_RENDER_STYLE_ALPHA;image.Color.Color_b.a=255;
 strcpy(image.szImage,"\\spr\\phongthan\\gather\\background.spr");
 image.oPosition.nX=center-120;image.oPosition.nY=top;
 image.oImgLTPos.nX=28;image.oImgLTPos.nY=36;
 image.oImgRBPos.nX=269;image.oImgRBPos.nY=55;
 rep->DrawPrimitives(1,&image,RU_T_IMAGE_PART,TRUE);
 if(done>0){
  image.uImage=0;image.nISPosition=IMAGE_IS_POSITION_INIT;
  strcpy(image.szImage,"\\spr\\phongthan\\gather\\fill.spr");
  image.oPosition.nX=center-118;image.oPosition.nY=top+3;
  image.oImgLTPos.nX=image.oImgLTPos.nY=0;
  image.oImgRBPos.nX=235*done/100;image.oImgRBPos.nY=12;
  rep->DrawPrimitives(1,&image,RU_T_IMAGE_PART,TRUE);
 }
 if(caption&&!caption[0])return;
 char text[80];sprintf(text,"§ang thu thËp... %d%%",done);
 rep->OutputText(14,text,KRF_ZERO_END,center-7*(int)strlen(text)/2,top-17,0xffffe2a0,0,TEXT_IN_SINGLE_PLANE_COORD,0xff000000);
}
#endif
