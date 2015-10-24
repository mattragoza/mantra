#ifndef MANTRA_H
#define MANTRA_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>


// define a buffer and pointer for reading in source code
#define BUFFER_SIZE 4096
FILE *SOURCE;		// global pointer to the source file being interpreted


// These macros define strings for the command line interface, in interactive mode
#define SPLASH_MESSAGE "The Mantra Language\n"
#define COMMAND_PROMPT "> "
#define CONTINUE_PROMPT "  "
int INTERACTIVE_MODE;	// in interactive mode, read code commands directly from a command line interface
char *PROMPT;		// the command line prompt, which can change for entering a new command or continuing an unfinished one


// These macros are used by the Lexer for determining token type of a string of characters
#define WHITESPACE_CHARS " \t\n"
#define SYMBOL_CHARS "+-=*/^%%_><abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define NUMERIC_CHARS "01234567890"
#define STRING_LITERAL_CHARS "\"'"




/*
 * Character represents a single character in the source code,
 * including the index, line and column where the character is found
 */
typedef struct character_t
{
	int source_index;
	int source_line;
	int source_col;
	char value;

} Character;
Character *new_Character(int, int, int, char); 	// constructor
void del_Character(Character *);		// destructor
int Character_isin(Character *, char *);	// check if a Character is found in a string



/*
 * Scanner reads from source file character by character,
 * generating Character objects for the Lexer
 *  
 * It also keeps track of its position in the source file,
 * and outputs the PROMPT after reading a newline character
 * in interactive mode
 */
typedef struct scanner_t
{
	FILE *source;		// the file pointer for the source text
	int curr_index;		// current index in buffer
	int curr_line;		// current line in buffer
	int curr_col;		// current column in buffer
	char last;		// the last char the was read

} Scanner;
Scanner *new_Scanner(FILE *); 		// constructor
void del_Scanner(Scanner *); 		// destructor
Character *Scanner_getnext(Scanner *);	// return Character at next position in source



/*
 * Token represents a string unit that has meaning in the syntax,
 * for example a symbol, numeric literal, string literal, parentheses,
 * or end of file marker
 *
 * These types are differentiated with the TokenType enumeration
 *
 * Also keeps track of position in the source file where the Token is found
*/
typedef enum {
	EOF_TOKEN, SYMBOL_TOKEN, NUMERIC_LITERAL_TOKEN, STRING_LITERAL_TOKEN,
	OPEN_PAREN_TOKEN, CLOSE_PAREN_TOKEN, COMMA_TOKEN, PERIOD_TOKEN, MINUS_TOKEN,
	SEMICOLON_TOKEN, UNKNOWN_TOKEN
} TokenType;
char * TOKEN_TYPES[11]; // string representation of TokenTypes
typedef struct token_t
{
	int source_index;	// keep track of location where token starts
	int source_line;
	int source_col;
	char *string;		// the buffer for the actual token string
	int len, cap;		// current length of string, capacity of buffer
	TokenType type;		// the type of token, seen enum above

} Token;
Token *new_Token(Character *);		 // constructor
void del_Token(Token *);		 // destructor
void Token_append(Token *, Character *); // append a Character to the Token's internal string



/*
 * Lexer iterates through Characters it receives from a Scanner,
 * and creates Tokens out of them. Keeps track of a current
 * character "frame" in the source file, which it iteratively
 * advances to build Tokens
 */
typedef struct lexer_t
{
	struct scanner_t *scanner;	// the Scanner used to get Characters from source
	struct character_t *frame; 	// current character "frame" for identifying tokens

} Lexer;
Lexer *new_Lexer(FILE *); 		// constructor
void del_Lexer(Lexer *);		// destructor
void Lexer_step(Lexer *); 		// moves the frame one step forward through the source text
Token *Lexer_getnext(Lexer *self);	// returns the next identified Token in the source text




/*
 * Nodes make up the abstract syntax tree that the Parser
 * constructs out of Tokens. Most Nodes have a Token that
 * is significant to their meaning, but not all (eg. sequences)
 *
 * Nodes also have a NodeType defined by the following enumeration
 * 
 * The tree nature comes from the fact that Nodes can have pointers
 * to children Nodes, which are relevant to their semantic evaluation
 */
typedef enum {
	PROGRAM_NODE, ERROR_NODE, EOF_NODE, SEQUENCE_NODE,
	SYMBOL_NODE, STRING_LITERAL_NODE, NUMERIC_LITERAL_NODE
} NodeType;
char * NODE_TYPES[10]; // string representation of NodeTypes
typedef struct node_t
{
	struct token_t *token;		// some have a specific Token representation in the source code
	NodeType type;			// the type of Node in the sytax, as defined by the enum above
	struct node_t **children;	// array of pointers to children Nodes
	int num_children;		// current number of children
	int max_children;		// maximum capacity of child array

} Node;
Node *new_Node(Token *, NodeType);	// constructor
void del_Node(Node *); 			// destructor
void Node_addchild(Node *, Node *);	// adds a Node as another Node's child, resizing if necessary
void Node_removechild(Node *, int);	// removes the child at some index in a Node
void Node_print(Node *, int);		// prints the type and token of a Node recursively, in a tree-like format



/*
 * The Parser uses the syntactical rules of Mantra to
 * build an abstract syntax tree out of Tokens it receives
 * from a Lexer. It keeps track of current Token, and
 * traverses through a series of recursive "syntax rule"
 * functions until finding a correct location in the tree
 * to generate a Node- if a rule is broken, it can create
 * an "error node" in the tree. Also keeps a pointer to the
 * root, or "program node", for the entire source file.
 */
typedef struct parser_t
{
	FILE *source;
	struct lexer_t *lexer;		// the lexer to receive tokens from
	struct token_t *curr;		// the current token being parsed
	struct node_t *root;		// the root of the abstract syntax tree

} Parser;
Parser *new_Parser(FILE *);			// constructor
void del_Parser(Parser *);			// destructor
void Parser_step(Parser *);			// gets the next Token from the lexer
int Parser_found(Parser *, TokenType);		// checks whether current token is a particular type
Node *Parser_element(Parser *, Node *);		// attempts to add an element node to the tree
Node *Parser_sequence(Parser *, Node *);	// attempts to add a sequence node to the tree
Node *Parser_getnext(Parser *);			// returns the next command-level node (ie. child of root)



/*
 * All data in Mantra are represented by the Object struct.
 * There are differnt underlying struct formats for the different
 * primitives (ie. number, string, sequence, function, error), but
 * they each start with the Object_HEAD macro which allows them to
 * be duck-typed and passed to the same functions.
 *
 * Errors encountered either while parsing (syntax errors) or interpreting
 * the parse tree are returned as Objects, with subcategories differentiated
 * by the ErrorType enumeration.
 */
typedef enum {
	NUMBER_OBJECT, STRING_OBJECT, SEQUENCE_OBJECT, FUNCTION_OBJECT, ERROR_OBJECT
} ObjectType;
char * OBJECT_TYPES[5];
typedef enum {
	SYNTAX_ERROR, TYPE_ERROR, REFERENCE_ERROR
} ErrorType;
char * ERROR_TYPES[3];

#define Object_HEADER \
	int ref_count;\
	ObjectType type;

typedef struct object_t {
	Object_HEADER
} Object;
typedef struct number_object_t {
	Object_HEADER
	double value;
} NumberObject;
typedef struct string_object_t {
	Object_HEADER
	int len, cap;
	char *buffer;
} StringObject;
typedef struct sequence_object_t {
	Object_HEADER
	int len, cap;
	Object **elem;
} SequenceObject;
typedef struct function_object_t {
	Object_HEADER
	Object *(*call)();	// for clarity, this is a pointer to a function that returns an Object pointer
} FunctionObject;
typedef struct error_object_t {
	Object_HEADER
	ErrorType err;
	char *message;
	Token *token;
} ErrorObject;
// constructor functions, destructor (TODO), and various utility functions
Object *new_NumberObject(double);		
Object *new_StringObject(char *);
Object *new_SequenceObject(int);
Object *new_FunctionObject(Object *(*)());
Object *new_ErrorObject(ErrorType, char *, Token *);
void del_Object(Object *);
char *Object_tostring(Object *);
void SequenceObject_append(SequenceObject *, Object *);
void StringObject_concat(StringObject *, StringObject *);
// macros for more convenient pointer conversion between object types
#define OBJ(x) ((Object *)x)
#define NUM(x) ((NumberObject *)x)
#define STR(x) ((StringObject *)x)
#define SEQ(x) ((SequenceObject *)x)
#define FUN(x) ((FunctionObject *)x)
#define ERR(x) ((ErrorObject *)x)



/*
 * Context is how symbols are bound to data in Mantra.
 * Each Context is a symbol table (implemented as a hash map)
 * and each time a new scope is entered, a new Context is created
 * and linked to the previous one. In this way, Contexts are also
 * implemented as a stack.
 */
typedef struct context_t
{
	struct context_t *parent;	// the context "above" this one in the stack, ie. more general or non-local
	int num, cap;			// current number of keys and maximum capacity of the hash table
	struct object_t **map;		// pointer to the hash table of Object (pointers)

} Context;
Context *new_Context(Context *);		// constructor
void del_Context(Context *);			// destructor
long Context_hash(Context *, char *);		// hash function, TODO uses open addressing
Object *Context_get(Context *, char *);		// recursively look up a key in a context and its parents
void Context_set(Context *, char *, Object *);	// set a key to an Object in a given context



typedef struct interpreter_t
{
	struct parser_t *parser;
	struct context_t *global;

} Interpreter;
Interpreter *new_Interpreter(FILE *);
void del_Interpreter(Interpreter *);
Object *Interpreter_eval(Interpreter *, Node *, Context *);
Object *Interpreter_evalnext(Interpreter *);



Object *Builtin_add(SequenceObject *);
Object *Builtin_sub(SequenceObject *);
Object *Builtin_mult(SequenceObject *);
Object *Builtin_div(SequenceObject *);

#endif // MANTRA_H
