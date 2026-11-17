#ifndef PT_SECURE_RELEASE_H
#define PT_SECURE_RELEASE_H
#include "../../../Headers/PhongThanDiagnostics.h"
// No class-layout changes: virtual stdio handles are owned entirely by Engine.
ENGINE_API bool PtReleaseEnabled();
ENGINE_API bool PtReleaseMount();
// Selects the catalog namespace used by subsequent PtRelease calls:
// the root folder whose Data\\release.catalog chain is mounted and
// searched. NULL or empty restores the exe folder (client chain). An
// embedded server switches this to its ServerData root while it runs.
ENGINE_API void PtReleaseSetContextRoot(const char* root);
ENGINE_API bool PtReleaseFind(unsigned long id, unsigned long* length);
ENGINE_API bool PtReleaseLocate(unsigned long id, const char* name, unsigned long* entry, unsigned long* length);
ENGINE_API unsigned long PtReleaseReadEntry(unsigned long entry, unsigned long offset, void* buffer, unsigned long length);
ENGINE_API unsigned long PtReleaseReadId(unsigned long id, unsigned long offset, void* buffer, unsigned long length);
ENGINE_API FILE* PtReleaseFopen(const char* path, const char* mode);
ENGINE_API FILE* PtReleaseFreopen(const char* path, const char* mode, FILE* stream);
ENGINE_API size_t PtReleaseFread(void* buffer, size_t size, size_t count, FILE* stream);
ENGINE_API int PtReleaseFclose(FILE* stream);
ENGINE_API int PtReleaseFseek(FILE* stream, long offset, int origin);
ENGINE_API long PtReleaseFtell(FILE* stream);
ENGINE_API char* PtReleaseFgets(char* line, int count, FILE* stream);
ENGINE_API int PtReleaseFgetc(FILE* stream);
ENGINE_API int PtReleaseFeof(FILE* stream);
ENGINE_API int PtReleaseFerror(FILE* stream);
ENGINE_API bool PtReleaseIsVirtual(FILE* stream);
ENGINE_API bool PtReleaseOpenVirtual(const char* path, const char* mode, FILE** stream);
ENGINE_API bool PtReleaseQuietPath(const char* path);
ENGINE_API unsigned long PtReleaseNextName(unsigned long cursor, const char* prefix, char* name, unsigned long capacity);

#if defined(PT_SECURE_RELEASE_PROFILE) && !defined(PT_RELEASE_IMPLEMENTATION) && !defined(_STANDALONE) && !defined(__linux)
// Real FILE* must stay in the caller's CRT: the legacy services mix /MT and /MD.
// Only Engine-owned virtual streams cross the DLL boundary.
#ifdef fopen
#undef fopen
#endif
inline FILE* PtLocalFopen(const char* path,const char* mode){FILE* stream=NULL;if(PtReleaseOpenVirtual(path,mode,&stream))return stream;return fopen(PtReleaseQuietPath(path)?"NUL":path,mode);}
inline FILE* PtLocalFreopen(const char* path,const char* mode,FILE* stream){return freopen(PtReleaseQuietPath(path)?"NUL":path,mode,stream);}
inline size_t PtLocalFread(void* p,size_t size,size_t count,FILE* stream){return PtReleaseIsVirtual(stream)?PtReleaseFread(p,size,count,stream):fread(p,size,count,stream);}
inline int PtLocalFclose(FILE* stream){return PtReleaseIsVirtual(stream)?PtReleaseFclose(stream):fclose(stream);}
inline int PtLocalFseek(FILE* stream,long offset,int origin){return PtReleaseIsVirtual(stream)?PtReleaseFseek(stream,offset,origin):fseek(stream,offset,origin);}
inline long PtLocalFtell(FILE* stream){return PtReleaseIsVirtual(stream)?PtReleaseFtell(stream):ftell(stream);}
inline char* PtLocalFgets(char* line,int count,FILE* stream){return PtReleaseIsVirtual(stream)?PtReleaseFgets(line,count,stream):fgets(line,count,stream);}
inline int PtLocalFgetc(FILE* stream){return PtReleaseIsVirtual(stream)?PtReleaseFgetc(stream):fgetc(stream);}
inline int PtLocalFeof(FILE* stream){return PtReleaseIsVirtual(stream)?PtReleaseFeof(stream):feof(stream);}
inline int PtLocalFerror(FILE* stream){return PtReleaseIsVirtual(stream)?PtReleaseFerror(stream):ferror(stream);}
#define fopen PtLocalFopen
#define freopen PtLocalFreopen
#define fread PtLocalFread
#define fclose PtLocalFclose
#define fseek PtLocalFseek
#define ftell PtLocalFtell
#define fgets PtLocalFgets
#define fgetc PtLocalFgetc
#ifdef feof
#undef feof
#endif
#ifdef ferror
#undef ferror
#endif
#define feof PtLocalFeof
#define ferror PtLocalFerror
#elif !defined(PT_RELEASE_IMPLEMENTATION) && !defined(_STANDALONE) && !defined(__linux)
#ifdef fopen
#undef fopen
#endif
#define fopen PtDiagnosticsFopen
#endif
#endif
