#include "mantra.h"



char * TOKEN_TYPES[] = {"end of file", "identifier", "numeric literal", "string literal",
						"open paren", "close paren", "comma", "period", "minus",
						"semicolon", "unknown"};

Token *new_Token(Character *c)
{
	Token *self = malloc(sizeof(Token));

	self->string = malloc(5*sizeof(char));
	self->string[0] = c->value;
	self->string[1] = '\0';
	self->len = 1;
	self->cap = 4;

	self->source_text  = c->source_text;
	self->source_index = c->source_index;
	self->source_line  = c->source_line;
	self->source_col   = c->source_col;

	//self->type = type;
	del_Character(c); // don't need this anymore
	return self;
}

void del_Token(Token *self)
{
	if (self == NULL) return;
	free(self->string);
	free(self);
	return;
}

/*
	Token_append
		Appends to a Token's string from a Character, resizing
		the string if necessary. This assumes that the Character
		is NOT the first one in the Token string, so it just
		extracts the char value and then frees the Character object
*/
void Token_append(Token *self, Character *c)
{
	char value = c->value;
	del_Character(c);
	if (value == '\0') return;
	if (self->len == self->cap)
	{
		int new_cap = 2*self->cap;
		char *new_string = malloc((new_cap+1)*sizeof(char));
		new_string[new_cap] = '\0';
		int i = 0;
		while (i < self->len)
		{
			new_string[i] = self->string[i];
			i++;
		}
		free(self->string);
		self->string = new_string;
		self->cap = new_cap;
	}
	self->string[self->len] = value;
	self->len++;
	self->string[self->len] = '\0';
	return;
}



Lexer *new_Lexer(FILE *source)
{
	Lexer *self = malloc(sizeof(Lexer));
	self->scanner = new_Scanner(source);
	self->frame = Scanner_getnext(self->scanner);
	return self;
}

void del_Lexer(Lexer *self)
{
	if (self == NULL) return;
	del_Scanner(self->scanner);
	free(self);
	return;
}

void Lexer_step(Lexer *self)
{
	// step forward one Character in source
	self->frame = Scanner_getnext(self->scanner);
}

Token *Lexer_getnext(Lexer *self)
{
	// ignore whitespace
	while (Character_isin(self->frame, WHITESPACE_CHARS))
	{
		del_Character(self->frame);
		Lexer_step(self);
	}
	
	// begin creating a Token
	Token *token = new_Token(self->frame);

	if (self->frame->value == '\0')
	{
		token->type = EOF_TOKEN;
	}

	else if (Character_isin(self->frame, IDENTIFIER_START_CHARS))
	{
		token->type = IDENTIFIER_TOKEN;
		Lexer_step(self);

		while (Character_isin(self->frame, IDENTIFIER_CHARS))
		{
			Token_append(token, self->frame);
			Lexer_step(self);
		}
		// TODO check for keywords here
	}

	else if (Character_isin(self->frame, NUMERIC_CHARS))
	{
		token->type = NUMERIC_LITERAL_TOKEN;
		Lexer_step(self);

		while (Character_isin(self->frame, NUMERIC_CHARS))
		{
			Token_append(token, self->frame);
			Lexer_step(self);
		}
		// TODO deal with decimals here too
	}

	else if (Character_isin(self->frame, STRING_LITERAL_CHARS))
	{
		char quote = self->frame->value;
		token->type = STRING_LITERAL_TOKEN;
		Lexer_step(self);

		while (self->frame->value != quote)
		{
			Token_append(token, self->frame);
			Lexer_step(self);
		}

		Token_append(token, self->frame);
		Lexer_step(self);
	}

	else if (self->frame->value == '(')
	{
		token->type = OPEN_PAREN_TOKEN;
		Lexer_step(self);
	}

	else if (self->frame->value == ')')
	{
		token->type = CLOSE_PAREN_TOKEN;
		Lexer_step(self);
	}

	else if (self->frame->value == ',')
	{
		token->type = COMMA_TOKEN;
		Lexer_step(self);
	}

	else if (self->frame->value == '-')
	{
		token->type = MINUS_TOKEN;
		Lexer_step(self);
	}

	else if (self->frame->value == ';')
	{
		token->type = SEMICOLON_TOKEN;
		Lexer_step(self);
	}
	// TODO the rest of operators down here too
	else
	{
		token->type = UNKNOWN_TOKEN;
		Lexer_step(self);
	}

	return token;
}

#ifdef LEXER_TEST
int main(int argc, char **argv)
{
	if (argc < 2) SOURCE = stdin;
	else SOURCE = fopen(argv[1], "r");
	if (isatty(SOURCE))
	{
		INTERACTIVE_MODE = 1;
		fputs(SPLASH_MESSAGE, stderr);
	}
	else INTERACTIVE_MODE = 0;
	
	Lexer *my_lexer = new_Lexer(SOURCE);
	while (1)
	{
		Token *token = Lexer_getnext(my_lexer);
		if (token->type == EOF_TOKEN)
		{
			del_Token(token);
			break;
		}
		printf("%s: %s\n", TOKEN_TYPES[token->type], token->string);
		del_Token(token);
	}
	del_Lexer(my_lexer);
	return 0;
}
#endif