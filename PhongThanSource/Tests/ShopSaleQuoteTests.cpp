#include <assert.h>
#include "../Headers/PhongThanShopSale.h"
int main()
{
    assert(PhongThanEquipmentTemplateCanSell(0));
    assert(PhongThanEquipmentTemplateCanSell(1));
    assert(!PhongThanEquipmentTemplateCanSell(-1));
    assert(!PhongThanEquipmentTemplateCanSell(6));
    // Single equipment, stacked consumable, and sub-scale rounding.
    assert(PhongThanShopSaleQuote(1000, 1, 4) == 250);
    assert(PhongThanShopSaleQuote(1000, 20, 4) == 5000);
    assert(PhongThanShopSaleQuote(7, 3, 4) == 3);
    assert(PhongThanShopSaleQuote(0, 20, 4) == 0);
    assert(PhongThanShopSaleQuote(0, 20, 4, 1) == 20);
    assert(PhongThanShopSaleQuote(0, 1, 4, 200) == 200);
    assert(PhongThanShopSaleQuote(1, 1, 4, 1) == 1);
    assert(PhongThanShopSaleQuote(200, 1, 4, 200) == 50);
    assert(PhongThanShopSaleQuote(0, INT_MAX, 4, 200) == -1);
    assert(PhongThanShopSaleQuote(1000, 0, 4) == -1);
    assert(PhongThanShopSaleQuote(-1, 1, 4) == -1);
    assert(PhongThanShopSaleQuote(100, 1, 0) == -1);
    assert(PhongThanShopSaleQuote(INT_MAX, 5, 4) == -1);
    return 0;
}
