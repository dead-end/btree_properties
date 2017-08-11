/***************************************************************************
 * btree_properties.h
 *
 *  Created on: Aug 5, 2017
 *      Author: Dead-End
 **************************************************************************/

#ifndef BTREE_PROPERTIES_H_
#define BTREE_PROPERTIES_H_

#include <stdbool.h>

/***************************************************************************
 * The struct is a simple wrapper around the pointer 'void *root'. The btree
 * api uses 'void *root' and 'void **rootp' pointers. The use is error
 * prone, because the wrong pointer does not lead to compiler warnings.
 **************************************************************************/

typedef struct BTP_ctx {
	void *root;
	int num_entries;
} BTP_ctx;

BTP_ctx *btp_create_ctx();

void btp_destroy_ctx(BTP_ctx *ctx);

int btp_get_num_entries(BTP_ctx *ctx);

char *btp_get_property_value(const BTP_ctx *ctx, char *key);

bool btp_add_property(BTP_ctx *ctx, char *key, const char *value, const bool replace);

void btp_read_properties(BTP_ctx *ctx, const char *filename);

void btp_iterate_properties(const BTP_ctx *ctx, void (*callback)(const char *key, const char *value));

bool btp_delete_property(BTP_ctx *ctx, char *key);

#define DEBUG

#endif /* BTREE_PROPERTIES_H_ */
