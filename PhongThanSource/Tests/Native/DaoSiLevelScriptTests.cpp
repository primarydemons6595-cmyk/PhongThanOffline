#include "KWin32.h"
#include "KFilePath.h"
#include "KTabFile.h"
#include "KLuaScript.h"
#include "KIniFile.h"
#include "../../Headers/PhongThanSkillExperience.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char** argv)
{
    if (argc != 2) return 2;
    g_SetRootPath(argv[1]);
    g_SetFilePath("\\");
    KTabFile table;
    if (!table.Load("\\settings\\skills.txt")) return 3;
    KIniFile progression;
    if (!progression.Load("\\settings\\skills.ini")) return 8;
    for (int book = 0; book < 58; ++book)
    {
        int skill = book < 49 ? book + 3 : 450 + book - 49;
        char section[16], value[512];
        sprintf(section, "%d", skill);
        progression.GetString(section, "SkillLiveUpExp", "", value, sizeof(value));
        if (!PhongThanSkillNextExperience(value, 1) ||
            !PhongThanSkillNextExperience(value, 9) ||
            PhongThanSkillNextExperience(value, 10)) return 9;
    }
    puts("PASS native skills.ini: 58 training curves loaded, learned cap 10");
    int checked = 0;
    for (int row = 2; row <= table.GetHeight(); ++row)
    {
        int id = 0;
        table.GetInteger(row, "SkillId", 0, &id);
        if (id < 3 || id > 26) continue;
        char path[260];
        table.GetString(row, "LvlSetScript", "", path, sizeof(path));
        KLuaScript script;
        if (!script.Init() || !script.Load(path))
        { printf("FAIL skill=%d level_script_load\n", id); return 4; }
        for (int level = 1; level <= 10; level += 9)
        {
            for (int field = 1; field <= 10; ++field)
            {
                char key[40], name[100], data[100];
                sprintf(key, "LvlSetting%d", field);
                table.GetString(row, key, "", name, sizeof(name));
                if (!name[0]) continue;
                sprintf(key, "LvlData%d", field);
                table.GetString(row, key, "", data, sizeof(data));
                int top;
                script.SafeCallBegin(&top);
                if (!script.CallFunction("GetSkillLevelData", 1, "ssd", name, data, level))
                { printf("FAIL skill=%d level=%d field=%s call\n", id, level, name); return 5; }
                if (lua_isnumber(script.m_LuaState, -1))
                    printf("skill=%d level=%d %s=%g\n", id, level, name, lua_tonumber(script.m_LuaState, -1));
                else if (lua_isstring(script.m_LuaState, -1))
                    printf("skill=%d level=%d %s=%s\n", id, level, name, lua_tostring(script.m_LuaState, -1));
                else
                { printf("FAIL skill=%d level=%d field=%s result\n", id, level, name); return 6; }
                script.SafeCallEnd(top);
            }
        }
        ++checked;
    }
    printf("PASS DaoSi level scripts=%d levels=1,10\n", checked);
    return checked == 24 ? 0 : 7;
}
