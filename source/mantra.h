#ifndef MANTRA_H
#define MANTRA_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define BUFFER_SIZE 4096
#define SPLASH_MESSAGE "The Mantra Language\n"
#define COMMAND_PROMPT "> "
#define CONTINUE_PROMPT "+ "

FILE *SOURCE;
char *PROMPT;
int INTERACTIVE_MODE;

#define WHITESPACE_CHARS " \t\n"
#define SYMBOL_CHARS "+-=*/^%%_><abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
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
	EOF_TOKEN, SYMBOL_TOKEN, NUMERIC_LITERAL_TOKEN, STRING_LITERAL_TOKEN,
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
	struct scanner_t *scanner;	// the Scanner used to get Characters from source
	struct character_t *frame; 	// current character "frame" for identifying tokens

} Lexer;
Lexer *new_Lexer(FILE *); 			// constructor
void del_Lexer(Lexer *);			// destructor
void Lexer_step(Lexer *); 			// moves the frame one step forward through the source text
Token *Lexer_getnext(Lexer *self);	// returns the next identified Token in the source text




/*
	Node is a node in the syntax tree
*/
typedef enum {
	PROGRAM_NODE, ERROR_NODE, EOF_NODE, SEQUENCE_NODE, SYMBOL_NODE, STRING_LITERAL_NODE, NUMERIC_LITERAL_NODE
} NodeType;
char * NODE_TYPES[10];
typedef struct node_t
{
	struct token_t *token;
	NodeType type;
	struct node_t **children;
	int num_children;
	int max_children;

} Node;
Node *new_Node(Token *, NodeType);	// constructor
void del_Node(Node *); 				// destructor
void Node_addchild(Node *, Node *);
void Node_removechild(Node *, int);
void Node_print(Node *, int);



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
	struct node_t *root;

} Parser;
Parser *new_Parser(FILE *);			// constructor
void del_Parser(Parser *);			// destructor
void Parser_step(Parser *);
int Parser_found(Parser *, TokenType);
int Parser_expect(Parser *, TokenType);
Node *Parser_element(Parser *, Node *);
Node *Parser_sequence(Parser *, Node *);
Node *Parser_getnext(Parser *);



typedef enum {
	NUMBER_OBJECT, STRING_OBJECT, SEQUENCE_OBJECT
} ObjectType;
char * OBJECT_TYPES[3];

#define Object_HEADER \
	int ref_count;\
	ObjectType type;

typedef struct object_t { // since all objects start with this, we can use it for duck typing
	Object_HEADER
} Object;
typedef struct number_object_t {
	Object_HEADER
	double value;
} NumberObject;
typedef struct string_object_t {
	Object_HEADER
	int len;
	int cap;
	char *buffer;
} StringObject;
typedef struct sequence_object_t {
	Object_HEADER
	int len;
	int cap;
	Object **elem;
} SequenceObject;
typedef struct function_object_t {
	Object_HEADER
	Object *arg;
	Object *(*f)();
} FunctionObject;
Object *new_NumberObject(Node *);
Object *new_StringObject(Node *);
Object *new_SequenceObject(Node *);
Object *new_FunctionObject(Node *);
void del_Object(Object *);
char *Object_tostring(Object *);
void Object_append(Object *, Object *);



typedef struct context_t
{
	struct context_t *parent;
	int num, cap;
	struct object_t **map;

} Context;
Context *new_Context(Context *);
void del_Context(Context *);
long Context_hash(Context *, char *);
Object *Context_get(Context *, char *);
void Context_set(Context *, char *, Object *);



typedef struct interpreter_t
{
	struct parser_t *parser;
	struct context_t *global;

} Interpreter;
Interpreter *new_Interpreter(FILE *);
void del_Interpreter(Interpreter *);
Object *Interpreter_eval(Interpreter *, Node *, Context *);
Object *Interpreter_evalnext(Interpreter *);



#endif // MANTRA_H