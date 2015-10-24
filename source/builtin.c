#include "mantra.h"

/*
 * (+ OBJ1 ... OBJn)
 * 	If all arguments are numeric, evaluates their arithmetic sum.
 * 	If all arguments are strings, concatenates them together.
 * 	If all arguments are sequences, joins them into one sequence.
 * 	If argument types are mixed, returns an error.
 */
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
			else if (ret->type != NUMBER_OBJECT) 
			{
				//del_Object(ret);
				//del_Object(arg);
				return new_ErrorObject(TYPE_ERROR, "can't add number to non-number");
			}
			else NUM(ret)->value += NUM(elem)->value;
		}	
		else if (elem->type == STRING_OBJECT)
		{
			if (i == 0) ret = new_StringObject(STR(elem)->buffer);
			else if (ret->type != STRING_OBJECT)
			{
				return new_ErrorObject(TYPE_ERROR, "can't add string to non-string");	
			}
			else StringObject_concat(STR(ret), STR(elem));
		}
		i++;
	}
	return ret;
}

Object *Builtin_sub(SequenceObject *arg)
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
				ret = OBJ(new_NumberObject(init));
			}
			else NUM(ret)->value -= NUM(elem)->value;

		} else return NULL;
		i++;
	}
	return ret;
}
