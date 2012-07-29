#ifndef __T_STRING_H
#define __T_STRING_H

void appendCommand(redisClient *c);
void decrCommand(redisClient *c);
void decrbyCommand(redisClient *c);
void getCommand(redisClient *c);
void getbitCommand(redisClient *c);
void getrangeCommand(redisClient *c);
void getsetCommand(redisClient *c);
void incrCommand(redisClient *c);
void incrbyCommand(redisClient *c);
void mgetCommand(redisClient *c);
void msetCommand(redisClient *c);
void msetnxCommand(redisClient *c);
void setCommand(redisClient *c);
void setbitCommand(redisClient *c);
void setexCommand(redisClient *c);
void setnxCommand(redisClient *c);
void setrangeCommand(redisClient *c);
void strlenCommand(redisClient *c);

#endif

