#include "mantra.h"



Character *new_Character(char *source_text, int source_index, int source_line, int source_col, char value)
{
	Character *self = malloc(sizeof(Character));
	self->source_text = source_text;
	self->source_index = source_index;
	self->source_line = source_line;
	self->source_col = source_col;
	self->value = value;
	return self;
}

void del_Character(Character *self)
{
	if (self == NULL) return;
	free(self);
	return;
}

int Character_isin(Character *c, char *string)
{
	int i = 0;
	while (i < strlen(string))
	{
		if (string[i] == c->value)
			return 1;
		i++;
	}
	return 0;
}



Scanner *new_Scanner(FILE *source)
{
	Scanner *self = malloc(sizeof(Scanner));
	self->source = source;
	self->source_text = malloc(BUFFER_SIZE*sizeof(char));
	memset(self->source_text, '\0', BUFFER_SIZE*sizeof(char));

	self->source_len = 0;
	self->source_cap = BUFFER_SIZE;

	self->curr_index = 0;
	self->curr_line  = 0;
	self->curr_col   = 0;

	return self;
}

void del_Scanner(Scanner *self)
{
	if (self == NULL) return;
	free(self->source_text);
	free(self);
	return;
}

/*
	Scanner_readline

		Reads a line from the Scanner source and appends it to 
		its source_text buffer, resizing if necessary

		Returns 0 if we reached the end of the source, 1 otherwise
*/
int Scanner_readline(Scanner *self)
{
	// if we're reading stdin from a terminal, output the prompt
	if (INTERACTIVE_MODE) fputs(PROMPT, stderr);

	// read input from the Scanner source into line_buffer until reaching \n or \0
	char *line_buffer = malloc(BUFFER_SIZE*sizeof(char));
	if (fgets(line_buffer, BUFFER_SIZE*sizeof(char), self->source) == NULL)
		return 0;

	// resize the Scanner source_text buffer if the new line won't fit
	if (self->source_len + strlen(line_buffer) >= self->source_cap)
	{
		// allocate and clear a larger buffer in memory
		int new_cap = self->source_cap + BUFFER_SIZE;
		char *new_buffer = malloc(new_cap*sizeof(char));
		memset(new_buffer, '\0', new_cap*sizeof(char));

		// copy the old buffer into the new buffer
		memcpy(new_buffer, (void *)self->source_text, self->source_len);
		free(self->source_text);

		// update the Scanner's buffer pointer and capacity
		self->source_text = new_buffer;
		self->source_cap  = new_cap;
	}

	// append the new line to the Scanner source_text buffer
	self->source_len += strlen(line_buffer);
	strncat(self->source_text, line_buffer, self->source_len);
	free(line_buffer);
	return 1;
}

/*
	Scanner_hasnext
		
		Returns 0 if the curr_index has reached the end of
		the source_text, 1 otherwise
*/
int Scanner_hasnext(Scanner *self)
{
	if (self->curr_index >= self->source_len)
		return 0;
	else
		return 1;
}

/*
	Scanner_getnext

		Returns the next Character in the source_text, which could
		be an EOF if we've reached the end of the source
*/
Character *Scanner_getnext(Scanner *self)
{
	// read a line from Scanner source if we're at the end of the buffer
	int read_ok = 1;
	if (!Scanner_hasnext(self))
		read_ok = Scanner_readline(self);

	if (read_ok)
	{
		char next = self->source_text[self->curr_index];
		Character *c = new_Character(self->source_text, self->curr_index,
			self->curr_line, self->curr_col, next);

		self->curr_col += 1;
		self->curr_index += 1;
		if (next == '\n')
		{
			self->curr_line += 1;
			self->curr_col = 0;
		}
		return c;
	}
	else
	{
		return new_Character(self->source_text, self->curr_index,
			self->curr_line, self->curr_col, '\0');
	}
}

char Scanner_peek(Scanner *self, int amount)
{
	if (self->curr_index + amount < self->source_len)
	{
		return self->source_text[self->curr_index + amount];
	}
	else return '\0';
}

#ifdef SCANNER_TEST
int main(int argc, char **argv)
{
	if (argc < 2) SOURCE = stdin;
	else SOURCE = fopen(argv[1], "r");
	if (isatty(fileno(SOURCE)))
	{
		INTERACTIVE_MODE = 1;
		fputs(SPLASH_MESSAGE, stderr);
		fputs(COMMAND_PROMPT, stderr);
	}
	else INTERACTIVE_MODE = 0;
	
	Scanner *my_scanner = new_Scanner(SOURCE);
	while (1)
	{
		Character *character = Scanner_getnext(my_scanner);
		if (character->value == '\0')
		{
			del_Character(character);
			break;
		}
		printf("%c", character->value);
		del_Character(character);
	}
	del_Scanner(my_scanner);
	return 0;
}
#endif