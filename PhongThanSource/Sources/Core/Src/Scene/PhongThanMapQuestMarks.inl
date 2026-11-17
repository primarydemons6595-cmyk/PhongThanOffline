// The scene map member m_nMapId is never assigned during world loading.
// Always pass the active canonical SubWorld ID; both mini and Tab maps use this painter.
static void PhongThanPaintMapQuestMarks(int nX,int nY,int map,const RECT& cover,int scaleH,int scaleV)
{
    // Authoritative per-player task markers use world coordinates, so NPCs
    // outside the local entity stream still appear on the Tab map.
    for(int mark=0;mark<PT_NPC_QUEST_MARK_MAX;++mark){
        int wx=0,wy=0;int variant=PhongThanGetMapQuestMark(mark,map,wx,wy);
        if(variant<0 || variant>=5)continue; // Available, accepted/in progress, and ready to turn in.
        int mx=wx/scaleH,my=wy/scaleV;
        if(mx<cover.left+8 || mx>=cover.right-8 ||
           my<cover.top+8 || my>=cover.bottom-8)continue;
        KRUImage book;memset(&book,0,sizeof(book));book.nType=ISI_T_SPR;
        book.Color.Color_b.a=255;book.bRenderStyle=IMAGE_RENDER_STYLE_ALPHA;
        book.nISPosition=IMAGE_IS_POSITION_INIT;strcpy(book.szImage,s_PTQuestMarkSprites[variant]);
        book.oPosition.nX=nX+mx-cover.left-8;
        book.oPosition.nY=nY+my-cover.top-8;
        // Original book canvas is 32x32. Use the native alpha SPR scaler at 50%.
        book.oEndPos.nX=book.oPosition.nX+16;book.oEndPos.nY=book.oPosition.nY+16;
        g_pRepresent->DrawPrimitives(1,&book,RU_T_IMAGE_STRETCH,true);
    }
}
