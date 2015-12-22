#include "builtin.h"

Object *Builtin_set(ListObject *arg, ListObject *context)
{
	if (arg->len == 2 && arg->arr[0]->type == SYMBOL_OBJECT)
	{
		SymbolObject *symbol = SYM(arg->arr[0]);
		Object *value = arg->arr[1];
		Context_set(context, symbol, value);
		return value;
	}
	else
	{
		cause_error(INCORRECT_ARGUMENT_ERROR, "(set <symbol> <object>)", 23);
		return NULL;
	}
}

Object *Builtin_add(ListObject *arg, ListObject *context)
{
	NumberObject *ret = NULL;

	int i = 0;
	while (i < arg->len)
	{
		Object *term = arg->arr[i];
		if (Object_istype(term, NUMBER_OBJECT))
		{
			if (i == 0)
			{
				double init = NUM(term)->value;
				ret = new_NumberObject(init);
			}
			else
				NUM(ret)->value += NUM(term)->value;
		}
		else
		{
			if (ret != NULL)
				del_Object(OBJ(ret));
			ret = NULL;
			break;
		}
		i++;
	}
	if (ret == NULL)
		cause_error(INCORRECT_ARGUMENT_ERROR, "(<number> <number> ...)", 23);

	return OBJ(ret);
}

Object *Builtin_subtract(ListObject *arg, ListObject *context)
{
	NumberObject *ret = NULL;

	int i = 0;
	while (i < arg->len)
	{
		Object *term = arg->arr[i];
		if (Object_istype(term, NUMBER_OBJECT))
		{
			if (i == 0)
			{
				double init = NUM(term)->value;
				if (arg->len == 1)
					init = -init;
				ret = new_NumberObject(init);
			}
			else
				NUM(ret)->value -= NUM(term)->value;
		}
		else
		{
			if (ret != NULL)
				del_Object(OBJ(ret));
			ret = NULL;
			break;
		}
		i++;
	}
	if (ret == NULL)
		cause_error(INCORRECT_ARGUMENT_ERROR, "(<number> <number> ...)", 23);

	return OBJ(ret);
}

/*
Object *Builtin_multiply(SequenceObject *arg, Context *context)
{
	Object *ret = NULL;
	int i = 0;
	while (i < arg->len)
	{
		Object *elem = arg->elem[i];
		if (elem->type == NUMBER_OBJECT)
		{
			if (i == 0) ret = new_NumberObject(NUM(elem)->value);
			else NUM(ret)->value *= NUM(elem)->value;
		}
		else return new_ErrorObject(TYPE_ERROR, "all arguments to multiply must be numeric", NULL);
		i++;
	}
	return ret;
}

Object *Builtin_divide(SequenceObject *arg, Context *context)
{
	Object *ret = NULL;
	int i = 0;
	while (i < arg->len)
	{
		Object *elem = arg->elem[i];
		if (elem->type == NUMBER_OBJECT)
		{
			if (i == 0) ret = new_NumberObject(NUM(elem)->value);
			else
			{
				if (NUM(elem)->value == 0)
					return new_ErrorObject(DIVISION_ERROR, "division by zero is undefined", NULL);
				else
					NUM(ret)->value /= NUM(elem)->value;
			}
		}
		else return new_ErrorObject(TYPE_ERROR, "all arguments to add must be numeric", NULL);
		i++;
	}
	return ret;
}
*/