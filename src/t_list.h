#ifndef __T_LIST_H
#define __T_LIST_H

void listTypeTryConversion(robj *subject, robj *value);
void listTypePush(robj *subject, robj *value, int where);
robj *listTypePop(robj *subject, int where);
unsigned long listTypeLength(robj *subject);
listTypeIterator *listTypeInitIterator(robj *subject, int index, unsigned char direction);
void listTypeReleaseIterator(listTypeIterator *li);
int listTypeNext(listTypeIterator *li, listTypeEntry *entry);
robj *listTypeGet(listTypeEntry *entry);
void listTypeInsert(listTypeEntry *entry, robj *value, int where);
int listTypeEqual(listTypeEntry *entry, robj *o);
void listTypeDelete(listTypeEntry *entry);
void listTypeConvert(robj *subject, int enc);
void popGenericCommand(redisClient *c, int where);

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

