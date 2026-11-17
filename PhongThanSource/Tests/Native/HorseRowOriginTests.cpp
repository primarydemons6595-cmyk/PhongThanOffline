#include "KWin32.h"
#include "KPakList.h"
#include "KFilePath.h"
#include "KTabFile.h"
#define CORE_API
#include "KPhongThanAppearance.h"
#include <assert.h>
#include <stdio.h>
KPakList* g_pPakList = NULL;
int main(int argc, char** argv)
{
    assert(argc == 2);
    g_SetRootPath(argv[1]);
    g_SetPakFileMode(0);
    KPhongThanAppearance appearance;
    assert(appearance.Init());
    KTabFile base, overrides;
    assert(base.Load("\\settings\\item\\VNG_HorsePart.txt"));
    assert(overrides.Load("\\settings\\item\\PhongThanHorseVisual.txt"));
    int checked = 0, repaired = 0;
    for (int row = 2; row <= base.GetHeight(); ++row)
    {
        int key = 0, nativeRow = 0, palette = 0;
        assert(base.GetInteger(row, 1, 0, &key));
        assert(base.GetInteger(row, 2, 0, &nativeRow));
        assert(base.GetInteger(row, 3, 0, &palette));
        int slot = nativeRow - 2, oldSlot = nativeRow - 1;
        for (int r = 2; r <= overrides.GetHeight(); ++r)
        {
            int overrideKey = 0, group = 0;
            overrides.GetInteger(r, 1, 0, &overrideKey);
            if (overrideKey != key) continue;
            assert(overrides.GetInteger(r, 2, 0, &group));
            overrides.GetInteger(r, 3, 0, &palette);
            slot = group - 1;
            oldSlot = slot;
            break;
        }
        PHONGTHAN_VISUAL_PART visual;
        assert(appearance.ResolveHorse(key, &visual));
        assert(visual.bVisible && visual.nResourceId == slot);
        assert(visual.nPaletteId == palette);
        assert(slot >= 0 && slot < 12 && palette >= 0 && palette <= 8);
        if (oldSlot >= 12) ++repaired;
        ++checked;
    }
    assert(checked == 6217 && repaired == 520);
    printf("PASS: %d actual ResolveHorse records, %d repaired row overflows; override groups and palettes preserved.\n", checked, repaired);
    return 0;
}
