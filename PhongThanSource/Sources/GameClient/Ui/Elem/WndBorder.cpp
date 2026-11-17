#include "KWin32.h"
#include "../../Represent/iRepresent/iRepresentShell.h"
#include "WndBorder.h"

extern iRepresentShell* g_pRepresentShell;

// Inventory quality is a fixed one-pixel color, independent of timers and
// animation state. Keep the existing call signature for object containers.
void DrawBorder(unsigned char& frame,unsigned char& delay,int& position,
    unsigned char& direction,unsigned char& lightDelay,
    int x,int y,int width,int height,int nature)
{
    static const unsigned int colors[]={0,0xff00ff00,0xffffd700,
        0xffffa020,0xffbd28ff,0xff6495ff};
    if(!g_pRepresentShell || nature<=0 || nature>=sizeof(colors)/sizeof(colors[0]) || width<2 || height<2)return;
    KRURect border;ZeroMemory(&border,sizeof(border));
    border.Color.Color_dw=colors[nature];
    border.oPosition.nX=x;border.oPosition.nY=y;
    border.oEndPos.nX=x+width-1;border.oEndPos.nY=y+height-1;
    g_pRepresentShell->DrawPrimitives(1,&border,RU_T_RECT,TRUE);
}
