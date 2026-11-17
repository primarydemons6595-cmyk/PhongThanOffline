#include "../../../Headers/PhongThanNpcNameColor.h"
// Keep original GBK identity for server Lua lookups. Only translate the wire
// display name to the character page supported by the current VNG font.
static const char* PhongThanRestoredNpcDisplayName(const char* original)
{
    static KTabFile names;
    static BOOL initialized = FALSE;
    static char display[32];
    if (!initialized)
    {
        names.Load("\\settings\\phongthan\\NpcDisplayNames.txt");
        initialized = TRUE;
    }
    display[0] = 0;
    if (original && original[0])
        names.GetString((char*)original, "DisplayName", "", display, sizeof(display));
    if (!display[0] && original && original[0])
    {
        char plain[128]; int count = 0;
        for (int i = original[0] == '#' ? 1 : 0; original[i] && count < 127; ++i)
        {
            if (original[i] == '<')
            {
                while (original[i] && original[i] != '>') ++i;
                if (!original[i]) break;
            }
            else plain[count++] = original[i];
        }
        while (count > 0 && plain[count - 1] == ' ') --count;
        plain[count] = 0;
        names.GetString(plain, "DisplayName", "", display, sizeof(display));
    }
    if(display[0] && original){
        char plain[128];unsigned int color=0;
        if(PhongThanParseNpcName(original,plain,sizeof(plain),color)){
            // Translation must not discard the author's explicit name colour.
            char translated[32];unsigned int ignored=0;PhongThanParseNpcName(display,translated,sizeof(translated),ignored);
            const char* token=0;
            switch(color&0xffffffU){case 0x00ff00:token="g";break;case 0xff0000:token="r";break;case 0x6464ff:token="b";break;case 0xffff00:token="y";break;case 0xffffff:token="w";break;}
            char prefix[16];if(token)sprintf(prefix,"<c=%s>",token);else sprintf(prefix,"<c=#%06x>",color&0xffffffU);
            g_StrCpyLen(display,prefix,sizeof(display));int used=strlen(display);g_StrCpyLen(display+used,translated,sizeof(display)-used);
        }
    }
    return display[0] ? display : NULL;
}
