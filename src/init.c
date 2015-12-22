#include "init.h"

FILE *init_Mantra(int argc, char **argv)
{
	// read from stdin if a source file is not provided
	FILE *source_fd = (argc < 2) ? stdin : fopen(argv[1], "r");

	// if the source is a terminal, use interactive mode
	if (isatty(fileno(source_fd)))
	{
		INTERACTIVE_MODE = 1;
		PROMPT = COMMAND_PROMPT;
		fputs(SPLASH_MESSAGE, stderr);
	}
	else
		INTERACTIVE_MODE = 0;

	clear_error();
	return source_fd;
}