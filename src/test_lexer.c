#include "lexer.h"

int main(int argc, char **argv)
{
	Lexer *l = new_Lexer(argc, argv);
	while (1)
	{
		Token *t = Lexer_getnext(l);

		if (check_error())
		{
			handle_error();
			del_Token(t);
			del_Lexer(l);
			return MANTRA_ERROR.type;
		}

		if (t->type == EOF_TOKEN)
		{
			del_Token(t);
			break;
		}

		printf("%s %s %d\n", TOKEN_TYPES[t->type], t->buffer, t->len);
		del_Token(t);
	}
	del_Lexer(l);
	return 0;
}