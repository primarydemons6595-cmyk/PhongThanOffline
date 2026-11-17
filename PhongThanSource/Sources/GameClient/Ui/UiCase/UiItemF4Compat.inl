// ASCII-only compatibility logic; legacy UiItem.cpp bytes remain unchanged
// outside the mechanically installed include/call hooks.
static int PhongThanInventoryMannequinFrame(KIniFile& ini)
{
    if (!g_pCoreShell)
        return 0;
    KUiPlayerBaseInfo info;
    memset(&info, 0, sizeof(info));
    info.nProfession = -1;
    g_pCoreShell->GetGameData(GDI_PLAYER_BASE_INFO, (unsigned int)&info, 0);
    const int male = g_pCoreShell->GetGameData(GDI_PLAYER_IS_MALE, 0, 0) ? 1 : 0;
    static const char* keys[3][2] = {
        { "Warrior_Female", "Warrior_Male" },
        { "Wizard_Female", "Wizard_Male" },
        { "Sprite_Female", "Sprite_Male" }
    };
    int frame = male;
    if (info.nProfession >= 0 && info.nProfession < 3)
        ini.GetInteger("Equip", keys[info.nProfession][male], male, &frame);
    return frame;
}
