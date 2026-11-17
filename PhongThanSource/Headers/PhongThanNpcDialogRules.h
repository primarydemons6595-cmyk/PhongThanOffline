#ifndef PHONGTHAN_NPC_DIALOG_RULES_H
#define PHONGTHAN_NPC_DIALOG_RULES_H
// Question Aux is otherwise unused when BooleanArgument (portrait) is false.
// Old servers and ordinary Say questions default to the original list UI.
enum { PHONGTHAN_DIALOG_BUTTONS = -20564 };
inline char* PhongThanDialogCallbackSeparator(char* option,int presentation)
{
    return presentation==PHONGTHAN_DIALOG_BUTTONS?strrchr(option,'/'):strchr(option,'/');
}
inline bool PhongThanDialogUsesButtons(int auxiliary, int portrait)
{
    return !portrait && auxiliary == PHONGTHAN_DIALOG_BUTTONS;
}
// Hide presentation-only close choices; retain original Lua answer indexes.
inline bool PhongThanIsDialogExit(const char* text, int length = -1)
{
    char plain[80]; int n = 0;
    if (length < 0) length = text ? (int)strlen(text) : 0;
    for (int i = 0; text && i < length && n < 79; ++i) {
        unsigned char c = (unsigned char)text[i];
        if (c == 2 || c == 4) { if (i + 3 >= length) return false; i += 3; continue; }
        if (c == 3 || c == 5) continue;
        if (c == '<') { while (i < length && text[i] != '>') ++i; if (i == length) break; continue; }
        if (c >= 'A' && c <= 'Z') c += 'a' - 'A';
        if (c == 0xb8) c = 'a'; // TCVN3: Thoat, with acute a.
        if (c == 0xae || c == 0xa7) c = 'd';
        if (c == 0xe3) c = 'o';
        if (c == 0xe8) c = 'o';
        if (c == 0xb9) c = 'a';
        if (c == 0xd5) c = 'e';
        if (c == 0xf3) c = 'u';
        if (c != ' ' && c != '\t' && c != '\r' && c != '\n') plain[n++] = (char)c;
    }
    plain[n] = 0;
    return !strcmp(plain,"thoat") || !strcmp(plain,"thoat(esc)") ||
        !strcmp(plain,"dong") || !strcmp(plain,"dong(esc)") || !strcmp(plain,"ketthuc") || !strcmp(plain,"ketthucdoithoai");
}
#endif
