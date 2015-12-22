#ifndef INIT_H
#define INIT_H

#include "common.h"
#include "error.h"

// strings for the command line interface
#define SPLASH_MESSAGE "The Mantra Language\nEnter ctrl+C to exit the interface.\n"
#define COMMAND_PROMPT "> "
#define CONTINUE_PROMPT "+ "

int INTERACTIVE_MODE;
char *PROMPT;

FILE *init_Mantra(int argc, char **argv);

#endif //INIT_H