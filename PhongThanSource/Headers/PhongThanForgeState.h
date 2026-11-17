#ifndef PHONGTHAN_FORGE_STATE_H
#define PHONGTHAN_FORGE_STATE_H
// Equipment-only ScriptParam namespace. Preserve the existing item/save/wire ABI.
// Other genres and ordinary script parameters retain their original meaning.
inline int PhongThanForgeRecipe(int param){return ((unsigned)param&0xffff0000U)==0xc17f0000U?param&65535:0;}
inline int PhongThanForgeParam(int recipe){return recipe>0&&recipe<=65535?(int)(0xc17f0000U|recipe):0;}
#endif
