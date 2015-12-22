#ifndef SCANNER_H
#define SCANNER_H

#include "common.h"
#include "init.h"
#include "error.h"

#define EOF_CHAR 4 // end of transmisson

/*
Scanner class
	reads from source file character by character
	keeps track of cursor position in the source file
	outputs prompt in interactive mode at beginning of each line
*/
typedef struct scanner_t
{
	FILE *source_fd;	// the source file descriptor
	int curr_index;		// current index in source file
	int curr_line;		// current line in source file
	int curr_col;		// current column in source file
	char curr_char;		// character at cursor in source file

} Scanner;

/* Scanner methods */
Scanner *new_Scanner(int argc, char **argv);// constructor
void del_Scanner(Scanner *self); 			// destructor
char Scanner_getcurr(Scanner *self);		// return curr char in source code
char Scanner_getnext(Scanner *self);		// return next char in source code

#endif //SCANNER_H