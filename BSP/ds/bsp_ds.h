/**
*  @brief Some Useful DataStruct
 *  @author tiny_fish
 *  @date 2025-11-20
 */

#pragma once

#include <stddef.h>
#include <stdint.h>

// Ring Queue
typedef struct {
    uint8_t *buf;
    int sz, l, r;
} ds_rq_t;

void ds_rq_init(ds_rq_t *q, uint8_t *buf, int sz);
int ds_rq_avail(ds_rq_t *q);
int ds_rq_size(ds_rq_t *q);
uint8_t ds_rq_push(ds_rq_t *q, const uint8_t *data, size_t len);
uint8_t ds_rq_pop(ds_rq_t *q, uint8_t *res, size_t len);