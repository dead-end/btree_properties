/***************************************************************************
 * btree_utils.c
 *
 *  Created on: Aug 5, 2017
 *      Author: Dead-End
 **************************************************************************/

#include <string.h>
#include <ctype.h>

#include "btree_utils.h"

/***************************************************************************
 * The method checks if a string has a given prefix.
 **************************************************************************/

int str_has_suffix(const char *str, const char *suffix) {
	const char *idx = strrchr(str, '.');
	if (idx == NULL) {
		return 0;
	}
	return strcmp(idx, suffix) == 0;
}

/***************************************************************************
 * The function removes the tailing whitespaces of a string, by setting them
 * to '\0'.
 **************************************************************************/

char *r_trim(char *str) {
	const int len = strlen(str);
	char *c;

	for (c = str + len - 1; isspace(*c); *(c--) = '\0')
		;

	return str;
}

/***************************************************************************
 * The function sets the pointer to the first non whitespace char.
 **************************************************************************/

char *l_trim(char *str) {
	char *c;

	for (c = str; isspace(*c); c++)
		;

	return c;
}

/***************************************************************************
 * The method returns a pointer to the first non whitespace char and sets
 * the tailing whitespaces to '\0'.
 **************************************************************************/

char *trim(char *str) {
	return r_trim(l_trim(str));
}
