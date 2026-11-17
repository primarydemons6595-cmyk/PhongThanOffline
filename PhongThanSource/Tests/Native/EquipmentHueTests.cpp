#include <stdio.h>
#include "../../Sources/Represent/iRepresent/PhongThanPaletteHue.h"

static int Check(int r, int g, int b, int hue, int er, int eg, int eb)
{
    PhongThanRotatePaletteHue(r, g, b, hue);
    if (r != er || g != eg || b != eb)
    {
        printf("FAIL hue=%d actual=%d,%d,%d expected=%d,%d,%d\n",hue,r,g,b,er,eg,eb);
        return 0;
    }
    return 1;
}
int main()
{
    if (!Check(255,0,0,120,0,255,0) || !Check(255,0,0,240,0,0,255) ||
        !Check(255,0,0,60,255,255,0) || !Check(128,128,128,240,128,128,128) ||
        !Check(0,0,0,275,0,0,0) || !Check(255,255,255,120,255,255,255) ||
        !Check(30,60,90,0,30,60,90) || !Check(30,60,90,360,30,60,90)) return 1;
    for (int hue=1; hue<360; ++hue)
    {
        for (int c=0; c<256; ++c)
            if (!Check(c,c,c,hue,c,c,c)) return 1;
        int r=80,g=160,b=240;
        PhongThanRotatePaletteHue(r,g,b,hue);
        if (r<0 || r>255 || g<0 || g>255 || b<0 || b>255) return 2;
        int maximum = r>g?r:g; if(b>maximum)maximum=b;
        int minimum = r<g?r:g; if(b<minimum)minimum=b;
        if (maximum<239 || maximum>240 || minimum<79 || minimum>80) return 3;
    }
    puts("PASS: 8 hue controls, 91904 achromatic checks, 359 range/lightness/saturation checks.");
    return 0;
}
