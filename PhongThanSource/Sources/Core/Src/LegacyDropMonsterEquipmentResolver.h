// Opt-in equipment-only hypothesis. Not a reconstruction of Kingsoft code.
#ifndef PT_LEGACY_DROP_MONSTER_EQUIPMENT_RESOLVER_H
#define PT_LEGACY_DROP_MONSTER_EQUIPMENT_RESOLVER_H
#include <string>
namespace LegacyDrop { namespace Execution {
class MonsterEquipmentResolver {
public:
    static bool Bounds(int npcLevel,int mainLow,int mainHigh,int minScale,int maxScale,
        bool explicitLevel,int requested,int& low,int& high,std::string& error){
        if(npcLevel<=0||minScale<=0||maxScale<=0||mainLow<1||mainHigh<mainLow||mainHigh-mainLow>256){error="EQUIPMENT_INVALID_BOUNDS_OR_SCALE";return false;}
        low=npcLevel/minScale;high=npcLevel/maxScale;
        if(low<mainLow)low=mainLow;if(low>mainHigh)low=mainHigh;
        if(high<mainLow)high=mainLow;if(high>mainHigh)high=mainHigh;
        if(low>high){error="EQUIPMENT_INVERTED_BOUNDS";return false;}
        if(explicitLevel){if(requested<low||requested>high){error="EQUIPMENT_EXPLICIT_LEVEL_OUTSIDE_MONSTER_BOUNDS";return false;}low=high=requested;}
        return true;
    }
};
}}
#endif
