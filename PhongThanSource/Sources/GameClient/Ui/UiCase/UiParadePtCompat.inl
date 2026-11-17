// Authentic PT resource contract. No invented positions or item-slot artwork.
void KUiParadeItem::LoadScheme(KIniFile* ini)
{
    if (!ini) return;
    Init(ini, "Main");
    m_Background.Init(ini, "ImgBackground");
    m_EquipImage.Init(ini, "ImgEquip");
    m_Avt.Init(ini, "Face");
    m_Name.Init(ini, "Name");
    const char* publicFields[10]={"Sign_Sex","Sign_Age","Sign_ShengXiao","Sign_BirthdayMonth","Sign_BirthdayDay","Sign_Star","Sign_Province","Sign_Activity","Sign_ActTime","Sign_Text"};
    for(int field=0;field<10;++field){m_PublicProfile[field].Init(ini,publicFields[field]);m_PublicProfile[field].SetText("");}

    m_Title.Init(ini, "Title");
    m_Tong.Init(ini, "TxtTongName");
    m_PKValue.Init(ini, "PKValue");
    m_MateName.Init(ini, "MateName");
    m_Close.Init(ini, "Close");
    m_WorldRank.Hide(); m_FuYuan.Hide(); m_Repute.Hide();
    m_TransLife.Hide(); m_EquipExpandBtn.Hide(); m_EquipExpandImg.Hide();
    for (int i = 0; i < PT_PARADE_SLOT_COUNT; ++i)
    {
        m_EquipSlotImage[i].Init(ini, CtrlItemMap[i].pIniSection);
        m_EquipBox[i].Init(ini, CtrlItemMap[i].pIniSection);
        m_EquipBox[i].EnablePickPut(false);
    }
    g_DebugLog("PARADE_PT_LAYOUT loaded Main slots=%d", PT_PARADE_SLOT_COUNT);
}
