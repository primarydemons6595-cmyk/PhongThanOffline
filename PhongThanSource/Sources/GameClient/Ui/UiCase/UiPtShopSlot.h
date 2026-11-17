#pragma once
#include "../Elem/WndObjContainer.h"

// Keep the Core-provided identity/region intact while displaying a PT row.
class KUiPtShopSlot : public KWndObjectBox {
public:
    void SetItem(const KUiObjAtContRegion& item) {
        HoldObject(item.Obj.uGenre, item.Obj.uId,
                   item.Region.Width, item.Region.Height);
        m_Object.DataX = item.Region.h;
        m_Object.DataY = item.Region.v;
    }
};
