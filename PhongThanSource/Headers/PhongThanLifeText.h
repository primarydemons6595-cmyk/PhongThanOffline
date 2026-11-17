#ifndef PHONGTHAN_LIFE_TEXT_H
#define PHONGTHAN_LIFE_TEXT_H

#include <string.h>
#include "../Sources/Engine/Src/Text.h"

// Original INI strings use a localization marker and Engine markup, including
// <c=yel>, <c> and the literal four-character <\n> token.
inline int PhongThanEncodeLifeText(char* text) {
    if (!text) return 0;
    char* begin = text;
    if (*begin == '$' || *begin == '#') {
        ++begin;
        while (*begin == ' ' || *begin == '\t') ++begin;
        memmove(text, begin, strlen(begin) + 1);
    }
    return TEncodeText(text, (int)strlen(text));
}

#endif
