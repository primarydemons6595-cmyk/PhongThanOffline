#ifndef _SERVER
static void PtSoulDescription(const KItem& item,char* text){
 PT_SOUL_STATE soul;int param=PtSoulInnerParam(item.GetParam());if(!PtSoulRead(param,soul)){if(PtSoulParam(param))PtSoulRequest(param);return;}
 const PTSoulFormula* f=PtSoulFormula(soul.Formula);if(!f)return;
 char line[320];sprintf(line,"\x0a\x3c\x63\x6f\x6c\x6f\x72\x3d\x63\x79\x61\x6e\x3e\x25\x73\x3c\x63\x6f\x6c\x6f\x72\x3e\x0a\x3c\x63\x6f\x6c\x6f\x72\x3d\x25\x73\x3e\x4c\x69\x6e\x68\x20\x6b\x68\x3f\x3a\x20\x25\x64\x2f\x25\x64\x20\x28\x2d\x25\x64\x2f\x6c\x3f\x6e\x29\x3c\x63\x6f\x6c\x6f\x72\x3e\x0a\x59\x3f\x75\x20\x63\x3f\x75\x3a\x20\x63\x3f\x70\x20\x25\x64\x0a",f->name,soul.Aura>=f->cost?"orange":"red",soul.Aura,f->capacity,f->cost,f->requiredLevel);strcat(text,line);
 if(soul.MagicType){KItemNormalAttrib attr;ZeroMemory(&attr,sizeof(attr));attr.nAttribType=soul.MagicType;attr.nValue[0]=soul.MagicValue;const char* desc=g_MagicDesc.GetDesc(&attr);if(desc){strcat(text,"<color=yellow>");strcat(text,desc);strcat(text,"<color>\n");}}
}
#endif
