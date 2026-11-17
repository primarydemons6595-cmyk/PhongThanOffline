#ifndef KSpriteDrawValidation_H
#define KSpriteDrawValidation_H

// Keep the draw-time RLE checks, but do not call IsBadReadPtr for every run.
// SPR loaders additionally validate frame lengths and palette indices.
// No pointer cache: freed/reused or modified sprite buffers must be rechecked.
static bool IsSafeIndexedSpriteRle(const void* lpSprite, int nWidth, int nHeight)
{
    if (lpSprite == NULL || nWidth <= 0 || nHeight <= 0 ||
        nWidth > 8192 || nHeight > 8192)
        return false;

    __try
    {
        unsigned long uPixels = (unsigned long)nWidth * nHeight;
        unsigned long uDone = 0;
        unsigned int uLine = 0;
        const volatile unsigned char* p = (const volatile unsigned char*)lpSprite;
        while (uDone < uPixels)
        {
            unsigned int uRun = p[0];
            unsigned int uAlpha = p[1];
            p += 2;
            if (uRun == 0 || uRun > uPixels - uDone ||
                uRun > (unsigned int)nWidth - uLine)
                return false;
            if (uAlpha != 0)
            {
                // A run contains at most 255 bytes, less than one Windows
                // memory page. Touch both ends to cover either page it spans.
                // Volatile reads must execute even though the indices are not
                // needed here. An unreadable/truncated mapping is caught below.
                unsigned char first = p[0];
                unsigned char last = p[uRun - 1];
                (void)first;
                (void)last;
                p += uRun;
            }
            uDone += uRun;
            uLine += uRun;
            if (uLine == (unsigned int)nWidth)
                uLine = 0;
        }
        return uLine == 0;
    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
        return false;
    }
}

#endif
