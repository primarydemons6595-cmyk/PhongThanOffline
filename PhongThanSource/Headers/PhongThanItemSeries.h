#ifndef PHONGTHAN_ITEM_SERIES_H
#define PHONGTHAN_ITEM_SERIES_H
// Resolve VNG's template marker without truncating it to an 8-bit wire value.
inline int PhongThanResolveItemSeries(int templateSeries, int requestedSeries,
    int seriesCount, int neutralSeries)
{
    if(templateSeries!=1000)return templateSeries;
    return requestedSeries>=0 && requestedSeries<seriesCount ? requestedSeries : neutralSeries;
}
// Original signet records also use 3000/4000 catalogue markers. They are not
// wire professions. Requirement 37, when authored, is the actual profession.
inline bool PhongThanSealSeriesMarker(int detail,int value)
{
    return detail==13 && (value==1000 || value==3000 || value==4000);
}
inline int PhongThanResolveEquipmentSeries(int detail,int templateSeries,
    int requestedSeries,int requiredSeries,int seriesCount,int neutralSeries)
{
    if(!PhongThanSealSeriesMarker(detail,templateSeries))
        return PhongThanResolveItemSeries(templateSeries,requestedSeries,seriesCount,neutralSeries);
    const int selected=requiredSeries>=0 && requiredSeries<seriesCount ? requiredSeries : requestedSeries;
    return PhongThanResolveItemSeries(1000,selected,seriesCount,neutralSeries);
}
#endif
