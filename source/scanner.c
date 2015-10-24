#include "mantra.h"



Character *new_Character(int source_index, int source_line, int source_col, char value)
{
	Character *self = malloc(sizeof(Character));
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
	self->curr_index = 0;
	self->curr_line  = 0;
	self->curr_col   = 0;
	return self;
}

void del_Scanner(Scanner *self)
{
	if (self == NULL) return;
	free(self);
	return;
}

/*
	Scanner_getnext

		Returns the next Character in the source_text, which could
		be an EOF if we've reached the end of the source
*/
char LAST_CHAR;
Character *Scanner_getnext(Scanner *self)
{
	// for interactive mode, output the prompt on first line and new lines
	if (INTERACTIVE_MODE && (!LAST_CHAR || LAST_CHAR == '\n'))
		fputs(PROMPT, stderr);

	int read = getc(self->source);
	if (read != EOF && read != 26)
	{
		char next = (char) read;
		Character *c = new_Character(self->curr_index, self->curr_line, self->curr_col, next);

		self->curr_col += 1;
		self->curr_index += 1;
		if (next == '\n')
		{
			self->curr_line += 1;
			self->curr_col = 0;
		}
		LAST_CHAR = next;
		return c;
	}
	else
	{
		LAST_CHAR = '\0';
		return new_Character(self->curr_index,self->curr_line, self->curr_col, '\0');
	}
}



#ifdef SCANNER_TEST
int main(int argc, char **argv)
{
	if (argc < 2) SOURCE = stdin;
	else SOURCE = fopen(argv[1], "r");
	if (isatty(fileno(SOURCE)))
	{
		INTERACTIVE_MODE = 1;
		PROMPT = COMMAND_PROMPT;
		fputs(SPLASH_MESSAGE, stderr);
	}
	else INTERACTIVE_MODE = 0;
	
	Scanner *my_scanner = new_Scanner(SOURCE);
	while (1)
	{
		Character *character = Scanner_getnext(my_scanner);
		printf("%c", character->value);
		if (character->value == '\0')
		{
			del_Character(character);
			break;
		}
		del_Character(character);
	}
	del_Scanner(my_scanner);
	return 0;
}
#endif
