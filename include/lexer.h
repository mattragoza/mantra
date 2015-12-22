#ifndef LEXER_H
#define LEXER_H

#include "common.h"
#include "init.h"
#include "error.h"
#include "scanner.h"

// macros for lexical analysis of input characters into tokens
#define WHITESPACE_CHARS " \t\n"
#define SYMBOL_CHARS "+-*/^%%=!><&|_@${}[]:#?,abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define NUMERIC_CHARS "0123456789"
#define STRING_QUOTE_CHARS "'\""
#define LIST_START_CHAR '('
#define LIST_END_CHAR ')'
#define ESCAPE_CHAR '\\'
#define EVALUATE_CHAR ';'

typedef enum
{
	EOF_TOKEN,
	SYMBOL_TOKEN,
	NUMERIC_LITERAL_TOKEN,
	STRING_LITERAL_TOKEN,
	LIST_START_TOKEN,
	LIST_END_TOKEN,
	ESCAPE_TOKEN,
	EVALUATE_TOKEN,
	UNKNOWN_TOKEN

} TokenType;
char *TOKEN_TYPES[10];


/*
Token class
	represents a string unit that has meaning in the syntax
	types are differentiated with the TokenType enumeration
	keeps track of position in the source file where the Token is found
*/
typedef struct token_t
{
	int src_index;	// location where token starts
	int src_line;
	int src_col;

	char *buffer;		// the buffer for the actual token string
	int len, cap;		// current length of string, capacity of buffer

	TokenType type;		// the type of token, see enum above

} Token;

/* Token methods */
Token *new_Token(char c, int src_index, int src_line, int src_col);
void del_Token(Token *self);
void Token_append(Token *self, char c);			// append char to the Token string
int Token_istype(Token *self, TokenType type);	// check if Token is a particular type


/*
Lexer class
	iterates through Characters it receives from a Scanner
	creates Tokens out of them by lexical analysis
*/
typedef struct lexer_t
{
	Scanner *scanner;	// the Scanner used to get chars from source

} Lexer;

/* Lexer methods */
Lexer *new_Lexer(int argc, char **argv);
void del_Lexer(Lexer *self);
Token *Lexer_getnext(Lexer *self);	// returns the next identified Token in the source text

#endif //LEXER_H