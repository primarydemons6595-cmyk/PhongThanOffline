#ifndef PHONGTHAN_TERRITORY_STATE_H
#define PHONGTHAN_TERRITORY_STATE_H

// Development is part of the same durable nation commit as its bank. These
// records contain no pointers; migration retains the complete v2 prefix.
enum {PT_TERRITORY_BUILDINGS=128,PT_TERRITORY_RESOURCES=10,
      PT_TERRITORY_MAX_CITIES=25,PT_TERRITORY_RECEIPT_TASK=5390};
struct PTTerritoryBuilding {
 int Type,X,Y;unsigned int CompleteAt;
};
struct PTNationDevelopment {
 unsigned int NationId;
 int Popularity,OnlineRemainder;
 unsigned int PopularityDay;
 int CityMapId,Land,Totem,CityLevel;
 char CityName[32];
 int Resources[PT_TERRITORY_RESOURCES],Prosperity;
 unsigned int ProsperityAt;
 int SustainedChecks,BuildingCount;
 PTTerritoryBuilding Buildings[PT_TERRITORY_BUILDINGS];
};
struct PTTerritoryContribution {
 unsigned int NationId;char Name[32];
 int Total,Usable,ThisWeek,LastWeek,Week,Day,Today;
 int PendingBefore,PendingAfter,PendingResource,PendingAmount,PendingPoints,PendingDay;
 int PendingCitySlotPlusOne,PendingLand,PendingTotem,PendingMoney,PendingBronze;
};
struct PTNationBook {unsigned int NationId;int Fragments[2],Migrated;};
inline bool PTTerritoryDevelopmentValid(const PTNationDevelopment& d,unsigned int owner){
 if(!d.NationId)return !owner;
 if(d.NationId!=owner||d.Popularity<0||d.Popularity>100000000||d.OnlineRemainder<0||d.OnlineRemainder>86400||
    d.CityLevel<0||d.CityLevel>4||d.Land<0||d.Land>4||d.Totem<0||d.Totem>2||d.Prosperity<0||
    d.SustainedChecks<0||d.BuildingCount<0||d.BuildingCount>PT_TERRITORY_BUILDINGS||
    !memchr(d.CityName,0,sizeof(d.CityName)))return false;
 if(d.CityLevel&&(!d.CityName[0]||d.CityMapId<2100||d.CityMapId>=2225||(d.CityMapId-2100)%5!=d.Land))return false;
 if(!d.CityLevel&&(d.CityMapId||d.BuildingCount))return false;
 for(int r=0;r<PT_TERRITORY_RESOURCES;++r)if(d.Resources[r]<0)return false;
 for(int b=0;b<d.BuildingCount;++b)if(d.Buildings[b].Type<0||d.Buildings[b].Type>=34||d.Buildings[b].X<0||d.Buildings[b].Y<0)return false;
 return true;
}
inline bool PTTerritoryContributionValid(const PTTerritoryContribution& c){
 if(!c.NationId)return true;
 if(!c.Name[0]||!memchr(c.Name,0,sizeof(c.Name))||c.Total<0||c.Usable<0||c.Usable>c.Total||
    c.ThisWeek<0||c.LastWeek<0||c.Week<0||c.Day<0||c.Today<0)return false;
 if(!c.PendingAfter)return c.PendingBefore==0&&c.PendingAmount==0&&c.PendingPoints==0&&c.PendingCitySlotPlusOne==0&&c.PendingMoney==0&&c.PendingBronze==0;
 if(c.PendingBefore<0||c.PendingBefore>=1000000000||c.PendingAfter!=c.PendingBefore+1||c.PendingResource<0||c.PendingResource>6||c.PendingAmount<0||c.PendingPoints<0||c.PendingDay<0)return false;
 if(c.PendingResource<2)return c.PendingAmount>0&&!c.PendingCitySlotPlusOne&&!c.PendingMoney&&!c.PendingBronze;
 if(c.PendingResource==2)return c.PendingCitySlotPlusOne>=1&&c.PendingCitySlotPlusOne<=25&&c.PendingLand>=0&&c.PendingLand<5&&c.PendingTotem>=0&&c.PendingTotem<3&&c.PendingMoney>=0&&c.PendingBronze>=0;
 return c.PendingAmount>0&&!c.PendingCitySlotPlusOne&&!c.PendingMoney&&!c.PendingBronze;
}
#endif
