#include "scanner.h"

Scanner *new_Scanner(int argc, char **argv)
{
	Scanner *self = malloc(sizeof(Scanner));
	self->source_fd = init_Mantra(argc, argv);
	self->curr_index = -1;
	self->curr_line  = -1;
	self->curr_col   = -1;
	self->curr_char = '\0';
	return self;
}

void del_Scanner(Scanner *self)
{
	free(self);
}

char Scanner_getcurr(Scanner *self)
{
	return self->curr_char;
}

char Scanner_getnext(Scanner *self)
{
	// on first line and new lines, adjust the cursor line
	// also output the prompt in interactive mode
	if (!self->curr_char || self->curr_char == '\n')
	{
		self->curr_line++;
		self->curr_col = -1;
		if (INTERACTIVE_MODE)
			fputs(PROMPT, stderr);
	}

	int curr_byte = getc(self->source_fd);

	if ((curr_byte == EOF)|(curr_byte == 0)|(curr_byte == 4)|(curr_byte == 26))
		self->curr_char = EOF_CHAR;

	else
	{
		// cast the byte to char
		char curr_char = (char) curr_byte;

		// advance the cursor index and column
		self->curr_index++;
		self->curr_col++;
		self->curr_char = curr_char;
	}

	//DEBUG printf("%d -> %d\n", curr_byte, self->curr_char);
	return self->curr_char;
}
