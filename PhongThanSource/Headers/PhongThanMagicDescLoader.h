#ifndef PHONGTHAN_MAGIC_DESC_LOADER_H
#define PHONGTHAN_MAGIC_DESC_LOADER_H
#include <string.h>

// Parse writable INI bytes into canonical numeric slots. Duplicate keys
// replace their previous description without allocating a second ID.
inline int PhongThanLoadMagicDesc(char* data, char (*output)[256], int capacity,
    int (*resolve)(char*))
{
    if (!data || !output || !resolve || capacity <= 0 || capacity > 512) return -1;
    unsigned char seen[512]; memset(seen, 0, sizeof(seen));
    memset(output, 0, capacity * 256);
    int count=0; bool inSection=false;
    char* cursor=data;
    while (*cursor) {
        char* line=cursor;
        while (*cursor && *cursor!='\r' && *cursor!='\n') ++cursor;
        if (*cursor) { *cursor++=0; while (*cursor=='\r' || *cursor=='\n') ++cursor; }
        while (*line==' ' || *line=='\t') ++line;
        char* tail=line+strlen(line);
        while (tail>line && (tail[-1]==' ' || tail[-1]=='\t')) *--tail=0;
        if (!*line || *line==';' || (line[0]=='/' && line[1]=='/')) continue;
        if (*line=='[') {
            if (!_stricmp(line,"[Descript]")) inSection=true;
            else if (inSection) break;
            continue;
        }
        if (!inSection) continue;
        char* value=strchr(line,'='); if (!value) continue;
        *value++=0; tail=line+strlen(line);
        while (tail>line && (tail[-1]==' ' || tail[-1]=='\t')) *--tail=0;
        const int id=resolve(line);
        if (id<0 || id>=capacity) return -1;
        if (!seen[id]) { seen[id]=1; ++count; }
        strncpy(output[id],value,255); output[id][255]=0;
    }
    // Missing descriptions are empty slots; newer IDs must not erase older text.
    return count;
}
#endif
