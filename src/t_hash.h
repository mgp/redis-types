#ifndef __T_HASH_H
#define __T_HASH_H

void hdelCommand(redisClient *c);
void hexistsCommand(redisClient *c);
void hgetCommand(redisClient *c);
void hgetallCommand(redisClient *c);
void hincrbyCommand(redisClient *c);
void hkeysCommand(redisClient *c);
void hlenCommand(redisClient *c);
void hmgetCommand(redisClient *c);
void hmsetCommand(redisClient *c);
void hsetCommand(redisClient *c);
void hsetnxCommand(redisClient *c);
void hvalsCommand(redisClient *c);

#endif

