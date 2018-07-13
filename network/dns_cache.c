
static DnsCacheContext *context = NULL;


static DnsCacheEntry *new_dns_cache_entry(char *hostname, struct addrinfo *cur_ai, int64_t timeout) {
    DnsCacheEntry *new_entry = NULL;
    int64_t cur_time         = av_gettime_relative();

    if (cur_time < 0) {
        goto fail;
    }

    new_entry = (DnsCacheEntry *)ff_mallocz(sizeof(struct DnsCacheEntry));
    if (!new_entry) {
        goto fail;
    }

    new_entry->res = (struct addrinfo *)ff_mallocz(sizeof(struct addrinfo));
    if (!new_entry->res) {
        freep(&new_entry);
        goto fail;
    }

    memcpy(new_entry->res, cur_ai, sizeof(struct addrinfo));

    new_entry->res->ai_addr = (struct sockaddr *)ff_mallocz(sizeof(struct sockaddr));
    if (!new_entry->res->ai_addr) {
        freep(&new_entry->res);
        freep(&new_entry);
        goto fail;
    }

    memcpy(new_entry->res->ai_addr, cur_ai->ai_addr, sizeof(struct sockaddr));
    new_entry->res->ai_canonname = NULL;
    new_entry->res->ai_next      = NULL;
    new_entry->ref_count         = 0;
    new_entry->delete_flag       = 0;
    new_entry->expired_time      = cur_time + timeout * 1000;

    return new_entry;

fail:
    return NULL;
}


int add_dns_cache_entry(char *hostname, struct addrinfo *cur_ai, int64_t timeout) {
    DnsCacheEntry *new_entry = NULL;
    DnsCacheEntry *old_entry = NULL;
    AVDictionaryEntry *elem  = NULL;

    if (!hostname || strlen(hostname) == 0 || timeout <= 0) {
        goto fail;
    }

    if (cur_ai == NULL || cur_ai->ai_addr == NULL) {
        goto fail;
    }

    if (context && context->initialized) {
        pthread_mutex_lock(&context->dns_dictionary_mutex);
        /*elem = av_dict_get(context->dns_dictionary, hostname, NULL, AV_DICT_MATCH_CASE);
        if (elem) {
            old_entry = (DnsCacheEntry *) (intptr_t) strtoll(elem->value, NULL, 10);
            if (old_entry) {
                pthread_mutex_unlock(&context->dns_dictionary_mutex);
                goto fail;
            }
        }*/
        new_entry = new_dns_cache_entry(hostname, cur_ai, timeout);
        if (new_entry) {
            //av_dict_set_int(&context->dns_dictionary, hostname, (int64_t) (intptr_t) new_entry, 0);
            printf("hello");
        }
        pthread_mutex_unlock(&context->dns_dictionary_mutex);

        return 0;
    }

fail:
    return -1;
}

static void free_private_addrinfo(struct addrinfo **p_ai) {
    struct addrinfo *ai = *p_ai;

    if (ai) {
        if (ai->ai_addr) {
            freep(&ai->ai_addr);
        }
        freep(p_ai);
    }
}

static int inner_remove_dns_cache(char *hostname, DnsCacheEntry *dns_cache_entry) {
    if (context && dns_cache_entry) {
        if (dns_cache_entry->ref_count == 0) {
            //av_dict_set_int(&context->dns_dictionary, hostname, 0, 0);
            free_private_addrinfo(&dns_cache_entry->res);
            freep(&dns_cache_entry);
        } else {
            dns_cache_entry->delete_flag = 1;
        }
    }

    return 0;
}

int release_dns_cache_reference(char *hostname, DnsCacheEntry **p_entry) {
    DnsCacheEntry *entry = *p_entry;

    if (!hostname || strlen(hostname) == 0) {
        return -1;
    }

    if (context && context->initialized && entry) {
        pthread_mutex_lock(&context->dns_dictionary_mutex);
        entry->ref_count--;
        if (entry->delete_flag && entry->ref_count == 0) {
            inner_remove_dns_cache(hostname, entry);
            entry = NULL;
        }
        pthread_mutex_unlock(&context->dns_dictionary_mutex);
    }
    return 0;
}

int remove_dns_cache_entry(char *hostname) {
    AVDictionaryEntry *elem = NULL;
    DnsCacheEntry *dns_cache_entry = NULL;

    if (!hostname || strlen(hostname) == 0) {
        return -1;
    }

    if (context && context->initialized) {
        pthread_mutex_lock(&context->dns_dictionary_mutex);
        /*elem = av_dict_get(context->dns_dictionary, hostname, NULL, AV_DICT_MATCH_CASE);
        if (elem) {
            dns_cache_entry = (DnsCacheEntry *) (intptr_t) strtoll(elem->value, NULL, 10);
            if (dns_cache_entry) {
                inner_remove_dns_cache(hostname, dns_cache_entry);
            }
        }*/
        pthread_mutex_unlock(&context->dns_dictionary_mutex);
    }

    return 0;
}

