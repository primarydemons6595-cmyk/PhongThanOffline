// Original game.dat 006DBD00: damage/heal digit atlases, plus animated
// 0..9/-.spr critical glyphs. Keep native pixels, palette, alpha and offsets.
static bool PhongThanPaintDamageGlyph(const char* path, int frame,
    int x, int y, int height)
{
    KImageParam param; ZeroMemory(&param,sizeof(param));
    if (!g_pRepresent || !g_pRepresent->GetImageParam(path,&param,ISI_T_SPR) ||
        frame<0 || frame>=param.nNumFrames) return false;
    KRUImage image; ZeroMemory(&image,sizeof(image));
    image.nType=ISI_T_SPR; image.nISPosition=IMAGE_IS_POSITION_INIT;
    image.bRenderStyle=IMAGE_RENDER_STYLE_ALPHA;
    image.Color.Color_b.a=255;
    image.oPosition.nX=x; image.oPosition.nY=y; image.oPosition.nZ=height;
    image.nFrame=frame;
    g_StrCpyLen(image.szImage,path,sizeof(image.szImage));
    g_pRepresent->DrawPrimitives(1,&image,RU_T_IMAGE,FALSE);
    return true;
}

static bool PhongThanPaintDamageNumber(const char* text, bool critical,
    int remaining, int x, int y, int height)
{
    if (!text || !*text) return false;
    const bool healing=*text=='-';
    const char* digits=healing?text+1:text;
    if (!strcmp(digits,"2147483647"))
        return PhongThanPaintDamageGlyph("\\spr\\pt_combat\\962ba753.spr",0,x-16,y,height);
    const int frame=critical?(60-remaining)*4/60:0;
    const int advance=critical?16+frame*15/10:16;
    const char* atlas=healing?"\\spr\\blood\\heal.spr":"\\spr\\blood\\damage.spr";
    for (int i=(int)strlen(digits)-1;i>=0;--i) {
        if (digits[i]<'0' || digits[i]>'9') return false;
        x-=advance;
        if (critical) {
            char path[64]; sprintf(path,"\\spr\\blood\\%c.spr",digits[i]);
            if (!PhongThanPaintDamageGlyph(path,frame,x,y,height)) return false;
        } else if (!PhongThanPaintDamageGlyph(atlas,digits[i]-'0',x,y,height)) return false;
    }
    x-=advance;
    return critical?PhongThanPaintDamageGlyph("\\spr\\blood\\-.spr",frame,x,y,height):
        PhongThanPaintDamageGlyph(atlas,10,x,y,height);
}
