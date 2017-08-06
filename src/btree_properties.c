/***************************************************************************
 * btree_properties.c
 *
 *  Created on: Aug 5, 2017
 *      Author: Dead-End
 **************************************************************************/

//
// Expose declaration of tdestroy()
//
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <search.h>

#include "btree_properties.h"
#include "btree_utils.h"

//
// definition of print_debug macro
//
#ifdef DEBUG
#define DEBUG_OUT stdout
#define print_debug(fmt, ...) fprintf(DEBUG_OUT, "DEBUG - " fmt, ##__VA_ARGS__)
#else
#define print_debug(fmt, ...)
#endif

#define MAX_LINE 1024

/***************************************************************************
 * An entry consists of a string key and a string value.
 **************************************************************************/

typedef struct Entry {
	char *key;
	char *value;
} Entry;

/***************************************************************************
 * The method creates an entry with a given key and value. NULL keys or
 * values are not allowed.
 **************************************************************************/

static Entry *create_entry(const char *key, const char *value) {

	//
	// allocate memory for the entry
	//
	Entry *entry = malloc(sizeof(Entry));

	if (entry == NULL) {
		fprintf(stderr, "create_entry() Unable allocate memory!\n");
		exit(EXIT_FAILURE);
	}

	//
	// ensure that the key and the value are not null
	//
	if (key == NULL) {
		fprintf(stderr, "create_entry() Key is NULL!\n");
		exit(EXIT_FAILURE);
	}

	if (value == NULL) {
		fprintf(stderr, "create_entry() Value is NULL for key: '%s'!\n", key);
		exit(EXIT_FAILURE);
	}

	//
	// duplicate the key and the value
	//
	entry->key = strdup(key);
	entry->value = strdup(value);

	if (entry->key == NULL || entry->value == NULL) {
		fprintf(stderr, "create_entry() Unable allocate memory!\n");
		exit(EXIT_FAILURE);
	}

	print_debug("create_entry() key: '%s' value: '%s'\n", entry->key, entry->value);

	return entry;
}

/***************************************************************************
 * The method deleted an entry, which means that the memory has to be freed.
 **************************************************************************/

static void delete_entry(void *ptr) {

	if (ptr == NULL) {
		fprintf(stderr, "delete_entry() Entry is NULL\n");
		exit(EXIT_FAILURE);
	}

	Entry *entry = (Entry *) ptr;

	print_debug("delete_entry() key: '%s' value: '%s'\n",
			entry->key,
			entry->value);

	//
	// free the duplicated key and value and the entry struct
	//
	free(entry->key);
	free(entry->value);
	free(entry);
}

/***************************************************************************
 * The method replaces the value of a given entry. The old value has to be
 * freed.
 **************************************************************************/

static void replace_entry_value(Entry *entry, const char *new_value) {
	print_debug(
			"replace_entry_value() Replace key: '%s' old value: '%s' new value: '%s'\n",
			entry->key, entry->value, new_value);

	free(entry->value);
	entry->value = strdup(new_value);
}

/***************************************************************************
 * The function is a callback handler that compares two entries. Two entries
 * are equal if the keys are equal. The method is used to find entries by a
 * given key.
 **************************************************************************/

static int compare_entries(const void *ptr1, const void *ptr2) {

	if (ptr1 == NULL || ptr2 == NULL) {
		fprintf(stderr, "compare_entries() Entry is NULL\n");
		exit(EXIT_FAILURE);
	}

	const Entry *entry1 = (const Entry *) ptr1;
	const Entry *entry2 = (const Entry *) ptr2;

	print_debug("compare_entries() key1: '%s' value1: '%s' key2: '%s' value2: '%s'\n",
			entry1->key,
			entry1->value,
			entry2->key,
			entry2->value);

	return strcmp(entry1->key, entry2->key);
}

/***************************************************************************
 * The method creates a btree context.
 **************************************************************************/
BTP_ctx *btp_create_ctx() {
	BTP_ctx *ctx = malloc(sizeof(BTP_ctx));

	if (ctx == NULL) {
		fprintf(stderr, "btp_create_ctx() Unable allocate memory!\n");
		exit(EXIT_FAILURE);
	}

	ctx->root = NULL;
	ctx->num_entries = 0;

	print_debug("btp_create_ctx() Created context.\n");
	return ctx;
}

/***************************************************************************
 * The method destroys a btree context.
 **************************************************************************/
void btp_destroy_ctx(BTP_ctx *ctx) {
	tdestroy(ctx->root, delete_entry);
	free(ctx);
	print_debug("btp_destroy_ctx() Finished.\n");
}

/***************************************************************************
 * The method returns the number of entries.
 **************************************************************************/
int btp_get_num_entries(BTP_ctx *ctx) {
	print_debug("btp_get_num_entries() Number of entries: %d\n", ctx->num_entries);
	return ctx->num_entries;
}

/***************************************************************************
 * The function is a callback handler that prints an entry.
 **************************************************************************/

static void print_action(const void *nodep, const VISIT which, const int depth) {

	if (which == leaf || which == preorder) {
		const Entry *entry = *(const Entry **) nodep;

		printf("print_action() Depth: %d key: '%s' value: '%s'\n", depth,
				entry->key, entry->value);
	}
}

/***************************************************************************
 * The function prints all entries.
 **************************************************************************/
void btp_print_properties(const BTP_ctx *ctx) {

	printf("btp_print_properties() Num entries: %d\n", ctx->num_entries);
	twalk(ctx->root, print_action);
}

/***************************************************************************
 * The function adds a key value pair to the properties, if the key not
 * already exists.
 **************************************************************************/

void btp_add_property(BTP_ctx *ctx, char *key, const char *value,
		const bool replace) {

	//
	// check if the property already exists
	//
	const Entry search_key = { key, NULL, true };
	print_debug("btp_add_property() Search key: '%s'\n", search_key.key);
	const void *ptr = tfind(&search_key, &(ctx->root), compare_entries);

	if (ptr != NULL) {
		Entry *search_result = *(Entry **) ptr;

		//
		// entry with the key found but replace is not allowed => error
		//
		if (!replace) {
			fprintf(stderr,
					"btp_add_property() Key: '%s' already defined with value: '%s'\n",
					search_result->key, search_result->value);
			exit(EXIT_FAILURE);

			//
			// entry with the key found => free old value and duplicate new value
			//
		} else {
			replace_entry_value(search_result, value);
			return;
		}
	}
	//
	// create and add property
	//
	const Entry *entry = create_entry(key, value);
	print_debug("btp_add_property() Created key: '%s' value: '%s'\n", entry->key,
			entry->value);

#ifdef DEBUG
	const Entry *add_result = *(Entry **)
#endif
	tsearch((void *) entry, &(ctx->root), compare_entries);
	ctx->num_entries++;

	print_debug("btp_add_property() Added key: '%s' value: '%s' num entries: %d\n", add_result->key,
			add_result->value, ctx->num_entries);
}

/***************************************************************************
 * The function adds a key value pair to the properties, if the key not
 * already exists.
 **************************************************************************/

bool btp_delete_property(BTP_ctx *ctx, char *key) {

	//
	// check if the property exists
	//
	const Entry delete_key = { key, NULL, true };
	print_debug("btp_delete_property() search key: '%s'\n", delete_key.key);
	void *ptr = tfind(&delete_key, &(ctx->root), compare_entries);

	//
	// if the entry does not exist there is noting to do
	//
	if (ptr == NULL) {
		print_debug("btp_delete_property() Search key: '%s' does not exist.\n",
				delete_key.key);
		return false;
	}

	Entry *search_result = *(Entry **) ptr;
	print_debug("btp_delete_property() Key: '%s' with value: '%s'\n",
			search_result->key, search_result->value);

	//
	// delete the entry from the btree and free the memory
	//
	tdelete(&delete_key, &(ctx->root), compare_entries);
	delete_entry(search_result);
	ctx->num_entries--;

	print_debug("btp_delete_property() Entry for key: '%s' deleted and freed. Num entries: %d\n",
			delete_key.key, ctx->num_entries);

	return true;
}

/***************************************************************************
 * The method returns the value for a given key or null if it does not
 * exist.
 **************************************************************************/

char *btp_get_property_value(const BTP_ctx *ctx, char *key) {

	const Entry search_key = { key, NULL, true };
	print_debug("btp_get_property_value() Search key: '%s'\n", search_key.key);

	const void *ptr = tfind(&search_key, &(ctx->root), compare_entries);
	if (ptr == NULL) {
		print_debug("btp_get_property_value() Key: '%s' not found!\n", key);
		return NULL;
	}

	const Entry *entry = *(Entry **) ptr;
	return entry->value;
}

/***************************************************************************
 * The method reads the properties from a configuration file.
 **************************************************************************/

void btp_read_properties(BTP_ctx *ctx, const char *filename) {
	FILE *file;
	char raw_line[MAX_LINE];
	char *line;
	char *idx;
	int line_no = 0;
	char *key;
	char *value;

	print_debug("btp_read_properties() Opening file: '%s'\n", filename);

	file = fopen(filename, "r");
	if (file == NULL) {
		fprintf(stderr,
				"btp_read_properties() Unable to open file: %s! Error: %s\n",
				filename, strerror(errno));
		exit(EXIT_FAILURE);
	}

	while (fgets(raw_line, MAX_LINE, file) != NULL) {
		line_no++;

		line = trim(raw_line);

		//
		// skip empty lines or comments
		//
		if (!*line || *line == '#') {
			continue;
		}

		print_debug("btp_read_properties() File: '%s' no: %d line: '%s'\n",
				filename, line_no, line);

		//
		// search = as a key / value delimiter
		//
		idx = index(line, '=');
		if (!idx) {
			fprintf(stderr,
					"btp_read_properties() File: '%s' line: %d does not contain '='!\n",
					filename, line_no);
			fclose(file);
			exit(EXIT_FAILURE);
		}
		//
		// split line by = and get key and value
		//
		(*idx) = '\0';
		key = r_trim(line);
		value = l_trim(idx + 1);
		print_debug("btp_read_properties() key: '%s' value: '%s'\n", key, value);
		btp_add_property(ctx, key, value, true);
	}

	fclose(file);
	return;
}

