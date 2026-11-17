int KCanvas::PrepareSurface()
{
    if(!m_pSurface)return 0;
    HRESULT status=m_pSurface->IsLost();
    if(status==DD_OK)return 1;
    if(status!=DDERR_SURFACELOST)return 0;
    if(m_pSurface->Restore()==DD_OK)return 2;
    // Recreate only the windowed canvas, retaining game state and dimensions.
    if(m_dwScreenMode!=WINDOWMODE || !g_pDirectDraw)return 0;
    LPDIRECTDRAWSURFACE replacement=g_pDirectDraw->CreateSurface(m_nWidth,m_nHeight);
    if(!replacement)return 0;
    m_pSurface->Release();m_pSurface=replacement;
    return 2;
}

