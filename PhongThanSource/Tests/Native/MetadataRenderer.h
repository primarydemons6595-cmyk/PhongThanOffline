// Test renderer: only image metadata queries are supported; all rendering stays headless.
struct PTMetadataRenderer : iRepresentShell {
void Release(){}
bool Create( int nWidth, int nHeight, bool bFullScreen){return 0;}
bool Reset( int nWidth, int nHeight, bool bFullScreen){return 0;}
bool CreateAFont( const char* pszFontFile, CHARACTER_CODE_SET CharaSet, int nId){return 0;}
void OutputText( int nFontId, const char* psText, int nCount = KRF_ZERO_END, int nX = KRF_FOLLOW, int nY = KRF_FOLLOW, unsigned int Color = 0xFF000000, int nLineWidth = 0, int nZ = TEXT_IN_SINGLE_PLANE_COORD, unsigned int BorderColor = 0){}
int OutputRichText( int nFontId, KOutputTextParam* pParam, const char* psText, int nCount = KRF_ZERO_END, int nLineWidth = 0){return 0;}
int LocateRichText( int nX, int nY, int nFontId, KOutputTextParam* pParam, const char* psText, int nCount = KRF_ZERO_END, int nLineWidth = 0){return 0;}
void ReleaseAFont( int nId){}
unsigned int CreateImage( const char* pszName, int nWidth, int nHeight, int nType){return 0;}
void FreeImage( const char* pszImage ){}
void FreeAllImage(){}
void* GetBitmapDataBuffer( const char* pszImage, KBitmapDataBuffInfo* pInfo){return 0;}
void ReleaseBitmapDataBuffer( const char* pszImage, void* pBuffer){}
bool GetImageParam( const char* pszImage, KImageParam* pImageData, int nType){ SPROFFS* off=NULL;SPRHEAD* h=SprGetHeader((char*)pszImage,off);if(!h)return false;memset(pImageData,0,sizeof(*pImageData));pImageData->nNumFrames=h->Frames;pImageData->nNumFramesGroup=h->Directions;pImageData->nInterval=h->Interval;SprReleaseHeader(h);return true;}
bool GetImageFrameParam( const char* pszImage, int nFrame, KRPosition2* pOffset, KRPosition2* pSize, int nType){return 0;}
int GetImagePixelAlpha( const char* pszImage, int nFrame, int nX, int nY, int nType){return 0;}
void SetImageStoreBalanceParam( int nNumImage, unsigned int uCheckPoint = 1000){}
bool SaveImage( const char* pszFile, const char* pszImage, int nFileType){return 0;}
void DrawPrimitives( int nPrimitiveCount, KRepresentUnit* pPrimitives, unsigned int uGenre, int bSinglePlaneCoord){}
void DrawPrimitivesOnImage( int nPrimitiveCount, KRepresentUnit* pPrimitives, unsigned int uGenre, const char* pszImage, unsigned int uImage, short& nImagePosition){}
void ClearImageData( const char* pszImage, unsigned int uImage, short nImagePosition){}
void LookAt(int nX, int nY, int nZ){}
bool CopyDeviceImageToImage( const char* pszName, int nDeviceX, int nDeviceY, int nImageX, int nImageY, int nWidth, int nHeight){return 0;}
bool RepresentBegin( int bClear, unsigned int Color){return 0;}
void RepresentEnd(){}
void ViewPortCoordToSpaceCoord( int& nX, int& nY, int nZ ){}
void SetLightInfo( int nX, int nY, unsigned int *pLighting){}
void SetOption( RepresentOption eOption, bool bOn){}
long AdviseRepresent(IInlinePicEngineSink*){return 0;}
long UnAdviseRepresent(IInlinePicEngineSink*){return 0;}
bool SaveScreenToFile(const char* pszName, ScreenFileType eType, unsigned int nQuality){return 0;}
void SetGamma(int nGamma){}
unsigned int SetAdjustColorList( unsigned int* puColorList, unsigned int uCount ){return 0;}
};