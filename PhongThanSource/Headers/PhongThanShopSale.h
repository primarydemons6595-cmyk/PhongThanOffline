#ifndef PHONGTHAN_SHOP_SALE_H
#define PHONGTHAN_SHOP_SALE_H
#include <limits.h>

// Original equipment tables use both 0 (legacy/default) and 1 for ordinary
// shop-sellable records. Verified with the online client selling starter
// equipment whose table cell is 0. Instance LockSell is a separate restriction.
inline int PhongThanEquipmentTemplateCanSell(int value)
{
    return value == 0 || value == 1;
}

// Price is per unit. A shop sale quote covers one inventory object (the
// entire stack); count must be applied exactly once by both client and server.
inline int PhongThanShopSaleQuote(int price, int count, int scale, int missingPriceUnit = 0)
{
    if (price < 0 || count <= 0 || scale <= 0 || missingPriceUnit < 0) return -1;
    int unit = price / scale;
    if (!unit) unit = missingPriceUnit;
    if (unit && count > INT_MAX / unit) return -1;
    return unit * count;
}
#endif
