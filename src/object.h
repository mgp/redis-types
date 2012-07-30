#ifndef __OBJECT_H
#define __OBJECT_H

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
int checkType(redisClient *c, robj *o, int type);
int isObjectRepresentableAsLongLong(robj *o, long long *llongval);
robj *tryObjectEncoding(robj *o);
robj *getDecodedObject(robj *o);
int compareStringObjects(robj *a, robj *b);
int equalStringObjects(robj *a, robj *b);
size_t stringObjectLen(robj *o);
int getDoubleFromObjectOrReply(redisClient *c, robj *o, double *target, const char *msg);
int getLongLongFromObject(robj *o, long long *target);
int getLongLongFromObjectOrReply(redisClient *c, robj *o, long long *target, const char *msg);
int getLongFromObjectOrReply(redisClient *c, robj *o, long *target, const char *msg);
unsigned long estimateObjectIdleTime(robj *o);

#endif

