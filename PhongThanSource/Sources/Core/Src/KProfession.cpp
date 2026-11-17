#include "KCore.h"
#include "KProfession.h"

KProfession g_Profession;

BOOL KProfession::Init()
{
	static const char *s_Keys[PHONGTHAN_PROFESSION_COUNT] =
	{
		"giapsi", "daosi", "dinhan", "vusi"
	};
	static const char *s_Names[PHONGTHAN_PROFESSION_COUNT] =
	{
		"\x47\x69\xB8\x70\x20\x53\xDC", "\xA7\xB9\x6F\x20\x53\xDC", "\x44\xDE\x20\x4E\x68\xA9\x6E", "\x56\xF2\x20\x53\xDC"
	};

	for (int i = 0; i < PHONGTHAN_PROFESSION_COUNT; ++i)
	{
		m_Profession[i].m_nId = i;
		m_Profession[i].m_nCamp = camp_begin;
		strcpy(m_Profession[i].m_szKey, s_Keys[i]);
		strcpy(m_Profession[i].m_szName, s_Names[i]);
	}
	return TRUE;
}

BOOL KProfession::IsValid(int nProfession) const
{
	return nProfession >= 0 && nProfession < PHONGTHAN_PROFESSION_COUNT;
}

int KProfession::GetID(const char *pszName) const
{
	if (!pszName || !pszName[0])
		return PHONGTHAN_PROFESSION_INVALID;
	for (int i = 0; i < PHONGTHAN_PROFESSION_COUNT; ++i)
	{
		if (stricmp(pszName, m_Profession[i].m_szKey) == 0 ||
			stricmp(pszName, m_Profession[i].m_szName) == 0)
			return i;
	}
	return PHONGTHAN_PROFESSION_INVALID;
}

int KProfession::GetCamp(int nProfession) const
{
	return IsValid(nProfession) ? m_Profession[nProfession].m_nCamp : camp_begin;
}

const char *KProfession::GetKey(int nProfession) const
{
	return IsValid(nProfession) ? m_Profession[nProfession].m_szKey : "";
}

const char *KProfession::GetName(int nProfession) const
{
	return IsValid(nProfession) ? m_Profession[nProfession].m_szName : "";
}
