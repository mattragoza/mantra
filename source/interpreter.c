#include "mantra.h"



char * OBJECT_TYPES[] = {"number", "string", "symbol", "sequence", "function", "error"};
char * ERROR_TYPES[] = {"Syntax", "Type", "Reference", "Division"};


/*
 * new_NumberObject
 * 	constructs an Object which contains a numeric value.
 * 	We do not distinguish between integers and floating-
 * 	point numbers- internally the NumberObject uses
 * 	a double-precision floating point representation.
 */
Object *new_NumberObject(double value)
{
	NumberObject *self = malloc(sizeof(NumberObject));
	self->ref_count = 0;
	self->type = NUMBER_OBJECT;

	self->value = value;
	return OBJ(self);
}

Object *new_StringObject(char *string)
{
	StringObject *self = malloc(sizeof(StringObject));
	self->ref_count = 0;
	self->type = STRING_OBJECT;

	self->len = strlen(string);
	self->cap = self->len;
	self->buffer = malloc((self->cap + 1) * sizeof(char));
	strncpy(self->buffer, string, self->len);
	self->buffer[0] = '"';
	self->buffer[self->len-1] = '"';
	self->buffer[self->len] = '\0';
	return OBJ(self);
}

Object *new_SymbolObject(char *string)
{
	SymbolObject *self = malloc(sizeof(SymbolObject));
	self->ref_count = 0;
	self->type = SYMBOL_OBJECT;

	self->len = strlen(string);
	self->cap = self->len;
	self->buffer = malloc((self->cap + 1) * sizeof(char));
	strncpy(self->buffer, string, self->len);
	self->buffer[self->len] = '\0';
	return OBJ(self);
}

Object *new_SequenceObject(int cap)
{
	SequenceObject *self = malloc(sizeof(SequenceObject));
	self->ref_count = 0;
	self->type = SEQUENCE_OBJECT;

	self->len = 0;
	self->cap = cap;
	self->elem = malloc(self->cap * sizeof(Object *));
	return OBJ(self);
}

Object *new_FunctionObject(Object *(*call)())
{
	FunctionObject *self = malloc(sizeof(FunctionObject));
	self->ref_count = 0;
	self->type = FUNCTION_OBJECT;

	self->call = call;
	return OBJ(self);
}

Object *new_ErrorObject(ErrorType err, char *message, Token *token)
{
	printf("%s error: %s %s\n", ERROR_TYPES[err], message, token?token->string:"");
	ErrorObject *self = malloc(sizeof(ErrorObject));
	self->ref_count = 0;
	self->type = ERROR_OBJECT;
	self->err = err;
	self->message = message;
	self->token = token;
	return OBJ(self);
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

	else if (self->type == SYMBOL_OBJECT)
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

	else if (self->type == FUNCTION_OBJECT)
	{
		return "<function>";
	}

	return "?"; //shouldn't be here
}


void SequenceObject_append(SequenceObject *seq, Object *obj)
{
	if (seq == NULL || obj == NULL) return;
	if (seq->len == seq->cap) // 2x resize
	{
		int new_cap = 2*seq->cap;
		Object **new_elem = malloc(new_cap*sizeof(Object *));
		int i = 0;
		while (i < seq->len) new_elem[i] = seq->elem[i++];
		free(seq->elem);
		seq->cap = new_cap;
		seq->elem = new_elem;
	}
	seq->elem[seq->len] = obj;
	seq->len++;
	return;
}

void StringObject_concat(StringObject *str1, StringObject *str2)
{
	if (str1 == NULL) str1 = str2;
	else if (str2 == NULL) return;
	if (str1->cap < str1->len + str2->len) // adequate resize
	{
		int new_cap = str1->len + str2->len;
		char *new_buffer = malloc((new_cap+1)*sizeof(char));
		int i = 0;
		while (i < str1->len) new_buffer[i] = str1->buffer[i++];
		free(str1->buffer);
		str1->cap = new_cap;
		str1->buffer = new_buffer;
	}
	str1->buffer[str1->len-1] = '\0';
	strcat(str1->buffer, str2->buffer+1);
	str1->len += (str2->len-2);
	str1->buffer[str1->len] = '\0';
	//del_Object(str2);
	return;
}

void Object_clear(Object *obj)
{
	if (obj == NULL) return;
	if (obj->type == SEQUENCE_OBJECT)
	{
		SequenceObject *seq = (void *) obj;
		seq->len = 0;
	}
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

Object *Interpreter_eval(Interpreter *self, Node *node, Context *context, int quoted)
{
	if (node == NULL || node->type == EOF_NODE)
		return NULL;
	
	else if (node->type == QUOTE_NODE)
		return Interpreter_eval(self, node->children[0], context, 1);
	
	else if (node->type == NUMERIC_LITERAL_NODE)
		return new_NumberObject(strtod(node->token->string, NULL));
	
	else if (node->type == STRING_LITERAL_NODE)
		return new_StringObject(node->token->string);
	
	else if (node->type == SEQUENCE_NODE)
	{
		// evaluate each of the sequence's element nodes.
		// if the first element evaluates to a function,
		// call that function with the rest of the sequence
		// as its arguments. otherwise return the whole sequence
	
		Object *seq = new_SequenceObject(node->num_children);
		Object *func = NULL;
		int i = 0;
		while (i < node->num_children)
		{
			Object *elem = Interpreter_eval(self, node->children[i], context, quoted);
			if (elem == NULL)
				return NULL; // EOF
			else if (i == 0 && !quoted && elem->type == FUNCTION_OBJECT)
				func = elem; // this sequence is a functon call
			else SequenceObject_append(SEQ(seq), elem);
			i++;
		}
		return func? FUN(func)->call(seq) : seq;
	}
	
	else if (node->type == SYMBOL_NODE)
	{
		if (quoted) return new_SymbolObject(node->token->string);
		else
		{
			Object *value = Context_get(context, node->token->string);
			if (value == NULL)
				return new_ErrorObject(REFERENCE_ERROR, "symbol is not defined in this context", node->token);
			return value;
		}
	}
	
	else if (node->type == ERROR_NODE)
		return new_ErrorObject(SYNTAX_ERROR, "parser encountered invalid syntax", node->token);
}

Object *Interpreter_evalnext(Interpreter *self)
{
	Node *next = Parser_getnext(self->parser);
	#ifdef DEBUG
	Node_print(next, 0);
	#endif
	return Interpreter_eval(self, next, self->global, 0);
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
	Context_set(mantra->global, "+", new_FunctionObject(Builtin_add));
	Context_set(mantra->global, "-", new_FunctionObject(Builtin_subtract));
	Context_set(mantra->global, "*", new_FunctionObject(Builtin_multiply));
	Context_set(mantra->global, "/", new_FunctionObject(Builtin_divide));

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
