#include "interpreter.h"

Interpreter *new_Interpreter(int argc, char **argv)
{
	Interpreter *self = malloc(sizeof(Interpreter));
	self->parser = new_Parser(argc, argv);
	self->context = new_ListObject(2);

	ListObject *global = new_ListObject(GLOBAL_CONTEXT_CAP);
	//Context_set(global, new_SymbolObject("@", 0), OBJ(global));
	Context_set(global, new_SymbolObject(":=", 0), OBJ(new_FunctionObject(Builtin_set)));
	Context_set(global, new_SymbolObject("+", 0), OBJ(new_FunctionObject(Builtin_add)));
	Context_set(global, new_SymbolObject("-", 0), OBJ(new_FunctionObject(Builtin_subtract)));
	
	ListObject_append(self->context, NULL);
	ListObject_append(self->context, OBJ(global));
	return self;
}

void del_Interpreter(Interpreter *self)
{
	del_Object(OBJ(self->context));
	del_Parser(self->parser);
	free(self);
	return;
}

// static
Object *Interpreter_eval(Object *object, ListObject *context, int eval_mod)
{
	if (object == NULL)
		return NULL;
	
	else if (Object_istype(object, SYMBOL_OBJECT))
	{
		Object *value;
		eval_mod += SYM(object)->eval_mod;

		if (eval_mod <= 0) // return the symbol itself
			return object;

		else // dereference the link
		{
			value = Context_get(context, SYM(object));
			if (value == NULL)
			{
				cause_error(UNBOUND_SYMBOL_ERROR, SYM(object)->buffer, SYM(object)->len);
				return NULL;
			}

			if (eval_mod > 1)
				value = Interpreter_eval(value, context, eval_mod-1);

			return value;
		}
	}

	else if (Object_istype(object, NUMBER_OBJECT))
	{
		return object;
	}
	
	else if (Object_istype(object, STRING_OBJECT))
	{
		return object;
	}
	
	else if (Object_istype(object, LIST_OBJECT))
	{
		int i = 0;
		FunctionObject *func = NULL;
		while (i < LST(object)->len)
		{
			Object *elem = ListObject_get(LST(object), i);
			elem = Interpreter_eval(elem, context, eval_mod);
			ListObject_set(LST(object), i, elem);

			if (check_error())
				return NULL;

			if (Object_istype(elem, FUNCTION_OBJECT))
				func = FUN(ListObject_pop(LST(object), i)); //TODO ambiguous call checking
			else
				i++;
		}

		if (func != NULL)
			return func->call(LST(object), context); //TODO everything in global context for now
		else
			return object;
	}

	else if (Object_istype(object, FUNCTION_OBJECT))
	{
		return object;
	}

	return NULL;
}

Object *Interpreter_getnext(Interpreter *self)
{
	Object *o = Parser_getnext(self->parser);
	ListObject *global = LST(self->context->arr[1]);
	return Interpreter_eval(o, global, 0);
}

Object *Context_get(ListObject *context, SymbolObject *symbol)
{
	Object *found;
	int hash = SymbolObject_hash(symbol);
	while (context)
	{
		int h = hash % context->cap;
		if (h < 0) h += context->cap;

		int i = 0;
		while (i < context->cap)
		{
			// linear probing
			found = context->arr[(h + i) % context->cap];
			if (!found)
				break;
			else if (strcmp(symbol->buffer, SYM(found)->buffer) == 0)
			{
				return SYM(found)->link;
			}
			i++;
		}
		context = NULL; // TODO look at parent context
	}
	return NULL;
}

int Context_set(ListObject *context, SymbolObject *symbol, Object *value)
{
	SymbolObject *found;
	int hash = SymbolObject_hash(symbol);

	int h = hash % context->cap;
	if (h < 0) h += context->cap;

	int i = 0;
	while (i < context->cap)
	{
		// linear probing
		found = SYM(context->arr[(h + i) % context->cap]);
		if (found == NULL)
		{
			found = new_SymbolObject(symbol->buffer, 0); //just clone it
			context->arr[(h + i) % context->cap] = OBJ(found);
			SymbolObject_link(found, value);
			return 1;
		}
		if (strcmp(symbol->buffer, found->buffer) == 0)
		{
			SymbolObject_link(found, value);
			return 1;
		}
		i++;
	}

	return 0;
}