#ifndef __T_ZSET_H
#define __T_ZSET_H

zskiplist *zslCreate(void);
void zslFree(zskiplist *zsl);
zskiplistNode *zslInsert(zskiplist *zsl, double score, robj *obj);
unsigned char *zzlInsert(unsigned char *zl, robj *ele, double score);
double zzlGetScore(unsigned char *sptr);
void zzlNext(unsigned char *zl, unsigned char **eptr, unsigned char **sptr);
void zzlPrev(unsigned char *zl, unsigned char **eptr, unsigned char **sptr);
unsigned int zsetLength(robj *zobj);
void zsetConvert(robj *zobj, int encoding);

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

