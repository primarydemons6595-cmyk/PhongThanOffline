// Original 0087D9D0 loads 9 values/resource (palette0=0, palette1..8 from
// columns2..9); 0075B8C6 selects by actor kind/resource/palette, not AdjustColor.
// Actor kinds: male JS/SS/YR/YS=0..3, female=4..7 (original name array).
static int PhongThanEquipmentHue(int npcIndex, int part,
    const PHONGTHAN_VISUAL_PART& weapon, const PHONGTHAN_VISUAL_PART& horse)
{
    if (npcIndex <= 0 || npcIndex >= MAX_NPC) return 0;
    const int group = part / MAX_BODY_PART_SECT;
    if (group != 2 && group != 4) return 0;
    const PHONGTHAN_VISUAL_PART& visual = group == 2 ? weapon : horse;
    if (visual.nPaletteId <= 0 || visual.nPaletteId > 8 || visual.nResourceId < 0)
        return 0;
    const int profession = Npc[npcIndex].m_nProfession;
    const int sex = Npc[npcIndex].m_nSex;
    if (profession < 0 || profession > 3 || sex < 0 || sex > 1) return 0;
    const int kind = profession + sex * 4;
    const int family = group == 2 ? 0 : 1;
    static KTabFile tables[2][8];
    static BOOL attempted[2][8] = {{FALSE}};
    static BOOL loaded[2][8] = {{FALSE}};
    if (!attempted[family][kind])
    {
        char path[80];
        sprintf(path, "\\settings\\item\\%s_%d.txt",
            family == 0 ? "MeleePal" : "HorsePal", kind);
        loaded[family][kind] = tables[family][kind].Load(path);
        attempted[family][kind] = TRUE;
    }
    if (!loaded[family][kind]) return 0;
    int hue = 0;
    tables[family][kind].GetInteger(visual.nResourceId + 2,
        visual.nPaletteId + 1, 0, &hue);
    return hue > 0 && hue < 360 ? hue : 0;
}
