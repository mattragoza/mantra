
#The Mantra Language

This is a toy language project I'm writing to test my knowledge of programming language syntax, semantics and interpretation. The name Mantra embodies the design philosophy being used to create the language, which is that concepts should be expressed concisely and understandably to produce the desired result.

##Usage

Mantra is implemented in C so it must be compiled differently depending on the environment. The executable included in this repository was compiled in 64-bit Windows 7.

The interpreter is invoked from the command line as such:
    
    C:/>mantra

Mantra source code files can be passed as an argument to run their contents:

    C:/>mantra tests/test.m

Or by piping them in:

    C:/>cat tests/test.m | mantra