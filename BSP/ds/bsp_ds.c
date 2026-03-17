/**
*  @brief Some Useful DataStruct
 *  @author tiny_fish
 *  @date 2025-11-20
 */
#include "bsp_ds.h"

#include <string.h>

void ds_rq_init(ds_rq_t *q, uint8_t *buf, const int sz) {
    q->buf = buf;
    q->sz = sz, q->l = 0, q->r = 0;
}

int ds_rq_size(ds_rq_t *q) {
    return (q->r - q->l + q->sz) % q->sz;
}

int ds_rq_avail(ds_rq_t *q) {
    return q->sz - 1 - ds_rq_size(q);
}

uint8_t ds_rq_push(ds_rq_t *q, const uint8_t *data, const size_t len) {
    size_t fs = ds_rq_avail(q);
    if (len > fs) return 0;
    for (size_t i = 0; i < len; i++) {
        q->buf[q->r] = data[i];
        q->r = (q->r + 1) % q->sz;
    }
    return 1;
}

uint8_t ds_rq_pop(ds_rq_t *q, uint8_t *res, const size_t len) {
    size_t cs = ds_rq_size(q);
    if (len > cs) return 0;
    for (size_t i = 0; i < len; i++) {
        res[i] = q->buf[q->l];
        q->l = (q->l + 1) % q->sz;
    }
    return 1;
}