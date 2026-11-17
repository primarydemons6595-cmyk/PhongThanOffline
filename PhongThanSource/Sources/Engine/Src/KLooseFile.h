#ifndef PT_LOOSE_FILE_H
#define PT_LOOSE_FILE_H

// Engine-internal helpers: no exported class layout / ABI changes.
bool PtLooseOnlyEnabled();
void PtLooseMissing(const char* caller, const char* path, unsigned long id);
bool PtLooseAliasPath(KPakList* owner, const XPackElemFileRef& ref, char* path);

#endif
