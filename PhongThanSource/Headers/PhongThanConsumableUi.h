#include "PhongThanTimedItems.h"
#pragma once
// Project-local Core/UI query, NOT a network opcode or an original PT API.
enum { PT_GDI_CONSUMABLE_BUFFS = 0x50544246, PT_SGDI_CHAT_COLOR = 0x50544343 };
struct PTConsumableUiState { unsigned long RemainingSeconds[PT_CONSUMABLE_SLOTS]; };
