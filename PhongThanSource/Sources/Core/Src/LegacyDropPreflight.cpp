// Inert diagnostics compiled into both Cores. No death hook or item/world writes.
#include "KCore.h"
#ifdef _SERVER
#include "LegacyDropEngine.h"
#include "LegacyDropItemAdapter.h"
extern "C" __declspec(dllexport) int __cdecl LegacyDropPreflight(
    const char* bytes,unsigned int length,unsigned long seed,int draws,
    int* count,int* range,unsigned long* total,unsigned long* trace)
{
    if(!count||!range||!total||!trace)return -1;
    *count=*range=0;*total=0;*trace=2166136261UL;
    if(!bytes||length>2*1024*1024||draws<0||draws>10000)return -1;
    LegacyDrop::Table table=LegacyDrop::Parse(std::string(bytes,length));
    LegacyDrop::PreparedTable prepared;std::string error;
    if(!prepared.Prepare(table,error))return 0;
    *count=table.count;*range=table.range;LegacyDrop::Random rng(seed);std::vector<int> selection;
    for(int d=0;d<draws;++d){if(!prepared.Draw(rng,selection,error))return -2;
        *total+=(unsigned long)selection.size();
        *trace=(*trace^(unsigned long)selection.size())*16777619UL;
        for(size_t i=0;i<selection.size();++i)*trace=(*trace^(unsigned long)(selection[i]+1))*16777619UL;
    }
    return 1;
}
extern "C" __declspec(dllexport) int __cdecl LegacyDropResolvePath(
    const char* bytes,unsigned int length,char* output,unsigned int capacity)
{
    if(!output||!capacity)return -1;output[0]=0;if(!bytes||length>=240)return -1;
    LegacyDrop::NpcSnapshot npc;npc.slots[0]=std::string(bytes,length);std::string resolved;
    if(!LegacyDrop::Resolve(npc,0,resolved))return 0;
    if(resolved.size()+1>capacity)return -1;memcpy(output,resolved.c_str(),resolved.size()+1);return 1;
}
extern "C" __declspec(dllexport) int __cdecl LegacyDropMapInitializedCatalog(
    int genre,int detail,int particular,int* nativeDetail,int* nativeParticular)
{
    // Intentionally requires the host's normal item catalog initialization.
    if(!nativeDetail||!nativeParticular)return -1;*nativeDetail=*nativeParticular=-1;
    if(ItemGen.Catalog().GetMagicScriptRecordNumber()<=0)return -2;
    LegacyDropItemAdapter adapter;if(!adapter.Init(ItemGen))return -2;
    PhongThanQuestItemTuple tuple;if(!adapter.Map(genre,detail,particular,tuple))return 0;
    *nativeDetail=tuple.detail;*nativeParticular=tuple.particular;return 1;
}
#endif
