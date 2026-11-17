int KUiPortraitMask::Init(KIniFile* pIni, const char* pSection) {
    int result = KWndImage::Init(pIni, pSection);
    m_MaskRuns.clear();
    m_MaskedParts.clear();
    m_bMaskConfigured = false;
    m_bPaintLogged = false;
    if (!result)
        return result;

    char maskPath[MAX_PATH];
    pIni->GetString(pSection, "MaskPath", "", maskPath, sizeof(maskPath));
    m_bMaskConfigured = maskPath[0] != 0;
    KImageParam mask;
    memset(&mask, 0, sizeof(mask));
    bool loaded = m_bMaskConfigured && g_pRepresentShell &&
        g_pRepresentShell->GetImageParam(maskPath, &mask, ISI_T_SPR);
    if (loaded && mask.nWidth > 0 && mask.nHeight > 0) {
        int width = mask.nWidth < m_Width ? mask.nWidth : m_Width;
        int height = mask.nHeight < m_Height ? mask.nHeight : m_Height;
        if (m_nClipWidth > 0 && width > m_nClipWidth) width = m_nClipWidth;
        if (m_nClipHeight > 0 && height > m_nClipHeight) height = m_nClipHeight;
        for (int y = 0; y < height; ++y) {
            int start = 0, previousAlpha = 0;
            for (int x = 0; x <= width; ++x) {
                int alpha = x < width ? g_pRepresentShell->GetImagePixelAlpha(
                    maskPath, 0, x, y, ISI_T_SPR) : -1;
                if (alpha != previousAlpha) {
                    if (previousAlpha > 0) {
                        MaskRun run;
                        run.x = start; run.y = y;
                        run.length = x - start; run.alpha = previousAlpha;
                        m_MaskRuns.push_back(run);
                    }
                    start = x;
                    previousAlpha = alpha;
                }
            }
        }
        m_MaskedParts.resize(m_MaskRuns.size());
    }
    FILE* trace = fopen("ui_portrait_mask.log", "a");
    if (trace) {
        fprintf(trace, "HUD_MASK_INIT path=%s loaded=%d mask=%dx%d slot=%dx%d runs=%u\n",
            maskPath, loaded, mask.nWidth, mask.nHeight, m_Width, m_Height,
            (unsigned int)m_MaskRuns.size());
        fclose(trace);
    }
    return result;
}

void KUiPortraitMask::PaintWindow() {
    if (!m_bMaskConfigured) {
        KWndImage::PaintWindow();
        return;
    }
    // Fail closed if the named authentic mask could not be loaded.
    if (!g_pRepresentShell || !m_Image.szImage[0] || m_MaskRuns.empty())
        return;
    KWndWindow::PaintWindow();
    for (unsigned int i = 0; i < m_MaskRuns.size(); ++i) {
        const MaskRun& run = m_MaskRuns[i];
        KRUImagePart& part = m_MaskedParts[i];
        memset(&part, 0, sizeof(part));
        memcpy(&part, &m_Image, sizeof(KRUImage));
        part.oPosition.nX = m_nAbsoluteLeft + run.x;
        part.oPosition.nY = m_nAbsoluteTop + run.y;
        part.oImgLTPos.nX = run.x;
        part.oImgLTPos.nY = run.y;
        part.oImgRBPos.nX = run.x + run.length;
        part.oImgRBPos.nY = run.y + 1;
        part.Color.Color_b.a = (unsigned char)(m_Image.Color.Color_b.a * run.alpha / 255);
    }
    g_pRepresentShell->DrawPrimitives((int)m_MaskedParts.size(),
        &m_MaskedParts[0], RU_T_IMAGE_PART, true);
    if (!m_bPaintLogged) {
        FILE* trace = fopen("ui_portrait_mask.log", "a");
        if (trace) {
            fprintf(trace, "HUD_MASK_DRAW portrait=%s parts=%u rect=%d,%d,%d,%d\n",
                m_Image.szImage, (unsigned int)m_MaskedParts.size(),
                m_nAbsoluteLeft, m_nAbsoluteTop, m_Width, m_Height);
            fclose(trace);
        }
        m_bPaintLogged = true;
    }
}
