
#The Mantra Language

The syntax is based off of lisp. All data and code are represented as objects, which can be symbols, strings, numbers, lists, or functions. A symbol object links a name to another object. A list object dynamically references multiple other objects. The code statements themselves are constructed as lists, and so there is little difference between code and data.

Lists are essential to the syntax and data structure of the language. A list takes the format:

	(1 2 3 4)

Where each element should be separated by whitespace. Lists can also contain other lists:

	("abc" ("hello, world"))

And probably most importantly, any list that contains a function object as an element is treated as a functional call, with the other objects as its arguments:

	(\x := 10)

The same function can be present in the link any number of times, which just adds readability in some cases:

	(10 - 4 - 20) instead of (- 10 4 20)

Objects can be treated different depending on the presence of evaluation modifiers. \ is the escape modifier, meaning "evaluate the next object less", and # is the evaluate modifier, meaning "evaluate the next object more". You can apply these before any object but they don't have any effect on atomic objects like strings and numbers. Applying them to a list object distributes the modifier onto each of its elements.

For symbols, the escape modifier tells the interpreter to return the symbol itself, rather than to dereference it to its linked object. This is useful for passing the symbol to binding functions, for example assigning the symbol to a new object- known as linking.

The evaluate object does the opposite- if an object was escaped, the evaluate modifier tells the interpreter to evaluate it.

Evaluate and escape modifiers can be stacked for more complex effects. For example, linking a symbol to another symbol- evaluating the symbol once would return the symbol it was linked to, but evaluating it with two modifiers would return the objet linked to the symbol the first symbol was linked to.

User-implemented functions are currently under contruction.
