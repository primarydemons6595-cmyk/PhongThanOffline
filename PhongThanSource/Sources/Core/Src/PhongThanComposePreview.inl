#include "PhongThanComposeRecipe.h"
#ifndef PHONGTHAN_EQUIPMENT_COMPOSE_TEST
#define PHONGTHAN_COMPOSE_QUOTE_ONLY
#include "PhongThanEquipmentCompose.inl"
#include "PhongThanSocketCompose.inl"
#include "PhongThanSoulCompose.inl"
#include "PhongThanFabaoCompose.inl"
#include "PhongThanWorkshopCompose.inl"
#undef PHONGTHAN_COMPOSE_QUOTE_ONLY
#endif
static void PhongThanGetComposePreview(PHONGTHAN_COMPOSE_PREVIEW& quote) {
    int metadataOnly=quote.MetadataOnly;
    int quantities[9];memcpy(quantities,quote.Quantities,sizeof(quantities));
    ZeroMemory(&quote,sizeof(quote));quote.Status=PT_COMPOSE_NO_RECIPE;
    int selected[9];ZeroMemory(selected,sizeof(selected));
    int indexes[9],details[9],counts[9],unique=0;bool equipment=false,materials=true;
    for(int slot=0;slot<9;++slot) {
        int i=indexes[slot]=Player[CLIENT_PLAYER_INDEX].m_ItemList.GetBuildItem(slot);
        if(!i)continue;
        if(i<0 || i>=MAX_ITEM || !Item[i].GetID()){quote.Status=PT_COMPOSE_INVALID_ITEMS;return;}
        quote.Genres[slot]=Item[i].GetGenre();quote.Details[slot]=Item[i].GetDetailType();quote.Stacks[slot]=Item[i].GetStackNum();quote.Levels[slot]=Item[i].m_CommonAttrib.nUpgradeLvl;
        if(quantities[slot]<-1 || quantities[slot]>quote.Stacks[slot]){quote.Status=PT_COMPOSE_INVALID_ITEMS;return;}
        selected[slot]=quantities[slot]==-1?0:(quantities[slot]?quantities[slot]:quote.Stacks[slot]);
        if(!selected[slot])continue;
        ++quote.Count;equipment=equipment || Item[i].GetGenre()==item_equip;
        materials=materials && Item[i].GetGenre()==item_materials && !Item[i].GetParticular();
        int n=0;while(n<unique && details[n]!=Item[i].GetDetailType())++n;
        if(n==unique){details[unique]=Item[i].GetDetailType();counts[unique++]=0;}
        if(Item[i].GetStackNum()<1 || counts[n]>2147483647-Item[i].GetStackNum()){quote.Status=PT_COMPOSE_INVALID_ITEMS;return;}
        counts[n]+=selected[slot];
    }
    if(metadataOnly || !quote.Count){quote.Status=PT_COMPOSE_OK;return;}
#ifndef PHONGTHAN_EQUIPMENT_COMPOSE_TEST
    const int legacyMode=PtSocketComposeMode();int customStation=legacyMode==1?1:legacyMode==7?5:(legacyMode>=2&&legacyMode<=6)?3:0;
    if(customStation){int result=PhongThanWorkshopCompose(CLIENT_PLAYER_INDEX,indexes,selected,customStation,quote.RecipeId,quote.Money,true);if(result!=PT_COMPOSE_NO_RECIPE){quote.Status=result;if(result==PT_COMPOSE_OK)for(int q=0;q<9;++q)quote.Quantities[q]=selected[q]?selected[q]:(indexes[q]?-1:0);return;}}
    if(PtSocketComposeMode()>=1001&&PtSocketComposeMode()<=1064){quote.Status=PhongThanWorkshopCompose(CLIENT_PLAYER_INDEX,indexes,selected,PtSocketComposeMode()-1000,quote.RecipeId,quote.Money,true);if(quote.Status==PT_COMPOSE_OK)for(int q=0;q<9;++q)quote.Quantities[q]=selected[q]?selected[q]:(indexes[q]?-1:0);return;}
    if(PtSocketComposeMode()==8||PtSocketComposeMode()==9){quote.Status=PtSoulCompose(CLIENT_PLAYER_INDEX,indexes,selected,PtSocketComposeMode(),quote.RecipeId,quote.Money,true);if(quote.Status==PT_COMPOSE_OK)for(int q=0;q<9;++q)quote.Quantities[q]=selected[q]?selected[q]:(indexes[q]?-1:0);return;}
    if(PtSocketComposeMode()>=2&&PtSocketComposeMode()<=6){quote.Status=PtSocketCompose(CLIENT_PLAYER_INDEX,indexes,selected,quote.RecipeId,quote.Money,true,PtSocketComposeMode()>=3?PtSocketComposeMode()-2:0);if(quote.Status==PT_COMPOSE_OK)for(int q=0;q<9;++q)quote.Quantities[q]=selected[q]?selected[q]:(indexes[q]?-1:0);return;}
    int fabao=PhongThanFabaoCompose(CLIENT_PLAYER_INDEX,indexes,selected,PtSocketComposeMode()==7?100:1,quote.RecipeId,quote.Money,true);
    if(fabao!=PT_COMPOSE_NO_RECIPE||PtSocketComposeMode()==7){quote.Status=fabao;if(fabao==PT_COMPOSE_OK)for(int fq=0;fq<9;++fq)quote.Quantities[fq]=selected[fq]?selected[fq]:(indexes[fq]?-1:0);return;}
#endif
    static KTabFile recipes,costs;static bool loaded=false;
    if(!loaded)loaded=recipes.Load("\\settings\\item\\001\\zhuang_bei_he_cheng_gui_ze_biao.txt") &&
        costs.Load("\\settings\\item\\001\\zhuang_bei_he_cheng_jin_qian_xu_qiu.txt");
    if(!loaded){quote.Status=PT_COMPOSE_TABLE_ERROR;return;}
    if(equipment){
        quote.Status=PhongThanEquipmentCompose(CLIENT_PLAYER_INDEX,indexes,recipes,costs,quote.RecipeId,true,&quote.Money,selected);
        if(quote.Status==PT_COMPOSE_NO_RECIPE)for(int er=2;er<=recipes.GetHeight();++er){
            PhongThanEquipmentRecipe r;if(!PhongThanReadEquipmentRecipe(recipes,er,r) || (r.type!=1 && r.type!=7) || r.requiredRecipe[0])continue;
            int trial[9],left[9];ZeroMemory(trial,sizeof(trial));memcpy(left,selected,sizeof(left));bool match=true;
            for(int need=0;need<r.inputCount && match;++need){
                int amount=r.inputs[need].quantity;
                for(int at=0;at<9 && amount>0;++at)if(left[at]>0 && PhongThanUpgradeItemMatches(indexes[at],r.inputs[need],r.native[need])){
                    int used=left[at]<amount?left[at]:amount;left[at]-=used;trial[at]+=used;amount-=used;
                }
                if(amount)match=false;
            }
            for(int extra=0;extra<9;++extra)if(left[extra]>0 && (!trial[extra] || Item[indexes[extra]].GetGenre()==item_ibitem))match=false;
            if(!match)continue;
            quote.Status=PhongThanEquipmentCompose(CLIENT_PLAYER_INDEX,indexes,recipes,costs,quote.RecipeId,true,&quote.Money,trial);
            if(quote.Status==PT_COMPOSE_OK){memcpy(selected,trial,sizeof(selected));break;}
        }
        if(quote.Status==PT_COMPOSE_OK)for(int eq=0;eq<9;++eq)quote.Quantities[eq]=selected[eq]?selected[eq]:(indexes[eq]?-1:0);
        return;
    }
    for(int row=2;materials && row<=recipes.GetHeight();++row) {
        PhongThanComposeRecipe recipe;
        if(!PhongThanReadComposeRecipe(recipes,row,recipe) || !PhongThanIsBasicMaterialRecipe(recipe))continue;
        int wanted[10],quantities[10];
        for(int n=0;n<recipe.inputCount;++n){wanted[n]=recipe.inputs[n].detail;quantities[n]=recipe.inputs[n].quantity;}
        bool match=true;int trial[9],left[9];ZeroMemory(trial,sizeof(trial));memcpy(left,selected,sizeof(left));
        for(int need=0;need<recipe.inputCount && match;++need){int amount=quantities[need];
            for(int at=0;at<9 && amount>0;++at)if(left[at]>0 && Item[indexes[at]].GetDetailType()==wanted[need]){
                int used=left[at]<amount?left[at]:amount;left[at]-=used;trial[at]+=used;amount-=used;
            }if(amount)match=false;
        }
        for(int extra=0;extra<9;++extra)if(left[extra]>0 && !trial[extra])match=false;
        if(!match)continue;
        for(int q=0;q<9;++q)quote.Quantities[q]=trial[q]?trial[q]:(indexes[q]?-1:0);
        quote.RecipeId=recipe.id;quote.Status=PhongThanReadComposeCost(costs,recipe.id,1,quote.Money)?PT_COMPOSE_OK:PT_COMPOSE_NO_COST;return;
    }
}
