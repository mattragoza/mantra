#include "mantra.h"

int main(int argc, char **argv)
{
	Interpreter *m = new_Interpreter(argc, argv);

	while (1)
	{
		Object *o = Interpreter_getnext(m);

		if (check_error())
			handle_error();

		if (o == NULL)
			break;

		fprintf(stdout, "%s\n", Object_tostring(o));
		//del_Object(o);
	}
	del_Interpreter(m);
	return 0;
}