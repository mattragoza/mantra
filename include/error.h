#ifndef ERROR_H
#define ERROR_H

#include "common.h"

typedef enum
{
	NO_ERROR,
	UNEXPECTED_TOKEN_ERROR,
	UNTERMINATED_STRING_ERROR,
	UNTERMINATED_LIST_ERROR,
	LIST_INDEX_ERROR,
	UNBOUND_SYMBOL_ERROR,
	LITERAL_DEREFERENCE_ERROR,
	INCORRECT_ARGUMENT_ERROR

} ErrorType;
char *ERROR_MESSAGES[8];

typedef struct error_t
{
	ErrorType type;
	int len;
	char *cause;

} Error;
Error MANTRA_ERROR;

/* setting and unsetting global error value */
void cause_error(ErrorType type, char *cause, int len);
int check_error();
void clear_error();
void handle_error();

#endif //ERROR_H