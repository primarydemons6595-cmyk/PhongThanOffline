#include "stdafx.h"
#include "PhongThanCharacterStore.h"
#include "../../../Headers/PhongThanAccountStorage.h"
#include "../../../Headers/PhongThanMagicRetention.h"
#include "../../../Headers/PhongThanCharacterBundle.h"
#include "../../../Headers/PhongThanDiagnostics.h"
#include <vector>
#include <set>

#include <stdio.h>
#include <string.h>

#ifndef INVALID_FILE_ATTRIBUTES
#define INVALID_FILE_ATTRIBUTES ((DWORD)-1)
#endif

#pragma pack(push, 1)
struct PHONGTHAN_CHARACTER_FILE_HEADER
{
	DWORD Magic;
	WORD FormatVersion;
	WORD HeaderSize;
	DWORD StateSize;
	DWORD Checksum;
};
#pragma pack(pop)

enum
{
	PHONGTHAN_CHARACTER_FILE_MAGIC = 0x43544850, // PHTC
	PHONGTHAN_CHARACTER_FILE_VERSION = 1
};

CPhongThanCharacterStore g_PhongThanCharacterStore;

static DWORD PhongThanCharacterChecksum(const BYTE* pData, DWORD nSize)
{
	DWORD nHash = 2166136261u;
	for (DWORD i = 0; i < nSize; ++i)
	{
		nHash ^= pData[i];
		nHash *= 16777619u;
	}
	return nHash;
}

static bool PhongThanReadAll(HANDLE hFile, void* pData, DWORD nSize)
{
	BYTE* pCurrent = (BYTE*)pData;
	while (nSize)
	{
		DWORD nRead = 0;
		if (!ReadFile(hFile, pCurrent, nSize, &nRead, NULL) || !nRead)
			return false;
		pCurrent += nRead;
		nSize -= nRead;
	}
	return true;
}

static bool PhongThanWriteAll(HANDLE hFile, const void* pData, DWORD nSize)
{
	const BYTE* pCurrent = (const BYTE*)pData;
	while (nSize)
	{
		DWORD nWritten = 0;
		if (!WriteFile(hFile, pCurrent, nSize, &nWritten, NULL) || !nWritten)
			return false;
		pCurrent += nWritten;
		nSize -= nWritten;
	}
	return true;
}

CPhongThanCharacterStore::CPhongThanCharacterStore()
{
	ZeroMemory(m_szRoot, sizeof(m_szRoot));
	InitializeCriticalSection(&m_Lock);
}

CPhongThanCharacterStore::~CPhongThanCharacterStore()
{
	DeleteCriticalSection(&m_Lock);
}

bool CPhongThanCharacterStore::Open(const char* pRootDirectory)
{
	EnterCriticalSection(&m_Lock);
	const char* pRoot = pRootDirectory && pRootDirectory[0] ?
		pRootDirectory : "CharacterStore";
	const size_t nLength = strlen(pRoot);
	bool bResult = nLength < sizeof(m_szRoot);
	if (bResult)
	{
		strcpy(m_szRoot, pRoot);
		bResult = EnsureRoot();
	}
	LeaveCriticalSection(&m_Lock);
	return bResult;
}

bool CPhongThanCharacterStore::EnsureRoot()
{
	if (!m_szRoot[0])
		strcpy(m_szRoot, "CharacterStore");
	const DWORD nAttributes = GetFileAttributesA(m_szRoot);
	if (nAttributes != INVALID_FILE_ATTRIBUTES)
		return (nAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
	return CreateDirectoryA(m_szRoot, NULL) != FALSE ||
		GetLastError() == ERROR_ALREADY_EXISTS;
}

bool CPhongThanCharacterStore::BuildRolePath(const char* role,char* output,size_t capacity) const
{
    std::string name;if(!PtRoleFileName(role,name))return false;
    // Global role names remain unique. Prefer the account copy; never choose
    // between two conflicting account owners merely by enumeration order.
    std::string selected;WIN32_FIND_DATAA entry;
    HANDLE find=FindFirstFileA((std::string(m_szRoot)+"\\*").c_str(),&entry);
    if(find!=INVALID_HANDLE_VALUE){do{
        if(!(entry.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)||(entry.dwFileAttributes&FILE_ATTRIBUTE_REPARSE_POINT)||entry.cFileName[0]=='.')continue;
        std::string candidate=std::string(m_szRoot)+"\\"+entry.cFileName+"\\"+name;
        if(GetFileAttributesA(candidate.c_str())!=(DWORD)-1){if(!selected.empty()){FindClose(find);return false;}selected=candidate;}
    }while(FindNextFileA(find,&entry));FindClose(find);}
    if(selected.empty())selected=std::string(m_szRoot)+"\\"+name;
    if(selected.size()>=capacity)return false;strcpy(output,selected.c_str());return true;
}

bool CPhongThanCharacterStore::LoadPathUnlocked(const char* pPath,
	BYTE* pOutput, PHONGTHAN_U32 nCapacity, PHONGTHAN_U32* pStateSize)
{
	if (pStateSize)
		*pStateSize = 0;
	if (!pPath || !pOutput || !pStateSize || !EnsureRoot())
		return false;
    PtCharacterBundle::Bundle bundle;
    if(!PtCharacterBundle::Read(pPath,bundle)||bundle.state.size()>nCapacity)return false;
    memcpy(pOutput,bundle.state.data(),bundle.state.size());*pStateSize=bundle.state.size();
	return true;
}

bool CPhongThanCharacterStore::LoadUnlocked(const char* pRoleName,
	BYTE* pOutput, PHONGTHAN_U32 nCapacity, PHONGTHAN_U32* pStateSize)
{
	char szPath[MAX_PATH];
	if (!BuildRolePath(pRoleName, szPath, sizeof(szPath)) ||
		!LoadPathUnlocked(szPath, pOutput, nCapacity, pStateSize))
		return false;
	const PHONGTHAN_CHARACTER_STATE_HEADER* pState =
		(const PHONGTHAN_CHARACTER_STATE_HEADER*)pOutput;
	std::string accountRoot,name;if(!PtAccountDirectory(m_szRoot,(const char*)pState->AccountName,accountRoot)||!PtRoleFileName(pRoleName,name))return false;
    const std::string accountPath=accountRoot+"\\"+name,legacyPath=std::string(m_szRoot)+"\\"+name;
    return (!_stricmp(szPath,accountPath.c_str())||!_stricmp(szPath,legacyPath.c_str()))&&stricmp((const char*)pState->RoleName,pRoleName)==0;
}

bool CPhongThanCharacterStore::Load(const char* pRoleName, BYTE* pOutput,
	PHONGTHAN_U32 nCapacity, PHONGTHAN_U32* pStateSize)
{
	EnterCriticalSection(&m_Lock);
	const bool bResult = LoadUnlocked(pRoleName, pOutput, nCapacity, pStateSize);
	LeaveCriticalSection(&m_Lock);
	return bResult;
}

int CPhongThanCharacterStore::ListUnlocked(const char* pAccountName,
	PHONGTHAN_CHARACTER_SUMMARY* pOutput, int nCapacity)
{
	if (!pAccountName || !pAccountName[0] || nCapacity < 0 || !EnsureRoot())
		return -1;
	std::string accountRoot;if(!PtAccountDirectory(m_szRoot,pAccountName,accountRoot))return -1;
    const std::string roots[2]={accountRoot,m_szRoot};std::set<std::string> seen;int nCount=0;
    for(int phase=0;phase<2;++phase){const char* scanRoot=roots[phase].c_str();
	char szPattern[MAX_PATH];
	if (_snprintf(szPattern, sizeof(szPattern), "%s\\*.pthc", scanRoot) < 0)
		return -1;

	WIN32_FIND_DATAA FindData;
	HANDLE hFind = FindFirstFileA(szPattern, &FindData);
	if (hFind == INVALID_HANDLE_VALUE)
		{DWORD error=GetLastError();if(error==ERROR_FILE_NOT_FOUND||error==ERROR_PATH_NOT_FOUND)continue;return -1;}
	do
	{
		if (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			continue;
		char szPath[MAX_PATH];
		if (_snprintf(szPath, sizeof(szPath), "%s\\%s",
				scanRoot, FindData.cFileName) < 0)
			continue;
		BYTE* pStateBuffer = new BYTE[PHONGTHAN_CHARACTER_MAX_STATE_SIZE];
		PHONGTHAN_U32 nStateSize = 0;
		if (LoadPathUnlocked(szPath, pStateBuffer,
				PHONGTHAN_CHARACTER_MAX_STATE_SIZE, &nStateSize))
		{
			const PHONGTHAN_CHARACTER_STATE_HEADER* pState =
				(const PHONGTHAN_CHARACTER_STATE_HEADER*)pStateBuffer;
			std::string roleKey=(const char*)pState->RoleName;for(size_t k=0;k<roleKey.size();++k)if(roleKey[k]>='A'&&roleKey[k]<='Z')roleKey[k]+='a'-'A';
            if (stricmp((const char*)pState->AccountName, pAccountName) == 0 && seen.insert(roleKey).second)
			{
				if (pOutput && nCount < nCapacity)
				{
					ZeroMemory(&pOutput[nCount], sizeof(pOutput[nCount]));
					strncpy((char*)pOutput[nCount].Name,
						(const char*)pState->RoleName,
						sizeof(pOutput[nCount].Name) - 1);
					pOutput[nCount].Gender = pState->Gender;
					pOutput[nCount].Profession = pState->Profession;
					pOutput[nCount].Level = (PHONGTHAN_U16)pState->FightLevel;
                    pOutput[nCount].FightExperience = pState->FightExperience;
                    pOutput[nCount].MapId = pState->UseRevive ? pState->ReviveMapId : pState->EnterMapId;
                    pOutput[nCount].Portrait = pState->HelmResource;
                    pOutput[nCount].Transcendence = pState->Transcendence;
                    ULARGE_INTEGER stamp;
                    stamp.LowPart = FindData.ftLastWriteTime.dwLowDateTime;
                    stamp.HighPart = FindData.ftLastWriteTime.dwHighDateTime;
                    pOutput[nCount].SavedUtc = (PHONGTHAN_U32)
                        (stamp.QuadPart / 10000000 - 11644473600);
                    char metadata[MAX_PATH];
                    _snprintf(metadata, sizeof(metadata), "%s.login.ini", szPath);
                    metadata[sizeof(metadata)-1] = 0;
                    PtCharacterBundle::Bundle login;bool embedded=PtCharacterBundle::Read(szPath,login)&&login.unified;
                    pOutput[nCount].LastLoginUtc = embedded?login.loginUtc:GetPrivateProfileIntA("Login", "Utc", 0, metadata);
                    pOutput[nCount].LastLoginIP = embedded?login.loginIp:GetPrivateProfileIntA("Login", "IP", 0, metadata);


				}
				++nCount;
			}
		}
		delete [] pStateBuffer;
	} while (FindNextFileA(hFind, &FindData));
	FindClose(hFind);}
	return nCount;
}

int CPhongThanCharacterStore::List(const char* pAccountName,
	PHONGTHAN_CHARACTER_SUMMARY* pOutput, int nCapacity)
{
	EnterCriticalSection(&m_Lock);
	const int nResult = ListUnlocked(pAccountName, pOutput, nCapacity);
	LeaveCriticalSection(&m_Lock);
	return nResult;
}

bool CPhongThanCharacterStore::WriteUnlocked(
	const PHONGTHAN_CHARACTER_STATE_HEADER* pState,
	PHONGTHAN_U32 nStateSize, bool bReplaceExisting)
{
	if (!PhongThanValidateCharacterState(pState, nStateSize) || !EnsureRoot())
		return false;
    std::string directory,name;if(!PtAccountDirectory(m_szRoot,(const char*)pState->AccountName,directory,true)||!PtRoleFileName((const char*)pState->RoleName,name))return false;
    char szPath[MAX_PATH];if(directory.size()+name.size()+2>=sizeof(szPath))return false;
    sprintf(szPath,"%s\\%s",directory.c_str(),name.c_str());

    if(!bReplaceExisting){std::vector<BYTE> prior(PHONGTHAN_CHARACTER_MAX_STATE_SIZE);PHONGTHAN_U32 size=0;
        if(LoadUnlocked((const char*)pState->RoleName,&prior[0],prior.size(),&size))return false;}

	if (!bReplaceExisting &&
		GetFileAttributesA(szPath) != INVALID_FILE_ATTRIBUTES)
		return false;

    std::string error;bool bResult=PtCharacterBundle::CommitPath(m_szRoot,szPath,pState,bReplaceExisting,error);
    if(!bResult)PtRuntimeError("CharacterCommit",error.c_str());
	return bResult;
}

bool CPhongThanCharacterStore::Create(
	const PHONGTHAN_CHARACTER_STATE_HEADER* pState,
	PHONGTHAN_U32 nStateSize)
{
	if (!PhongThanValidateCharacterState(pState, nStateSize))
		return false;
	EnterCriticalSection(&m_Lock);
	const int nAccountCharacters = ListUnlocked(
		(const char*)pState->AccountName, NULL, 0);
	const bool bResult = nAccountCharacters >= 0 &&
		nAccountCharacters < PHONGTHAN_CHARACTER_LIMIT &&
		WriteUnlocked(pState, nStateSize, false);
	LeaveCriticalSection(&m_Lock);
	return bResult;
}

bool CPhongThanCharacterStore::Save(
	const PHONGTHAN_CHARACTER_STATE_HEADER* pState,
	PHONGTHAN_U32 nStateSize)
{
	if (!PhongThanValidateCharacterState(pState, nStateSize))
		return false;
	EnterCriticalSection(&m_Lock);
	BYTE* pExisting = new BYTE[PHONGTHAN_CHARACTER_MAX_STATE_SIZE];
	PHONGTHAN_U32 nExistingSize = 0;
	const bool bOwned = LoadUnlocked((const char*)pState->RoleName,
		pExisting, PHONGTHAN_CHARACTER_MAX_STATE_SIZE, &nExistingSize) &&
		stricmp((const char*)((PHONGTHAN_CHARACTER_STATE_HEADER*)pExisting)->AccountName,
			(const char*)pState->AccountName) == 0;
    const bool fresh=bOwned && pState->Revision>=((const PHONGTHAN_CHARACTER_STATE_HEADER*)pExisting)->Revision;
	delete [] pExisting;
	const bool bResult = fresh && WriteUnlocked(pState, nStateSize, true);
	LeaveCriticalSection(&m_Lock);
	return bResult;
}

bool CPhongThanCharacterStore::Delete(const char* pAccountName,
	const char* pRoleName)
{
	if (!pAccountName || !pAccountName[0] || !pRoleName || !pRoleName[0])
		return false;
	EnterCriticalSection(&m_Lock);
	BYTE* pExisting = new BYTE[PHONGTHAN_CHARACTER_MAX_STATE_SIZE];
	PHONGTHAN_U32 nExistingSize = 0;
	char szPath[MAX_PATH];
	const bool bOwned = LoadUnlocked(pRoleName, pExisting,
		PHONGTHAN_CHARACTER_MAX_STATE_SIZE, &nExistingSize) &&
		stricmp((const char*)((PHONGTHAN_CHARACTER_STATE_HEADER*)pExisting)->AccountName,
			pAccountName) == 0 &&
		BuildRolePath(pRoleName, szPath, sizeof(szPath));
	delete [] pExisting;
	bool bResult = bOwned && DeleteFileA(szPath) != FALSE;
    if(bResult){DeleteFileA((std::string(szPath)+".login.ini").c_str());
        std::string name;if(PtRoleFileName(pRoleName,name)){std::string flat=std::string(m_szRoot)+"\\"+name;
            if(_stricmp(flat.c_str(),szPath)){std::vector<BYTE> legacy(PHONGTHAN_CHARACTER_MAX_STATE_SIZE);PHONGTHAN_U32 size=0;
                if(LoadPathUnlocked(flat.c_str(),&legacy[0],legacy.size(),&size)&&!_stricmp((const char*)((PHONGTHAN_CHARACTER_STATE_HEADER*)&legacy[0])->AccountName,pAccountName)){
                    bResult=DeleteFileA(flat.c_str())!=FALSE;DeleteFileA((flat+".login.ini").c_str());}}}}

	LeaveCriticalSection(&m_Lock);
	return bResult;
}

bool CPhongThanCharacterStore::RecordLogin(const char* account, const char* role, PHONGTHAN_U32 ip)
{
    if (!account || !role || !ip) return false;
    EnterCriticalSection(&m_Lock);
    BYTE* data = new BYTE[PHONGTHAN_CHARACTER_MAX_STATE_SIZE];
    PHONGTHAN_U32 size = 0;
    bool ok = LoadUnlocked(role, data, PHONGTHAN_CHARACTER_MAX_STATE_SIZE, &size);
    if (ok) ok = stricmp((const char*)((PHONGTHAN_CHARACTER_STATE_HEADER*)data)->AccountName, account) == 0;
    char path[MAX_PATH], metadata[MAX_PATH];
    if (ok) ok = BuildRolePath(role, path, sizeof(path));
    if (ok) {
        _snprintf(metadata, sizeof(metadata), "%s.login.ini", path);
        metadata[sizeof(metadata)-1] = 0;
        FILETIME time; GetSystemTimeAsFileTime(&time);
        ULARGE_INTEGER stamp; stamp.LowPart = time.dwLowDateTime; stamp.HighPart = time.dwHighDateTime;
        PHONGTHAN_U32 utc = (PHONGTHAN_U32)(stamp.QuadPart / 10000000 - 11644473600);
        ok=PtCharacterBundle::Login(path,(const PHONGTHAN_CHARACTER_STATE_HEADER*)data,utc,ip);
    }
    delete [] data;
    LeaveCriticalSection(&m_Lock);
    return ok;
}

#include "PhongThanCharacterAdmin.inl"
