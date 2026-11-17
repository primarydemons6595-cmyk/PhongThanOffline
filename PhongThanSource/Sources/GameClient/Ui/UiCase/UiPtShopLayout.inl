// Compatibility presentation for the authentic PT ItemBox_0..13 contract.
// No shop IDs, prices or purchase requests are manufactured here.
void KUiShop::InitPtLayout(KIniFile* ini) {
    int font = 14;
    int red = 255, green = 255, blue = 255;
    ini->GetInteger("Main", "TextFont", 14, &font);
    ini->GetInteger3("Main", "TextColor", &red, &green, &blue);
    for (int i = 0; i < 14; ++i) {
        char section[32];
        sprintf(section, "ItemBox_%d", i);
        int x, y, w, h, ix, iy, iw, ih, nx, ny, px, py;
        ini->GetInteger(section, "Left", 0, &x);
        ini->GetInteger(section, "Top", 0, &y);
        ini->GetInteger(section, "Width", 0, &w);
        ini->GetInteger(section, "Height", 0, &h);
        ini->GetInteger(section, "ImageLeft", 0, &ix);
        ini->GetInteger(section, "ImageTop", 0, &iy);
        ini->GetInteger(section, "ImageWidth", 0, &iw);
        ini->GetInteger(section, "ImageHeight", 0, &ih);
        ini->GetInteger(section, "ItemNameLeft", 0, &nx);
        ini->GetInteger(section, "ItemaNameTop", 0, &ny);
        ini->GetInteger(section, "PriceTextLeft", 0, &px);
        ini->GetInteger(section, "PriceTextTop", 0, &py);
        m_PtSlots[i].Init(ini, section);
        m_PtSlots[i].SetPosition(x + ix, y + iy);
        m_PtSlots[i].SetSize(iw, ih);
        m_PtSlots[i].SetContainerId(UOC_NPC_SHOP);
        m_PtSlots[i].EnablePickPut(false);
        m_PtNames[i].Init(ini, section);
        m_PtNames[i].SetPosition(x + nx, y + ny);
        m_PtNames[i].SetSize(w - nx, py - ny);
        m_PtNames[i].SetFontSize(font);
        m_PtNames[i].SetTextColor(0xff000000 | RGB(red, green, blue));
        m_PtPrices[i].Init(ini, section);
        m_PtPrices[i].SetPosition(x + px, y + py);
        m_PtPrices[i].SetSize(w - px, h - py);
        m_PtPrices[i].SetFontSize(font);
        m_PtPrices[i].SetTextColor(0xff000000 | RGB(red, green, blue));
    }
    m_ItemsBox.Hide();
}

void KUiShop::SetPtPage(int page) {
    for (int i = 0; i < 14; ++i) {
        m_PtSlots[i].Celar();
        m_PtNames[i].Clear();
        m_PtPrices[i].Clear();
        int index = page * 14 + i;
        if (!m_pObjsList || index >= m_nObjCount) continue;
        KUiObjAtContRegion& item = m_pObjsList[index];
        m_PtSlots[i].SetItem(item);
        KUiItemBuySelInfo price = {0};
        KUiObjAtContRegion priceObject = item;
        priceObject.eContainer = UOC_NPC_SHOP;
        if (g_pCoreShell && g_pCoreShell->GetGameData(GDI_TRADE_ITEM_PRICE,
                (unsigned int)&priceObject, (int)&price)) {
            m_PtNames[i].SetText(price.szItemName);
            m_PtPrices[i].SetMoneyUnitPrice(price.nCurPrice, price.nMoneyUnit);
        }
    }
    m_nCurrentPage = page;
    g_DebugLog("[PTShop] page=%d items=%d pages=%d", page, m_nObjCount, m_nPageCount);
}
