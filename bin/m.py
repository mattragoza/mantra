import sys
import string
EOF = '\0'
RUNNING = True
SPLASH = "The M Language \nEnter commands, or ctrl+Z to signal EOF.\n"

KEYWORDS = """
if
else
while
break
print
return
""".split()

ONE_CHAR_SYMBOLS = """
( )
{ }
= < >
+ - * / %
! & |
. ,
;
""".split()

TWO_CHAR_SYMBOLS = """
== <= >= !=
&& ||
""".split()

IDENTIFIER_START_CHARS = string.ascii_letters
IDENTIFIER_CHARS = string.ascii_letters + string.digits + '_'

NUMBER_CHARS = string.digits + '.'

STRING_LITERAL_CHARS = "\"\'"
WHITESPACE_CHARS = " \t\n"
COMMENT_START_SYMBOL = "/*"
COMMENT_END_SYMBOL = "*/"

# token types
STRING = "string"
IDENTIFIER = "identifier"
NUMBER = "number"
WHITESPACE = "whitespace"
COMMENT = "comment"
END_OF_FILE = "end of file"
NA = "#NA"

# parsed type
BLOCK = "block"
STATEMENT = "statement"
EVALUATION = "evaluation"
VARIABLE = "variable"
ASSIGNMENT = "assignment"
FUNCTION = "function"
CONDITION = "condition"
DEFINITION = "definition"
ARGUMENTS = "arguments"
EXPRESSION = "expression"
TERM = "term"
















class Character:

    def __init__(self, sourceText, sourceIndex, lineIndex, colIndex, c):
        
        self.sourceText  = sourceText
        self.sourceIndex = sourceIndex
        self.lineIndex   = lineIndex
        self.colIndex    = colIndex
        self.cargo       = c

    def __str__(self):

        c = self.cargo
        if   c == ' ' : c = " "
        elif c == '\n': c = "\\n"
        elif c == '\t': c = "\\t"
        elif c == EOF : c = "EOF"

        return (str(self.lineIndex).rjust(6) + \
                str(self.colIndex).rjust(4)  + " " + c)















class Scanner:

    def __init__(self, what):

        global source, sourceText, sourceLen, sourceIndex, lineIndex, colIndex
        source  = what
        sourceText = ""
        sourceLen = 0
        sourceIndex = -1
        lineIndex   =  0
        colIndex    = -1

    def get(self):

        global sourceText, sourceLen, sourceIndex, lineIndex, colIndex
        sourceIndex += 1

        if sourceText == "" or sourceIndex >= sourceLen:
            sourceText += source.readline()
            sourceLen = len(sourceText)

        if sourceIndex < sourceLen:

            if colIndex > 0 and sourceText[sourceIndex - 1] == '\n':
                lineIndex += 1
                colIndex   = 0
            else:
                colIndex += 1

            char = Character(sourceText, sourceIndex, lineIndex, colIndex, \
                             sourceText[sourceIndex])

        else: #EOF
            char = Character(sourceText, sourceIndex, lineIndex, colIndex, EOF)

        return char

    def lookahead(self, amount):

        if sourceIndex + amount < sourceLen:
            return sourceText[sourceIndex + amount]
        else:
            return EOF

























class Token:

    def __init__(self, startChar):

        self.cargo      = startChar.cargo
        self.sourceText = startChar.sourceText
        self.lineIndex  = startChar.lineIndex
        self.colIndex   = startChar.colIndex
        self.type = None

    def __str__(self):

        s = ""
        if self.type == self.cargo:
            s = s + "Symbol :\t" + self.type
        elif self.type == "Whitespace": 
            s = s + "Whitespace :\t" + repr(self.cargo)
        else:
            s = s + self.type + " :\t" + self.cargo
        return s










class Lexer:

    def __init__(self, scanner):

        self.scanner = scanner
        self.next()

    def next(self):

        global frame, c1, c2
        self.frame = self.scanner.get()
        self.c1 = self.frame.cargo
        self.c2 = self.c1 + self.scanner.lookahead(1)

    def get(self):

        while self.c1 in WHITESPACE_CHARS or self.c2 == COMMENT_START_SYMBOL:

            # Whitespace
            while self.c1 in WHITESPACE_CHARS:
                token = Token(self.frame)
                token.type = WHITESPACE
                self.next()

                while self.c1 in WHITESPACE_CHARS:
                    token.cargo += self.c1
                    self.next()

                #return token

            # Comment
            while self.c2 == COMMENT_START_SYMBOL:
                token = Token(self.frame)
                token.type = COMMENT
                token.cargo = self.c2
                self.next()
                self.next()

                while not self.c2 == COMMENT_END_SYMBOL:
                    if self.c1 == EOF: break
                    token.cargo += self.c1 # append comment
                    self.next()

                token.cargo += self.c2
                self.next()
                self.next()
                
                #return token

        token = Token(self.frame)

        # EOF
        if self.c1 == EOF:
            token.type = END_OF_FILE
            return token

        # Identifier or keyword
        if self.c1 in IDENTIFIER_START_CHARS:
            token.type = IDENTIFIER
            self.next()

            while self.c1 in IDENTIFIER_CHARS:
                token.cargo += self.c1
                self.next()

            if token.cargo in KEYWORDS:
                token.type = token.cargo
            
            
            return token

        # Numeric constant
        if self.c1 in NUMBER_CHARS:
            token.type = NUMBER
            self.next()

            while self.c1 in NUMBER_CHARS:
                token.cargo += self.c1
                self.next()

            
            return token

        # String literal
        if self.c1 in STRING_LITERAL_CHARS:
            token.type = STRING
            quoteChar = self.c1
            self.next()

            while self.c1 != quoteChar:
                if self.c1 == EOF: break
                token.cargo += self.c1
                self.next()

            token.cargo += self.c1
            self.next()
            
            return token

        # Two-character symbol
        if self.c2 in TWO_CHAR_SYMBOLS:
            token.cargo = self.c2
            token.type = token.cargo
            self.next()
            self.next()
            
            return token

        # One-character symbol
        if self.c1 in ONE_CHAR_SYMBOLS:
            token.type = token.cargo
            self.next()
            
            return token
























class Node:

    def __init__(self, token):
        self.token = token
        if token is None: self.type  = None
        else: self.type = self.token.type
        self.children = []
        self.level = 0

    def addChild(self, node):
        self.children.append(node)

    def __eq__(self, other):
        if isinstance(other, self.__class__):
            if self.type is not None and other.type is not None and self.type == other.type:
                if self.token is not None and other.token is not None and self.token.cargo == other.token.cargo:
                    if len(self.children) == len(other.children):
                        return True
        return False

    def __ne__(self, other):
        return not self.__eq__(other)

    def __str__(self):

        s = "    " * self.level
        if self.token == None:
            s += self.type + "\n"
        else:
            s += self.type + " " + self.token.cargo + "\n"

        for child in self.children:
            child.level = self.level + 1
            s += child.__str__()

        return s













"""
Grammer:

PROGRAM         -> STATEMENT; [ STATEMENT; ] EOF
STATEMENT       -> ASSIGNMENT | IF_STATEMENT | WHILE_STATEMENT | BLOCK
BLOCK           -> { STATEMENT; [ STATEMENT; ] }
ASSIGNMENT      -> IDENTIFIER = EXPRESSION
IF_STATEMENT    -> if ( CONDITION ) STATEMENT
WHILE-STATEMENT -> while ( CONDITION ) STATEMENT
CONDITION       -> EXPRESSION COMPARE EXPRESSION
EXPRESSION      -> TERM [ ADD_SUB TERM ]
TERM            -> FACTOR [ MULT_DIV TERM ]
FACTOR          -> IDENTIFIER | NUMBER | STRING | ( EXPRESSION )
COMPARE         -> == | != | >= | <= | > | <
ADD_SUB         -> + | -
MULT_DIV        -> * | /

"""

class Parser:

    def __init__(self, lexer):

        self.lexer = lexer
        self.root = Node(None)
        self.root.type = None
        self.curr  = None

    def next(self):

        self.curr = self.lexer.get()
        #sys.stderr.write(self.curr + "\n")

    def found(self, t):

        if self.curr.type == t:
            return True
        else:
            return False

    def expect(self, t):

        global RUNNING
        if self.found(t):
            return True
        else:
            sys.stderr.write(self.curr.colIndex * "-" + "^\n")
            sys.stderr.write("Error : expected " + t + "\n")
            RUNNING = False

    def factor(self, parent):

        global RUNNING
        atom = self.curr
        if self.found(IDENTIFIER):
            self.next()
            node = Node(atom)

            if self.found("("):
                node.type = FUNCTION
                self.next()
                args = self.arguments(node)
                node.addChild(args)
                self.expect(")")
                self.next()
            else:
                node.type = VARIABLE

            return node

        elif self.found(NUMBER):
            self.next()
            return Node(atom)

        elif self.found(STRING):
            self.next()
            return Node(atom)

        elif self.found("("):
            self.next()
            cond = self.evaluation(parent)
            self.expect(")")
            self.next()
            return cond

        else:
            sys.stderr.write(self.curr.colIndex * "-" + "^\n")
            sys.stderr.write("error: expected factor\n")
            RUNNING = False

    def term(self, parent):

        node = Node(None)
        node.type = TERM

        fact = self.factor(node)
        node.addChild(fact)

        while self.curr.type == "*" or self.curr.type == "/":
            op = Node(self.curr)
            node.addChild(op)
            self.next()
            fact = self.factor(node)
            node.addChild(fact)

        if len(node.children) == 1: return fact
        return node

    def expression(self, parent):

        node = Node(None)
        node.type = EXPRESSION

        term = self.term(node)
        node.addChild(term)

        while self.curr.type in ["+", "-"]:
            op = Node(self.curr)
            node.addChild(op)
            self.next()
            term = self.term(node)
            node.addChild(term)

        if len(node.children) == 1: return term
        return node

    def evaluation(self, parent):

        global RUNNING
        node = Node(None)
        node.type = EVALUATION

        expr = self.expression(node)
        node.addChild(expr)

        if self.found("="):
            if expr.type == VARIABLE:
                node.type = ASSIGNMENT
                self.next()
                assg = self.expression(node)
                node.addChild(assg)
                return node

            elif expr.type == FUNCTION:
                node.type = DEFINITION
                self.next()
                defn = self.statement(node)
                node.addChild(defn)
                return node
                        
            else:
                sys.stderr.write(self.curr.colIndex * "-" + "^\n")
                sys.stderr.write("error: can't assign to " + expr.type + "\n")
                RUNNING = False

        while self.curr.type in ["==", "!=", ">", "<", ">=", "<="]:
            node.type = CONDITION
            op = Node(self.curr)
            node.addChild(op)
            self.next()
            expr = self.expression(node)
            node.addChild(expr)

        if len(node.children) == 1: return expr
        return node

    def arguments(self, parent):

        node = Node(None)
        node.type = ARGUMENTS

        if self.found(IDENTIFIER) or self.found(NUMBER) or self.found(STRING) or self.found("("):
            arg = self.evaluation(node)
            node.addChild(arg)
            while self.found(","):
                self.next()
                arg = self.evaluation(node)
                node.addChild(arg)

        return node

    def block(self, parent):

        node = Node(None)
        node.type = BLOCK

        while not self.found("}"):
            stmt = self.statement(node)
            node.addChild(stmt)
            self.next()

        return node


    def statement(self, parent):

        global RUNNING
        node = Node(None)
        node.type = STATEMENT

        if self.found("if"):
            node.type = self.curr.type
            self.next()
            self.expect("(")
            self.next()
            cond = self.evaluation(node)
            node.addChild(cond)
            self.expect(")")
            self.next()
            stmt = self.statement(node)
            node.addChild(stmt)

        elif self.found("else"):
            node.type = self.curr.type
            self.next()
            stmt = self.statement(node)
            node.addChild(stmt)

        elif self.found("while"):
            node.type = self.curr.type
            self.next()
            self.expect("(")
            self.next()
            cond = self.evaluation(node)
            node.addChild(cond)
            self.expect(")")
            self.next()
            stmt = self.statement(node)
            node.addChild(stmt)

        elif self.found("print"):
            node.type = self.curr.type
            self.next()
            mesg = self.evaluation(node)
            node.addChild(mesg)
            self.expect(";")
        
        elif self.found(IDENTIFIER) or self.found(NUMBER) or self.found(STRING) or self.found("("):
            node.type = EVALUATION
            stmt = self.evaluation(node)
            node.addChild(stmt)
            if stmt.type != DEFINITION: self.expect(";")
            if len(node.children) == 1: return stmt
            
        elif self.found("{"):
            self.next()
            bloc = self.block(node)
            self.expect("}")
            return bloc

        elif self.found("return"):
            node.type = self.curr.type
            self.next()
            retn = self.evaluation(node)
            node.addChild(retn)
            self.expect(";")


        elif self.found(END_OF_FILE):
            node.type = END_OF_FILE

        else:
            sys.stderr.write(self.curr.colIndex * "-" + "^\n")
            sys.stderr.write("error: unexpected " + self.curr.__str__()  + "\n")
            RUNNING = False

        return node

    def parse(self):

        self.next()
        node = self.statement(self.root)
        return node




def number(n):
    try: return int(n)
    except ValueError:
        return float(n)

def string(n):
    n = str(n)
    if n[0] in STRING_LITERAL_CHARS:
        return n[1:-1]
    else:
        return n








class Interpreter:

    def __init__(self, parser):

        self.parser = parser
        self.stack = []

    def interpret(self):
        n = self.parser.parse()
        return self.evaluate(n)

    def inScope(self, i):

        for n in range(len(self.stack)):
            if self.stack[n].children[0] == i:
                return n

        return NA

    def evaluate(self, n):
        
        global RUNNING
        sys.stderr.write(str(n))


        if n is None or n.type == END_OF_FILE:
            RUNNING = False;
            return NA;

        elif n.type == NUMBER:
            return n

        elif n.type == STRING:
            return n

        elif n.type == VARIABLE:
            i = self.inScope(n)
            if i == NA:
                sys.stderr.write("error: symbol \"" + n.token.cargo + "\" not found\n")
                return NA
            else:
                return self.stack[i].children[1]

        elif n.type == FUNCTION:
            i = self.inScope(n)
            if i == NA:
                sys.stderr.write("error: function \"" + n.token.cargo + "\" not found\n")
                return NA
            else:

                args = self.stack[i].children[0].children[0].children
                given = n.children[0].children
                if len(given) == len(args):
                    for i in range(len(args)):
                        self.stack.append(self.evaluate(given[i]));

                    ret = self.evaluate(self.stack[i].children[1])
                    return ret

                else:
                    sys.stderr.write("error: function \"" + n.token.cargo + "\" takes " + str(len(args)) + " argument(s)\n")
                    return NA

        elif n.type == ASSIGNMENT:
            i = self.inScope(n.children[0])
            if i == NA:
                self.stack.append(n)
            else:
                self.stack[i].children[1] = n.children[1]

        elif n.type == DEFINITION:
            i = self.inScope(n.children[0])
            if i == NA:
                self.stack.append(n)
            else:
                self.stack[i].children[1] = n.children[1]

        elif n.type == TERM:
            i = 0
            ret = self.evaluate(n.children[i])
            if ret == NA: return NA
            i += 1
            while i < len(n.children):
                op = n.children[i].type
                i += 1
                val = self.evaluate(n.children[i])
                if val == NA: return NA
                i += 1
                if op == "*":
                    ret *= val
                elif op == "/":
                    ret /= val
            return ret

        elif n.type == EXPRESSION:
            i = 0
            ret = self.evaluate(n.children[i])
            if ret == NA: return NA
            i += 1
            while i < len(n.children):
                op = n.children[i].type
                i += 1
                val = self.evaluate(n.children[i])
                if val == NA: return NA
                i += 1
                if op == "+":
                    ret += val
                elif op == "-":
                    ret -= val
            return ret

        elif n.type == CONDITION:
            i = 0
            val1 = self.evaluate(n.children[i])
            if val1 == NA: return NA
            ret  = bool(val1)
            i += 1
            while i < len(n.children):
                op = n.children[i].type
                i += 1
                val2 = self.evaluate(n.children[i])
                if val2 == NA: return NA
                i += 1
                if op == "==":
                    ret = (val1 == val2)
                elif op == "!=":
                    ret = (val1 != val2)
                elif op == ">":
                    ret = (val1 > val2)
                elif op == "<":
                    ret = (val1 < val2)
                elif op == ">=":
                    ret = (val1 >= val2)
                elif op == "<=":
                    ret = (val1 <= val2)
                if not ret: return ret
                val1 = val2
            return ret

        elif n.type == "if":
            cond = bool(self.evaluate(n.children[0]))
            if cond == NA: return NA
            if cond:
                sys.stderr.write(n.children[1])
                self.evaluate(n.children[1])
            else:
                if len(n.children) == 3:
                    self.evaluate(n.children[2])

        elif n.type == "while":
            cond = self.evaluate(n.children[0])
            if cond == NA: return NA
            while bool(cond):
                self.evaluate(n.children[1])
                cond = self.evaluate(n.children[0])
                if cond == NA: return NA

        elif n.type == BLOCK:
            i = 0;
            ret = NA
            while i < len(n.children):
                ret = self.evaluate(n.children[i])
                i += 1
            return ret

        elif n.type == "return":
            return self.evaluate(n.children[0])

        elif n.type == "print":
            mesg = self.evaluate(n.children[0])
            if mesg == NA: return NA
            if mesg is not None:
                sys.stdout.write(string(mesg) + "\n")




















if __name__ == "__main__":

    if sys.stdin.isatty():
        sys.stderr.write(SPLASH)

    f = sys.stdin
    s = Scanner    (f)
    l = Lexer      (s)
    p = Parser     (l)
    m = Interpreter(p)

    while RUNNING:
        m.interpret()

    