# Redis Makefile
# Copyright (C) 2009 Salvatore Sanfilippo <antirez at gmail dot com>
# This file is released under the BSD license, see the COPYING file

uname_S := $(shell sh -c 'uname -s 2>/dev/null || echo not')
OPTIMIZATION?=-O2

CCCOLOR="\033[34m"
LINKCOLOR="\033[34;1m"
SRCCOLOR="\033[33m"
BINCOLOR="\033[37;1m"
MAKECOLOR="\033[32;1m"
ENDCOLOR="\033[0m"

ifndef V
QUIET_CC = @printf '    %b %b\n' $(CCCOLOR)CC$(ENDCOLOR) $(SRCCOLOR)$@$(ENDCOLOR);
QUIET_LINK = @printf '    %b %b\n' $(LINKCOLOR)LINK$(ENDCOLOR) $(BINCOLOR)$@$(ENDCOLOR);
endif

ifeq ($(uname_S),Linux)
  ifneq ($(FORCE_LIBC_MALLOC),yes)
    USE_JEMALLOC=yes
  endif
endif

ifeq ($(uname_S),SunOS)
  CFLAGS?=-std=c99 -pedantic $(OPTIMIZATION) -Wall -W -D__EXTENSIONS__ -D_XPG6
  CCLINK?=-ldl -lnsl -lsocket -lm -lpthread
  DEBUG?=-g -ggdb
else
  CFLAGS?=-std=c99 -pedantic $(OPTIMIZATION) -Wall -W $(ARCH) $(PROF)
  CCLINK?=-lm -pthread
  DEBUG?=-g -rdynamic -ggdb
endif

ifeq ($(USE_JEMALLOC),yes)
  ALLOC_DEP=../deps/jemalloc/lib/libjemalloc.a
  ALLOC_LINK=$(ALLOC_DEP) -ldl
  ALLOC_FLAGS=-DUSE_JEMALLOC -I../deps/jemalloc/include
endif

CCOPT= $(CFLAGS) $(ARCH) $(PROF)

OBJ = adlist.o dict.o redis.o zmalloc.o zipmap.o ziplist.o t_string.o t_list.o t_set.o t_zset.o t_hash.o intset.o

# Deps (use make dep to generate this)
adlist.o: adlist.c adlist.h zmalloc.h
dict.o: dict.c fmacros.h dict.h zmalloc.h
endian.o: endian.c
intset.o: intset.c zmalloc.h endian.h
object.o: object.c intset.h sds.h util.h ziplist.h zmalloc.h
pqsort.o: pqsort.c
redis.o: redis.c redis.h dict.h adlist.h \
  zmalloc.h zipmap.h ziplist.h intset.h
sds.o: sds.c zmalloc.h
t_hash.o: t_hash.c redis.h dict.h adlist.h \
  zmalloc.h zipmap.h ziplist.h intset.h
t_list.o: t_list.c redis.h dict.h adlist.h \
  zmalloc.h zipmap.h ziplist.h intset.h
t_set.o: t_set.c redis.h dict.h adlist.h \
  zmalloc.h zipmap.h ziplist.h intset.h
t_string.o: t_string.c redis.h dict.h \
  adlist.h zmalloc.h zipmap.h ziplist.h intset.h
t_zset.o: t_zset.c redis.h dict.h adlist.h \
  zmalloc.h zipmap.h ziplist.h intset.h
util.o: util.c
ziplist.o: ziplist.c zmalloc.h util.h endian.h
zipmap.o: zipmap.c zmalloc.h endian.h
zmalloc.o: zmalloc.c

main: $(OBJ)
	$(QUIET_LINK)$(CC) -o yourmom $(CCOPT) $(DEBUG) $(OBJ) $(CCLINK) $(ALLOC_LINK)

../deps/jemalloc/lib/libjemalloc.a:
	@printf '%b %b\n' $(MAKECOLOR)MAKE$(ENDCOLOR) $(BINCOLOR)jemalloc$(ENDCOLOR)
	@cd ../deps/jemalloc && ./configure $(JEMALLOC_CFLAGS) --with-jemalloc-prefix=je_ --enable-cc-silence && $(MAKE) lib/libjemalloc.a

# Because the jemalloc.h header is generated as a part of the jemalloc build
# process, building it should complete before building any other object.
%.o: %.c $(ALLOC_DEP)
	$(QUIET_CC)$(CC) -c $(CFLAGS) $(ALLOC_FLAGS) $(DEBUG) $(COMPILE_TIME) $<

clean:
	rm -rf *.o

