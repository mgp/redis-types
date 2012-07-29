#ifndef __DB_H
#define __DB_H

int removeExpire(redisDb *db, robj *key);
int expireIfNeeded(redisDb *db, robj *key);
time_t getExpire(redisDb *db, robj *key);
void setExpire(redisDb *db, robj *key, time_t when);
robj *lookupKey(redisDb *db, robj *key);
robj *lookupKeyRead(redisDb *db, robj *key);
robj *lookupKeyWrite(redisDb *db, robj *key);
robj *lookupKeyReadOrReply(redisClient *c, robj *key, robj *reply);
robj *lookupKeyWriteOrReply(redisClient *c, robj *key, robj *reply);
void dbAdd(redisDb *db, robj *key, robj *val);
void dbOverwrite(redisDb *db, robj *key, robj *val);
void setKey(redisDb *db, robj *key, robj *val);
int dbExists(redisDb *db, robj *key);
robj *dbRandomKey(redisDb *db);
int dbDelete(redisDb *db, robj *key);
long long emptyDb();

void delCommand(redisClient *c);
void existsCommand(redisClient *c);
void randomkeyCommand(redisClient *c);
void keysCommand(redisClient *c);
void dbsizeCommand(redisClient *c);
void typeCommand(redisClient *c);
void renameCommand(redisClient *c);
void renamenxCommand(redisClient *c);
void expireCommand(redisClient *c);
void expireatCommand(redisClient *c);
void ttlCommand(redisClient *c);
void persistCommand(redisClient *c);

#endif

