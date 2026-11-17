#include <windows.h>
#include <stdio.h>
#include "KWin32.h"
#include "KFilePath.h"
#include "KPakList.h"
#include "KImageStore2.h"
#include "KSprite.h"
#include "KColors.h"
#include "KDDraw.h"

int main(int argc, char** argv)
{
    if (argc != 2) return 2;
    // Headless palette conversion still consults the engine's draw object.
    // Zero-initialized colour mask selects its RGB555 conversion branch.
    static KDirectDraw draw;
    g_SetRootPath(NULL);
    // The interactive renderer selects this callback during DirectDraw init.
    g_RGB = g_RGB565;
    KPakList pak;
    if (!pak.Open("\\package.ini")) return 3;
    KImageStore2 store;
    if (!store.Init()) return 4;
    FILE* list = fopen(argv[1], "rb");
    if (!list) return 5;
    char path[1024]; int files = 0; unsigned long accesses = 0;
    LARGE_INTEGER frequency, start, end;
    QueryPerformanceFrequency(&frequency);
    double total = 0;
    while (fgets(path, sizeof(path), list))
    {
        path[strcspn(path, "\r\n")] = 0;
        if (!path[0]) continue;
        unsigned id = 0; short position = -1; void* frame = NULL;
        SPRHEAD* header = (SPRHEAD*)store.GetImage(path, id, position, 0, ISI_T_SPR, frame);
        if (!header || !frame) { printf("LOAD_FAIL %s\n", path); return 6; }
        int frames = header->Frames;
        // Warm every animation frame before measuring the cache hit path.
        for (int i = 0; i < frames; ++i)
            if (!store.GetImage(path, id, position, i, ISI_T_SPR, frame) || !frame) return 7;
        QueryPerformanceCounter(&start);
        for (i = 0; i < 20000; ++i)
            if (!store.GetImage(path, id, position, i % frames, ISI_T_SPR, frame) || !frame) return 8;
        QueryPerformanceCounter(&end);
        total += (double)(end.QuadPart-start.QuadPart)*1000/frequency.QuadPart;
        accesses += 20000; ++files;
        store.FreeImage(path);
        id = 0; position = -1;
        if (!store.GetImage(path,id,position,frames-1,ISI_T_SPR,frame) || !frame) return 9;
        store.FreeImage(path);
    }
    fclose(list);
    printf("CACHE_FILES=%d CACHE_ACCESSES=%lu CACHE_HIT_MS=%.3f RELOAD=PASS\n",files,accesses,total);
    return files ? 0 : 10;
}
