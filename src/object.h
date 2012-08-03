#ifndef __OBJECT_H
#define __OBJECT_H

/* Object types */
#define REDIS_STRING 0
#define REDIS_LIST 1
#define REDIS_SET 2
#define REDIS_ZSET 3
#define REDIS_HASH 4

/* Objects encoding. Some kind of objects like Strings and Hashes can be
 * internally represented in multiple ways. The 'encoding' field of the object
 * is set to one of this fields for this object. */
#define REDIS_ENCODING_RAW 0     /* Raw representation */
#define REDIS_ENCODING_INT 1     /* Encoded as integer */
#define REDIS_ENCODING_HT 2      /* Encoded as hash table */
#define REDIS_ENCODING_ZIPMAP 3  /* Encoded as zipmap */
#define REDIS_ENCODING_LINKEDLIST 4 /* Encoded as regular linked list */
#define REDIS_ENCODING_ZIPLIST 5 /* Encoded as ziplist */
#define REDIS_ENCODING_INTSET 6  /* Encoded as intset */
#define REDIS_ENCODING_SKIPLIST 7  /* Encoded as skiplist */

typedef struct redisObject {
    unsigned type:4;
    unsigned encoding:4;
    unsigned lru:22;        /* lru time (relative to server.lruclock) */
    int refcount;
    void *ptr;
} robj;

robj *createObject(int type, void *ptr);
robj *createStringObject(char *ptr, size_t len);
robj *createStringObjectFromLongLong(long long value);
robj *createListObject(void);
robj *createZiplistObject(void);
robj *createSetObject(void);
robj *createIntsetObject(void);
robj *createHashObject(void);
robj *createZsetObject(void);
robj *createZsetZiplistObject(void);
void freeStringObject(robj *o);
void freeListObject(robj *o);
void freeSetObject(robj *o);
void freeZsetObject(robj *o);
void freeHashObject(robj *o);
void incrRefCount(robj *o);
void decrRefCount(void *o);
int isObjectRepresentableAsLongLong(robj *o, long long *llongval);
robj *tryObjectEncoding(robj *o);
robj *getDecodedObject(robj *o);
int compareStringObjects(robj *a, robj *b);
int equalStringObjects(robj *a, robj *b);
size_t stringObjectLen(robj *o);

/* XXX(mgp)
int checkType(redisClient *c, robj *o, int type);
int getDoubleFromObjectOrReply(redisClient *c, robj *o, double *target, const char *msg);
int getLongLongFromObject(robj *o, long long *target);
int getLongLongFromObjectOrReply(redisClient *c, robj *o, long long *target, const char *msg);
int getLongFromObjectOrReply(redisClient *c, robj *o, long *target, const char *msg);
unsigned long estimateObjectIdleTime(robj *o);
*/

#endif

