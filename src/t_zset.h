#ifndef __T_ZSET_H
#define __T_ZSET_H

void zaddCommand(redisClient *c);
void zcardCommand(redisClient *c);
void zcountCommand(redisClient *c);
void zincrbyCommand(redisClient *c);
void zinterstoreCommand(redisClient *c);
void zrangeCommand(redisClient *c);
void zrangebyscoreCommand(redisClient *c);
void zrankCommand(redisClient *c);
void zremCommand(redisClient *c);
void zremrangebyrankCommand(redisClient *c);
void zremrangebyscoreCommand(redisClient *c);
void zrevrangeCommand(redisClient *c);
void zrevrangebyscoreCommand(redisClient *c);
void zrevrankCommand(redisClient *c);
void zscoreCommand(redisClient *c);
void zunionstoreCommand(redisClient *c);

#endif

