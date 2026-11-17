//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KPakList.cpp
// Date:	2000.08.08
// Code:	WangWei(Daphnis)
// Desc:	Pack Data List Class
//---------------------------------------------------------------------------
#include <windows.h>
#include "KWin32.h"
#include "KDebug.h"
#include "KFilePath.h"
#include "KIniFile.h"
#include "KPakList.h"
#include "crtdbg.h"
#include "KLooseFile.h"
#include "KLooseFile.inl"

//---------------------------------------------------------------------------
ENGINE_API KPakList* g_pPakList = NULL;

//---------------------------------------------------------------------------
// ????????????:	??????????????????
//---------------------------------------------------------------------------
KPakList::KPakList()
{
	g_pPakList = this;
	m_nPakNumber = 0;
	m_nPtMode = 0;
}

//---------------------------------------------------------------------------
// ????????????:	??????????????????
//---------------------------------------------------------------------------
KPakList::~KPakList()
{
	Close();
}

//---------------------------------------------------------------------------
// ????????????:	????????????????????????????
//---------------------------------------------------------------------------
void KPakList::Close()
{
	m_nPtMode = 0;
	PtLooseClose(this);
	for (int i = 0; i < m_nPakNumber; i++)
		delete m_PakFilePtrList[i];
	m_nPakNumber = 0;
}

//---------------------------------------------------------------------------
// ????????????:	??????????????????????????????????????????????????
// ????????????:	uId			??????????????ID
//			ElemRef		?????????????????????????????????????????????????????
// ????????????:	?????????????????????
//---------------------------------------------------------------------------
bool KPakList::FindElemFile(unsigned long uId, XPackElemFileRef& ElemRef)
{
	if (m_nPtMode == 1 || (m_nPtMode == 0 && PtReleaseEnabled())) { unsigned long size=0,entry=0; if(!PtReleaseLocate(uId,NULL,&entry,&size)) return false; memset(&ElemRef,0,sizeof(ElemRef)); ElemRef.uId=uId; ElemRef.nPackIndex=-4; ElemRef.nElemIndex=entry; ElemRef.nSize=size; return true; }
	if (m_nPtMode == 2 || (m_nPtMode == 0 && PtLooseOnlyEnabled())) return PtLooseFind(this, uId, ElemRef);
	bool bFounded = false;
	for (int i = 0; i < m_nPakNumber; i++)
	{
		if (m_PakFilePtrList[i]->FindElemFile(uId, ElemRef))
		{
			bFounded = true;
			break;
		}
	}
	return bFounded;
}

//---------------------------------------------------------------------------
// ????????????:	????????????????????????????????????????????id
// ????????????:	pszFileName	??????????????
// ????????????:	??????????????????????????????????????id
//---------------------------------------------------------------------------
unsigned long KPakList::FileNameToId(const char* pszFileName)
{
	_ASSERT(pszFileName && pszFileName[0]);
	unsigned long id = 0;
	const char *ptr = pszFileName;
	int index = 0;
	while(*ptr)
	{
		if(*ptr >= 'A' && *ptr <= 'Z') id = (id + (++index) * (*ptr + 'a' - 'A')) % 0x8000000b * 0xffffffef;
		else id = (id + (++index) * (*ptr)) % 0x8000000b * 0xffffffef;
		ptr++;
	}
	return (id ^ 0x12345678);
}

//---------------------------------------------------------------------------
// ????????????:	??????????????????????????????????????????????????
// ????????????:	pszFileName	??????????????
//			ElemRef	?????????????????????????????????????????????????????
// ????????????:	?????????????????????
//---------------------------------------------------------------------------
bool KPakList::FindElemFile(const char* pszFileName, XPackElemFileRef& ElemRef)
{
	if (m_nPtMode == 1 || (m_nPtMode == 0 && PtReleaseEnabled()))
	{
		if(!pszFileName || !pszFileName[0]) return false;
		char pack[MAX_PATH], current[MAX_PATH]; g_GetFilePath(current);
		if(strlen(pszFileName)+strlen(current)+2>=sizeof(pack)) return false;
		pack[0]='\\'; g_GetPackPath(pack+1,(char*)pszFileName);
		unsigned long size=0,entry=0;if(!PtReleaseLocate(0,pack,&entry,&size))return false;
		memset(&ElemRef,0,sizeof(ElemRef));ElemRef.uId=FileNameToId(pack);ElemRef.nPackIndex=-4;ElemRef.nElemIndex=entry;ElemRef.nSize=size;return true;
	}
	if (m_nPtMode == 2 || (m_nPtMode == 0 && PtLooseOnlyEnabled()))
	{
		if (!pszFileName || !pszFileName[0]) return false;
		char pack[MAX_PATH], current[MAX_PATH];
		g_GetFilePath(current);
		if (strlen(pszFileName) + strlen(current) + 2 >= sizeof(pack))
		{
			PtLooseMissing("FindElemFile/path_too_long", pszFileName, 0);
			return false;
		}
		pack[0] = '\\';
		g_GetPackPath(pack + 1, (char*)pszFileName);
		unsigned long id = FileNameToId(pack);
		bool found = PtLooseFind(this, id, ElemRef);
		if (!found) PtLooseMissing("FindElemFile/name", pack, id);
		return found;
	}
	bool bFounded = false;
	if (pszFileName && pszFileName[0])
	{
		char szPackName[128];
		#ifdef WIN32
			szPackName[0] = '\\';
		#else
			szPackName[0] = '/';
		#endif
		g_GetPackPath(szPackName + 1, (char*)pszFileName);
		unsigned long uId = FileNameToId(szPackName);
		bFounded = FindElemFile(uId, ElemRef);
	}
	return bFounded;
}

//--------------------------------------------------------------------
// ????????????:	Open package ini file
// ????????????:	char* filename
// ????????????:	BOOL
//---------------------------------------------------------------------------
bool KPakList::Open(const char* pPakListFile)
{
	Close();
	// The data mode is pinned per instance, decided by the root that is
	// active while Open() runs. A standalone process holds one instance
	// under one root, so its behavior is unchanged. An embedded (offline)
	// process holds several instances under different roots: the client
	// instances open under the game folder (secure release catalog), the
	// embedded server instance opens under ServerData (loose_only.flag),
	// and each lookup then follows its own instance's mode instead of the
	// process-global release state, which can only ever mount one role.
	if (PtLooseOnlyEnabled()) { m_nPtMode = 2; return PtLooseOpen(this); }
	if (PtReleaseEnabled()) { m_nPtMode = 1; g_pPakList = this; return PtReleaseMount(); }
	m_nPtMode = 3;

	KIniFile IniFile;
	#define	SECTION "Package"

	bool bResult = false;
	if (IniFile.Load(pPakListFile))
	{
		char	szBuffer[32], szKey[16], szFile[MAX_PATH];

		if (IniFile.GetString(SECTION, "Path", "", szBuffer, sizeof(szBuffer)))
		{
			g_GetFullPath(szFile, szBuffer);
			int nNameStartPos = strlen(szFile);
			if (szFile[nNameStartPos - 1] != '\\' || szFile[nNameStartPos - 1] != '/')
			{
				#ifdef WIN32
					szFile[nNameStartPos++] = '\\';
				#else
					szFile[nNameStartPos++] = '/';
				#endif
				szFile[nNameStartPos] = 0;
			}

			for (int i = 0; i < MAX_PAK; i++)
			{
				if (m_nPakNumber >= MAX_PAK)
					break;
				itoa(i, szKey, 10);
				if (!IniFile.GetString(SECTION, szKey, "", szBuffer, sizeof(szBuffer)))
					break;
				if (szBuffer[0] == 0)
					break;
				strcpy(szFile + nNameStartPos, szBuffer);
				m_PakFilePtrList[m_nPakNumber] = new XPackFile;
				if (m_PakFilePtrList[m_nPakNumber])
				{
					if (m_PakFilePtrList[m_nPakNumber]->Open(szFile, m_nPakNumber))
					{
						m_nPakNumber++;
						//g_DebugLog("PakList Open : %s ... Ok", szFile);
					}
					else
					{
						//g_DebugLog("PakList Open : %s ... Fail", szFile);
						delete (m_PakFilePtrList[m_nPakNumber]);
					}
				}
			}
			bResult = true;
		}
	}
	return bResult;
}

//????????????????????????????????????
int KPakList::ElemFileRead(XPackElemFileRef& ElemRef,
					void* pBuffer, unsigned uSize)
{
	if (m_nPtMode == 1 || (m_nPtMode == 0 && PtReleaseEnabled())) { unsigned long n=PtReleaseReadEntry(ElemRef.nElemIndex,ElemRef.nOffset,pBuffer,uSize); ElemRef.nOffset+=n; return n; }
	if (m_nPtMode == 2 || (m_nPtMode == 0 && PtLooseOnlyEnabled())) return PtLooseRead(this, ElemRef, pBuffer, uSize);
	if (ElemRef.nPackIndex >= 0 && ElemRef.nPackIndex < m_nPakNumber)
		return m_PakFilePtrList[ElemRef.nPackIndex]->ElemFileRead(ElemRef, pBuffer, uSize);
	return 0;
}

//????????spr??????????????????????????????????spr
SPRHEAD* KPakList::GetSprHeader(XPackElemFileRef& ElemRef, SPROFFS*& pOffsetTable)
{
	if (ElemRef.nPackIndex >= 0 && ElemRef.nPackIndex < m_nPakNumber)
		return (m_PakFilePtrList[ElemRef.nPackIndex]->GetSprHeader(ElemRef, pOffsetTable));
	return NULL;
}

//??????????????????????????????spr????????????????????????????
SPRFRAME* KPakList::GetSprFrame(int nPackIndex, SPRHEAD* pSprHeader, int nFrame)
{
	if (nPackIndex >= 0 && nPackIndex < m_nPakNumber)
		return m_PakFilePtrList[nPackIndex]->GetSprFrame(pSprHeader, nFrame);
	return NULL;
}


