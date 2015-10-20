#ifndef MANTRA_H
#define MANTRA_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define BUFFER_SIZE 4096
#define SPLASH_MESSAGE "The Mantra Language\n"
#define COMMAND_PROMPT "> "
#define CONTINUE_PROMPT "  "

FILE *SOURCE;
char *PROMPT;
int INTERACTIVE_MODE;

#define WHITESPACE_CHARS " \t\n"
#define IDENTIFIER_START_CHARS "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define IDENTIFIER_CHARS "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_"
#define NUMERIC_CHARS "01234567890"
#define STRING_LITERAL_CHARS "\"'"




/*
	Character is an object representing a single character in a
	source code file, including a pointer to the source text and
	the index, line and column where the character is found
*/
typedef struct character_t
{
	char *source_text;
	int source_index;
	int source_line;
	int source_col;
	char value;

} Character;
Character *new_Character(char *, int, int, int, char); 	// constructor
void del_Character(Character *);						// destructor
int Character_isin(Character *, char *);				// check if a Character is found in a string



/*
	Scanner is used to both read in and iterate through the
	source file contents; its purpose is to return Character
	objects to the Lexer

	It reads lines from a source file into an internal buffer,
	and creates Character objects iteratively through the buffer
	using its Scanner_getnext() function
*/
typedef struct scanner_t
{
	FILE *source;		// the file pointer for the source text
	char *source_text;	// a buffer of the actual source text
	int source_len;		// the current length of the buffer
	int source_cap;		// the maximum length of the buffer

	int curr_index;		// current index in buffer
	int curr_line;		// current line in buffer
	int curr_col;		// current column in buffer

} Scanner;
Scanner *new_Scanner(FILE *); 			// constructor
void del_Scanner(Scanner *); 			// destructor
int Scanner_readline(Scanner *);		// read next line from source into internal buffer
int Scanner_hasnext(Scanner *);			// check if curr_index hasn't reached end of buffer
Character *Scanner_getnext(Scanner *);	// return Character at the curr_index in buffer and step forward
char Scanner_peek(Scanner *, int); 		// peek at the char some given steps ahead in buffer



/*
	Token represents a string unit that has some assigned meaning
	in the syntax, for example a keyword, numeric literal,
	string literal, identifier, or operator

	These types are differentiated using the TokenType enumeration
*/
typedef enum {
	EOF_TOKEN, IDENTIFIER_TOKEN, NUMERIC_LITERAL_TOKEN, STRING_LITERAL_TOKEN,
	OPEN_PAREN_TOKEN, CLOSE_PAREN_TOKEN, COMMA_TOKEN, PERIOD_TOKEN, MINUS_TOKEN,
	SEMICOLON_TOKEN, UNKNOWN_TOKEN
} TokenType;
char * TOKEN_TYPES[11];
typedef struct token_t
{
	char *source_text;
	int source_index;
	int source_line;
	int source_col;
	char *string;
	int len, cap;
	TokenType type;

} Token;
Token *new_Token(Character *);				// constructor
void del_Token(Token *);					// destructor
void Token_append(Token *, Character *);	// append a Character to the Token's internal string



/*
	Lexer is used to identify strings in the source text that
	have meaning, as prescribed by the syntax. It uses a Scanner
	to read Characters in the source and join them into Tokens
*/
typedef struct lexer_t
{
	Scanner *scanner;	// the Scanner used to get Characters from source
	Character *frame; 	// current character "frame" for identifying tokens

} Lexer;
Lexer *new_Lexer(FILE *); 			// constructor
void del_Lexer(Lexer *);			// destructor
void Lexer_step(Lexer *); 			// moves the frame one step forward through the source text
Token *Lexer_getnext(Lexer *self);	// returns the next identified Token in the source text




/*
	Node is a node in the syntax tree
*/
typedef enum {
	PROGRAM_NODE, ERROR_NODE, EOF_NODE, EVALUATION_NODE, STRING_LITERAL_NODE, NUMERIC_LITERAL_NODE
} NodeType;
char * NODE_TYPES[10];
typedef struct node_t
{
	struct token_t *token;
	NodeType type;
	struct node_t **children;
	int num_children;
	int max_children;
	int level;
} Node;
Node *new_Node(Token *, NodeType);	// constructor
void del_Node(Node *); 				// destructor
void Node_addchild(Node *, Node *);
void Node_removechild(Node *, int);
void Node_printnode(Node *, int);



/*
	Parser receives Tokens from a Lexer, and uses
	syntax rules to generate an abstract syntax tree
	of syntactical Nodes
*/
typedef struct parser_t
{
	FILE *source;
	struct lexer_t *lexer;
	struct token_t *curr;
	struct node_t  *root;

} Parser;
Parser *new_Parser(FILE *);			// constructor
void del_Parser(Parser *);			// destructor
void Parser_step(Parser *);
int Parser_found(Parser *, TokenType);
int Parser_expect(Parser *, TokenType);
Node *Parser_statement(Parser *, Node *);
Node *Parser_evaluation(Parser *, Node *);
Node *Parser_sequence(Parser *, Node *);


#endif // MANTRA_H