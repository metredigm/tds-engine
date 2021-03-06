#pragma once

#include "texture.h"
#include <stdint.h>

#define TDS_BLOCK_TYPE_RTSLOPE   1
#define TDS_BLOCK_TYPE_LTSLOPE  (1 << 1)
#define TDS_BLOCK_TYPE_RBSLOPE  (1 << 2)
#define TDS_BLOCK_TYPE_LBSLOPE  (1 << 3)
#define TDS_BLOCK_TYPE_SOLID  (1 << 4)
#define TDS_BLOCK_TYPE_NOLIGHT  (1 << 5)
#define TDS_BLOCK_TYPE_NODRAW   (1 << 6)

struct tds_block_type {
	struct tds_texture* texture;
	int flags;
};

struct tds_block_map {
	struct tds_block_type buffer[256];
};

struct tds_block_map* tds_block_map_create(void);
void tds_block_map_free(struct tds_block_map* ptr);

void tds_block_map_add(struct tds_block_map* ptr, struct tds_texture* tex, int solid, uint8_t id);
struct tds_block_type tds_block_map_get(struct tds_block_map* ptr, uint8_t id);
