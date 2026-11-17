// Persist each logical slot's frame state. Filled slots keep the animated
// original hole below their separate gem image.
static void PaintSocketTooltipLayer(KUiImageRef& image,const char* path,int x,int y){
 if(!path||!path[0]||!g_pRepresentShell)return;
 if(strcmp(image.szImage,path)){
  IR_InitUiImageRef(image);g_StrCpyLen(image.szImage,path,sizeof(image.szImage));
  image.nType=ISI_T_SPR;image.nISPosition=IMAGE_IS_POSITION_INIT;
  image.bRenderStyle=IMAGE_RENDER_STYLE_ALPHA;
 }
 IR_NextFrame(image);
 KRPosition2 offset,size;
 if(!g_pRepresentShell->GetImageFrameParam(image.szImage,image.nFrame,&offset,&size,ISI_T_SPR)||size.nX<=0||size.nY<=0)return;
 // Hole canvases include their glow (20/25px); center both layers on the
 // authored socket instead of assuming every canvas is exactly16px wide.
 image.bRenderFlag=RUIMAGE_RENDER_FLAG_FRAME_DRAW;
 image.oPosition.nX=x-size.nX/2;image.oPosition.nY=y-size.nY/2;
 g_pRepresentShell->DrawPrimitives(1,&image,RU_T_IMAGE,true);
}
static void PaintSocketTooltipLayers(const PT_SOCKET_VISUALS& visuals,KUiImageRef holes[4],KUiImageRef gems[4],int x,int y,int width,int height){
 for(int at=0;at<visuals.Count&&at<4;++at){
  int slot=visuals.Slot[at];if(slot<0||slot>=4)continue;
  int cx=x+visuals.X[at]*width/64,cy=y+visuals.Y[at]*height/96;
  PaintSocketTooltipLayer(holes[slot],visuals.HoleImage[at],cx,cy);
  if(strcmp(visuals.Image[at],visuals.HoleImage[at]))
   PaintSocketTooltipLayer(gems[slot],visuals.Image[at],cx,cy);
 }
}
