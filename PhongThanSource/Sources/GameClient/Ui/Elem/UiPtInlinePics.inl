// PT FaceDetail supplies the actual SPR paths, not legacy numbered Ui3 paths.
// Compatibility IDs reuse the selector's existing one-based system-ID contract.
BOOL KInlinePicSink::Init(iRepresentShell *pShell)
{
    m_pRepShell=pShell;
    if (!pShell) return FALSE;
    KIniFile ini;
    if (!ini.Load("\\Ui\\FaceDetail.ini") && !ini.LoadPakEntry(0x02A39B43UL)) return FALSE;
    int count=0;
    ini.GetInteger("List","Count",0,&count);
    if (count<=0 || count>=MAX_SYSTEM_INLINE_PICTURES) return FALSE;
    m_Images.clear(); m_ImageParams.clear();
    for (int i=0;i<=count;++i) {
        KUiImageRef image; IR_InitUiImageRef(image);
        KImageParam param; ZeroMemory(&param,sizeof(param));
        image.nType=ISI_T_SPR; image.nISPosition=IMAGE_IS_POSITION_INIT;
        image.bRenderStyle=IMAGE_RENDER_STYLE_ALPHA;
        // Inline coordinates describe the top-left, not a world SPR reference spot.
        image.bRenderFlag=0;
        if (i) {
            char section[24]; sprintf(section,"Face%d",i);
            ini.GetString(section,"Spr","",image.szImage,sizeof(image.szImage));
            if (!image.szImage[0] || !pShell->GetImageParam(image.szImage,&param,ISI_T_SPR) ||
                param.nWidth<=0 || param.nHeight<=0 || param.nNumFrames<=0) {
                image.szImage[0]=0; ZeroMemory(&param,sizeof(param));
            } else {
                image.nNumFrames=param.nNumFrames; image.nInterval=param.nInterval;
                image.nFlipTime=IR_GetCurrentTime();
            }
        }
        m_Images.push_back(image); m_ImageParams.push_back(param);
    }
    if (FAILED(pShell->AdviseRepresent(this))) return FALSE;
    if (FAILED(AdviseEngine(this))) {pShell->UnAdviseRepresent(this);return FALSE;}
    return TRUE;
}
