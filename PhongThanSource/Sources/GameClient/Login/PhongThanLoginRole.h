#ifndef PHONGTHAN_LOGIN_ROLE_H
#define PHONGTHAN_LOGIN_ROLE_H

#include "../../../Headers/PhongThanProtocol.h"
#include <string.h>

// Storage must preserve the server identity. The UI's new-name input limit
// must never truncate a character already returned by the server.
struct KRoleChiefInfo {
    char Name[sizeof(((PHONGTHAN_CHARACTER_SUMMARY*)0)->Name)];
    unsigned char Gender;
    unsigned char Profession;
    int FightExperience;
    int MapId;
    unsigned long SavedUtc;
    unsigned long LastLoginUtc, LastLoginIP;
    unsigned char Portrait;
    unsigned char Transcendence;
    union {
        unsigned short NativePlaceId;
        short nLevel;
    };
};

inline bool PhongThanReadLoginRole(const PHONGTHAN_CHARACTER_SUMMARY& source,
                                 KRoleChiefInfo& destination) {
    memset(&destination, 0, sizeof(destination));
    if (!source.Name[0] || !memchr(source.Name, 0, sizeof(source.Name)))
        return false;
    if (source.Gender >= PHONGTHAN_GENDER_COUNT || source.Profession >= PHONGTHAN_PROFESSION_COUNT) return false;
    memcpy(destination.Name, source.Name, sizeof(source.Name));
    destination.Profession = source.Profession;
    destination.Gender = source.Gender;
    destination.nLevel = source.Level;
    destination.FightExperience = source.FightExperience;
    destination.MapId = source.MapId;
    destination.SavedUtc = source.SavedUtc;
    destination.LastLoginUtc = source.LastLoginUtc;
    destination.LastLoginIP = source.LastLoginIP;
    destination.Portrait = source.Portrait;
    destination.Transcendence = source.Transcendence;
    return true;
}

#endif
