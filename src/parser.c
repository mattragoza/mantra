#include "parser.h"

Parser *new_Parser(int argc, char **argv)
{
	Parser *self = malloc(sizeof(Parser));
	self->lexer = new_Lexer(argc, argv);
	return self;
}

void del_Parser(Parser *self)
{
	del_Lexer(self->lexer);
	free(self);
}

Object *Parser_symbol(Parser *self, Token *token, int eval_mod)
{
	return OBJ(new_SymbolObject(token->buffer, eval_mod));
}

Object *Parser_number(Parser *self, Token *token)
{
	return OBJ(new_NumberObject(strtod(token->buffer, NULL)));
}

Object *Parser_string(Parser *self, Token *token)
{
	return OBJ(new_StringObject(token->buffer));
}

Object *Parser_list(Parser *self, Token *token, int deref)
{
	ListObject *list = new_ListObject(DEFAULT_LIST_CAP);
	token = Lexer_getnext(self->lexer);
	while (!Token_istype(token, LIST_END_TOKEN))
	{
		Object *elem = Parser_parse(self, token, deref);
		del_Token(token);
		if (check_error())
			break;
		
		else if (!elem)
		{
			cause_error(UNTERMINATED_LIST_ERROR, NULL, 0);
			break;
		}

		ListObject_append(list, elem);
		token = Lexer_getnext(self->lexer);
	}
	del_Token(token);
	return OBJ(list);
}

Object *Parser_parse(Parser *self, Token *token, int eval_mod)
{
	Object *next;

	if (Token_istype(token, ESCAPE_TOKEN))
	{
		token = Lexer_getnext(self->lexer);
		next = Parser_parse(self, token, eval_mod-1);
		del_Token(token);
	}

	else if (Token_istype(token, EVALUATE_TOKEN))
	{
		token = Lexer_getnext(self->lexer);
		next = Parser_parse(self, token, eval_mod+1);
		del_Token(token);
	}

	else if (Token_istype(token, SYMBOL_TOKEN))
		next = Parser_symbol(self, token, eval_mod);

	else if (Token_istype(token, NUMERIC_LITERAL_TOKEN))
		next = Parser_number(self, token);

	else if (Token_istype(token, STRING_LITERAL_TOKEN))
		next = Parser_string(self, token);

	else if (Token_istype(token, LIST_START_TOKEN))
		next = Parser_list(self, token, eval_mod);

	else if (Token_istype(token, EOF_TOKEN))
		next = NULL;

	else
	{
		cause_error(UNEXPECTED_TOKEN_ERROR, token->buffer, token->len);
		next = NULL;
	}

	return next;
}

Object *Parser_getnext(Parser *self)
{
	Token *token = Lexer_getnext(self->lexer);
	PROMPT = CONTINUE_PROMPT;
	Object *next = Parser_parse(self, token, 1);
	PROMPT = COMMAND_PROMPT;
	del_Token(token);
	return next;
}