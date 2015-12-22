#include "error.h"

char *ERROR_MESSAGES[] =
{
	"mantra: no error",
	"mantra: syntax error: unexpected token",
	"mantra: syntax error: unterminated string",
	"mantra: syntax error: unterminated list",
	"mantra: index error: list access out of range",
	"mantra: context error: unbound symbol",
	"mantra: context error: can't dereference",
	"mantra: usage error: expected arguments"
};

void cause_error(ErrorType type, char *cause, int len)
{
	if (MANTRA_ERROR.cause)
		free(MANTRA_ERROR.cause);

	MANTRA_ERROR.type = type;
	if (cause && len)
	{
		MANTRA_ERROR.cause = malloc(len + 1);
		memcpy(MANTRA_ERROR.cause, cause, len);
		MANTRA_ERROR.cause[len] = 0;
	}
}

void handle_error()
{
	if (MANTRA_ERROR.cause)
	{
		fprintf(stderr, "%s %s\n",
			ERROR_MESSAGES[MANTRA_ERROR.type],
			MANTRA_ERROR.cause);
		free(MANTRA_ERROR.cause);	
	}
	else fprintf(stderr, "%s\n",
		ERROR_MESSAGES[MANTRA_ERROR.type]);

	MANTRA_ERROR.type = NO_ERROR;
}

int check_error()
{
	return (MANTRA_ERROR.type != NO_ERROR);
}

void clear_error()
{
	MANTRA_ERROR.type = NO_ERROR;
	if (MANTRA_ERROR.cause)
		free(MANTRA_ERROR.cause);
}