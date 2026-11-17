// Standalone fixture owns one engine package-list global. The import-DLL
// version would validate a different engine root than the static loader.
#include "SprLoaderAudit.cpp"
KPakList* g_pPakList=NULL;
