#ifdef _SERVER
static BOOL LoadPhongThanScriptAliasRegistry(const char* pszRelativeFile)
{
	FILE* pDiag = fopen("script_alias_registry_diag.log", "w");
	char szFullPath[MAX_PATH];
	g_GetFullPath(szFullPath, (char*)pszRelativeFile);
	FILE* pFile = fopen(szFullPath, "rb");
	if (!pFile)
	{
		printf("[CORE][FATAL] Missing script alias registry: %s\n", pszRelativeFile);
		if (pDiag) { fprintf(pDiag, "status=FAIL reason=MISSING_REGISTRY path=%s\n", pszRelativeFile); fclose(pDiag); }
		return FALSE;
	}

	DWORD dwSeenIds[512];
	unsigned int nSeenCount = 0;
	char szLine[1024];
	unsigned int nLine = 0;
	BOOL bValid = TRUE;
	while (fgets(szLine, sizeof(szLine), pFile))
	{
		++nLine;
		size_t nLength = strlen(szLine);
		while (nLength && (szLine[nLength - 1] == '\r' || szLine[nLength - 1] == '\n'))
			szLine[--nLength] = 0;
		if (!nLength || szLine[0] == '#')
			continue;
		if (nLine == 1 && !strncmp(szLine, "ScriptIdHex\t", 12))
			continue;

		char* pszId = strtok(szLine, "\t");
		char* pszPath = strtok(NULL, "\t");
		char* pszClassification = strtok(NULL, "\t");
		char* pszProvenance = strtok(NULL, "\t");
		char* pszSha256 = strtok(NULL, "\t");
		if (!pszId || !pszPath || !pszClassification || !pszProvenance ||
			!pszSha256 || strtok(NULL, "\t"))
		{
			printf("[CORE][FATAL] Invalid script alias row %u.\n", nLine);
			if (pDiag) fprintf(pDiag, "status=FAIL reason=INVALID_ROW line=%u\n", nLine);
			bValid = FALSE;
			break;
		}
		char* pszEnd = NULL;
		DWORD dwId = strtoul(pszId, &pszEnd, 0);
		if (!dwId || !pszEnd || *pszEnd || strlen(pszSha256) != 64 ||
			(strcmp(pszClassification, "NORMAL_EXIT") &&
			 strcmp(pszClassification, "SUPERTRAP_OR_OTHER_TRANSITION")))
		{
			printf("[CORE][FATAL] Invalid script alias fields at row %u.\n", nLine);
			if (pDiag) fprintf(pDiag, "status=FAIL reason=INVALID_FIELDS line=%u id=%s path=%s\n", nLine, pszId, pszPath);
			bValid = FALSE;
			break;
		}
		for (unsigned int nSeen = 0; nSeen < nSeenCount; ++nSeen)
		{
			if (dwSeenIds[nSeen] == dwId)
			{
				printf("[CORE][FATAL] Duplicate script alias id 0x%08lX.\n", dwId);
				if (pDiag) fprintf(pDiag, "status=FAIL reason=DUPLICATE_ID line=%u id=0x%08lX\n", nLine, dwId);
				bValid = FALSE;
				break;
			}
		}
		if (!bValid || nSeenCount >= sizeof(dwSeenIds) / sizeof(dwSeenIds[0]))
		{
			bValid = FALSE;
			break;
		}

		KSortScriptNode PathNode;
		PathNode.SetScriptID(g_FileName2Id(pszPath));
		if (!g_ScriptBinTree.Find(PathNode) || !PathNode.GetScript())
		{
			printf("[CORE][FATAL] Script alias file was not loaded: id=0x%08lX path=%s\n",
				dwId, pszPath);
			if (pDiag) fprintf(pDiag, "status=FAIL reason=LOOSE_SCRIPT_NOT_LOADED line=%u id=0x%08lX path=%s\n", nLine, dwId, pszPath);
			bValid = FALSE;
			break;
		}
		KSortScriptNode ExistingNode;
		ExistingNode.SetScriptID(dwId);
		if (g_ScriptBinTree.Find(ExistingNode))
		{
			if (ExistingNode.GetScript() != PathNode.GetScript())
			{
				printf("[CORE][FATAL] Conflicting script alias id 0x%08lX path=%s\n",
					dwId, pszPath);
				if (pDiag) fprintf(pDiag, "status=FAIL reason=ID_COLLISION line=%u id=0x%08lX path=%s\n", nLine, dwId, pszPath);
				bValid = FALSE;
				break;
			}
		}
		else
		{
			KSortScriptNode AliasNode;
			AliasNode.SetScriptID(dwId);
			AliasNode.SetScriptIndex(PathNode.GetScriptIndex());
			g_ScriptBinTree.Insert(AliasNode);
		}
		dwSeenIds[nSeenCount++] = dwId;
	}
	fclose(pFile);
	g_nScriptAliasCount = bValid ? nSeenCount : 0;
	if (pDiag)
	{
		if (bValid) fprintf(pDiag, "status=PASS aliases=%u\n", nSeenCount);
		fclose(pDiag);
	}
	return bValid && nSeenCount > 0;
}
#endif
