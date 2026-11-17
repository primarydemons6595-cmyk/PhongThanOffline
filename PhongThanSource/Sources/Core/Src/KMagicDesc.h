#ifndef KMagicDescH
#define	KMagicDescH

#include "KIniFile.h"
#include "KMagicAttrib.h"

class KMagicDesc
{
private:
	enum
	{
		MAGIC_DESC_COUNT = magic_normal_end,
		MAGIC_DESC_LENGTH = 256
	};
	// Template slots follow canonical attribute IDs, independently of INI row count.
	char		m_aryTemplate[MAGIC_DESC_COUNT][MAGIC_DESC_LENGTH];
	int		m_nTemplateCount;
	char		m_szDesc[256];
public:
	KMagicDesc();
	~KMagicDesc();
	BOOL		Init();
	const char*	GetDesc(void* pData);
	int	String2MagicID(char * szMagicAttribName);
};

extern KMagicDesc	g_MagicDesc;
const char* g_MagicID2String(int nAttrib);
int g_String2MagicID(char* szMagicAttribName);
#endif
