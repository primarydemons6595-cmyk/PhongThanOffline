#include <assert.h>
#include <stdio.h>
#include "../../Headers/PhongThanEquipmentAppearance.h"

int main()
{
	// Independent expected values from original PT item levels and player-part rows.
	assert(PhongThanResolveBasicPartResource(2, 0, 2) == 1);  // Cu Dau
	assert(PhongThanResolveBasicPartResource(2, 0, 6) == 3);  // Tinh Cang
	assert(PhongThanResolveBasicPartResource(2, 0, 8) == 4);  // Khai Thien
	assert(PhongThanResolveBasicPartResource(2, 0, 10) == 5); // Hoang Kim Chan Dan; row 6 is unequipped ascended body
	assert(PhongThanResolveBasicPartResource(2, 1, 1) == 1);  // reset: Dao Sy
	assert(PhongThanResolveBasicPartResource(2, 2, 1) == 1);  // reset: Di Nhan
	assert(PhongThanResolveBasicPartResource(7, 0, 8) == 4);  // helm
	assert(PhongThanResolveBasicPartResource(9, 0, 8) == 4);  // cape
	assert(PhongThanResolveBasicPartResource(9, 12, 8) == -1);// unknown special cape
	assert(PhongThanResolveBasicPartResource(2, 0, 11) == -1);// invalid tier
	puts("PASS: authentic PT basic equipment appearance boundaries.");
	return 0;
}
