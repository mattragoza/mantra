#ifndef PARSER_H
#define PARSER_H

#include "common.h"
#include "init.h"
#include "error.h"
#include "scanner.h"
#include "lexer.h"
#include "object.h"

/*
Parser class
	builds a tree of Objects from lexical tokens
*/
typedef struct parser_t
{
	Lexer *lexer;

} Parser;

/* Parser methods */
Parser *new_Parser(int argc, char **argv);
void del_Parser(Parser *self);
Object *Parser_symbol(Parser *self, Token *token, int eval_mod);
Object *Parser_number(Parser *self, Token *token);
Object *Parser_string(Parser *self, Token *token);
Object *Parser_list(Parser *self, Token *token, int eval_mod);
Object *Parser_parse(Parser *self, Token *token, int eval_mod);
Object *Parser_getnext(Parser *self);

#endif //PARSER_H