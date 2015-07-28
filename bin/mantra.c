#include <stdlib.h>
#include <float.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#define BUFFER_SIZE 1024
#define CONT_PROMPT "  "
#define NEXT_PROMPT "> "
#define PRINT_PARSE true
char *PROMPT = NEXT_PROMPT;

char EOF_CHAR = '\0';
char *WHITESPACE_CHARS = " \n\t";
char *IDENTIFIER_START_CHARS = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
char *IDENTIFIER_CHARS = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_";
char *NUMERIC_CHARS = "01234567890";
char *STRING_LITERAL_CHARS = "\"'";

int EOF_TOKEN = -1;
int IDENTIFIER_TOKEN = 0;
int IF_TOKEN = 1;
int ELSE_TOKEN = 2;
int WHILE_TOKEN = 3;
int BREAK_TOKEN = 4;
int NUMERIC_LITERAL_TOKEN = 5;
int STRING_LITERAL_TOKEN = 6;
int EQUAL_TOKEN = 7;
int NOT_EQUAL_TOKEN = 8;
int GREATER_THAN_OR_EQUAL_TOKEN = 9;
int LESS_THAN_OR_EQUAL_TOKEN = 10;
int LOGICAL_AND_TOKEN = 11;
int LOGICAL_OR_TOKEN = 12;
int ASSIGNMENT_TOKEN = 13;
int PLUS_TOKEN = 14;
int MINUS_TOKEN = 15;
int MULTIPLY_TOKEN = 16;
int DIVIDE_TOKEN = 17;
int POWER_TOKEN = 18;
int MODULO_TOKEN = 19;
int GREATER_THAN_TOKEN = 20;
int LESS_THAN_TOKEN = 21;
int NOT_TOKEN = 22;
int COMMA_TOKEN = 23;
int SEMICOLON_TOKEN = 24;
int OPEN_PAREN_TOKEN = 25;
int CLOSE_PAREN_TOKEN = 26;
int OPEN_BRACKET_TOKEN = 27;
int CLOSE_BRACKET_TOKEN = 28;
int NULL_TOKEN = 29;
int PERIOD_TOKEN = 30;
int RETURN_TOKEN = 31;
int UNKNOWN_TOKEN = 32;
int DEFINE_TOKEN = 33;

int PROGRAM_NODE = 0;
int EVALUATION_NODE = 1;
int ASSIGNMENT_NODE = 2;
int CONDITIONAL_NODE = 3;
int LOOP_NODE = 4;
int BOOLEAN_EXPRESSION_NODE = 5;
int BOOLEAN_TERM_NODE = 6;
int COMPARISON_NODE = 7;
int ARITHMETIC_EXPRESSION_NODE = 8;
int ARITHMETIC_TERM_NODE = 9;
int NUMERIC_LITERAL_NODE = 10;
int STRING_LITERAL_NODE = 11;
int VARIABLE_NODE = 12;
int OPERATION_NODE = 13;
int DEFINITION_NODE = 14;
int INSTANCE_NODE = 15;
int NULL_NODE = 16;
int RETURN_NODE = 17;
int BLOCK_NODE = 18;
int ARGUMENTS_NODE = 19;
int ELSE_NODE = 20;
int BREAK_NODE = 21;

int INTEGER_TYPE = 0;
int FLOAT_TYPE = 1;
int STRING_TYPE = 2;

char *TOKEN_DICT[] = {"identifier", "\"if\"", "\"else\"", "\"while\"", "\"break\"", "numeric literal", "string literal",
				"\"==\"", "\"!=\"", "\">=\"", "\"<=\"", "\"&&\"", "\"||\"", "\"=\"", "\"+\"", "\"-\"", "\"*\"", "\"/\"",
				"\"^\"", "\"mod\"", "\">\"", "\"<\"", "\"!\"", "\",\"", "\";\"", "\"(\"", "\")\"", "\"{\"", "\"}\"",
				"null", "\".\"", "\"return\"", "unknown token", "\"define\""};

char *NODE_DICT[] = {"PROGRAM", "EVALUATION", "ASSIGNMENT", "CONDITIONAL", "LOOP", "BOOLEAN EXPRESSION",
					  "BOOLEAN TERM", "COMPARISON", "ARITHMETIC EXPRESSION", "ARITHMETIC TERM",
					  "NUMERIC LITERAL", "STRING LITERAL", "VARIABLE", "OPERATION", "DEFINITION",
					  "INSTANCE", "NULL", "RETURN", "BLOCK", "ARGUMENTS", "ELSE", "BREAK"};

bool is_in(char *array, char c)
{
	int i = 0;
	while (i < strlen(array))
	{
		if (array[i] == c)
			return true;
		i++;
	}
	return false;
}

typedef struct character_t
{
	char *sourceText;
	int sourceIndex;
	int sourceLine;
	int sourceCol;
	char character;

} Character;

Character *new_Character(char* sourceText, int sourceIndex,
	int sourceLine, int sourceCol, char c)
{
	Character *self = malloc(sizeof(Character));
	self->sourceText = sourceText;
	self->sourceIndex = sourceIndex;
	self->sourceLine = sourceLine;
	self->sourceCol = sourceCol;
	self->character = c;
	return self;
}

typedef struct scanner_t
{
	FILE *source;
	char *sourceText;
	int sourceLen;
	int sourceCap;
	int currIndex;
	int currLine;
	int currCol;

} Scanner;

Scanner *new_Scanner(FILE* source)
{
	Scanner *self = malloc(sizeof(Scanner));
	self->source = source;
	self->sourceText = malloc(1024*sizeof(char));
	memset(self->sourceText, '\0', 1024*sizeof(char));
	self->sourceLen = 0;
	self->sourceCap = 1024;
	self->currIndex = -1;
	self->currLine = 0;
	self->currCol = -1;
	return self;
}

void Scanner_readLine(Scanner *self)
{
	if (self->source == stdin && isatty(fileno(self->source)))
	{
		fputs(PROMPT, stderr);
	}

	char *buffer = malloc(1024*sizeof(char));
	if (fgets(buffer, 1024*sizeof(char), self->source) == NULL)
		return;


	if (self->sourceLen + strlen(buffer) >= self->sourceCap)
	{
		int newCap = self->sourceCap + 1024;
		char *newBuffer = malloc(newCap*sizeof(char));
		memset(newBuffer, '\0', newCap*sizeof(char));
		memcpy(newBuffer, (void *)self->sourceText, self->sourceLen);
		free(self->sourceText);
		self->sourceText = newBuffer;
		self->sourceCap = newCap;
	}
	self->sourceLen += strlen(buffer);
	strncat(self->sourceText, buffer, self->sourceLen);
}

Character *Scanner_getNext(Scanner *self)
{
	Character *character;
	self->currIndex += 1;
	if (self->sourceLen == 0 || self->currIndex >= self->sourceLen)
		Scanner_readLine(self);

	if (self->currIndex < self->sourceLen)
	{
		if (self->currCol > 0 && self->sourceText[self->currIndex - 1] == '\n')
		{
			self->currLine += 1;
			self->currCol = 0;

		}
		else self->currCol += 1;

		character = new_Character(self->sourceText, self->currIndex,
			self->currLine, self->currCol, self->sourceText[self->currIndex]);
	}

	else
	{
		character = new_Character(self->sourceText, self->currIndex,
			self->currLine, self->currCol, EOF_CHAR);
	}

	return character;
}

char Scanner_peek(Scanner *self, int amount)
{
	if (self->currIndex + amount < self->sourceLen)
	{
		return self->sourceText[self->currIndex + amount];
	}
	else return EOF_CHAR;
}

bool Scanner_hasNext(Scanner *self)
{
	bool ret;
	if (self->currIndex+1 >= self->sourceLen)
		ret = false;
	else ret = true;
	return ret;
}

typedef struct token_t
{
	char *sourceText;
	int sourceIndex;
	int sourceLine;
	int sourceCol;
	char *string;
	int l, L;
	int type;

} Token;

Token *new_Token(Character *start, int type)
{
	Token *self = malloc(sizeof(Token));

	self->string = malloc(8*sizeof(char));
	self->string[0] = start->character;
	self->string[1] = '\0';
	self->l = 1;
	self->L = 8;

	self->sourceText = start->sourceText;
	self->sourceLine = start->sourceLine;
	self->sourceCol = start->sourceCol;

	self->type = type;

	return self;
}

void Token_append(Token *self, char c)
{
	if (c == '\0') return;
	if (self->l == self->L - 1)
	{
		int newL = 2*self->L;
		char *newString = malloc(newL*sizeof(char));
		int i = 0;
		while (i < self->L)
		{
			newString[i] = self->string[i];
			i++;
		}
		free(self->string);
		self->string = newString;
		self->L = newL;
	}
	self->string[self->l] = c;
	self->l++;
	self->string[self->l] = '\0';
}

typedef struct lexer_t
{
	struct scanner_t *scanner;
	struct character_t *frame;
	char c;
	char *cc;

} Lexer;

void Lexer_next(Lexer *self)
{
	self->frame = Scanner_getNext(self->scanner);
	self->c = self->frame->character;
	self->cc[0] = self->frame->character;
	self->cc[1] = Scanner_peek(self->scanner, 1);
	//printf("|%c|%s|\n", self->c, self->cc);
}

Lexer *new_Lexer(FILE *source)
{
	Lexer *self = malloc(sizeof(Lexer));
	self->scanner = new_Scanner(source);
	self->cc = malloc(3*sizeof(char));
	self->cc[2] = '\0';
	Lexer_next(self);
	return self;
}

Token *Lexer_getNext(Lexer *self)
{
	// ignore whitespace
	while (is_in(WHITESPACE_CHARS, self->c))
		Lexer_next(self);
	
	Token *token = new_Token(self->frame, -1);

	if (self->c == EOF_CHAR)
	{
		token->type = EOF_TOKEN;
		return token;
	}

	else if (is_in(IDENTIFIER_START_CHARS, self->c))
	{
		token->type = IDENTIFIER_TOKEN;
		Lexer_next(self);

		while (is_in(IDENTIFIER_CHARS, self->c))
		{
			Token_append(token, self->c);
			Lexer_next(self);
		}

		if (!strncmp(token->string, "if", 2))
			token->type = IF_TOKEN;
		else if (!strncmp(token->string, "else", 4))
			token->type = ELSE_TOKEN;
		else if (!strncmp(token->string, "while", 5))
			token->type = WHILE_TOKEN;
		else if (!strncmp(token->string, "break", 5))
			token->type = BREAK_TOKEN;
		else if (!strncmp(token->string, "null", 4))
			token->type = NULL_TOKEN;
		else if (!strncmp(token->string, "return", 6))
			token->type = RETURN_TOKEN;
		else if (!strncmp(token->string, "define", 6))
			token->type = DEFINE_TOKEN;

		return token;
	}

	else if (is_in(NUMERIC_CHARS, self->c))
	{
		token->type = NUMERIC_LITERAL_TOKEN;
		Lexer_next(self);

		while (is_in(NUMERIC_CHARS, self->c))
		{
			Token_append(token, self->c);
			Lexer_next(self);
		}

		return token;
	}

	else if (is_in(STRING_LITERAL_CHARS, self->c))
	{
		char q = self->c;
		token->type = STRING_LITERAL_TOKEN;
		Lexer_next(self);

		while (self->c != q)
		{
			Token_append(token, self->c);
			Lexer_next(self);
		}

		Token_append(token, self->c);
		Lexer_next(self);

		return token;
	}

	else if (!strncmp(self->cc, "==", 2))
	{
		token->type = EQUAL_TOKEN;
		Lexer_next(self);
		Token_append(token, self->c);
		Lexer_next(self);
		return token;
	}

	else if (!strncmp(self->cc, "!=", 2))
	{
		token->type = NOT_EQUAL_TOKEN;
		Lexer_next(self);
		Token_append(token, self->c);
		Lexer_next(self);
		return token;
	}

	else if (!strncmp(self->cc, ">=", 2))
	{
		token->type = GREATER_THAN_OR_EQUAL_TOKEN;
		Lexer_next(self);
		Token_append(token, self->c);
		Lexer_next(self);
		return token;
	}

	else if (!strncmp(self->cc, "<=", 2))
	{
		token->type = LESS_THAN_OR_EQUAL_TOKEN;
		Lexer_next(self);
		Token_append(token, self->c);
		Lexer_next(self);
		return token;
	}

	else if (!strncmp(self->cc, "&&", 2))
	{
		token->type = LOGICAL_AND_TOKEN;
		Lexer_next(self);
		Token_append(token, self->c);
		Lexer_next(self);
		return token;
	}

	else if (!strncmp(self->cc, "||", 2))
	{
		token->type = LOGICAL_OR_TOKEN;
		Lexer_next(self);
		Token_append(token, self->c);
		Lexer_next(self);
		return token;
	}

	else if (self->c == '=')
	{
		token->type = ASSIGNMENT_TOKEN;
		Lexer_next(self);
		return token;
	}

	else if (self->c == '+')
	{
		token->type = PLUS_TOKEN;
		Lexer_next(self);
		return token;
	}

	else if (self->c == '-')
	{
		token->type = MINUS_TOKEN;
		Lexer_next(self);
		return token;
	}

	else if (self->c == '*')
	{
		token->type = MULTIPLY_TOKEN;
		Lexer_next(self);
		return token;
	}

	else if (self->c == '/')
	{
		token->type = DIVIDE_TOKEN;
		Lexer_next(self);
		return token;
	}

	else if (self->c == '^')
	{
		token->type = POWER_TOKEN;
		Lexer_next(self);
		return token;
	}

	else if (self->c == '%')
	{
		token->type = MODULO_TOKEN;
		Lexer_next(self);
		return token;
	}

	else if (self->c == '>')
	{
		token->type = GREATER_THAN_TOKEN;
		Lexer_next(self);
		return token;
	}

	else if (self->c == '<')
	{
		token->type = LESS_THAN_TOKEN;
		Lexer_next(self);
		return token;
	}

	else if (self->c == '!')
	{
		token->type = NOT_TOKEN;
		Lexer_next(self);
		return token;
	}

	else if (self->c == ',')
	{
		token->type = COMMA_TOKEN;
		Lexer_next(self);
		return token;
	}

	else if (self->c == ';')
	{
		token->type = SEMICOLON_TOKEN;
		Lexer_next(self);
		return token;
	}

	else if (self->c == '(')
	{
		token->type = OPEN_PAREN_TOKEN;
		Lexer_next(self);
		return token;
	}

	else if (self->c == ')')
	{
		token->type = CLOSE_PAREN_TOKEN;
		Lexer_next(self);
		return token;
	}

	else if (self->c == '{')
	{
		token->type = OPEN_BRACKET_TOKEN;
		Lexer_next(self);
		return token;
	}

	else if (self->c == '}')
	{
		token->type = CLOSE_BRACKET_TOKEN;
		Lexer_next(self);
		return token;
	}

	else if (self->c == '.')
	{
		token->type = PERIOD_TOKEN;
		Lexer_next(self);
		return token;
	}

	else
	{
		token->type = UNKNOWN_TOKEN;
		Lexer_next(self);
		return token;
	}
}

typedef struct node_t
{
	struct token_t *token;
	int type;
	struct node_t **children;
	int r, R, level;

} Node;

Node *new_Node(Token *token, int type)
{
	Node *self = malloc(sizeof(Node));
	self->token = token;
	self->type = type;
	self->children = malloc(sizeof(Node *));
	self->r = 0;
	self->R = 1;
	return self;
}

void Node_addChild(Node *parent, Node *child)
{
	if (child == NULL) return;
	if (parent->r == parent->R)
	{
		int newR = 2*parent->R;
		Node **newChildren = malloc(newR*sizeof(Node *));
		int i = 0;
		while (i < parent->R)
		{
			newChildren[i] = parent->children[i];
			i++;
		}
		free(parent->children);
		parent->children = newChildren;
		parent->R = newR;
	}

	parent->children[parent->r] = child;
	parent->r++;
}

void Node_removeChild(Node *parent, int index)
{
	if (index < parent->r)
	{
		parent->children[index] = NULL;
		parent->r--;
		while (index < parent->r)
		{
			parent->children[index] = parent->children[index+1];
			parent->children[index+1] = NULL;
			index++;
		}
	}
}

void Node_printNode(Node *self, int level)
{
	int i = level;
	while (i--)
		printf("    ");

	char *data = "";
	if (self->token) data = self->token->string;
	printf("%s %s\n", NODE_DICT[self->type], data);

	i = 0;
	while (i < self->r)
		Node_printNode(self->children[i++], level+1);
}

typedef struct parser_t
{
	FILE *source;
	struct lexer_t *lexer;
	struct token_t *curr;
	struct node_t *root;

} Parser;

Parser *new_Parser(FILE *source)
{
	Parser *self = malloc(sizeof(Parser));
	self->source = source;
	self->lexer = new_Lexer(source);
	self->curr = NULL;
	self->root = new_Node(NULL, PROGRAM_NODE);
	return self;
}

void Parser_next(Parser *self)
{
	self->curr = Lexer_getNext(self->lexer);
}

bool Parser_found(Parser *self, int match)
{
	if (self->curr->type == match)
		return true;
	else
		return false;
}

void Parser_expect(Parser *self, int match)
{
	if (Parser_found(self, match)) return;
	else printf("Error at line %d col %d: expected %s, got %s\n",
		self->curr->sourceLine, self->curr->sourceCol,
		TOKEN_DICT[match], TOKEN_DICT[self->curr->type]);
	exit(0);
}

Node *Parser_boolean_expression(Parser *, Node *);

Node *Parser_arguments(Parser *self, Node *parent)
{
	Node *node = new_Node(NULL, ARGUMENTS_NODE);
	while (!Parser_found(self, CLOSE_PAREN_TOKEN))
	{
		Node_addChild(node, Parser_boolean_expression(self, node));
		if (!Parser_found(self, CLOSE_PAREN_TOKEN))
		{
			Parser_expect(self, COMMA_TOKEN);
			Parser_next(self);
		}
	}
	return node;
}

Node *Parser_factor(Parser *self, Node *parent)
{
	Node *node = new_Node(self->curr, -2);
	if (Parser_found(self, NUMERIC_LITERAL_TOKEN)) {
		node->type = NUMERIC_LITERAL_NODE;
		Parser_next(self);
	}

	else if (Parser_found(self, STRING_LITERAL_TOKEN)) {
		node->type = STRING_LITERAL_NODE;
		Parser_next(self);
	}

	else if (Parser_found(self, IDENTIFIER_TOKEN)) {
		
		Parser_next(self);
		if (Parser_found(self, OPEN_PAREN_TOKEN))
		{
			node->type = INSTANCE_NODE;
			Parser_next(self);
			Node_addChild(node, Parser_arguments(self, node));
			Parser_next(self);

		} else node->type = VARIABLE_NODE;

		if (Parser_found(self, PERIOD_TOKEN))
		{
			Parser_next(self);
			Node_addChild(node, Parser_factor(self, node));
		}
	}

	else if (Parser_found(self, OPEN_PAREN_TOKEN)) {
		node->type = BOOLEAN_EXPRESSION_NODE;
		node->token = NULL;
		Parser_next(self);
		Node_addChild(node, Parser_boolean_expression(self, node));
		Parser_expect(self, CLOSE_PAREN_TOKEN);
		Parser_next(self);

	} else if (Parser_found(self, NULL_TOKEN)) {
		node->type = NULL_NODE;
		Parser_next(self);

	} else 
	{
		printf("Error at line %d col %d: expected factor, got %s\n",
			self->curr->sourceLine, self->curr->sourceCol, TOKEN_DICT[self->curr->type]);
		exit(0);
	}

	
	return node;
}

Node *Parser_arithmetic_term(Parser *self, Node *parent)
{
	Node *node = new_Node(NULL, ARITHMETIC_TERM_NODE);
	Node_addChild(node, Parser_factor(self, node));
	while (Parser_found(self, MULTIPLY_TOKEN) || Parser_found(self, DIVIDE_TOKEN) || Parser_found(self, MODULO_TOKEN))
	{
		Node_addChild(node, new_Node(self->curr, OPERATION_NODE));
		Parser_next(self);
		Node_addChild(node, Parser_factor(self, node));
	}
	return node;
}

Node *Parser_arithmetic_expression(Parser *self, Node *parent)
{
	Node *node = new_Node(NULL, ARITHMETIC_EXPRESSION_NODE);
	Node_addChild(node, Parser_arithmetic_term(self, node));
	while (Parser_found(self, PLUS_TOKEN) || Parser_found(self, MINUS_TOKEN))
	{
		Node_addChild(node, new_Node(self->curr, OPERATION_NODE));
		Parser_next(self);
		Node_addChild(node, Parser_arithmetic_term(self, node));
	}
	return node;
}

Node *Parser_comparison(Parser *self, Node *parent)
{
	Node *node = new_Node(NULL, COMPARISON_NODE);
	Node_addChild(node, Parser_arithmetic_expression(self, node));
	while (Parser_found(self, EQUAL_TOKEN) || Parser_found(self, NOT_EQUAL_TOKEN) ||
		   Parser_found(self, GREATER_THAN_TOKEN) || Parser_found(self, LESS_THAN_TOKEN) ||
		   Parser_found(self, GREATER_THAN_OR_EQUAL_TOKEN) || Parser_found(self, LESS_THAN_OR_EQUAL_TOKEN))
	{
		Node_addChild(node, new_Node(self->curr, OPERATION_NODE));
		Parser_next(self);
		Node_addChild(node, Parser_arithmetic_expression(self, node));
	}
	return node;
}

Node *Parser_boolean_term(Parser *self, Node *parent)
{
	Node *node = new_Node(NULL, BOOLEAN_TERM_NODE);
	Node_addChild(node, Parser_comparison(self, node));
	while (Parser_found(self, LOGICAL_AND_TOKEN))
	{
		Node_addChild(node, new_Node(self->curr, OPERATION_NODE));
		Parser_next(self);
		Node_addChild(node, Parser_comparison(self, node));
	}
	return node;
}

Node *Parser_boolean_expression(Parser *self, Node *parent)
{
	Node *node = new_Node(NULL, BOOLEAN_EXPRESSION_NODE);
	Node_addChild(node, Parser_boolean_term(self, node));
	while (Parser_found(self, LOGICAL_OR_TOKEN))
	{
		Node_addChild(node, new_Node(self->curr, OPERATION_NODE));
		Parser_next(self);
		Node_addChild(node, Parser_boolean_term(self, node));
	}
	return node;
}

Node *Parser_statement(Parser *self, Node *parent)
{
	Node *node = new_Node(NULL, (int)NULL);

	if (Parser_found(self, IF_TOKEN))
	{
		node->type = CONDITIONAL_NODE;
		Parser_next(self);
		Parser_expect(self, OPEN_PAREN_TOKEN);
		Parser_next(self);
		Node_addChild(node, Parser_boolean_expression(self, node));
		Parser_expect(self, CLOSE_PAREN_TOKEN);
		Parser_next(self);
		Node_addChild(node, Parser_statement(self, node));
	}

	else if (Parser_found(self, ELSE_TOKEN))
	{
		node->type = ELSE_NODE;
		Parser_next(self);
		Node_addChild(node, Parser_statement(self, node));
	}

	else if (Parser_found(self, RETURN_TOKEN))
	{
		node->type = RETURN_NODE;
		Parser_next(self);
		if (!Parser_found(self, SEMICOLON_TOKEN))
			Node_addChild(node, Parser_boolean_expression(self, node));
		Parser_expect(self, SEMICOLON_TOKEN);
	}

	else if (Parser_found(self, WHILE_TOKEN))
	{
		node->type = LOOP_NODE;
		Parser_next(self);
		Parser_expect(self, OPEN_PAREN_TOKEN);
		Parser_next(self);
		Node_addChild(node, Parser_boolean_expression(self, node));
		Parser_expect(self, CLOSE_PAREN_TOKEN);
		Parser_next(self);
		Node_addChild(node, Parser_statement(self, node));
	}

	else if (Parser_found(self, BREAK_TOKEN))
	{
		node->type = BREAK_NODE;
		Parser_next(self);
		Parser_expect(self, SEMICOLON_TOKEN);
	}

	else if (Parser_found(self, OPEN_BRACKET_TOKEN))
	{
		node->type = BLOCK_NODE;
		Parser_next(self);
		while (!Parser_found(self, CLOSE_BRACKET_TOKEN))
		{
			Node_addChild(node, Parser_statement(self, node));
			Parser_next(self);
			if (Parser_found(self, EOF_TOKEN)) break;
		}
		Parser_expect(self, CLOSE_BRACKET_TOKEN);
	}

	else if (Parser_found(self, EOF_TOKEN))
	{
		free(node);
		return NULL;
	}

	else if (Parser_found(self, DEFINE_TOKEN))
	{
		node->type = DEFINITION_NODE;
		Parser_next(self);
		Parser_expect(self, IDENTIFIER_TOKEN);
		node->token = self->curr;
		Parser_next(self);
		Parser_expect(self, OPEN_PAREN_TOKEN);
		Parser_next(self);
		Node_addChild(node, Parser_arguments(self, node));
		Parser_next(self);
		Node_addChild(node, Parser_statement(self, node));
	}

	else
	{
		Node_addChild(node, Parser_boolean_expression(self, node));
		if (Parser_found(self, ASSIGNMENT_TOKEN))
		{
			node->type = ASSIGNMENT_NODE;
			Parser_next(self);
			Node_addChild(node, Parser_boolean_expression(self, node));
			Parser_expect(self, SEMICOLON_TOKEN);

		} else
		{
			node->type = EVALUATION_NODE;
			Parser_expect(self, SEMICOLON_TOKEN);
		}
	}

	return node;
}

Node *Node_coalesce(Node *curr, Node *parent, int index)
{

	if (curr->type == ELSE_NODE)
	{
		if (index > 0 && parent->children[index-1]->type == CONDITIONAL_NODE)
		{
			Node_addChild(parent->children[index-1], curr);
			Node_removeChild(parent, index);
			Node_coalesce(curr, parent, index);
		}
		else printf("Error: unreachable else statement\n");
	}

	Node *next = curr;
	if (parent != NULL && index >= 0)
	{
		while (next->r == 1 && curr->token == NULL &&
			!(curr->type == RETURN_NODE || curr->type == ARGUMENTS_NODE))
		{
			next = next->children[0];
			curr = next;
		}
		parent->children[index] = curr;
	}	

	int i = 0;
	while (i < curr->r)
	{
		Node_coalesce(curr->children[i], curr, i);
		i++;
	}

	return curr;
}

Node *Parser_parseNext(Parser *self)
{
	Parser_next(self);
	PROMPT = CONT_PROMPT;
	Node *next = Parser_statement(self, self->root);
	PROMPT = NEXT_PROMPT;
	if (next)
	{	
		Node_addChild(self->root, next);
		next = Node_coalesce(next, self->root, self->root->r - 1);
		if (PRINT_PARSE) Node_printNode(next, 0);
	}
	return next;
}

#define Object_HEAD \
	int ref_count;\
	int type;\

typedef struct object_t
{
	Object_HEAD

} Object;

typedef struct int_object_t
{
	Object_HEAD
	int val;

} IntObject;

typedef struct float_object_t
{
	Object_HEAD
	double val;

} FloatObject;

typedef struct string_object_t
{
	Object_HEAD
	int length;
	char *ref;

} StringObject;

typedef struct list_object_t
{
	Object_HEAD
	int num_elem;
	Object **ref;

} ListObject;

Object *new_IntObject(int val)
{
	IntObject *self = malloc(sizeof(IntObject));
	self->ref_count = 0;
	self->type = INTEGER_TYPE;
	self->val = val;
	return (Object *) self;
}

Object *new_FloatObject(double val)
{
	IntObject *self = malloc(sizeof(IntObject));
	self->ref_count = 0;
	self->type = INTEGER_TYPE;
	self->val = val;
	return (Object *) self;
}

Object *new_StringObject(char *ref)
{
	StringObject *self = malloc(sizeof(StringObject));
	self->ref_count = 0;
	self->type = STRING_TYPE;
	self->length = strlen(ref);
	self->ref = ref;
	return (Object *) self;
}

void Object_print(Object *self)
{
	if (self == NULL)
		printf("null\n");
	else if (self->type == INTEGER_TYPE)
		printf("%d\n",  ((IntObject *) self)->val);
	else if (self->type == FLOAT_TYPE)
		printf("%f\n",  ((FloatObject *) self)->val);
	else if (self->type == STRING_TYPE)
		printf("%s\n", ((StringObject *) self)->ref);
}

typedef struct scope_t // TODO implement this as a hash table
{
	struct scope_t *parent;
	struct object_t *head;

} Scope;

typedef struct interpreter_t
{
	struct parser_t *parser;

} Interpreter;

Interpreter *new_Interpreter(FILE *source)
{
	Interpreter *self = malloc(sizeof(Interpreter));
	self->parser = new_Parser(source);
	return self;
}

void Inter_multiply(Object *a, Object *b)
{
	if (a->type == INTEGER_TYPE && b->type == INTEGER_TYPE)
	{
		((IntObject *)a)->val = ((IntObject *)a)->val * ((IntObject *)b)->val;
		free((IntObject *)b);
	}
}

void Inter_divide(Object *a, Object *b)
{
	if (a->type == INTEGER_TYPE && b->type == INTEGER_TYPE)
	{
		((IntObject *)a)->val = ((IntObject *)a)->val / ((IntObject *)b)->val;
		free((IntObject *)b);
	}
}

void Inter_add(Object *a, Object *b)
{
	if (a->type == INTEGER_TYPE && b->type == INTEGER_TYPE)
	{
		((IntObject *)a)->val = ((IntObject *)a)->val + ((IntObject *)b)->val;
		free((IntObject *)b);
	}
}

void Inter_subtract(Object *a, Object *b)
{
	if (a->type == INTEGER_TYPE && b->type == INTEGER_TYPE)
	{
		((IntObject *)a)->val = ((IntObject *)a)->val - ((IntObject *)b)->val;
		free((IntObject *)b);
	}
}

void Inter_equal(Object *a, Object *b)
{
	if (a->type == INTEGER_TYPE && b->type == INTEGER_TYPE)
	{
		((IntObject *)a)->val = ((IntObject *)a)->val == ((IntObject *)b)->val;
		free((IntObject *)b);
	}
}

void Inter_not_equal(Object *a, Object *b)
{
	if (a->type == INTEGER_TYPE && b->type == INTEGER_TYPE)
	{
		((IntObject *)a)->val = ((IntObject *)a)->val != ((IntObject *)b)->val;
		free((IntObject *)b);
	}
}

void Inter_greater_equal(Object *a, Object *b)
{
	if (a->type == INTEGER_TYPE && b->type == INTEGER_TYPE)
	{
		((IntObject *)a)->val = ((IntObject *)a)->val >= ((IntObject *)b)->val;
		free((IntObject *)b);
	}
}

void Inter_greater(Object *a, Object *b)
{
	if (a->type == INTEGER_TYPE && b->type == INTEGER_TYPE)
	{
		((IntObject *)a)->val = ((IntObject *)a)->val > ((IntObject *)b)->val;
		free((IntObject *)b);
	}
}

void Inter_less_equal(Object *a, Object *b)
{
	if (a->type == INTEGER_TYPE && b->type == INTEGER_TYPE)
	{
		((IntObject *)a)->val = ((IntObject *)a)->val <= ((IntObject *)b)->val;
		free((IntObject *)b);
	}
}

void Inter_less(Object *a, Object *b)
{
	if (a->type == INTEGER_TYPE && b->type == INTEGER_TYPE)
	{
		((IntObject *)a)->val = ((IntObject *)a)->val < ((IntObject *)b)->val;
		free((IntObject *)b);
	}
}

void Inter_and(Object *a, Object *b)
{
	if (a->type == INTEGER_TYPE && b->type == INTEGER_TYPE)
	{
		((IntObject *)a)->val = ((IntObject *)a)->val && ((IntObject *)b)->val;
		free((IntObject *)b);
	}
}

void Inter_or(Object *a, Object *b)
{
	if (a->type == INTEGER_TYPE && b->type == INTEGER_TYPE)
	{
		((IntObject *)a)->val = ((IntObject *)a)->val || ((IntObject *)b)->val;
		free((IntObject *)b);
	}
}

Object *Inter_eval(Node *node)
{
	if (node->type == NUMERIC_LITERAL_NODE)
	{
		int value = atoi(node->token->string);
		return new_IntObject(value);
	}

	else if (node->type == STRING_LITERAL_NODE)
	{
		// remove quotes
		char *value = node->token->string+1;
		value[strlen(value)-1] = '\0';
		return new_StringObject(value);
	}

	else if (node->type == ARITHMETIC_TERM_NODE)
	{
		Object *a = Inter_eval(node->children[0]);
		int i = 1;
		while (i < node->r - 1)
		{
			char op = node->children[i++]->token->string[0];
			Object *b = Inter_eval(node->children[i++]);
			if (op == '*') Inter_multiply(a, b);
			else if (op == '/') Inter_divide(a, b);
		}
		return a;
	}

	else if (node->type == ARITHMETIC_EXPRESSION_NODE)
	{
		Object *a = Inter_eval(node->children[0]);
		int i = 1;
		while (i < node->r - 1)
		{
			char op = node->children[i++]->token->string[0];
			Object *b = Inter_eval(node->children[i++]);
			if (op == '+') Inter_add(a, b);
			else if (op == '-') Inter_subtract(a, b);
		}
		return a;
	}

	else if (node->type == COMPARISON_NODE)
	{
		Object *a = Inter_eval(node->children[0]);
		int i = 1;
		while (i < node->r - 1)
		{
			char *op = node->children[i++]->token->string;
			Object *b = Inter_eval(node->children[i++]);
			if (!strncmp(op, "==", 2)) Inter_equal(a, b);
			else if (!strncmp(op, "!=", 2)) Inter_not_equal(a, b);
			else if (!strncmp(op, ">=", 2)) Inter_greater_equal(a, b);
			else if (!strncmp(op, ">", 1)) Inter_greater(a, b);
			else if (!strncmp(op, "<=", 2)) Inter_less_equal(a, b);
			else if (!strncmp(op, "<", 1)) Inter_less(a, b);
		}
		return a;
	}

	else if (node->type == BOOLEAN_TERM_NODE)
	{
		Object *a = Inter_eval(node->children[0]);
		int i = 1;
		while (i < node->r - 1)
		{
			char *op = node->children[i++]->token->string;
			Object *b = Inter_eval(node->children[i++]);
			if (!strncmp(op, "&&", 2)) Inter_add(a, b);
		}
		return a;
	}

	else if (node->type == BOOLEAN_EXPRESSION_NODE)
	{
		Object *a = Inter_eval(node->children[0]);
		int i = 1;
		while (i < node->r - 1)
		{
			char *op = node->children[i++]->token->string;
			Object *b = Inter_eval(node->children[i++]);
			if (!strncmp(op, "||", 2)) Inter_add(a, b);
		}
		return a;
	}

	else if (node->type == CONDITIONAL_NODE)
	{
		Object *test = Inter_eval(node->children[0]);
		if (((IntObject *)test)->val)
			return Inter_eval(node->children[1]);
		else if (node->r == 3)
			return Inter_eval(node->children[2]);
	}

	else if (node->type == ASSIGNMENT_NODE)
	{
		return NULL;
	}

	else if (node->type == EVALUATION_NODE)
	{
		return Inter_eval(node->children[0]);
	}

	else return NULL;
}

bool Inter_execNext(Interpreter *self)
{
	Node *next = Parser_parseNext(self->parser);
	if (!next) return false;
	else Object_print(Inter_eval(next));
	return true;
}


int main(int argc, char **argv)
{
	FILE *source;
	if (argc < 2) source = stdin;
	else source = fopen(argv[1], "r");

	if (source == stdin && isatty(fileno(source)))
		fputs("The Mantra Language\n", stderr);
	Interpreter *m = new_Interpreter(source);
	while (Inter_execNext(m))
		continue;
	
	return 1;
}