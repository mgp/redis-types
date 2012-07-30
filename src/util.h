#ifndef __UTIL_H
#define __UTIL_H

int stringmatchlen(const char *p, int plen, const char *s, int slen, int nocase);
int ll2string(char *s, size_t len, long long value);
int string2ll(char *s, size_t slen, long long *value);
int string2l(char *s, size_t slen, long *value);
int d2string(char *buf, size_t len, double value);

#endif
