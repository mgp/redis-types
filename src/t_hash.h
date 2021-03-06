#ifndef __T_HASH_H
#define __T_HASH_H

#include "dict.h"

/* Structure to hold hash iteration abstration. Note that iteration over
 * hashes involves both fields and values. Because it is possible that
 * not both are required, store pointers in the iterator to avoid
 * unnecessary memory allocation for fields/values. */
typedef struct {
    int encoding;
    unsigned char *zi;
    unsigned char *zk, *zv;
    unsigned int zklen, zvlen;

    dictIterator *di;
    dictEntry *de;
} hashTypeIterator;

void convertToRealHash(robj *o);
void hashTypeTryConversion(robj *subject, robj **argv, int start, int end);
void hashTypeTryObjectEncoding(robj *subject, robj **o1, robj **o2);
int hashTypeGet(robj *o, robj *key, robj **objval, unsigned char **v, unsigned int *vlen);
robj *hashTypeGetObject(robj *o, robj *key);
int hashTypeExists(robj *o, robj *key);
int hashTypeSet(robj *o, robj *key, robj *value);
int hashTypeDelete(robj *o, robj *key);
unsigned long hashTypeLength(robj *o);
hashTypeIterator *hashTypeInitIterator(robj *subject);
void hashTypeReleaseIterator(hashTypeIterator *hi);
int hashTypeNext(hashTypeIterator *hi);
int hashTypeCurrent(hashTypeIterator *hi, int what, robj **objval, unsigned char **v, unsigned int *vlen);
robj *hashTypeCurrentObject(hashTypeIterator *hi, int what);
robj *hashTypeLookupWriteOrCreate(redisClient *c, robj *key);

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

