BOOL KDirectDraw::PresentWindowCanvas(LPDIRECTDRAWSURFACE surface, HWND window)
{
    if(!surface || !window || !m_lpDirectDraw)return FALSE;
    RECT target;if(!GetClientRect(window,&target))return FALSE;
    ClientToScreen(window,(POINT*)&target);
    ClientToScreen(window,((POINT*)&target)+1);
    if(target.left<0 || target.top<0 || target.right>GetSystemMetrics(SM_CXSCREEN) ||
        target.bottom>GetSystemMetrics(SM_CYSCREEN))return FALSE;
    HRESULT status=m_lpDDSPrimary?m_lpDDSPrimary->IsLost():DDERR_SURFACELOST;
    if(status==DDERR_SURFACELOST){
        status=m_lpDDSPrimary?m_lpDDSPrimary->Restore():DDERR_WRONGMODE;
        if(status!=DD_OK){
            // Desktop mode changes may make Restore return WRONGMODE. Only
            // replace the primary target, never the game's canvas or images.
            if(m_lpDDSPrimary)m_lpDDSPrimary->Release();m_lpDDSPrimary=NULL;
            DDSURFACEDESC desc;ZeroMemory(&desc,sizeof(desc));desc.dwSize=sizeof(desc);
            desc.dwFlags=DDSD_CAPS;desc.ddsCaps.dwCaps=DDSCAPS_PRIMARYSURFACE;
            status=m_lpDirectDraw->CreateSurface(&desc,&m_lpDDSPrimary,NULL);
            if(status!=DD_OK)return FALSE;
            if(m_lpClipper && m_lpDDSPrimary->SetClipper(m_lpClipper)!=DD_OK){
                m_lpDDSPrimary->Release();m_lpDDSPrimary=NULL;return FALSE;
            }
        }
    }else if(status!=DD_OK)return FALSE;
    return m_lpDDSPrimary->Blt(&target,surface,NULL,DDBLT_WAIT,NULL)==DD_OK;
}
