/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include <stdlib.h>

#include "aos/kernel.h"
#include "k_config.h"

#define SEED_MAGIC 0x123

extern void hal_reboot(void);

struct component_version_t {
    const char *name;
    const char *version;
};

static struct component_version_t comp_version[SYSINFO_VERSION_MAX] = {
                                                                          {"kernel", SYSINFO_KERNEL_VERSION},
                                                                          {"system", SYSINFO_SYSTEM_VERSION},
                                                                          {"aosapi", SYSINFO_AOSAPI_VERSION},
                                                                      };

void aos_reboot(void)
{
    hal_reboot();
}

int32_t aos_get_hz(void)
{
    return RHINO_CONFIG_TICKS_PER_SECOND;
}

aos_status_t aos_version_str_get(char *buf, size_t len)
{
    const char *ver_str = SYSINFO_KERNEL_VERSION;

    if (buf == NULL) {
        return -EINVAL;
    }

    if (len <= strlen(ver_str)) {
        return -EINVAL;
    }

    memcpy(buf, ver_str, strlen(ver_str));
    return 0;
}

#if !defined (ENABLE_RNG) && defined (AOS_COMP_KV)
#include "aos/kv.h"
#endif

void aos_srand(uint32_t seed)
{
#if !defined (ENABLE_RNG) && defined (AOS_COMP_KV)
    int           ret        = 0;
    int           seed_len   = 0;
    uint32_t      seed_val   = 0;
    static char  *g_seed_key = "seed_key";

    seed_len = sizeof(seed_val);
    ret = aos_kv_get(g_seed_key, &seed_val, &seed_len);
    if (ret) {
        seed_val = SEED_MAGIC;
    }
    seed_val += seed;
    srandom(seed_val);

    seed_val = (uint32_t)random();
    ret = aos_kv_set(g_seed_key, &seed_val, sizeof(seed_val), 1);
    if (ret) {
        printf("aos_kv_set error, return :%d\r\n", ret);
    }
#else
    srandom(seed);
#endif
}

int32_t aos_rand(void)
{
    return (int32_t)random();
}

const char *aos_comp_version_get(const char *comp_name)
{
    int i;

    if (comp_name == NULL) {
        return NULL;
    }

    for (i = 0; i < SYSINFO_VERSION_MAX; i++) {
        if (strcmp(comp_version[i].name, comp_name) == 0) {
            return comp_version[i].version;
        }
    }

    return NULL;
}

int32_t aos_comp_version_set(const char *comp_name, const char *version)
{
    int i;

    if ((comp_name == NULL) || (version == NULL)) {
        return -1;
    }

    for (i = 0; i < SYSINFO_VERSION_MAX; i++) {
        if (comp_version[i].name == NULL) {
            comp_version[i].name    = comp_name;
            comp_version[i].version = version;
            return 0;
        }

        if (strcmp(comp_version[i].name, comp_name) == 0) {
            comp_version[i].version = version;
            return 0;
        }
   }

    return -1;
}

