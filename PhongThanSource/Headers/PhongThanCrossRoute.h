#pragma once
enum {PT_GOI_CROSS_ROUTE_STEP=0x50540031};
struct PTCrossRouteStep {int MapId,X,Y,Portal;};
enum {PT_GOI_CROSS_ROUTE_STATUS=0x50540033};
struct PTCrossRoutePosition {int MapId,X,Y,Moving;};
