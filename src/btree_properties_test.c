/***************************************************************************
 * btree_properties_test.c
 *
 *  Created on: Aug 5, 2017
 *      Author: Dead-End
 **************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "btree_properties.h"

#define MAX_KEY_VALUE 32

//
// Definition of the test resources
//
#define TEST_1_PROPS "resources/test-1.props"
#define TEST_2_PROPS "resources/test-2.props"

/***************************************************************************
 * The function is a callback for the iterator function. It simply prints
 * the key and value.
 **************************************************************************/

void print_properties(const char *key, const char *value) {
	printf("Key: '%s' Value: '%s'\n", key, value);
}

/***************************************************************************
 * The method ensures that two int values are equal.
 **************************************************************************/

void ensure_int(const int expected_value, const int current_value) {
	printf("Checking int expected: %d current value: %d\n", expected_value, current_value);

	if (expected_value != current_value) {
		fprintf(stderr, "FAILED - Expected value: %d found value: %d\n", expected_value, current_value);
		exit(EXIT_FAILURE);
	}
}

/***************************************************************************
 * The method ensures that the btree contains a property with a given value.
 **************************************************************************/

void ensure(BTP_ctx *ctx, char *key, char* expected_value) {

	printf("Checking key: %s with expected value: %s\n", key, expected_value);

	//
	// get the current value from the btree
	//
	const char* current_value = btp_get_property_value(ctx, key);

	//
	// ensure that the property with the key was found
	//
	if (current_value == NULL) {
		fprintf(stderr, "FAILED - Key: %s not found!\n", key);
		exit(EXIT_FAILURE);
	}

	//
	// ensure that the property value is the expected
	//
	if (strcmp(expected_value, current_value) != 0) {
		fprintf(stderr, "FAILED - Key: %s expected value: %s found value: %s\n", key, expected_value, current_value);
		exit(EXIT_FAILURE);
	}
}

/***************************************************************************
 * The first test reads a property file and ensures that the key / value
 * pairs are properly inserted in the tree.
 **************************************************************************/

void test_1() {

	printf("Starting test: 1\n");

	BTP_ctx *ctx = btp_create_ctx();

	btp_read_properties(ctx, TEST_1_PROPS);

	ensure(ctx, "key-1", "value-1");
	ensure(ctx, "key-2", "value-2");
	ensure(ctx, "key-3", "value-3");
	ensure(ctx, "key-4", "value-4");

	btp_iterate_properties(ctx, print_properties);

	btp_destroy_ctx(ctx);

	printf("Finished test: 1\n");
}

/***************************************************************************
 * The method ensures that the btree contains a property with a given value.
 * Both, the key and the value can contain an index.
 **************************************************************************/

bool ensure_indexed(BTP_ctx *ctx, const char *key_format,
		const char *value_format, const int idx, const bool allow_missing) {
	char key[MAX_KEY_VALUE];
	char value[MAX_KEY_VALUE];

	//
	// create the key and the value with the index.
	//
	snprintf(key, MAX_KEY_VALUE, key_format, idx);
	snprintf(value, MAX_KEY_VALUE, value_format, idx);

	//
	// check that the index
	//
	if (allow_missing && btp_get_property_value(ctx, key) == NULL) {
		return false;
	}

	ensure(ctx, key, value);
	return true;
}

/***************************************************************************
 * The second test reads a property file with a list of database
 * connections. The list ends if the first key (db.<INDEX>.db) with the
 * index was not found. Example:
 *
 * db.1.db=db-1
 * db.1.user=user-1
 * db.1.password=password-1
 * db.1.connection=localhost:5432
 *
 * db.2.db=db-2
 * db.2.user=user-2
 * db.2.password=password-2
 * db.2.connection=localhost:5432
 **************************************************************************/

void test_2() {

	printf("Starting test: 2\n");

	BTP_ctx *ctx = btp_create_ctx();

	btp_read_properties(ctx, TEST_2_PROPS);

	for (int idx = 1; idx < 4; idx++) {

		if (!ensure_indexed(ctx, "db.%d.db", "db-%d", idx, true)) {
			if (idx != 4) {
				fprintf(stderr, "FAILED - Unexpected key for index: %d not found!\n", idx);
				exit(EXIT_FAILURE);
			}
			break;
		}

		ensure_indexed(ctx, "db.%d.user", "user-%d", idx, false);
		ensure_indexed(ctx, "db.%d.password", "password-%d", idx, false);
		ensure_indexed(ctx, "db.%d.connection", "localhost:5432", idx, false);
	}

	btp_iterate_properties(ctx, print_properties);

	btp_destroy_ctx(ctx);

	printf("Finished test: 2\n");
}

/***************************************************************************
 * The third test creates a btree property without a property file.
 **************************************************************************/

void test_3() {

	printf("Starting test 3\n");

	BTP_ctx *ctx = btp_create_ctx();
	ensure_int(0, btp_get_num_entries(ctx));

	btp_add_property(ctx, "key-1", "value-1", false);
	ensure_int(1, btp_get_num_entries(ctx));
	ensure(ctx, "key-1", "value-1");

	btp_add_property(ctx, "key-2", "value-2", false);
	ensure_int(2, btp_get_num_entries(ctx));
	ensure(ctx, "key-2", "value-2");

	btp_delete_property(ctx, "key-2");
	ensure_int(1, btp_get_num_entries(ctx));
	btp_add_property(ctx, "key-2", "new-value-2", false);
	ensure_int(2, btp_get_num_entries(ctx));
	ensure(ctx, "key-2", "new-value-2");

	btp_add_property(ctx, "key-2", "new-new-value-2", true);
	ensure_int(2, btp_get_num_entries(ctx));
	ensure(ctx, "key-2", "new-new-value-2");

	btp_iterate_properties(ctx, print_properties);

	btp_destroy_ctx(ctx);

	printf("Finished test 3\n");
}

/***************************************************************************
 * The main function simply triggers the tests.
 **************************************************************************/

int main(int argc, char *argv[]) {

	test_1();

	test_2();

	test_3();

	printf("Tests successfully finished!");
	return EXIT_SUCCESS;
}

