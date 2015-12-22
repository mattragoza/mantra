#include "parser.h"

int main(int argc, char **argv)
{
	Parser *p = new_Parser(argc, argv);
	while (1)
	{
		Object *o = Parser_getnext(p);

		if (check_error())
		{
			handle_error();
			if (o != NULL)
				del_Object(o);
			del_Parser(p);
			return 1;
		}

		if (o == NULL)
			break;

		Object_printtree(o, 0);
		del_Object(o);
	}
	del_Parser(p);
	return 0;
}