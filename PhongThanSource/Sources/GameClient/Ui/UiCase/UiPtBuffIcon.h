#pragma once
#include "../Elem/WndButton.h"
#include "../../../Represent/iRepresent/iRepresentShell.h"
extern iRepresentShell* g_pRepresentShell;
// Only the skill bank uses the original 16-pixel portrait strip.
class KPtBuffIcon:public KWndButton {
 bool m_Compact;int m_CancelId;char m_StatusTip[1024];int m_StatusTipLength;
 int WndProc(unsigned int msg,unsigned int param,int data);
public:
 KPtBuffIcon():m_Compact(false),m_CancelId(0),m_StatusTipLength(0){m_StatusTip[0]=0;}
 void SetStatusTip(const char* text);
 void SetCancelId(int id){m_CancelId=id;}
 void SetCompact(bool compact){m_Compact=compact;}
 void PaintWindow(){
  if(!m_Compact){KWndButton::PaintWindow();return;}
  KWndWindow::PaintWindow();
  if(!g_pRepresentShell||!m_Image.szImage[0])return;
  KRUImageStretch fit;memset(&fit,0,sizeof(fit));
  memcpy(&fit,&m_Image,sizeof(KRUImage));
  fit.bRenderFlag=RUIMAGE_RENDER_FLAG_FRAME_DRAW;
  fit.oPosition.nX=m_nAbsoluteLeft;fit.oPosition.nY=m_nAbsoluteTop;
  fit.oEndPos.nX=m_nAbsoluteLeft+m_Width;fit.oEndPos.nY=m_nAbsoluteTop+m_Height;
  g_pRepresentShell->DrawPrimitives(1,&fit,RU_T_IMAGE_STRETCH,true);
 }
};
