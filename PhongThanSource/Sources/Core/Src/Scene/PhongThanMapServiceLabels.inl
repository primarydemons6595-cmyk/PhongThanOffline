// Named services remain visible on mini/Tab maps beyond the entity stream.
static void PhongThanPaintMapServiceLabels(int x,int y,int map,const RECT& cover,int scaleH,int scaleV)
{
    static KTabFile table;
    static bool attempted=false,loaded=false;
    if(!attempted){attempted=true;loaded=table.Load("\\settings\\phongthan\\MapServiceLabels.txt")!=0;}
    if(!loaded || !g_pRepresent || scaleH<=0 || scaleV<=0)return;
    for(int row=2;row<=table.GetHeight() && row<=257;++row){
        int world=0,wx=0,wy=0;char name[64];
        table.GetInteger(row,1,0,&world);
        if(world!=map)continue;
        table.GetInteger(row,2,0,&wx);table.GetInteger(row,3,0,&wy);
        table.GetString(row,4,"",name,sizeof(name));
        if(wx<=0 || wy<=0 || !name[0])continue;
        int mx=wx/scaleH,my=wy/scaleV;
        int width=g_StrLen(name)*NORMAL_FONTSIZE/2;
        if(width>cover.right-cover.left || NORMAL_FONTSIZE>cover.bottom-cover.top)continue;
        if(mx<cover.left || mx>=cover.right || my<cover.top || my>=cover.bottom)continue;
        int px=mx-cover.left-width/2,py=my-cover.top-NORMAL_FONTSIZE-3;
        if(px<0)px=0;
        if(px>cover.right-cover.left-width)px=cover.right-cover.left-width;
        if(py<0)py=0;
        g_pRepresent->OutputText(NORMAL_FONTSIZE,name,KRF_ZERO_END,
            x+px,y+py,0xffffff00,0,0,0xff000000);
    }
}
