#include "object.h"

char * OBJECT_TYPES[] = {
	"symbol",
	"number",
	"string",
	"list",
	"function"
};


SymbolObject *new_SymbolObject(char *buffer, int eval_mod)
{
	SymbolObject *self = malloc(sizeof(SymbolObject));
	self->link_cnt = 0;
	self->type = SYMBOL_OBJECT;

	self->len = strlen(buffer);
	self->cap = self->len;
	self->buffer = malloc((self->cap + 1) * sizeof(char));
	strncpy(self->buffer, buffer, self->len);
	self->buffer[self->len] = '\0';
	self->eval_mod = eval_mod;
	self->link = NULL;

	return self;
}

NumberObject *new_NumberObject(double value)
{
	NumberObject *self = malloc(sizeof(NumberObject));
	self->link_cnt = 0;
	self->type = NUMBER_OBJECT;

	self->value = value;

	return self;
}

StringObject *new_StringObject(char *buffer)
{
	StringObject *self = malloc(sizeof(StringObject));
	self->link_cnt = 0;
	self->type = STRING_OBJECT;

	self->len = strlen(buffer);
	self->cap = self->len;
	self->buffer = malloc((self->cap + 1) * sizeof(char));
	strncpy(self->buffer, buffer, self->len);
	self->buffer[0] = '"';
	self->buffer[self->len-1] = '"';
	self->buffer[self->len] = '\0';

	return self;
}

ListObject *new_ListObject(int cap)
{
	ListObject *self = malloc(sizeof(ListObject));
	self->link_cnt = 0;
	self->type = LIST_OBJECT;

	self->len = 0;
	self->cap = cap;
	self->arr = malloc(self->cap * sizeof(Object *));

	int i = 0;
	while (i < cap)
	{
		self->arr[i] = NULL;
		i++;
	}

	return self;
}

FunctionObject *new_FunctionObject(c_func *call)
{
	FunctionObject *self = malloc(sizeof(FunctionObject));
	self->link_cnt = 0;
	self->type = FUNCTION_OBJECT;

	self->call = call;

	return self;
}

int Object_istype(Object *self, ObjectType type)
{
	return (self->type == type);
}

char *Object_tostring(Object *self)
{
	if (self == NULL)
		return "_";

	char *str;
	if (Object_istype(self, SYMBOL_OBJECT))
	{
		if (SYM(self)->eval_mod > 0)
		{
			int len = SYM(self)->len + SYM(self)->eval_mod;
			str = malloc(len + 1);
			int i = 0;
			while (i < SYM(self)->eval_mod)
				str[i++] = EVALUATE_CHAR;
			strcpy(str + SYM(self)->eval_mod, SYM(self)->buffer);	
		}

		else if (SYM(self)->eval_mod < 0)
		{
			int len = SYM(self)->len - SYM(self)->eval_mod;
			str = malloc(len + 1);
			int i = 0;
			while (i < -SYM(self)->eval_mod)
				str[i++] = ESCAPE_CHAR;
			strcpy(str - SYM(self)->eval_mod, SYM(self)->buffer);	
		}

		else
		{
			int len = SYM(self)->len;
			str = malloc(len + 1);
			strcpy(str, SYM(self)->buffer);	
		}
	}
	
	else if (Object_istype(self, NUMBER_OBJECT))
	{
		// this determines required buffer len without writing anything
		int len = snprintf(NULL, 0, "%G", NUM(self)->value);
		str = malloc(len + 1);
		sprintf(str, "%G", NUM(self)->value);
	}

	else if (Object_istype(self, STRING_OBJECT))
	{
		int len = STR(self)->len;
		str = malloc(len + 1);
		strcpy(str, STR(self)->buffer);
	}

	else if (Object_istype(self, LIST_OBJECT))
	{
		int len = 0, cap = 2 * LST(self)->cap + 1;
		str = malloc(cap + 1);
		memset(str, 0, cap + 1);

		str[0] = '(';
		len++;

		int i = 0;
		while (i < LST(self)->len)
		{
			char *elem_str = Object_tostring(LST(self)->arr[i]);
			i++;

			if (len + strlen(elem_str) + 1 > cap) // 2x resize
			{
				int new_cap = 2 * (len + strlen(elem_str) + 1);
				char *new_str = malloc(new_cap + 1);
				strncpy(new_str, str, len);
				free(str);
				cap = new_cap;
				str = new_str;
				str[len] = 0;
			}

			strcat(str, elem_str);
			len += strlen(elem_str);
			free(elem_str);

			if (i < LST(self)->len) // only add space if it's not the last elem
			{
				strcat(str, " ");
				len++;
			}
		}
		str[len] = ')';
		len++;

		str[len] = 0;
	}

	else if (Object_istype(self, FUNCTION_OBJECT))
	{
		str = "<function>";
	}

	else
	{
		str = "?"; //shouldn't be here
	}

	return str;
}

void Object_printtree(Object *self, int depth)
{
	if (self == NULL)
		return;

	int i = 0;
	while (i < depth)
	{
		fprintf(stderr, "  ");
		i++;
	}

	if (Object_istype(self, LIST_OBJECT))
	{
		fputs("(\n", stderr);
		int i = 0;
		while (i < LST(self)->len)
		{
			Object_printtree(LST(self)->arr[i], depth + 1);
			i++;
		}
		
		i = 0;
		while (i < depth)
		{
			fprintf(stderr, "  ");
			i++;
		}
		fputs(")\n", stderr);
	}

	else
	{
		char *str = Object_tostring(self);
		fprintf(stderr, "%s\n", str);
		free(str);
	}
}


void del_Object(Object *self)
{
	if (self == NULL)
		return;

	else if (Object_istype(self, SYMBOL_OBJECT))
	{
		free(SYM(self)->buffer);
		free(SYM(self));
	}

	else if (Object_istype(self, NUMBER_OBJECT))
	{
		free(NUM(self));
	}

	else if (Object_istype(self, STRING_OBJECT))
	{
		free(STR(self)->buffer);
		free(STR(self));
	}

	else if (Object_istype(self, LIST_OBJECT))
	{
		int i = 0;
		while (i < LST(self)->len)
		{
			del_Object(LST(self)->arr[i]);
			i++;
		}
		free(LST(self)->arr);
		free(LST(self));
	}
}

int SymbolObject_hash(SymbolObject *self)
{
	char *bytes = self->buffer;
	size_t len = self->len;

	size_t hash = 5381;

	int i = 0;
	while (i < len)
	{
		hash = ((hash << 5) + hash) + bytes[i];
		i++;
	}

	return hash;
}

void SymbolObject_link(SymbolObject *self, Object *object)
{
	if (self->link)
		self->link->link_cnt--;
	self->link = object;
	object->link_cnt++;
}

void StringObject_concat(StringObject *a, StringObject *b)
{
	if (a->cap < b->len + a->len) // adequate resize
	{
		int new_cap = a->len + b->len;
		char *new_buffer = malloc(new_cap + 1);
		int i = 0;
		while (i < a->len)
		{
			new_buffer[i] = a->buffer[i];
			i++;
		}
		free(a->buffer);
		a->cap = new_cap;
		a->buffer = new_buffer;
	}
	a->buffer[a->len - 1] = 0;
	strcat(a->buffer, b->buffer+1);
	a->len += (b->len - 2);
	a->buffer[a->len] = 0;
	return;
}

void ListObject_append(ListObject *self, Object *elem)
{
	if (self->len == self->cap) // 2x resize
	{
		int new_cap = 2 * self->cap;
		Object **new_arr = malloc(new_cap * sizeof(Object *));
		int i = 0;
		while (i < new_cap)
		{
			if (i < self-> len)
				new_arr[i] = self->arr[i];
			else
				new_arr[i] = NULL;
			i++;
		}
		free(self->arr);
		self->cap = new_cap;
		self->arr = new_arr;
	}
	self->arr[self->len] = elem;
	self->len++;
	return;
}

void ListObject_set(ListObject *self, int index, Object *elem)
{
	if (index < 0 || index >= self->len)
	{
		cause_error(LIST_INDEX_ERROR, NULL, 0);
		return;
	}

	self->arr[index] = elem;
}

Object *ListObject_get(ListObject *self, int index)
{
	if (index < 0 || index >= self->len)
	{
		cause_error(LIST_INDEX_ERROR, NULL, 0);
		return NULL;
	}

	return self->arr[index];
}

Object *ListObject_pop(ListObject *self, int index)
{
	Object *elem = self->arr[index];
	self->len--;

	int i = index;
	while (i < self->len)
	{
		self->arr[i] = self->arr[i+1];
		i++;
	}

	self->arr[self->len] = NULL;
	return elem;
}
