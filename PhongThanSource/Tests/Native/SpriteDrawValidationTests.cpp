#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "KSpriteDrawValidation.h"
#include "SpriteDrawValidationBefore.h"

static int failures = 0;
static void Check(bool condition, const char* label)
{
    if (!condition) { printf("FAIL %s\n", label); failures++; }
}

int main()
{
    unsigned char valid[] = { 4,255,1,2,3,4,4,0 };
    Check(IsSafeIndexedSpriteRle(valid,4,2), "valid opaque and transparent rows");
    Check(!IsSafeIndexedSpriteRle(NULL,4,2), "null");
    Check(!IsSafeIndexedSpriteRle(valid,0,2), "zero width");
    Check(!IsSafeIndexedSpriteRle(valid,8193,2), "oversized width");
    unsigned char zero[] = {0,0};
    unsigned char cross[] = {5,0};
    Check(!IsSafeIndexedSpriteRle(zero,4,1), "zero run");
    Check(!IsSafeIndexedSpriteRle(cross,4,2), "run crosses row");

    unsigned char* pages = (unsigned char*)VirtualAlloc(NULL,8192,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE);
    if (!pages) return 2;
    DWORD oldProtect;
    VirtualProtect(pages+4096,4096,PAGE_NOACCESS,&oldProtect);
    Check(!IsSafeIndexedSpriteRle(pages+4096,1,1), "unreadable header");
    pages[4095]=1;
    Check(!IsSafeIndexedSpriteRle(pages+4095,1,1), "split unreadable header");
    pages[4092]=4;pages[4093]=255;
    Check(!IsSafeIndexedSpriteRle(pages+4092,4,1), "payload crosses inaccessible page");
    pages[4094]=1;pages[4095]=255;
    Check(!IsSafeIndexedSpriteRle(pages+4094,1,1), "unreadable payload start");
    pages[4092]=2;pages[4093]=255;
    Check(IsSafeIndexedSpriteRle(pages+4092,2,1), "payload ends at page boundary");
    pages[4092]=0;
    Check(!IsSafeIndexedSpriteRle(pages+4092,2,1), "modified buffer revalidated");
    VirtualFree(pages,0,MEM_RELEASE);

    unsigned char fuzz[32768];
    srand(20261030);
    for (int i=0;i<5000;i++)
    {
        for(int j=0;j<sizeof(fuzz);j++) fuzz[j]=(unsigned char)rand();
        int w=1+rand()%64,h=1+rand()%64;
        Check(IsSafeIndexedSpriteRle(fuzz,w,h)==BeforeIsSafeIndexedSpriteRle(fuzz,w,h),"legacy decision parity");
    }

    for (i=0;i<1000;i++)
    {
        int w=1+rand()%128,h=1+rand()%32;
        std::vector<unsigned char> rows;
        for(int y=0;y<h;y++)
            for(int x=0;x<w;)
            {
                int run=1+rand()%(w-x);
                bool opaque=(rand()%2)!=0;
                rows.push_back((unsigned char)run);rows.push_back(opaque?255:0);
                if(opaque)for(int pixel=0;pixel<run;pixel++)rows.push_back((unsigned char)rand());
                x+=run;
            }
        Check(IsSafeIndexedSpriteRle(&rows[0],w,h)&&BeforeIsSafeIndexedSpriteRle(&rows[0],w,h),"randomized valid rows");
        rows[0]=0;
        Check(!IsSafeIndexedSpriteRle(&rows[0],w,h),"randomized mutated rows");
    }

    std::vector<unsigned char> sprite;
    for (int row=0;row<512;row++)
        for (int run=0;run<64;run++)
        {
            sprite.push_back(8);sprite.push_back(255);
            for(int pixel=0;pixel<8;pixel++)sprite.push_back((unsigned char)pixel);
        }
    LARGE_INTEGER frequency,start,finish;
    QueryPerformanceFrequency(&frequency);
    volatile int accepted=0;
    QueryPerformanceCounter(&start);
    for(int repeat=0;repeat<100;repeat++)accepted+=BeforeIsSafeIndexedSpriteRle(&sprite[0],512,512);
    QueryPerformanceCounter(&finish);
    double before=1000.0*(finish.QuadPart-start.QuadPart)/frequency.QuadPart;
    QueryPerformanceCounter(&start);
    for(repeat=0;repeat<100;repeat++)accepted+=IsSafeIndexedSpriteRle(&sprite[0],512,512);
    QueryPerformanceCounter(&finish);
    double after=1000.0*(finish.QuadPart-start.QuadPart)/frequency.QuadPart;
    Check(accepted==200,"benchmark accepted all sprites");
    printf("before_ms=%.3f after_ms=%.3f speedup=%.2fx failures=%d\n",before,after,before/after,failures);
    return failures ? 1 : 0;
}
