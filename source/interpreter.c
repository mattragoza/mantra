#include "mantra.h"

#define OBJ(x) ((Object *)x)
#define NUM(x) ((NumberObject *)x)
#define STR(x) ((StringObject *)x)
#define SEQ(x) ((SequenceObject *)x)
char * OBJECT_TYPES[] = {"number", "string", "sequence"};



Object *new_NumberObject(Node *node)
{
	NumberObject *self = malloc(sizeof(NumberObject));
	self->ref_count = 0;
	self->type = NUMBER_OBJECT;
	self->value = strtod(node->token->string, NULL);
	return (Object *) self;
}

Object *new_StringObject(Node *node)
{
	StringObject *self = malloc(sizeof(StringObject));
	self->ref_count = 0;
	self->type = STRING_OBJECT;
	self->len = node->token->len;
	self->cap = self->len;
	self->buffer = malloc((self->cap + 1) * sizeof(char));
	strncpy(self->buffer, node->token->string, self->len);
	self->buffer[0] = '\'';
	self->buffer[self->len-1] = '\'';
	self->buffer[self->len] = '\0';
	return (Object *) self;
}

Object *new_SequenceObject(Node *node)
{
	SequenceObject *self = malloc(sizeof(SequenceObject));
	self->ref_count = 0;
	self->type = SEQUENCE_OBJECT;
	self->len = 0;
	self->cap = node? node->num_children : 4;
	self->elem = malloc(self->cap * sizeof(Object *));
	return (Object *) self;
}

char *Object_tostring(Object *self)
{
	if (self->type == NUMBER_OBJECT)
	{
		// this determines required buffer len without writing anything
		int str_len = snprintf(NULL, 0, "%G", NUM(self)->value);
		char *str = malloc((str_len+1)*sizeof(char));
		sprintf(str, "%G", NUM(self)->value);
		return str;
	}

	else if (self->type == STRING_OBJECT)
	{
		if (!self) printf("the string doesn't exist\n");
		int str_len = STR(self)->len;
		char *str = malloc((str_len+1)*sizeof(char));
		strcpy(str, STR(self)->buffer);
		return str;
	}

	else if (self->type == SEQUENCE_OBJECT)
	{
		int str_cap = 2;
		char *str = malloc((str_cap+1)*sizeof(char));

		str[0] = '(';
		str[1] = '\0';
		int str_len = 1;

		int i = 0;
		while (i < SEQ(self)->len)
		{
			char *elem_str = Object_tostring(SEQ(self)->elem[i++]);
			if (str_len + strlen(elem_str) + 1 >= str_cap) // resize
			{
				int new_cap = 2*(str_len + strlen(elem_str) + 1); // 2x just to make sure
				char *new_str = malloc((new_cap+1)*sizeof(char));
				strncpy(new_str, str, str_len);
				free(str);
				str_cap = new_cap;
				str = new_str;
				str[str_len] = '\0';
			}
			strcat(str, elem_str);
			str_len += strlen(elem_str);
			free(elem_str);

			if (i < SEQ(self)->len) // only add space if it's not the last elem
			{
				strcat(str, " ");
				str_len += 1;	
			}
		}
		str[str_len] = ')';
		str[str_len+1] = '\0';
		str_len++;

		return str;
	}
	return "?"; //shouldn't be here
}

void Object_append(Object *a, Object *b)
{
	if (a == NULL || b == NULL) return;
	if (a->type == SEQUENCE_OBJECT)
	{
		SequenceObject *seq = (void *) a;
		if (seq->len >= seq->cap) // 2x resize
		{
			printf("resizing SequenceObject\n");
			int new_cap = 2 * seq->cap;
			Object **new_elem = malloc(new_cap * sizeof(Object *));
			int i = 0;
			while (i < seq->len)
				new_elem[i] = seq->elem[i++];
			seq->cap = new_cap;
			seq->elem = new_elem;
		}
		seq->elem[seq->len] = b;
		seq->len++;
	} // TODO handle other object types
	return;
}



Context *new_Context(Context *parent)
{
	Context *self = malloc(sizeof(Context));
	self->parent = parent;
	self->num = 0;
	self->cap = parent ? 11 : 57; // global is bigger
	self->map = malloc(self->cap * sizeof(Object *));
	int i = 0;
	while (i < self->cap) self->map[i++] = NULL;
	return self;
}

void del_Context(Context *self)
{
	// del_Objects ref'd in this context only... how? TODO
	free(self); // don't delete parent context
	return;
}

long Context_hash(Context *self, char *key)
{
	long hash = 5381;
	int c;
	int i = 0;
	while (key[i])
		hash = ((hash << 5) + hash) + key[i++];
	hash = hash % self->cap;
	if (hash < 0) hash += self->cap;
	printf("%ld\n", hash);
	return hash;
}

Object *Context_get(Context *self, char *key)
{
	long hash = Context_hash(self, key);
	Object *found = self->map[hash];
	if (!found && self->parent != NULL)
		found = Context_get(self->parent, key);
	return found;
}

void Context_set(Context *self, char *key, Object *value) // TODO should store key too, so we can resize
{
	long hash = Context_hash(self, key);
	if (self->map[hash] != NULL)
		printf("collision\n"); // TODO
	else self->map[hash] = value;
	return;
}



Interpreter *new_Interpreter(FILE *source)
{
	Interpreter *self = malloc(sizeof(Interpreter));
	self->parser = new_Parser(source);
	self->global = new_Context(NULL);
	return self;
}

void del_Interpreter(Interpreter *self)
{
	del_Parser(self->parser);
	del_Context(self->global);
	free(self);
	return;
}


Object *Interpreter_eval(Interpreter *self, Node *node, Context *context)
{
	if (node == NULL || node->type == EOF_NODE || node->type == ERROR_NODE)
		return NULL;

	else if (node->type == NUMERIC_LITERAL_NODE)
		return new_NumberObject(node);

	else if (node->type == STRING_LITERAL_NODE)
		return new_StringObject(node);

	else if (node->type == SEQUENCE_NODE)
	{
		Object *seq = new_SequenceObject(node);
		int i = 0;
		while (i < node->num_children)
		{
			Object *elem = Interpreter_eval(self, node->children[i], context);
			if (elem == NULL) return NULL; //EOF
			Object_append(seq, elem);
			i++;
		}
		return seq;
	}

	else if (node->type == SYMBOL_NODE)
	{
		Object *ref = Context_get(context, node->token->string);
		if (ref == NULL)
		{
			printf("Interpreter error at line %d col %d: unbound symbol %s\n",
				node->token->source_line+1, node->token->source_col+1,
				node->token->string);
			exit(1);
		}
		return ref;
	}
}

Object *Interpreter_evalnext(Interpreter *self)
{
	Node *next = Parser_getnext(self->parser);
	#ifdef DEBUG
	Node_print(next, 0);
	#endif
	return Interpreter_eval(self, next, self->global);
}


int main(int argc, char **argv)
{
	if (argc < 2) SOURCE = stdin;
	else SOURCE = fopen(argv[1], "r");

	if (isatty(fileno(SOURCE)))
	{
		INTERACTIVE_MODE = 1;
		PROMPT = COMMAND_PROMPT;
		fputs(SPLASH_MESSAGE, stderr);
	}
	else INTERACTIVE_MODE = 0;

	Interpreter *mantra = new_Interpreter(SOURCE);
	while (1)
	{
		Object *obj = Interpreter_evalnext(mantra);
		if (obj == NULL) break;

		char *objstr = Object_tostring(obj);
		printf("%s\n", objstr);
		free(objstr);
	}
	del_Interpreter(mantra);
	return 0;
}

/*
	Since I'm using a very simple syntax without any keywords or 
	special characters, going to have to provide pretty extensive
	built-in functions

	control flow
		if while for break

	variables and functions
		set def

	basic math
		add sub mult div pow mod

	logic and comparison
		and or not gt gteq lt lteq eq neq

	input output
		read write open close print
*/