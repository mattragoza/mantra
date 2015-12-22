#include "lexer.h"

char *TOKEN_TYPES[] =
{
	"end of file",
	"symbol",
	"numeric literal",
	"string literal",
	"sequence start",
	"sequence end",
	"escape modifier",
	"evaluate modifier",
	"unknown token"
};

/*
new_Token
	construct a Token with an intial char and position in source text
	type field is not initialized and must be set separately
*/
Token *new_Token(char c, int src_index, int src_line, int src_col)
{
	Token *self = malloc(sizeof(Token));

	self->src_index = src_index;
	self->src_line  = src_line;
	self->src_col   = src_col;

	self->buffer = malloc(2);
	self->buffer[0] = c;
	self->buffer[1] = 0;
	self->len = 1;
	self->cap = 1;
	
	return self;
}

void del_Token(Token *self)
{
	free(self->buffer);
	free(self);
}

/*
Token_append
	append a char to a Token string
	double the buffer capacity if necessary
*/
void Token_append(Token *self, char c)
{
	if (c == 0) return;

	if (self->len == self->cap) // 2x resize
	{
		int new_cap = 2 * self->cap;
		char *new_buffer = malloc(new_cap + 1);

		int i = 0;
		while (i < self->len)
		{
			new_buffer[i] = self->buffer[i];
			i++;
		}
		free(self->buffer);

		self->buffer = new_buffer;
		self->cap = new_cap;
	}

	self->buffer[self->len++] = c;
	self->buffer[self->len] = 0;
}

int Token_istype(Token *self, TokenType type)
{
	return (self->type == type);
}

/*
new_Lexer
	construct a Lexer from a source file pointer
	automatically creates a scanner and loads the first char
*/
Lexer *new_Lexer(int argc, char **argv)
{
	Lexer *self = malloc(sizeof(Lexer));
	self->scanner = new_Scanner(argc, argv);
	Scanner_getnext(self->scanner);
	return self;
}

void del_Lexer(Lexer *self)
{
	del_Scanner(self->scanner);
	free(self);
}

/*
Lexer_getnext
	returns the next Token in the source text
	can potentially return EOF or unknown token, and/or set_error
	uses lexical definition macros to generate appropriate tokens
*/
Token *Lexer_getnext(Lexer *self)
{
	char c;
	if (self->scanner->curr_index == -1)
		c = Scanner_getnext(self->scanner);
	else
		c = Scanner_getcurr(self->scanner);

	// ignore whitespace
	while (c && strchr(WHITESPACE_CHARS, c))
		c = Scanner_getnext(self->scanner);

	// begin creating a Token
	Token *token = new_Token(c, self->scanner->curr_index,
		self->scanner->curr_line, self->scanner->curr_col);

	if (c == EOF_CHAR)
		token->type = EOF_TOKEN;

	else if (c == '.') // decimal numeric literal
	{
		token->type = UNKNOWN_TOKEN;
		c = Scanner_getnext(self->scanner);

		while (strchr(NUMERIC_CHARS, c))
		{
			if (token->type != NUMERIC_LITERAL_TOKEN)
				token->type = NUMERIC_LITERAL_TOKEN;

			Token_append(token, c);
			c = Scanner_getnext(self->scanner);
		}

		if (token->type == UNKNOWN_TOKEN)
			cause_error(UNEXPECTED_TOKEN_ERROR, token->buffer, token->len);
	}

	else if (strchr(SYMBOL_CHARS, c))
	{
		token->type = SYMBOL_TOKEN;
		c = Scanner_getnext(self->scanner);

		while (strchr(SYMBOL_CHARS, c))
		{
			Token_append(token, c);
			c = Scanner_getnext(self->scanner);
		}
	}

	else if (strchr(NUMERIC_CHARS, c))
	{
		token->type = NUMERIC_LITERAL_TOKEN;
		c = Scanner_getnext(self->scanner);

		int has_decimal = 0;
		while (strchr(NUMERIC_CHARS, c) || (!has_decimal && c == '.'))
		{
			if (c == '.') has_decimal = 1;
			Token_append(token, c);
			c = Scanner_getnext(self->scanner);
		}
	}

	else if (strchr(STRING_QUOTE_CHARS, c))
	{
		token->type = STRING_LITERAL_TOKEN;
		char end_quote = c;
		c = Scanner_getnext(self->scanner);

		while (c != end_quote)
		{
			if (c == EOF_CHAR) // EOF in string
			{
				token->type = EOF_TOKEN;
				cause_error(UNTERMINATED_STRING_ERROR, token->buffer, token->len);
				Scanner_getnext(self->scanner);
				return token;
			}

			Token_append(token, c);
			c = Scanner_getnext(self->scanner);
		}

		Token_append(token, c);
		Scanner_getnext(self->scanner);
	}

	else if (c == LIST_START_CHAR)
	{
		token->type = LIST_START_TOKEN;
		Scanner_getnext(self->scanner);
	}

	else if (c == LIST_END_CHAR)
	{
		token->type = LIST_END_TOKEN;
		Scanner_getnext(self->scanner);
	}

	else if (c == ESCAPE_CHAR)
	{
		token->type = ESCAPE_TOKEN;
		Scanner_getnext(self->scanner);
	}

	else if (c == EVALUATE_CHAR)
	{
		token->type = EVALUATE_TOKEN;
		Scanner_getnext(self->scanner);
	}

	else
	{
		token->type = UNKNOWN_TOKEN;
		cause_error(UNEXPECTED_TOKEN_ERROR, token->buffer, token->len);
		Scanner_getnext(self->scanner);
	}

	return token;
}