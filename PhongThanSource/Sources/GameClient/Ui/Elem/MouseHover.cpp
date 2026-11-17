/*****************************************************************************************
//	������ʾ����
//	Copyright : Kingsoft 2002-2003
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-12-23
*****************************************************************************************/
#include "KWin32.h"
#include "PhongThanCompose.h"
#include "KIniFile.h"
#include "KTabFile.h"
#include "KStrBase.h"
#include "GameDataDef.h"
#include "KCore.h"
#include "MouseHover.h"
#include "AutoLocateWnd.h"
#include "WndWindow.h"
#include "Wnds.h"
#include "../UiBase.h"
#include "../../../Represent/iRepresent/iRepresentShell.h"
#include "../../../Engine/Src/Text.h"
#include "SpecialFuncs.h"
#include <stdio.h>
#include <stdarg.h>

extern iRepresentShell *g_pRepresentShell;

#include "../../../core/src/CoreShell.h"

extern iCoreShell *g_pCoreShell;
extern IInlinePicEngineSink* g_pIInlinePicSink;
#include "SocketTooltipPictures.inl"
#include "SocketTooltipAnimation.inl"

KMouseOver g_MouseOver;
#define SCHEME_INI                       "UiMouseHover.ini"
#define INFO_MIN_LEN                     26
#define FOLLOW_CURSOR_OFFSET_X           16
#define FOLLOW_CURSOR_OFFSET_Y           8

// Exact text metrics used by the original Phong Than VNG mouse-over scheme.
// The old SwordOnline loader looked under [MouseOverWnd], while the VNG file
// stores these values under [Main].  That mismatch silently reduced the font
// and padding and made long equipment descriptions appear vertically clipped.
#define VNG_TOOLTIP_DEFAULT_IMG_WIDTH    12
#define VNG_TOOLTIP_DEFAULT_IMG_HEIGHT   9
#define VNG_TOOLTIP_DEFAULT_INDENT       6
#define VNG_TOOLTIP_DEFAULT_FONT_SIZE    14
#define VNG_TOOLTIP_LINE_GAP             1
#define VNG_TOOLTIP_PADDING_TOP          5
#define VNG_TOOLTIP_PADDING_BOTTOM       5

static int GetVngTooltipLineHeight(int nFontSize)
{
    return nFontSize + VNG_TOOLTIP_LINE_GAP;
}

static unsigned int s_uHoverObjDestTextColor = 0xffffffff;
// Trace the full item hover path.  The legacy UI otherwise fails silently
// when a child window or an object-table lookup misses the pointer event.
static void ItemHoverDiag(const char *pszFormat, ...)
{
    FILE *pFile = fopen("item_hover_diag.log", "a+b");
    if (!pFile)
        return;
    va_list args;
    va_start(args, pszFormat);
    vfprintf(pFile, pszFormat, args);
    va_end(args);
    fprintf(pFile, "\r\n");
    fclose(pFile);
}

void SetHoverObjDescColor(unsigned int uColor) {
    s_uHoverObjDestTextColor = uColor;
}

void SetMouseHoverObjectDesc(void *pWnd,int nObj,unsigned int uGenre,unsigned int uId,int nContainer,int x,int y,bool LAlign) {
    SetMouseHoverObjectDescInto(g_MouseOver,pWnd,nObj,uGenre,uId,nContainer,x,y,LAlign);
}
void SetMouseHoverObjectDescInto(KMouseOver& hover, void *pWnd, int nObj, unsigned int uGenre,
                             unsigned int uId, int nContainer, int x, int y, bool LAlign) {
    KGameObjDesc Desc;

    int nLenTitle = 0, nLenProp = 0, nLenDesc = 0;

    hover.CancelMouseHoverInfo();
    if (g_pCoreShell) {
        KUiObjAtContRegion Obj;
        Obj.Obj.uGenre = uGenre;
        Obj.Obj.uId = uId;
        Obj.Region.h = Obj.Region.v = 0;
        Obj.Region.Width = Obj.Region.Height = 0;
        Obj.nContainer = nContainer;
        Desc.szDesc[0] = 0;
        Desc.szProp[0] = 0;
        Desc.szTitle[0] = 0;
        unsigned uIndex = GDI_GAME_OBJ_DESC;
        if (g_UiBase.GetStatus() == UIS_S_TRADE_REPAIR && !g_pCoreShell->OperationRequest(PHONGTHAN_SMITH_CORE_STATE,0,0))
            uIndex = GDI_GAME_OBJ_DESC_INCLUDE_REPAIRINFO;
        else if (g_UiBase.GetStatus() != UIS_S_IDLE && g_UiBase.GetStatus() < UIS_S_LOCK_ITEM && !g_pCoreShell->OperationRequest(PHONGTHAN_SMITH_CORE_STATE,0,0))
            uIndex = GDI_GAME_OBJ_DESC_INCLUDE_TRADEINFO;

        // Invalidate the previous portrait before resolving the new object.
        // World/NPC hover can return no image; retaining this buffer showed
        // the portrait belonging to the previously hovered object.
        hover.m_HoverImage.szImage[0] = 0;
        ZeroMemory(&hover.m_SocketVisuals,sizeof(hover.m_SocketVisuals));
        int nDescRet = g_pCoreShell->GetGameData(uIndex, (unsigned int) &Obj, (int) &Desc);
        ItemHoverDiag("resolve wnd=%p obj=%d genre=%u id=%u container=%d xy=%d,%d status=%d ret=%d title=%d prop=%d desc=%d",
                      pWnd, nObj, uGenre, uId, nContainer, x, y,
                      (int)g_UiBase.GetStatus(), nDescRet,
                      Desc.szTitle[0] ? 1 : 0, Desc.szProp[0] ? 1 : 0,
                      Desc.szDesc[0] ? 1 : 0);

        // A missing/malformed description script must not make an item
        // unhoverable.  Use the canonical item name as a safe fallback.
        if (!Desc.szTitle[0] &&
            (uGenre == CGOG_ITEM || uGenre == CGOG_IME_ITEM ||
             uGenre == CGOG_NPCSELLITEM || uGenre == CGOG_PLAYERSELLITEM ||
             uGenre == CGOG_PLAYERVIEWITEM) && uId > 0 && uId < MAX_ITEM)
        {
            char szFallback[GOD_MAX_OBJ_TITLE_LEN];
            szFallback[0] = 0;
            if (g_pCoreShell->GetGameData(GDI_ITEM_NAME,
                                           (unsigned int)szFallback, (int)uId) &&
                szFallback[0])
            {
                strncpy(Desc.szTitle, szFallback, sizeof(Desc.szTitle) - 1);
                Desc.szTitle[sizeof(Desc.szTitle) - 1] = 0;
                ItemHoverDiag("fallback_name id=%u name=%s", uId, Desc.szTitle);
            }
        }
        hover.SetMouseHoverInfo(pWnd, nObj, x, y, false, false, LAlign);
        int imageFlags = g_pCoreShell->GetGameData(GDI_GAME_OBJ_DESC_INCLUDE_MOUSEHOVER,
            (unsigned int)&Obj, (int)&hover.m_HoverImage.szImage);
        hover.SetMouseHoverImage(imageFlags != 0, (imageFlags & 2) != 0, (imageFlags & 4) != 0);
        if(imageFlags)g_pCoreShell->GetGameData(PT_SOCKET_VISUAL_QUERY,(unsigned int)&Obj,(int)&hover.m_SocketVisuals);


        if (Desc.szTitle[0]) {
            nLenTitle = EncodeSocketTooltip(Desc.szTitle, strlen(Desc.szTitle),hover.m_SocketVisuals);
            hover.SetMouseHoverTitle(Desc.szTitle, nLenTitle, s_uHoverObjDestTextColor);
        }
        if (Desc.szProp[0]) {
            nLenProp = TEncodeText(Desc.szProp, strlen(Desc.szProp));
            hover.SetMouseHoverProp(Desc.szProp, nLenProp, s_uHoverObjDestTextColor);
        }
        if (Desc.szDesc[0]) {
            nLenDesc = TEncodeText(Desc.szDesc, strlen(Desc.szDesc));
            hover.SetMouseHoverDesc(Desc.szDesc, nLenDesc, s_uHoverObjDestTextColor);
        }
    }
}

int DrawDraggingGameObjFunc(int x, int y, const KUiDraggedObject &Obj, int nDropQueryResult) {
    g_pCoreShell->DrawGameObj(Obj.uGenre, Obj.uId, x, y, 0, 0, 0);
    return false;
}

KMouseOver::KMouseOver() {
    ZeroMemory(&m_SocketVisuals,sizeof(m_SocketVisuals));
    ZeroMemory(m_SocketHoleImages,sizeof(m_SocketHoleImages));
    ZeroMemory(m_SocketGemImages,sizeof(m_SocketGemImages));
    m_nImgWidth = VNG_TOOLTIP_DEFAULT_IMG_WIDTH;
    m_nImgHeight = VNG_TOOLTIP_DEFAULT_IMG_HEIGHT;
    m_nLeft = 0;
    m_nTop = 0;
    m_nWndWidth = 0;
    m_nWndHeight = 0;
    m_nWndWidthReduce = 0;
    m_nWndHeightReduce = 0;
    m_nWidthReduce = 0;
    m_nHeightReduce = 0;
    m_nMaxWidthReduce = 0;
    m_nMaxHeightReduce = 0;
    m_nIndent = VNG_TOOLTIP_DEFAULT_INDENT;
    m_nFontSize = VNG_TOOLTIP_DEFAULT_FONT_SIZE;
    m_nApplyX = 0;
    m_nApplyY = 0;
    m_nTitleLineNum = 0;
    m_nPropLineNum = 0;
    m_nDescLineNum = 0;
    m_nMaxLineLen = 0;
    m_pMouseHoverWnd = NULL;
    m_nObj = 0;
    m_ObjTitle[0] = 0;
    m_nTitleLen = 0;
    m_ObjProp[0] = 0;
    m_nPropLen = 0;
    m_ObjDesc[0] = 0;
    m_nDescLen = 0;
    m_uTitleColor = 0;
    m_uPropColor = 0;
    m_uDescColor = 0;
    m_uTitleBgColor = 0;
    m_uPropBgColor = 0;
    m_uDescBgColor = 0;
    //////////////////
    m_uBoderShadowColor = 0;      //����������ɫ
    m_uRectBordetShadowColor = 0;    //�������ֱ�����ɫ
    m_uRectBordetColor = 0;      //��������������ɫ
    //////////////////
    memset(&m_Image, 0, sizeof(KRUImage));
    IR_InitUiImageRef(m_HoverSmallImage);
    m_nSmallImageWidth = m_nSmallImageHeight = 0;
    IR_InitUiImageRef(m_HoverImage);
    m_bHeadTailImg = true;
    m_bTempImg = false;
    m_bFollowCursor = false;
    m_bShow = false;
    m_LAlign = false;
}

int KMouseOver::IsMoseHoverWndObj(void *pWnd, int nObj) {
    return ((pWnd == m_pMouseHoverWnd) && (nObj == m_nObj));
}

void KMouseOver::CancelMouseHoverInfo() {
    m_pMouseHoverWnd = 0;
    m_nObj = 0;
    m_ObjTitle[0] = 0;
    m_nTitleLen = 0;
    m_ObjProp[0] = 0;
    m_nPropLen = 0;
    m_ObjDesc[0] = 0;
    m_nDescLen = 0;
    m_nTitleLineNum = 0;
    m_nPropLineNum = 0;
    m_nDescLineNum = 0;
    m_nMaxLineLen = 0;
    m_bShow = false;
    if (m_bTempImg)
        SetMouseHoverImage(false);
}

/***********************************************************************
*���ܣ����������ʾ���ڵĻ�����Ϣ
************************************************************************/
void KMouseOver::SetMouseHoverInfo(void *pWnd, int nObj, int x, int y,
                                   bool bHeadTailImg, bool bFollowCursor, bool LAlign) {
    m_pMouseHoverWnd = pWnd;
    m_nObj = nObj;
    m_bHeadTailImg = bHeadTailImg;
    m_bFollowCursor = bFollowCursor;
    m_nApplyX = x;
    m_nApplyY = y;
    m_ObjTitle[0] = 0;
    m_nTitleLen = 0;
    m_ObjProp[0] = 0;
    m_nPropLen = 0;
    m_ObjDesc[0] = 0;
    m_nDescLen = 0;
    m_nTitleLineNum = 0;
    m_nPropLineNum = 0;
    m_nDescLineNum = 0;
    m_nMaxLineLen = 0;
    m_bShow = false;
    m_LAlign = LAlign;
}

void KMouseOver::SetMouseHoverImage(bool bAdd, bool bAlternateSex, bool bInventoryIcon) {
    KImageParam Param;
    IR_InitUiImageRef(m_HoverSmallImage);
    m_nSmallImageWidth = m_nSmallImageHeight = 0;
    if (bAdd) {
        if (g_pRepresentShell->GetImageParam(m_HoverImage.szImage, &Param, ISI_T_SPR) == true) {
            m_HoverImage.nFlipTime = IR_GetCurrentTime();
            //m_HoverImage.nInterval = Param.nInterval;
            m_HoverImage.nInterval = 80; //true
            m_HoverImage.nNumFrames = Param.nNumFrames;
            m_HoverImage.nType = ISI_T_SPR;
            m_HoverImage.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
            m_HoverImage.Color.Color_b.a = 255;
            m_HoverImage.uImage = 0;
            m_HoverImage.nISPosition = IMAGE_IS_POSITION_INIT;
            m_HoverImage.nFrame = 0;

            // Every object uses the same original portrait canvas.
            m_nWidthReduce = 66;
            m_nHeightReduce = 98;
            m_nWndWidthReduce = m_nWidthReduce + m_nIndent * 2;
            m_nWndHeightReduce = m_nHeightReduce;
            // The inset is the opposite-sex large artwork at half size.
            // Show it only when the equipment catalog marks sex-specific art.
            char alternatePath[256];
            strcpy(alternatePath, m_HoverImage.szImage);
            char* suffix = strstr(alternatePath, "_big_male.spr");
            bool female = false;
            if (!suffix) { suffix = strstr(alternatePath, "_big_female.spr"); female = true; }
            if (suffix && bAlternateSex) {
                strcpy(suffix, female ? "_big_male.spr" : "_big_female.spr");
                KImageParam previewParams;
                if (g_pRepresentShell->GetImageParam(alternatePath, &previewParams, ISI_T_SPR) &&
                    previewParams.nWidth > 0 && previewParams.nHeight > 0) {
                    m_HoverSmallImage = m_HoverImage;
                    strcpy(m_HoverSmallImage.szImage, alternatePath);
                    m_HoverSmallImage.uImage = 0;
                    m_HoverSmallImage.nISPosition = IMAGE_IS_POSITION_INIT;
                    m_HoverSmallImage.nNumFrames = previewParams.nNumFrames;
                    m_nSmallImageWidth = 33;
                    m_nSmallImageHeight = 49;
                    m_nWndWidthReduce += m_nSmallImageWidth / 2;
                    m_nWndHeightReduce += m_nSmallImageHeight / 2;
                }
            }
            m_bTempImg = true;
        } else {
            m_bTempImg = false;
            m_nWndWidthReduce = m_nWndHeightReduce = 0;
            m_nWidthReduce = m_nHeightReduce = 0;
        }
    } else {
        IR_InitUiImageRef(m_HoverSmallImage);
        m_nSmallImageWidth = m_nSmallImageHeight = 0;
        IR_InitUiImageRef(m_HoverImage);
        m_nWndWidthReduce = 0;
        m_nWndHeightReduce = 0;
        m_nWidthReduce = 0;
        m_nHeightReduce = 0;
        m_nMaxWidthReduce = 0;
        m_nMaxHeightReduce = 0;
        m_bTempImg = false;
    }
}

/***********************************************************************
*���ܣ����������ʾ���ڵı���(���������)
************************************************************************/
void KMouseOver::SetMouseHoverTitle(const char *pTitleText, int nTitleTextLen, UINT uColor) {
    if (nTitleTextLen > 0 && pTitleText && nTitleTextLen <= GOD_MAX_OBJ_TITLE_LEN) {
        memcpy(m_ObjTitle, pTitleText, nTitleTextLen);
        m_nTitleLen = nTitleTextLen;
        m_uTitleColor = uColor;
        Update(m_nApplyX, m_nApplyY);
    } else {
        m_ObjTitle[0] = 0;
        m_nTitleLen = 0;
    }
}


/***********************************************************************
*���ܣ����������ʾ���ڵ��������
************************************************************************/
void KMouseOver::SetMouseHoverProp(const char *pPropText, int nPropTextLen, UINT uColor) {
    if (nPropTextLen > 0 && pPropText && nPropTextLen <= GOD_MAX_OBJ_PROP_LEN) {
        memcpy(m_ObjProp, pPropText, nPropTextLen);
        m_nPropLen = nPropTextLen;
        m_uPropColor = uColor;
        Update(m_nApplyX, m_nApplyY);
    } else {
        m_ObjProp[0] = 0;
        m_nPropLen = 0;
    }
}


/***********************************************************************
*���ܣ����������ʾ���ڵ����˵��
************************************************************************/
void KMouseOver::SetMouseHoverDesc(const char *pDescText, int nDescTextLen, UINT uColor) {
    if (nDescTextLen > 0 && pDescText && nDescTextLen <= GOD_MAX_OBJ_DESC_LEN) {
        memcpy(m_ObjDesc, pDescText, nDescTextLen);
        m_nDescLen = nDescTextLen;
        m_uDescColor = uColor;
        Update(m_nApplyX, m_nApplyY);
    } else {
        m_ObjDesc[0] = 0;
        m_nDescLen = 0;
    }
}


static int TooltipTitleRowPixels(const char* text, int length, int font, int skip, int rows)
{
    int height = GetVngTooltipLineHeight(font), span = 1;
    for (int pos = 0; pos < length; ) {
        unsigned char code = (unsigned char)text[pos];
        if (code == KTC_COLOR || code == KTC_BORDER_COLOR) { pos += 4; continue; }
        if (code == KTC_INLINE_PIC && pos + 2 < length) {
            unsigned short index = (unsigned char)text[pos + 1] | ((unsigned char)text[pos + 2] << 8);
            int w, h;
            if (g_pIInlinePicSink && SUCCEEDED(g_pIInlinePicSink->GetPicSize(index, w, h))) {
                if (h + 2 > height) height = h + 2;
                int pictureRows = (h + font - 1) / font;
                if (pictureRows > span) span = pictureRows;
            }
            pos += 3; continue;
        }
        ++pos;
        if (code == KTC_ENTER) {
            if (skip <= 0) return height;
            skip -= span;
            height = GetVngTooltipLineHeight(font); span = 1;
        }
    }
    return height;
}

static int TooltipTitlePixels(const char* text, int length, int font)
{
    int skip = 0, pixels = 0, lineLength = 0, rows;
    while ((rows = TGetEncodedTextLineCount(text, length, 0, lineLength, font, skip, 1)) > 0) {
        pixels += TooltipTitleRowPixels(text, length, font, skip, rows);
        skip += rows;
    }
    return pixels;
}

void KMouseOver::Update(int nX, int nY) {
    m_bShow = false;

    if (g_pRepresentShell == NULL)
        return;

    int nMaxTitleLen = 0, nMaxPropLen = 0, nMaxDescLen = 0;

    m_nMaxLineLen = 0;
    if (m_nTitleLen > 0) {
        m_nTitleLineNum = TGetEncodedTextLineCount(
                m_ObjTitle, m_nTitleLen, 0, nMaxTitleLen, m_nFontSize);
        m_nMaxLineLen = nMaxTitleLen;
    } else {
        m_nTitleLineNum = 0;
    }
    if (m_nPropLen > 0) {
        m_nPropLineNum = TGetEncodedTextLineCount(
                m_ObjProp, m_nPropLen, 0, nMaxPropLen, m_nFontSize);
        if (m_nMaxLineLen < nMaxPropLen + 3)    //��+3������չtab�ַ�ռ�Ŀռ�
            m_nMaxLineLen = nMaxPropLen + 3;
    } else {
        m_nPropLineNum = 0;
    }
    if (m_nDescLen > 0) {
        m_nDescLineNum = TGetEncodedTextLineCount(
                m_ObjDesc, m_nDescLen, 0, nMaxDescLen, m_nFontSize);
        if (m_nMaxLineLen < nMaxDescLen)
            m_nMaxLineLen = nMaxDescLen;
    } else {
        m_nDescLineNum = 0;
    }


    int nNumLine = m_nTitleLineNum + m_nPropLineNum + m_nDescLineNum;
    if (nNumLine == 0)
        return;

    if (m_bFollowCursor == false && m_nMaxLineLen < INFO_MIN_LEN)
        m_nMaxLineLen = INFO_MIN_LEN;

    if (m_nMaxWidthReduce < m_nWndWidthReduce)
        m_nMaxWidthReduce = m_nWndWidthReduce;
    if (m_nMaxHeightReduce < m_nWndHeightReduce)
        m_nMaxHeightReduce = m_nWndHeightReduce;

    m_nWndWidth = m_nMaxWidthReduce + m_nFontSize * m_nMaxLineLen / 2 + m_nIndent * 2;
    // Size the body from the real encoded line count.  Do not reintroduce a
    // fixed SwordOnline height: Phong Than equipment may legitimately contain
    // 20 or more visible/hidden/set-property lines.
    m_nWndHeight = TooltipTitlePixels(m_ObjTitle, m_nTitleLen, m_nFontSize) +
                   GetVngTooltipLineHeight(m_nFontSize) * (m_nPropLineNum + m_nDescLineNum) +
                   VNG_TOOLTIP_PADDING_TOP + VNG_TOOLTIP_PADDING_BOTTOM;

    if (m_bHeadTailImg)
        m_nWndHeight += m_nImgHeight * 2;

    if (m_nWndHeight < m_nMaxHeightReduce + VNG_TOOLTIP_PADDING_TOP + VNG_TOOLTIP_PADDING_BOTTOM)
        m_nWndHeight = m_nMaxHeightReduce + VNG_TOOLTIP_PADDING_TOP + VNG_TOOLTIP_PADDING_BOTTOM;

    if (m_bFollowCursor) {
        m_nLeft = nX + FOLLOW_CURSOR_OFFSET_X;
        m_nTop = nY + FOLLOW_CURSOR_OFFSET_Y;
    } else {
        ALW_GetWndPosition(m_nLeft, m_nTop, m_nWndWidth, m_nWndHeight);
    }

    //������ʾ����������!
    m_bShow = true;
}


//����λ�ø�����
void KMouseOver::UpdateCursorPos(int nX, int nY) {
    if (m_bFollowCursor && m_bShow) {
        m_nLeft = nX + FOLLOW_CURSOR_OFFSET_X;
        m_nTop = nY + FOLLOW_CURSOR_OFFSET_Y;
    }
}


void KMouseOver::OnWndClosed(void *pWnd) {
    if (pWnd && pWnd == m_pMouseHoverWnd)
        CancelMouseHoverInfo();
}

//������淽��
void KMouseOver::LoadScheme(const char *pScheme) {
    if (pScheme == NULL)
        return;
    char Buff[128];
    KIniFile Ini;
    sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI);
    if (Ini.Load(Buff)) {
        // VNG stores the tooltip geometry in [Main].  Keep a fallback for an
        // old user scheme, but never let it replace the VNG defaults with 0.
        if (!Ini.GetInteger("Main", "ImgWidth", VNG_TOOLTIP_DEFAULT_IMG_WIDTH, &m_nImgWidth))
            Ini.GetInteger("MouseOverWnd", "ImgWidth", VNG_TOOLTIP_DEFAULT_IMG_WIDTH, &m_nImgWidth);
        if (!Ini.GetInteger("Main", "ImgHeight", VNG_TOOLTIP_DEFAULT_IMG_HEIGHT, &m_nImgHeight))
            Ini.GetInteger("MouseOverWnd", "ImgHeight", VNG_TOOLTIP_DEFAULT_IMG_HEIGHT, &m_nImgHeight);
        if (!Ini.GetInteger("Main", "Indent", VNG_TOOLTIP_DEFAULT_INDENT, &m_nIndent))
            Ini.GetInteger("MouseOverWnd", "Indent", VNG_TOOLTIP_DEFAULT_INDENT, &m_nIndent);
        if (!Ini.GetInteger("Main", "Font", VNG_TOOLTIP_DEFAULT_FONT_SIZE, &m_nFontSize))
            Ini.GetInteger("MouseOverWnd", "Font", VNG_TOOLTIP_DEFAULT_FONT_SIZE, &m_nFontSize);

        if (m_nImgWidth < 0)
            m_nImgWidth = VNG_TOOLTIP_DEFAULT_IMG_WIDTH;
        if (m_nImgHeight < 0)
            m_nImgHeight = VNG_TOOLTIP_DEFAULT_IMG_HEIGHT;
        if (m_nIndent < 0)
            m_nIndent = VNG_TOOLTIP_DEFAULT_INDENT;
        if (m_nFontSize < 8 || m_nFontSize >= 64)
            m_nFontSize = VNG_TOOLTIP_DEFAULT_FONT_SIZE;
        int nValue;
        Ini.GetInteger("Main", "ImgType", 0, &nValue);
        if (nValue == 1) {
            m_Image.nType = ISI_T_BITMAP16;
            m_Image.bRenderStyle = IMAGE_RENDER_STYLE_OPACITY;
        } else {
            m_Image.nType = ISI_T_SPR;
            m_Image.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
            m_Image.Color.Color_b.a = 255;
        }
        m_Image.uImage = 0;
        m_Image.nISPosition = IMAGE_IS_POSITION_INIT;
        Ini.GetString("Main", "Image", "", m_Image.szImage, sizeof(m_Image.szImage));
        Ini.GetInteger("Main", "Frame", 0, &nValue);
        m_Image.nFrame = nValue;

        Ini.GetString("Main", "TitleBgColor", "0, 0, 0", Buff, sizeof(Buff));
        m_uTitleBgColor = ((GetColor(Buff) & 0xffffff) | 0x0a000000);

        Ini.GetString("Main", "PropBgColor", "0, 0, 0", Buff, sizeof(Buff));
        m_uPropBgColor = ((GetColor(Buff) & 0xffffff) | 0x0a000000);

        Ini.GetString("Main", "DescBgColor", "0, 0, 0", Buff, sizeof(Buff));
        m_uDescBgColor = ((GetColor(Buff) & 0xffffff) | 0x0a000000);

        ////////////////////////////////////////
        Ini.GetString("Main", "BorderBgColor", "0, 0, 0", Buff, sizeof(Buff));
        m_uBoderShadowColor = (GetColor(Buff) & 0xffffff) | ((180 << 21) & 0xff000000);

        Ini.GetString("Main", "RectBoderBgColor", "0, 0, 0", Buff, sizeof(Buff));
        m_uRectBordetShadowColor = (GetColor(Buff) & 0xffffff) | ((180 << 21) & 0xff000000);

        Ini.GetString("Main", "RectBorder", "0, 0, 0", Buff, sizeof(Buff));
        m_uRectBordetColor = (GetColor(Buff) & 0xffffff) | ((180 << 21) & 0xff000000);
        ////////////////////////////////////////

        Update((m_nLeft + m_nWndWidth) / 2, m_nTop);
    }
}


static void PaintTooltipPreviewBackground(int x, int y, int width, int height, int frame)
{
    KRUImage background;
    memset(&background, 0, sizeof(background));
    strcpy(background.szImage, "\\spr\\ui4\\\xB5\xC0\xBE\xDF\xC0\xB8\\\xD7\xB0\xB1\xB8\xB4\xF3\xB1\xB3\xBE\xB0.spr");
    background.nType = ISI_T_SPR;
    background.nFrame = frame;
    background.nISPosition = IMAGE_IS_POSITION_INIT;
    background.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
    background.Color.Color_b.a = 255;
    background.oPosition.nX = x;
    background.oPosition.nY = y;
    background.oEndPos.nX = x + width;
    background.oEndPos.nY = y + height;
    g_pRepresentShell->DrawPrimitives(1, &background, RU_T_IMAGE_STRETCH, true);
}

static void PaintTooltipPreviewArt(KUiImageRef& source, int x, int y, int width, int height, int percent)
{
    KRPosition2 offset, size;
    if (!g_pRepresentShell->GetImageFrameParam(source.szImage, source.nFrame,
        &offset, &size, ISI_T_SPR) || size.nX <= 0 || size.nY <= 0) return;
    int w = size.nX * percent / 100;
    int h = size.nY * percent / 100;
    if (w > width - 2) { h = h * (width - 2) / w; w = width - 2; }
    if (h > height - 2) { w = w * (height - 2) / h; h = height - 2; }
    if (w < 1 || h < 1) return;
    KRUImage image = source;
    image.bRenderFlag = RUIMAGE_RENDER_FLAG_FRAME_DRAW;
    image.oPosition.nX = x + (width - w) / 2;
    image.oPosition.nY = y + (height - h) / 2;
    image.oEndPos.nX = image.oPosition.nX + w;
    image.oEndPos.nY = image.oPosition.nY + h;
    g_pRepresentShell->DrawPrimitives(1, &image, RU_T_IMAGE_STRETCH, true);
}

static void TooltipBorderPoint(int offset, int x, int y, int w, int h, KRPosition3& pos)
{
    offset %= 2 * (w + h);
    if (offset < w) { pos.nX = x + offset; pos.nY = y; }
    else if ((offset -= w) < h) { pos.nX = x + w; pos.nY = y + offset; }
    else if ((offset -= h) < w) { pos.nX = x + w - offset; pos.nY = y + h; }
    else { offset -= w; pos.nX = x; pos.nY = y + h - offset; }
}

static void PaintTooltipPreviewBorder(int x, int y, int width, int height)
{
    if (width <= 1 || height <= 1) return;
    // Follow the portrait background's outer pixels, never the item bounds.
    --width; --height;
    KRURect border;
    memset(&border, 0, sizeof(border));
    border.oPosition.nX = x; border.oPosition.nY = y;
    border.oEndPos.nX = x + width; border.oEndPos.nY = y + height;
    border.Color.Color_dw = 0x00506430;
    g_pRepresentShell->DrawPrimitives(1, &border, RU_T_RECT, true);
    // Two long, opposing trails: bright heads with smoothly fading tails.
    // A shared four-second period keeps large and inset previews in step.
    int perimeter = 2 * (width + height);
    int phase = (GetTickCount() % 4000) * perimeter / 4000;
    int trailLength = perimeter * 3 / 8;
    if (trailLength > 256) trailLength = 256;
    KRULine glow[512];
    memset(glow, 0, sizeof(glow));
    int count = 0;
    for (int trail = 0; trail < 2; ++trail) {
        int head = phase + trail * perimeter / 2;
        for (int tail = 0; tail < trailLength; ++tail) {
            int remaining = trailLength - tail;
            int alpha = 255 * remaining * remaining / (trailLength * trailLength);
            if (alpha < 8) continue;
            int position = (head - tail + perimeter) % perimeter;
            TooltipBorderPoint(position, x, y, width, height, glow[count].oPosition);
            TooltipBorderPoint(position + 1, x, y, width, height, glow[count].oEndPos);
            // RU_T_LINE opacity is 0..255; the tail blends into the frame.
            glow[count].Color.Color_dw = (alpha << 24) | (tail < 3 ? 0x00dcffdc : 0x0078e078);
            ++count;
        }
    }
    g_pRepresentShell->DrawPrimitives(count, glow, RU_T_LINE, true);
}

void KMouseOver::GetPreviewBounds(int& x,int& y,int& width,int& height) {
    int sw,sh;Wnd_GetScreenSize(sw,sh);
    width=m_nWndWidth;height=m_nWndHeight;x=m_nLeft;y=m_nTop;
    if(x+width>sw)x=x-width-10<0?0:x-width-10;
    if(x+width>sw)x=sw-width;if(x<0)x=0;
    if(y+height>sh)y=sh-height;if(y<0)y=0;
}
void KMouseOver::PaintMouseHoverInfo() {
    if (m_bShow == false || g_pRepresentShell == NULL)
        return;

    //��������Ӱ�ͱ߿�
    KRUShadow Shadow;
    memset(&Shadow, 0, sizeof(Shadow));
    //д���ֳ�ʼ��
    KOutputTextParam Param;
    Param.BorderColor = 0;
    Param.nZ = TEXT_IN_SINGLE_PLANE_COORD;

    // Resolve the window once, then use the same origin for background and
    // text.  This also keeps a tall tooltip inside the visible screen area.
    int nWindowLeft,windowTop,windowWidth,windowHeight;
    GetPreviewBounds(nWindowLeft,windowTop,windowWidth,windowHeight);
    int nBodyTop = windowTop;
    int nBodyHeight = m_nWndHeight;
    if (m_bHeadTailImg) {
        nBodyTop += m_nImgHeight;
        nBodyHeight -= m_nImgHeight * 2;
    }
    if (nBodyHeight < VNG_TOOLTIP_PADDING_TOP + VNG_TOOLTIP_PADDING_BOTTOM)
        nBodyHeight = VNG_TOOLTIP_PADDING_TOP + VNG_TOOLTIP_PADDING_BOTTOM;

    int nBodyBottom = nBodyTop + nBodyHeight;
    int nTextY = nBodyTop + VNG_TOOLTIP_PADDING_TOP;
    int nLineHeight = GetVngTooltipLineHeight(m_nFontSize);
    int nTextLeft = nWindowLeft + m_nMaxWidthReduce + m_nIndent;
    int nTextWidth = m_nWndWidth - m_nMaxWidthReduce - m_nIndent * 2;
    if (nTextWidth < m_nFontSize)
        nTextWidth = m_nFontSize;

    Shadow.oPosition.nX = nWindowLeft;
    Shadow.oEndPos.nX = nWindowLeft + m_nWndWidth;
    Shadow.oPosition.nY = nBodyTop;
    Shadow.oEndPos.nY = nBodyTop;

    if (m_nTitleLen > 0) {
        Shadow.Color.Color_dw = m_uTitleBgColor;
        Shadow.oEndPos.nY = nTextY + TooltipTitlePixels(m_ObjTitle, m_nTitleLen, m_nFontSize);
        if (m_nPropLen <= 0 && m_nDescLen <= 0)
            Shadow.oEndPos.nY = nBodyBottom;
        g_pRepresentShell->DrawPrimitives(1, &Shadow, RU_T_SHADOW, true);

        Param.nSkipLine = 0;
        Param.nNumLine = 1;
        Param.Color = m_uTitleColor;
        Param.nY = nTextY;
        int nLineLen;
        while (true) {
            // A tall inline picture may occupy several text rows. Paint it
            // once, then skip all rows it owns instead of repainting it.
            int occupiedRows = TGetEncodedTextLineCount(m_ObjTitle, m_nTitleLen, 0,
                nLineLen, m_nFontSize, Param.nSkipLine, 1);
            if (occupiedRows == 0)
                break;
            if (m_LAlign)
                Param.nX = nTextLeft;
            else
                Param.nX = nTextLeft + nTextWidth / 2 - (nLineLen * m_nFontSize) / 4;
            g_pRepresentShell->OutputRichText(m_nFontSize, &Param, m_ObjTitle, m_nTitleLen, 0);
            Param.nY += TooltipTitleRowPixels(m_ObjTitle, m_nTitleLen,
                m_nFontSize, Param.nSkipLine, occupiedRows);
            Param.nSkipLine += occupiedRows;
        };
        nTextY += TooltipTitlePixels(m_ObjTitle, m_nTitleLen, m_nFontSize);
    }

    //====���Բ���====
    if (m_nPropLen > 0) {
        Shadow.Color.Color_dw = m_uPropBgColor;
        Shadow.oPosition.nY = nTextY;
        Shadow.oEndPos.nY = nTextY + nLineHeight * m_nPropLineNum;
        if (m_nDescLen <= 0)
            Shadow.oEndPos.nY = nBodyBottom;
        g_pRepresentShell->DrawPrimitives(1, &Shadow, RU_T_SHADOW, true);

        Param.nSkipLine = 0;
        Param.Color = m_uPropColor;
        Param.nNumLine = m_nPropLineNum;
        Param.nX = nTextLeft;
        Param.nY = nTextY;
        OutputTabSplitText(m_ObjProp, m_nPropLen, nTextWidth, m_nFontSize, &Param);
        nTextY += nLineHeight * m_nPropLineNum;
    }

    //====��������====
    if (m_nDescLen > 0) {
        Shadow.Color.Color_dw = m_uDescBgColor;
        Shadow.oPosition.nY = nTextY;
        Shadow.oEndPos.nY = nBodyBottom;
        g_pRepresentShell->DrawPrimitives(1, &Shadow, RU_T_SHADOW, true);

        Param.nSkipLine = 0;
        Param.Color = m_uDescColor;
        Param.nNumLine = m_nDescLineNum;
        Param.nX = nTextLeft;
        Param.nY = nTextY;
        g_pRepresentShell->OutputRichText(m_nFontSize, &Param, m_ObjDesc, m_nDescLen, 0);
    }

    //����ͼ�ͱ߿�ͼ
    if (m_bHeadTailImg && m_nImgWidth > 0) {
        m_Image.oPosition.nX = nWindowLeft;
        while (m_Image.oPosition.nX < nWindowLeft + m_nWndWidth) {
            m_Image.oPosition.nY = windowTop;
            g_pRepresentShell->DrawPrimitives(1, &m_Image, RU_T_IMAGE, true);
            m_Image.oPosition.nY = nBodyBottom;
            g_pRepresentShell->DrawPrimitives(1, &m_Image, RU_T_IMAGE, true);
            m_Image.oPosition.nX += m_nImgWidth;
        };
    }
    // ClearAlpha uses retained-background weight 0..32 (0 is opaque).
    KRUShadow border[4];
    memset(border, 0, sizeof(border));
    for (int edge=0; edge<4; ++edge) {
        border[edge].Color.Color_dw = 0x0082712b;
        border[edge].oPosition.nX = nWindowLeft;
        border[edge].oEndPos.nX = nWindowLeft + m_nWndWidth;
        border[edge].oPosition.nY = nBodyTop;
        border[edge].oEndPos.nY = nBodyBottom;
    }
    border[0].oEndPos.nY = nBodyTop + 1;
    border[1].oPosition.nY = nBodyBottom - 1;
    border[2].oEndPos.nX = nWindowLeft + 1;
    border[3].oPosition.nX = nWindowLeft + m_nWndWidth - 1;
    g_pRepresentShell->DrawPrimitives(4, border, RU_T_SHADOW, true);
    if (m_bTempImg) {
        m_HoverImage.oPosition.nX = nWindowLeft + m_nIndent;
        m_HoverImage.oPosition.nY = nBodyTop + (nBodyHeight - m_nWndHeightReduce) / 2;
        PaintTooltipPreviewBackground(m_HoverImage.oPosition.nX, m_HoverImage.oPosition.nY,
            m_nWidthReduce, m_nHeightReduce, 1);
        IR_NextFrame(m_HoverImage);
        PaintTooltipPreviewArt(m_HoverImage, m_HoverImage.oPosition.nX, m_HoverImage.oPosition.nY,
            m_nWidthReduce, m_nHeightReduce, 100);
        PaintSocketTooltipLayers(m_SocketVisuals,m_SocketHoleImages,m_SocketGemImages,
            m_HoverImage.oPosition.nX,m_HoverImage.oPosition.nY,m_nWidthReduce,m_nHeightReduce);
        PaintTooltipPreviewBorder(m_HoverImage.oPosition.nX, m_HoverImage.oPosition.nY,
            m_nWidthReduce, m_nHeightReduce);
        if (m_nSmallImageWidth > 0 && m_nSmallImageHeight > 0) {
            m_HoverSmallImage.oPosition.nX = m_HoverImage.oPosition.nX + m_nWidthReduce - m_nSmallImageWidth / 2;
            m_HoverSmallImage.oPosition.nY = m_HoverImage.oPosition.nY + m_nHeightReduce - m_nSmallImageHeight / 2;
            IR_NextFrame(m_HoverSmallImage);
            PaintTooltipPreviewBackground(m_HoverSmallImage.oPosition.nX, m_HoverSmallImage.oPosition.nY,
                m_nSmallImageWidth, m_nSmallImageHeight, 0);
            m_HoverSmallImage.oEndPos.nX = m_HoverSmallImage.oPosition.nX + m_nSmallImageWidth;
            m_HoverSmallImage.oEndPos.nY = m_HoverSmallImage.oPosition.nY + m_nSmallImageHeight;
            PaintTooltipPreviewArt(m_HoverSmallImage, m_HoverSmallImage.oPosition.nX, m_HoverSmallImage.oPosition.nY,
                m_nSmallImageWidth, m_nSmallImageHeight, 50);
            PaintTooltipPreviewBorder(m_HoverSmallImage.oPosition.nX, m_HoverSmallImage.oPosition.nY,
                m_nSmallImageWidth, m_nSmallImageHeight);
        }
    }
}


/***********************************************************************
*���ܣ����ȶ��������Ȱ����Բ��ָ�ʽ��(���ҿ�)
************************************************************************/
/*void KMouseOver::FormatProp()
{
    if(m_ObjProp[0])
	{
		char szBuffer[MAX_OBJ_PROP_LEN], *pPos = NULL, *pHead = NULL, *pTail = NULL;
	    int nLeftLen = 0, nRightLen = 0, i, j, k;
        //��ʼ׼��
		memset(szBuffer, 0, MAX_OBJ_PROP_LEN);
		memcpy(szBuffer, m_ObjProp, m_nPropLen);
		memset(m_ObjProp, 0, MAX_OBJ_PROP_LEN);
	    szBuffer[MAX_OBJ_PROP_LEN - 1] = 0;
		pHead = szBuffer;
		//��ʼѭ������
        while(pHead[0])
	    {
            pPos = strchr(pHead, 0x20);
			if(pPos == NULL) break;
			nLeftLen = pPos - pHead;

			pTail = strchr(pPos, 0x0a);
			if(pTail == NULL)
			{
				pTail = strlen(pPos) + pPos;
				if((pTail - pPos) <= 1)
				    break;
			}
			nRightLen = pTail - pPos - 1;

			*pPos = *pTail = 0;
			strcat(m_ObjProp, pHead);
			j = m_nMaxLineLen - nLeftLen - nRightLen;
			k = strlen(m_ObjProp);
			for(i=0;i < j;i++)
			{
				m_ObjProp[k] = ' ';
				k++;
			}
			m_ObjProp[k] = 0;
			strcat(m_ObjProp, pPos + 1);
			k = strlen(m_ObjProp);
			m_ObjProp[k] = 0x0a;
			m_ObjProp[k+1] = 0;

			pHead = pTail + 1;
			pPos =  NULL;
			pTail = NULL;
	    };

		j = strlen(m_ObjProp);
		m_ObjProp[j] = 0;
	    m_nPropLen = strlen(m_ObjProp);
    }
}*/
