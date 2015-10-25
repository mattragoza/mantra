#include "mantra.h"

Object *Builtin_add(SequenceObject *arg)
{
	Object *ret = NULL;
	int i = 0;
	while (i < arg->len)
	{
		Object *elem = arg->elem[i];
		if (elem->type == NUMBER_OBJECT)
		{
			if (i == 0) ret = new_NumberObject(NUM(elem)->value);
			else NUM(ret)->value += NUM(elem)->value;
		}
		else return new_ErrorObject(TYPE_ERROR, "all arguments to add must be numeric", NULL);
		i++;
	}
	return ret;
}

Object *Builtin_subtract(SequenceObject *arg)
{
	Object *ret = NULL;
	int i = 0;
	while (i < arg->len)
	{
		Object *elem = arg->elem[i];
		if (elem->type == NUMBER_OBJECT)
		{
			if (i == 0) 
			{
				// init the return value to value of first
				// element in args if there's more than one
				// arg, otherwise set it to negative of first element
	
				double init;
				if (arg->len == 1) init = -(NUM(elem)->value);
				else init = NUM(elem)->value;
				ret = new_NumberObject(init);
			}
			else NUM(ret)->value -= NUM(elem)->value;
		} 
		else return new_ErrorObject(TYPE_ERROR, "all arguments to subtract must be numeric", NULL);
		i++;
	}
	return ret;
}

Object *Builtin_multiply(SequenceObject *arg)
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

Object *Builtin_divide(SequenceObject *arg)
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

