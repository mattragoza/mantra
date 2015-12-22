#include "scanner.h"

int main(int argc, char **argv)
{
	Scanner *s = new_Scanner(argc, argv);
	while (1)
	{
		char c = Scanner_getnext(s);
		if (c == EOF_CHAR) break;
		else if (c == '\n')
			printf("\\n\t| index: %d line: %d column: %d\n",
				s->curr_index, s->curr_line, s->curr_col);
		else if (c == '\t')
			printf("\\t\t| index: %d line: %d column: %d\n",
				s->curr_index, s->curr_line, s->curr_col);
		else
			printf("%c\t| index: %d line: %d column: %d\n",
				c, s->curr_index, s->curr_line, s->curr_col);
	}
	del_Scanner(s);
	return 0;
}