/* SDSLib, A C dynamic strings library
 *
 * Copyright (c) 2006-2010, Salvatore Sanfilippo <antirez at gmail dot com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of Redis nor the names of its contributors may be used
 *     to endorse or promote products derived from this software without
 *     specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * History:
 *
 * - 22 March 2011: History section created on top of sds.c
 * - 22 March 2011: Fixed a problem with "\xab" escapes convertion in
 *                  function sdssplitargs().
 */

#define SDS_ABORT_ON_OOM

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "zmalloc.h"

#include "sds.h"

static void sdsOomAbort(void) {
    fprintf(stderr,"SDS: Out Of Memory (SDS_ABORT_ON_OOM defined)\n");
    abort();
}

sds sdsnewlen(const void *init, size_t initlen) {
    struct sdshdr *sh;

    sh = zmalloc(sizeof(struct sdshdr)+initlen+1);
#ifdef SDS_ABORT_ON_OOM
    if (sh == NULL) sdsOomAbort();
#else
    if (sh == NULL) return NULL;
#endif
    sh->len = initlen;
    sh->free = 0;
    if (initlen) {
        if (init) memcpy(sh->buf, init, initlen);
        else memset(sh->buf,0,initlen);
    }
    sh->buf[initlen] = '\0';
    return (char*)sh->buf;
}

sds sdsempty(void) {
    return sdsnewlen("",0);
}

sds sdsnew(const char *init) {
    size_t initlen = (init == NULL) ? 0 : strlen(init);
    return sdsnewlen(init, initlen);
}

sds sdsdup(const sds s) {
    return sdsnewlen(s, sdslen(s));
}

void sdsfree(sds s) {
    if (s == NULL) return;
    zfree(s-sizeof(struct sdshdr));
}

static sds sdsMakeRoomFor(sds s, size_t addlen) {
    struct sdshdr *sh, *newsh;
    size_t free = sdsavail(s);
    size_t len, newlen;

    if (free >= addlen) return s;
    len = sdslen(s);
    sh = (void*) (s-(sizeof(struct sdshdr)));
    newlen = (len+addlen);
    if (newlen < SDS_MAX_PREALLOC)
        newlen *= 2;
    else
        newlen += SDS_MAX_PREALLOC;
    newsh = zrealloc(sh, sizeof(struct sdshdr)+newlen+1);
#ifdef SDS_ABORT_ON_OOM
    if (newsh == NULL) sdsOomAbort();
#else
    if (newsh == NULL) return NULL;
#endif

    newsh->free = newlen - len;
    return newsh->buf;
}

/* Grow the sds to have the specified length. Bytes that were not part of
 * the original length of the sds will be set to zero. */
sds sdsgrowzero(sds s, size_t len) {
    struct sdshdr *sh = (void*)(s-(sizeof(struct sdshdr)));
    size_t totlen, curlen = sh->len;

    if (len <= curlen) return s;
    s = sdsMakeRoomFor(s,len-curlen);
    if (s == NULL) return NULL;

    /* Make sure added region doesn't contain garbage */
    sh = (void*)(s-(sizeof(struct sdshdr)));
    memset(s+curlen,0,(len-curlen+1)); /* also set trailing \0 byte */
    totlen = sh->len+sh->free;
    sh->len = len;
    sh->free = totlen-sh->len;
    return s;
}

sds sdscatlen(sds s, void *t, size_t len) {
    struct sdshdr *sh;
    size_t curlen = sdslen(s);

    s = sdsMakeRoomFor(s,len);
    if (s == NULL) return NULL;
    sh = (void*) (s-(sizeof(struct sdshdr)));
    memcpy(s+curlen, t, len);
    sh->len = curlen+len;
    sh->free = sh->free-len;
    s[curlen+len] = '\0';
    return s;
}

sds sdscat(sds s, char *t) {
    return sdscatlen(s, t, strlen(t));
}

int sdscmp(sds s1, sds s2) {
    size_t l1, l2, minlen;
    int cmp;

    l1 = sdslen(s1);
    l2 = sdslen(s2);
    minlen = (l1 < l2) ? l1 : l2;
    cmp = memcmp(s1,s2,minlen);
    if (cmp == 0) return l1-l2;
    return cmp;
}

sds sdsfromlonglong(long long value) {
    char buf[32], *p;
    unsigned long long v;

    v = (value < 0) ? -value : value;
    p = buf+31; /* point to the last character */
    do {
        *p-- = '0'+(v%10);
        v /= 10;
    } while(v);
    if (value < 0) *p-- = '-';
    p++;
    return sdsnewlen(p,32-(p-buf));
}

#ifdef SDS_TEST_MAIN
#include <stdio.h>
#include "testhelp.h"

int main(void) {
    {
        sds x = sdsnew("foo"), y;

        test_cond("Create a string and obtain the length",
            sdslen(x) == 3 && memcmp(x,"foo\0",4) == 0)

        sdsfree(x);
        x = sdsnewlen("foo",2);
        test_cond("Create a string with specified length",
            sdslen(x) == 2 && memcmp(x,"fo\0",3) == 0)

        x = sdscat(x,"bar");
        test_cond("Strings concatenation",
            sdslen(x) == 5 && memcmp(x,"fobar\0",6) == 0);

        sdsfree(x);
        x = sdsnew("foo");
        y = sdsnew("foa");
        test_cond("sdscmp(foo,foa)", sdscmp(x,y) > 0)

        sdsfree(y);
        sdsfree(x);
        x = sdsnew("bar");
        y = sdsnew("bar");
        test_cond("sdscmp(bar,bar)", sdscmp(x,y) == 0)

        sdsfree(y);
        sdsfree(x);
        x = sdsnew("aar");
        y = sdsnew("bar");
        test_cond("sdscmp(bar,bar)", sdscmp(x,y) < 0)
    }
    test_report()
}
#endif
