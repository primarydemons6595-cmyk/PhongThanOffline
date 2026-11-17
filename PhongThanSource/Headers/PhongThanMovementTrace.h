#ifndef PHONGTHAN_MOVEMENT_TRACE_H
#define PHONGTHAN_MOVEMENT_TRACE_H
#ifndef _SERVER
void PhongThanMovementFrame(unsigned int now,unsigned int prepared,unsigned int finished,int count);
void PhongThanMovementPosition(int mode,int action,int x,int y,bool ignored,unsigned int stamp=0,bool sameStep=false);
void PhongThanMovementSkill(int id,int x,int y,int incoming,int direct);
void PhongThanMovementRequest(int mode,int x,int y);
#endif
#endif
