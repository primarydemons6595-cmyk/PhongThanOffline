// Shared offline/runtime-preflight adapter. No NPC death hook or world effects.
#ifndef PT_LEGACY_DROP_ITEM_ADAPTER_H
#define PT_LEGACY_DROP_ITEM_ADAPTER_H
#include <map>
#include <string>
#include "PhongThanQuestItemTuple.h"
#include "KItemGenerator.h"
#include "KItemSet.h"
class LegacyDropItemAdapter {
    std::map<int,int> categoryZero;
    bool initialized;
public:
    LegacyDropItemAdapter():initialized(false){}
    bool Init(const KItemGenerator& generator) {
        categoryZero.clear();initialized=false;
        char file[256];if(!BuildActiveItemTablePath("magicscript.txt",file,sizeof(file)))return false;
        KTabFile raw;if(!raw.Load(file))return false;
        const KLibOfBPT& catalog=generator.Catalog();
        for(int row=2;row<=raw.GetHeight();++row) {
            // Category zero is accepted only from an explicit genre/category/
            // particular/SPR anchor, backed by the exact production catalog row.
            int genre=-1,category=-1,key=-1;char image[80];image[0]=0;
            raw.GetInteger(row,2,-1,&genre);raw.GetInteger(row,3,-1,&category);raw.GetInteger(row,4,-1,&key);
            raw.GetString(row,5,"",image,sizeof(image));
            if(genre!=item_magicscript||category!=0||key<=0||!strstr(image,".spr"))continue;
            const KBASICPROP_MAGICSCRIPT* record=catalog.MagicScriptAt(row-2);
            if(!record||record->m_nItemGenre!=genre||record->m_nDetailType!=key||
               catalog.GetMagicScript(key)!=record||strcmp(record->m_szImageName,image)!=0)continue;
            categoryZero[key]=key;
        }
        initialized=true;return true;
    }
    bool Map(int genre,int detail,int particular,PhongThanQuestItemTuple& tuple) const {
        if(!initialized)return false;
        if(genre==item_event){const KBASICPROP_EVENTITEM* e=ItemGen.Catalog().GetEvent(detail);if(particular!=0||detail<0||!e||e->m_nItemGenre!=genre||e->m_nDetailType!=detail)return false;tuple.genre=genre;tuple.detail=detail;tuple.particular=0;tuple.level=-1;tuple.series=-1;tuple.luck=0;return true;}
        if(genre==item_magicscript&&detail==0) {
            std::map<int,int>::const_iterator i=categoryZero.find(particular);
            if(i==categoryZero.end())return false;
            tuple.genre=genre;tuple.detail=i->second;tuple.particular=0;
            tuple.level=-1;tuple.series=-1;tuple.luck=0;return true;
        }
        return PhongThanMapQuestItemIdentity(genre,detail,particular,tuple);
    }
    bool GenerateMapped(const PhongThanQuestItemTuple& tuple,int& result,std::string& error) const {
        result=0;error="";if(!initialized){error="ADAPTER_UNINITIALIZED";return false;}
        if(tuple.level<0){error="LEVEL_UNRESOLVED";return false;}
        int magic[MAX_ITEM_MAGICLEVEL];ZeroMemory(magic,sizeof(magic));
        int index=ItemSet.Add(tuple.genre,tuple.series,tuple.level,tuple.luck,tuple.detail,tuple.particular,magic,ITEM_VERSION);
        if(!index){error="GENERATOR_REJECT";return false;}
        KItem& item=Item[index];
        bool levelMatches=tuple.genre!=item_equip&&tuple.genre!=item_medicine&&tuple.genre!=item_skillbook||item.GetLevel()==tuple.level;
        if(item.GetGenre()!=tuple.genre||item.GetDetailType()!=tuple.detail||item.GetParticularType()!=tuple.particular||!levelMatches||
           !item.GetID()||!item.GetName()[0]||item.GetWidth()<=0||item.GetHeight()<=0) {
            ItemSet.Remove(index);error="GENERATED_ITEM_MISMATCH";return false;
        }
        result=index;return true;
    }
};
#endif
