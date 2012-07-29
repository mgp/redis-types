#ifndef __T_LIST_H
#define __T_LIST_H

void lindexCommand(redisClient *c);
void linsertCommand(redisClient *c);
void llenCommand(redisClient *c);
void lpopCommand(redisClient *c);
void lpushCommand(redisClient *c);
void lpushxCommand(redisClient *c);
void lrangeCommand(redisClient *c);
void lremCommand(redisClient *c);
void lsetCommand(redisClient *c);
void ltrimCommand(redisClient *c);
void rpopCommand(redisClient *c);
void rpoplpushCommand(redisClient *c);
void rpushCommand(redisClient *c);
void rpushxCommand(redisClient *c);

#endif

