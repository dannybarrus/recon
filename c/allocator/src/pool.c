#include "recon/pool.h"

#include <assert.h>

void pool_init(Pool* pool, unsigned char* buffer, size_t block_size, size_t block_count) {
    assert(block_size >= sizeof(void*) &&
           "pool block_size must be at least sizeof(void*) -- see pool.h for why");

    pool->buffer = buffer;
    pool->block_size = block_size;
    pool->block_count = block_count;
    pool->free_list = NULL;

    for (size_t i = block_count; i > 0; i--) {
        unsigned char* block = buffer + (i - 1) * block_size;
        *(void**)block = pool->free_list;
        pool->free_list = block;
    }
}

void* pool_alloc(Pool* pool) {
    if (!pool->free_list) {
        return NULL;
    }
    void* block = pool->free_list;
    pool->free_list = *(void**)block;
    return block;
}

void pool_free(Pool* pool, void* ptr) {
    *(void**)ptr = pool->free_list;
    pool->free_list = ptr;
}

size_t pool_blocks_free(const Pool* pool) {
    size_t count = 0;
    for (void* node = pool->free_list; node != NULL; node = *(void**)node) {
        count++;
    }
    return count;
}
