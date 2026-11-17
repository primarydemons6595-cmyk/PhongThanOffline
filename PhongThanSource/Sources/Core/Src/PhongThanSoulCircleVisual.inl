#ifndef PHONGTHAN_SOUL_CIRCLE_VISUAL_INL
#define PHONGTHAN_SOUL_CIRCLE_VISUAL_INL
// These four stationary effect templates are complete one-direction SPRs,
// independent of character equipment-part visibility and action selectors.
static bool PhongThanPaintSoulCircle(int tid,int x,int y,unsigned long tick)
{
 if(tid<551||tid>554||!g_pRepresent)return false;
 char path[80];sprintf(path,"\\spr\\npcres\\passerby\\skill%03d.spr",tid-550);
 KImageParam info;memset(&info,0,sizeof(info));
 if(!g_pRepresent->GetImageParam(path,&info,ISI_T_SPR)||info.nNumFrames<=0)return false;
 KRUImage image;memset(&image,0,sizeof(image));image.nType=ISI_T_SPR;
 image.Color.Color_b.a=255;image.bRenderStyle=IMAGE_RENDER_STYLE_ALPHA;
 image.bRenderFlag=RUIMAGE_RENDER_FLAG_REF_SPOT;
 image.nISPosition=IMAGE_IS_POSITION_INIT;strcpy(image.szImage,path);
 image.nFrame=(tick/2)%info.nNumFrames;
 image.oPosition.nX=x;image.oPosition.nY=y;image.oPosition.nZ=0;
 g_pRepresent->DrawPrimitives(1,&image,RU_T_IMAGE,false);return true;
}
#endif
