#ifndef PHONGTHAN_UI_NATION_H
#define PHONGTHAN_UI_NATION_H

#include "../../../../Headers/PhongThanNationProtocol.h"
void PhongThanNationOpenGuide();
void PhongThanNationClose();
void PhongThanNationArrive(const PTNationResponse* response,unsigned int size);

#endif
