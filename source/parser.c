#include "mantra.h"

int PARSER_DEBUG;



char * NODE_TYPES[] = {"program", "error", "end of file", "evaluation", "string literal", "numeric literal"};

Node *new_Node(Token *token, NodeType type)
{
	Node *self = malloc(sizeof(Node));
	self->token = token;
	self->type = type;
	self->children = malloc(2*sizeof(Node *));
	self->num_children = 0;
	self->max_children = 2;
	//self->level;
	return self;
}

void del_Node(Node *self) // recursive
{
	if (self == NULL) return;
	int i = 0;
	while (i < self->num_children)
	{
		del_Node(self->children[i]);
		i++;
	}
	del_Token(self->token);
	free(self->children);
	free(self);
	return;
}

void Node_addchild(Node *self, Node *child)
{
	if (self == NULL || child == NULL) return;
	if (self->num_children == self->max_children)
	{
		int new_max = 2*self->max_children;
		Node **new_children = malloc(new_max*sizeof(Node *));
		int i = 0;
		while (i < self->max_children)
		{
			new_children[i] = self->children[i];
			i++;
		}
		free(self->children);
		self->children = new_children;
		self->max_children = new_max;
	}

	self->children[self->num_children] = child;
	self->num_children++;
	return;
}

void Node_removechild(Node *self, int index)
{
	if (self == NULL) return;
	if (index < self->num_children)
	{
		del_Node(self->children[index]);
		self->children[index] = NULL;
		self->num_children--;
		while (index < self->num_children)
		{
			self->children[index] = self->children[index+1];
			self->children[index+1] = NULL;
			index++;
		}
	}
	return;
}

void Node_printnode(Node *self, int level)
{
	if (self == NULL) return;
	int i = level;
	while (i--) printf("    ");

	char *string = "";
	if (self->token) string = self->token->string;
	printf("%s %s\n", NODE_TYPES[self->type], string);
	i = 0;
	while (i < self->num_children)
		Node_printnode(self->children[i++], level+1);
}



Parser *new_Parser(FILE *source)
{
	Parser *self = malloc(sizeof(Parser));
	self->source = source;
	self->lexer = new_Lexer(source);
	self->curr = NULL;
	self->root = new_Node(NULL, PROGRAM_NODE);
	return self;
}

void del_Parser(Parser *self)
{
	if (self == NULL) return;
	del_Lexer(self->lexer);
	del_Token(self->curr);
	del_Node(self->root);
	free(self);
	return;
}

void Parser_step(Parser *self)
{
	self->curr = Lexer_getnext(self->lexer);
	if (PARSER_DEBUG) printf("[Parser steps to %s]\n", self->curr->string);
	return;
}

int Parser_found(Parser *self, TokenType match)
{
	if (PARSER_DEBUG) printf("[Parser found %s, was looking for %s]\n", TOKEN_TYPES[self->curr->type], TOKEN_TYPES[match]);
	if (self->curr->type == match)
		return 1;
	else
		return 0;
}

int Parser_expect(Parser *self, TokenType expected)
{
	if (PARSER_DEBUG) printf("[Parser expects %s]\n", TOKEN_TYPES[expected]);
	if (Parser_found(self, expected)) return 1;
	else fprintf(stderr, "ParseError at line %d col %d: expected %s, got %s\n",
		self->curr->source_line, self->curr->source_col,
		TOKEN_TYPES[expected], TOKEN_TYPES[self->curr->type]);
	return 0;
}

Node *Parser_error(Parser *self, Node *err)
{
	if (PARSER_DEBUG) printf("[Parser is complaining, but we should keep going]\n");
	err->type = ERROR_NODE;
	return err;
}

Node *Parser_statement(Parser *self, Node *parent)
{
	if (Parser_found(self, EOF_TOKEN))
	{
		return new_Node(self->curr, EOF_NODE);
	}

	else
	{
		Node *statement = Parser_evaluation(self, parent);
		if (!Parser_expect(self, SEMICOLON_TOKEN))
			return Parser_error(self, statement);
		return statement;
	}
}

Node *Parser_evaluation(Parser *self, Node *parent)
{
	if (Parser_found(self, IDENTIFIER_TOKEN))
	{
		Node *eval = new_Node(self->curr, EVALUATION_NODE);
		Parser_step(self);
		if (Parser_found(self, OPEN_PAREN_TOKEN))
		{
			Parser_step(self);
			Parser_sequence(self, eval);
			if (!Parser_expect(self, CLOSE_PAREN_TOKEN))
				return Parser_error(self, eval);
			Parser_step(self);
		}

		return eval;
	}

	else if (Parser_found(self, STRING_LITERAL_TOKEN))
	{
		Node *string = new_Node(self->curr, STRING_LITERAL_NODE);
		Parser_step(self);
		return string;
	}

	else if (Parser_found(self, NUMERIC_LITERAL_TOKEN))
	{
		Node *number = new_Node(self->curr, NUMERIC_LITERAL_NODE);
		Parser_step(self);
		return number;
	}

	else
	{
		Node *unknown = new_Node(self->curr, ERROR_NODE);
		printf("ParseError at line %d col %d: expected evaluation, got %s\n",
				self->curr->source_line, self->curr->source_col,
				self->curr->string);
		return Parser_error(self, unknown);
	}
}

Node *Parser_sequence(Parser *self, Node *parent)
{
	if (!Parser_found(self, CLOSE_PAREN_TOKEN))
	{
		Node *elem = Parser_evaluation(self, parent);
		Node_addchild(parent, elem);

		while (Parser_found(self, COMMA_TOKEN))
		{
			Parser_step(self);
			Node *elem = Parser_evaluation(self, parent);
			Node_addchild(parent, elem);
		}
	}
	return parent;
}

Node *Parser_getnext(Parser *self)
{
	Parser_step(self);
	PROMPT = CONTINUE_PROMPT;
	Node *next = Parser_statement(self, self->root);
	PROMPT = COMMAND_PROMPT;
	if (next && next->type != EOF_NODE) Node_addchild(self->root, next);
	return next;
}



#ifdef PARSER_TEST
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
	
#ifdef DEBUG
	PARSER_DEBUG = 1;
#endif
	if (PARSER_DEBUG) printf("[Parser debugging mode]\n");

	Parser *my_parser = new_Parser(SOURCE);
	while (1)
	{
		Node *node = Parser_getnext(my_parser);
		if (node->type == EOF_NODE)
		{
			del_Node(node);
			break;
		}
		Node_printnode(node, 0);
	}
	del_Parser(my_parser);
	return 0;
}
#endif