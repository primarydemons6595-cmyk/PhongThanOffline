#ifndef PHONGTHAN_NPC_DIALOG_BINDING_H
#define PHONGTHAN_NPC_DIALOG_BINDING_H
#include <string.h>
// DialogNpc checks the path before dispatching by hash. Always bind both.
template<class NpcType>
inline bool PhongThanBindNpcDialogScript(NpcType& npc,const char* path,unsigned long hash)
{
    if(!path || !path[0] || !hash || strlen(path)>=sizeof(npc.ActionScript))return false;
    strcpy(npc.ActionScript,path);
    npc.m_ActionScriptID=hash;
    return true;
}
#endif
