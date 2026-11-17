#pragma once

#include "../Elem/WndImage.h"
#include <vector>

// HUD-only compatibility for the authentic ImgPortrait.MaskPath contract.
// The status-window portrait and other KWndImage controls are unaffected.
class KUiPortraitMask : public KWndImage {
public:
    KUiPortraitMask() : m_bMaskConfigured(false), m_bPaintLogged(false) {}
    virtual int Init(KIniFile* pIni, const char* pSection);
    virtual void PaintWindow();

private:
    struct MaskRun {
        int x, y, length, alpha;
    };
    std::vector<MaskRun> m_MaskRuns;
    std::vector<KRUImagePart> m_MaskedParts;
    bool m_bMaskConfigured;
    bool m_bPaintLogged;
};
