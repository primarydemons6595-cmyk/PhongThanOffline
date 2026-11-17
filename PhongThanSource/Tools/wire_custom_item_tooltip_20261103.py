from pathlib import Path
p=Path(__file__).resolve().parents[1]/'Sources/Core/Src/KItem.cpp';d=p.read_bytes();a=b'void KItem::GetDesc(char* pszMsg, bool bShowPrice, bool bPriceScale, int nActiveAttrib, int nGoldActiveAttrib)';assert d.count(a)==1
d=d.replace(a,b'#include "PhongThanCustomItemTooltip.inl"\n'+a)
a=b'if (m_CommonAttrib.szIntro[0])';assert d.count(a)==1;d=d.replace(a,b'const char* customEventIntro=PhongThanCustomItemTooltip(m_CommonAttrib.nParam);\n\tif (customEventIntro || m_CommonAttrib.szIntro[0])')
a=b'const char* pszIntro = SkipVngDisplayPrefix(m_CommonAttrib.szIntro);';assert d.count(a)==1;d=d.replace(a,b'const char* pszIntro = customEventIntro ? customEventIntro : SkipVngDisplayPrefix(m_CommonAttrib.szIntro);')
# Existing wrap buffer is too short for the new configurable intro text.
d=d.replace(b'char szIntro[SZBUFLEN_1];',b'char szIntro[1600];')
p.write_bytes(d);print('Wired instance-only custom tooltip text with game colour tags')
