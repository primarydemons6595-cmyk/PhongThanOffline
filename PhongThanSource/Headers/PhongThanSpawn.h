#ifndef PHONGTHAN_SPAWN_H
#define PHONGTHAN_SPAWN_H
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "PhongThanMapIdentity.h"
#include "PhongThanStarterSpawn.h"
#ifdef PT_SECURE_RELEASE_PROFILE
#include "../Sources/Engine/Src/KWin32.h"
#include "../Sources/Engine/Src/KIniFile.h"
#endif

// Runtime IDs are resolved back to explicit PT/content IDs. Coordinates are
// read unchanged from the original VNG revivepos.ini, never from minimap pixels.
inline BOOL PhongThanResolveRevivalPoint(unsigned int runtimeMap, int revivalId, POINT* point)
{
	if (!point) return FALSE;
	point->x = 0; point->y = 0;
	char path[MAX_PATH];
	if (!GetFullPathNameA("settings\\revivepos.ini", sizeof(path), path, 0)) return FALSE;
	#ifdef PT_SECURE_RELEASE_PROFILE
	KIniFile packed;
	if (!packed.Load("\\settings\\revivepos.ini")) return FALSE;
	#endif
	PHONGTHAN_MAP_RESOLUTION resolution;
	if (!PhongThanResolveRuntimeMap(runtimeMap, &resolution)) return FALSE;
	// Starter villages use the same destination for creation and revival.
	if (resolution.ContentMapId >= 2 && resolution.ContentMapId <= 4)
	{
		int map=0,x=0,y=0;
		if (!PhongThanStarterSpawn(resolution.ContentMapId-2,map,x,y)) return FALSE;
		point->x=x;point->y=y;return TRUE;
	}
	char section[24], key[24], value[128], choices[512];
	sprintf(section, "%u", resolution.ContentMapId);
	if (revivalId > 0)
	{
		sprintf(key, "%d", revivalId);
		#ifdef PT_SECURE_RELEASE_PROFILE
		packed.GetString(section,key,"",value,sizeof(value));
#else
		GetPrivateProfileStringA(section, key, "", value, sizeof(value), path);
#endif
		int x, y;
		if (sscanf(value, "%d,%d", &x, &y) == 2 && x > 0 && y > 0)
		{ point->x = x; point->y = y; return TRUE; }
		return FALSE;
	}
	#ifdef PT_SECURE_RELEASE_PROFILE
	packed.GetString(section,"region","",choices,sizeof(choices));
#else
	GetPrivateProfileStringA(section, "region", "", choices, sizeof(choices), path);
#endif
	char* cursor = choices;
	while (*cursor)
	{
		char* end = cursor;
		const long candidate = strtol(cursor, &end, 10);
		if (end == cursor) return FALSE;
		sprintf(key, "%ld", candidate);
		#ifdef PT_SECURE_RELEASE_PROFILE
		packed.GetString(section,key,"",value,sizeof(value));
#else
		GetPrivateProfileStringA(section, key, "", value, sizeof(value), path);
#endif
		int x, y;
		if (sscanf(value, "%d,%d", &x, &y) == 2 && x > 0 && y > 0)
		{ point->x = x; point->y = y; return TRUE; }
		cursor = end;
		while (*cursor == ' ' || *cursor == ',') ++cursor;
	}
	return FALSE;
}
#endif
