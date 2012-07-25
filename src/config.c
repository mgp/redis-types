#include "redis.h"

/*-----------------------------------------------------------------------------
 * Config file parsing
 *----------------------------------------------------------------------------*/

int yesnotoi(char *s) {
    if (!strcasecmp(s,"yes")) return 1;
    else if (!strcasecmp(s,"no")) return 0;
    else return -1;
}

void appendServerSaveParams(time_t seconds, int changes) {
    server.saveparams = zrealloc(server.saveparams,sizeof(struct saveparam)*(server.saveparamslen+1));
    server.saveparams[server.saveparamslen].seconds = seconds;
    server.saveparams[server.saveparamslen].changes = changes;
    server.saveparamslen++;
}

void resetServerSaveParams() {
    zfree(server.saveparams);
    server.saveparams = NULL;
    server.saveparamslen = 0;
}

/* I agree, this is a very rudimental way to load a configuration...
   will improve later if the config gets more complex */
void loadServerConfig(char *filename) {
  // yay
}

/*-----------------------------------------------------------------------------
 * CONFIG command for remote configuration
 *----------------------------------------------------------------------------*/

void configSetCommand(redisClient *c) {
    robj *o;
    long long ll;
    redisAssert(c->argv[2]->encoding == REDIS_ENCODING_RAW);
    redisAssert(c->argv[3]->encoding == REDIS_ENCODING_RAW);
    o = c->argv[3];

    if (!strcasecmp(c->argv[2]->ptr,"dbfilename")) {
        zfree(server.dbfilename);
        server.dbfilename = zstrdup(o->ptr);
    } else if (!strcasecmp(c->argv[2]->ptr,"requirepass")) {
        zfree(server.requirepass);
        server.requirepass = ((char*)o->ptr)[0] ? zstrdup(o->ptr) : NULL;
    } else if (!strcasecmp(c->argv[2]->ptr,"masterauth")) {
        zfree(server.masterauth);
        server.masterauth = zstrdup(o->ptr);
    } else if (!strcasecmp(c->argv[2]->ptr,"maxmemory")) {
        if (getLongLongFromObject(o,&ll) == REDIS_ERR ||
            ll < 0) goto badfmt;
        server.maxmemory = ll;
        if (server.maxmemory) freeMemoryIfNeeded();
    } else if (!strcasecmp(c->argv[2]->ptr,"maxmemory-policy")) {
        if (!strcasecmp(o->ptr,"volatile-lru")) {
            server.maxmemory_policy = REDIS_MAXMEMORY_VOLATILE_LRU;
        } else if (!strcasecmp(o->ptr,"volatile-random")) {
            server.maxmemory_policy = REDIS_MAXMEMORY_VOLATILE_RANDOM;
        } else if (!strcasecmp(o->ptr,"volatile-ttl")) {
            server.maxmemory_policy = REDIS_MAXMEMORY_VOLATILE_TTL;
        } else if (!strcasecmp(o->ptr,"allkeys-lru")) {
            server.maxmemory_policy = REDIS_MAXMEMORY_ALLKEYS_LRU;
        } else if (!strcasecmp(o->ptr,"allkeys-random")) {
            server.maxmemory_policy = REDIS_MAXMEMORY_ALLKEYS_RANDOM;
        } else if (!strcasecmp(o->ptr,"noeviction")) {
            server.maxmemory_policy = REDIS_MAXMEMORY_NO_EVICTION;
        } else {
            goto badfmt;
        }
    } else if (!strcasecmp(c->argv[2]->ptr,"maxmemory-samples")) {
        if (getLongLongFromObject(o,&ll) == REDIS_ERR ||
            ll <= 0) goto badfmt;
        server.maxmemory_samples = ll;
    } else if (!strcasecmp(c->argv[2]->ptr,"timeout")) {
        if (getLongLongFromObject(o,&ll) == REDIS_ERR ||
            ll < 0 || ll > LONG_MAX) goto badfmt;
        server.maxidletime = ll;
    } else if (!strcasecmp(c->argv[2]->ptr,"appendfsync")) {
        if (!strcasecmp(o->ptr,"no")) {
            server.appendfsync = APPENDFSYNC_NO;
        } else if (!strcasecmp(o->ptr,"everysec")) {
            server.appendfsync = APPENDFSYNC_EVERYSEC;
        } else if (!strcasecmp(o->ptr,"always")) {
            server.appendfsync = APPENDFSYNC_ALWAYS;
        } else {
            goto badfmt;
        }
    } else if (!strcasecmp(c->argv[2]->ptr,"no-appendfsync-on-rewrite")) {
        int yn = yesnotoi(o->ptr);

        if (yn == -1) goto badfmt;
        server.no_appendfsync_on_rewrite = yn;
    } else if (!strcasecmp(c->argv[2]->ptr,"appendonly")) {
        int old = server.appendonly;
        int new = yesnotoi(o->ptr);

        if (new == -1) goto badfmt;
        if (old != new) {
            if (new == 0) {
                stopAppendOnly();
            } else {
                if (startAppendOnly() == REDIS_ERR) {
                    addReplyError(c,
                        "Unable to turn on AOF. Check server logs.");
                    return;
                }
            }
        }
    } else if (!strcasecmp(c->argv[2]->ptr,"auto-aof-rewrite-percentage")) {
        if (getLongLongFromObject(o,&ll) == REDIS_ERR || ll < 0) goto badfmt;
        server.auto_aofrewrite_perc = ll;
    } else if (!strcasecmp(c->argv[2]->ptr,"auto-aof-rewrite-min-size")) {
        if (getLongLongFromObject(o,&ll) == REDIS_ERR || ll < 0) goto badfmt;
        server.auto_aofrewrite_min_size = ll;
    } else if (!strcasecmp(c->argv[2]->ptr,"save")) {
        int vlen, j;
        sds *v = sdssplitlen(o->ptr,sdslen(o->ptr)," ",1,&vlen);

        /* Perform sanity check before setting the new config:
         * - Even number of args
         * - Seconds >= 1, changes >= 0 */
        if (vlen & 1) {
            sdsfreesplitres(v,vlen);
            goto badfmt;
        }
        for (j = 0; j < vlen; j++) {
            char *eptr;
            long val;

            val = strtoll(v[j], &eptr, 10);
            if (eptr[0] != '\0' ||
                ((j & 1) == 0 && val < 1) ||
                ((j & 1) == 1 && val < 0)) {
                sdsfreesplitres(v,vlen);
                goto badfmt;
            }
        }
        /* Finally set the new config */
        resetServerSaveParams();
        for (j = 0; j < vlen; j += 2) {
            time_t seconds;
            int changes;

            seconds = strtoll(v[j],NULL,10);
            changes = strtoll(v[j+1],NULL,10);
            appendServerSaveParams(seconds, changes);
        }
        sdsfreesplitres(v,vlen);
    } else if (!strcasecmp(c->argv[2]->ptr,"slave-serve-stale-data")) {
        int yn = yesnotoi(o->ptr);

        if (yn == -1) goto badfmt;
        server.repl_serve_stale_data = yn;
    } else if (!strcasecmp(c->argv[2]->ptr,"dir")) {
        if (chdir((char*)o->ptr) == -1) {
            addReplyErrorFormat(c,"Changing directory: %s", strerror(errno));
            return;
        }
    } else if (!strcasecmp(c->argv[2]->ptr,"hash-max-zipmap-entries")) {
        if (getLongLongFromObject(o,&ll) == REDIS_ERR || ll < 0) goto badfmt;
        server.hash_max_zipmap_entries = ll;
    } else if (!strcasecmp(c->argv[2]->ptr,"hash-max-zipmap-value")) {
        if (getLongLongFromObject(o,&ll) == REDIS_ERR || ll < 0) goto badfmt;
        server.hash_max_zipmap_value = ll;
    } else if (!strcasecmp(c->argv[2]->ptr,"list-max-ziplist-entries")) {
        if (getLongLongFromObject(o,&ll) == REDIS_ERR || ll < 0) goto badfmt;
        server.list_max_ziplist_entries = ll;
    } else if (!strcasecmp(c->argv[2]->ptr,"list-max-ziplist-value")) {
        if (getLongLongFromObject(o,&ll) == REDIS_ERR || ll < 0) goto badfmt;
        server.list_max_ziplist_value = ll;
    } else if (!strcasecmp(c->argv[2]->ptr,"set-max-intset-entries")) {
        if (getLongLongFromObject(o,&ll) == REDIS_ERR || ll < 0) goto badfmt;
        server.set_max_intset_entries = ll;
    } else if (!strcasecmp(c->argv[2]->ptr,"zset-max-ziplist-entries")) {
        if (getLongLongFromObject(o,&ll) == REDIS_ERR || ll < 0) goto badfmt;
        server.zset_max_ziplist_entries = ll;
    } else if (!strcasecmp(c->argv[2]->ptr,"zset-max-ziplist-value")) {
        if (getLongLongFromObject(o,&ll) == REDIS_ERR || ll < 0) goto badfmt;
        server.zset_max_ziplist_value = ll;
    } else if (!strcasecmp(c->argv[2]->ptr,"slowlog-log-slower-than")) {
        if (getLongLongFromObject(o,&ll) == REDIS_ERR) goto badfmt;
        server.slowlog_log_slower_than = ll;
    } else if (!strcasecmp(c->argv[2]->ptr,"slowlog-max-len")) {
        if (getLongLongFromObject(o,&ll) == REDIS_ERR || ll < 0) goto badfmt;
        server.slowlog_max_len = (unsigned)ll;
    } else if (!strcasecmp(c->argv[2]->ptr,"loglevel")) {
        if (!strcasecmp(o->ptr,"warning")) {
            server.verbosity = REDIS_WARNING;
        } else if (!strcasecmp(o->ptr,"notice")) {
            server.verbosity = REDIS_NOTICE;
        } else if (!strcasecmp(o->ptr,"verbose")) {
            server.verbosity = REDIS_VERBOSE;
        } else if (!strcasecmp(o->ptr,"debug")) {
            server.verbosity = REDIS_DEBUG;
        } else {
            goto badfmt;
        }
    } else {
        addReplyErrorFormat(c,"Unsupported CONFIG parameter: %s",
            (char*)c->argv[2]->ptr);
        return;
    }
    addReply(c,shared.ok);
    return;

badfmt: /* Bad format errors */
    addReplyErrorFormat(c,"Invalid argument '%s' for CONFIG SET '%s'",
            (char*)o->ptr,
            (char*)c->argv[2]->ptr);
}

void configGetCommand(redisClient *c) {
}

void configCommand(redisClient *c) {
    if (!strcasecmp(c->argv[1]->ptr,"set")) {
        if (c->argc != 4) goto badarity;
        configSetCommand(c);
    } else if (!strcasecmp(c->argv[1]->ptr,"get")) {
        if (c->argc != 3) goto badarity;
        configGetCommand(c);
    } else if (!strcasecmp(c->argv[1]->ptr,"resetstat")) {
        if (c->argc != 2) goto badarity;
        server.stat_keyspace_hits = 0;
        server.stat_keyspace_misses = 0;
        server.stat_numcommands = 0;
        server.stat_numconnections = 0;
        server.stat_expiredkeys = 0;
        addReply(c,shared.ok);
    } else {
        addReplyError(c,
            "CONFIG subcommand must be one of GET, SET, RESETSTAT");
    }
    return;

badarity:
    addReplyErrorFormat(c,"Wrong number of arguments for CONFIG %s",
        (char*) c->argv[1]->ptr);
}
