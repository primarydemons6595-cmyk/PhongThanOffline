#pragma once
#include "../Elem/WndImage.h"
#include <map>
#include <string>
#include "../../../Represent/iRepresent/iRepresentShell.h"
extern iRepresentShell* g_pRepresentShell;
// NPC SPR canvas/anchor coordinates belong to world rendering. A preview
// instead centers the visible frame inside its UI slot.
class KPtPetPreview:public KWndImage {
public:
 int PetHue,PetTransparency;KPtPetPreview():PetHue(0),PetTransparency(0){}
 void PaintWindow(){
  KWndWindow::PaintWindow();if(!g_pRepresentShell||!m_Image.szImage[0])return;
  KImageParam param;memset(&param,0,sizeof(param));
  if(g_pRepresentShell->GetImageParam(m_Image.szImage,&param,m_Image.nType)&&param.nNumFrames>1){
   int groups=param.nNumFramesGroup;
   if(groups<1||groups>param.nNumFrames||param.nNumFrames%groups)groups=1;
   int frames=param.nNumFrames/groups;
   int interval=param.nInterval>0?param.nInterval:120;
   m_Image.nFrame=(GetTickCount()/interval)%frames;
  }
  KRPosition2 offset,size;
  if(!g_pRepresentShell->GetImageFrameParam(m_Image.szImage,m_Image.nFrame,&offset,&size,m_Image.nType)||size.nX<=0||size.nY<=0)return;
  KRUImage image;memset(&image,0,sizeof(image));memcpy(&image,&m_Image,sizeof(image));
  image.bRenderFlag=RUIMAGE_RENDER_FLAG_FRAME_DRAW;
  image.bRenderStyle=IMAGE_RENDER_STYLE_ALPHA_PALETTE_HUE;image.Color.Color_dw=((unsigned int)(255-PetTransparency)<<24)|PetHue;
  // Use one envelope for the standing animation. Re-centering each cropped
  // frame makes the pet's body jump when wings/tails extend.
  static std::map<std::string,RECT> envelopes;
  std::map<std::string,RECT>::iterator found=envelopes.find(m_Image.szImage);
  if(found==envelopes.end()){
   RECT envelope;envelope.left=offset.nX;envelope.top=offset.nY;
   envelope.right=offset.nX+size.nX;envelope.bottom=offset.nY+size.nY;
   int groups=param.nNumFramesGroup;
   if(groups<1||groups>param.nNumFrames||param.nNumFrames%groups)groups=1;
   int frames=param.nNumFrames>0?param.nNumFrames/groups:1;
   for(int frame=0;frame<frames;++frame){KRPosition2 fo,fs;
    if(g_pRepresentShell->GetImageFrameParam(m_Image.szImage,frame,&fo,&fs,m_Image.nType)&&fs.nX>0&&fs.nY>0){
     if(fo.nX<envelope.left)envelope.left=fo.nX;if(fo.nY<envelope.top)envelope.top=fo.nY;
     if(fo.nX+fs.nX>envelope.right)envelope.right=fo.nX+fs.nX;
     if(fo.nY+fs.nY>envelope.bottom)envelope.bottom=fo.nY+fs.nY;
    }
   }
   envelopes[m_Image.szImage]=envelope;found=envelopes.find(m_Image.szImage);
  }
  const RECT& envelope=found->second;int width=envelope.right-envelope.left,height=envelope.bottom-envelope.top;
  if(width<=0||height<=0)return;
  int fitWidth=width,fitHeight=height;
  if(fitWidth>m_Width){fitHeight=fitHeight*m_Width/fitWidth;fitWidth=m_Width;}
  if(fitHeight>m_Height){fitWidth=fitWidth*m_Height/fitHeight;fitHeight=m_Height;}
  int x=m_nAbsoluteLeft+(m_Width-fitWidth)/2,y=m_nAbsoluteTop+(m_Height-fitHeight)/2;
  image.oPosition.nX=x+(offset.nX-envelope.left)*fitWidth/width;
  image.oPosition.nY=y+(offset.nY-envelope.top)*fitHeight/height;
  if(fitWidth==width&&fitHeight==height)g_pRepresentShell->DrawPrimitives(1,&image,RU_T_IMAGE,true);
  else{
   KRUImageStretch fit=image;
   fit.oEndPos.nX=x+(offset.nX-envelope.left+size.nX)*fitWidth/width;
   fit.oEndPos.nY=y+(offset.nY-envelope.top+size.nY)*fitHeight/height;
   g_pRepresentShell->DrawPrimitives(1,&fit,RU_T_IMAGE_STRETCH,true);
  }
 }
};
