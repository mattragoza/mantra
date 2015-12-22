#ifndef OBJECT_H
#define OBJECT_H

#include "common.h"
#include "error.h"
#include "lexer.h"

#define DEFAULT_LIST_CAP 4

typedef enum 
{
	SYMBOL_OBJECT,
	NUMBER_OBJECT,
	STRING_OBJECT,
	LIST_OBJECT,
	FUNCTION_OBJECT

} ObjectType;
char * OBJECT_TYPES[5];

#define Object_HEADER \
	int link_cnt;\
	ObjectType type;

typedef struct object_t
{
	Object_HEADER

} Object;

typedef struct symbol_object_t
{
	Object_HEADER;

	int len, cap;
	char *buffer;	// the symbol itself
	Object *link;	// the object it refers to

	int eval_mod;
	// how deeply to evaluate
	// 0 = use the symbol itself
	// 1 = use the object it refers to
	// 2 = 

} SymbolObject;

typedef struct number_object_t
{
	Object_HEADER
	double value;

} NumberObject;

typedef struct string_object_t
{
	Object_HEADER
	int len, cap;
	char *buffer;

} StringObject;

typedef struct list_object_t 
{
	Object_HEADER
	int len, cap;
	Object **arr;

} ListObject;

typedef Object *(c_func)(ListObject *, ListObject *);
typedef struct function_object_t 
{
	Object_HEADER
	c_func *call;

} FunctionObject;

SymbolObject *new_SymbolObject(char *buffer, int deref);
NumberObject *new_NumberObject(double value);		
StringObject *new_StringObject(char *buffer);
ListObject *new_ListObject(int cap);
FunctionObject *new_FunctionObject(c_func *call);

int Object_istype(Object *self, ObjectType type);
char *Object_tostring(Object *self);
void Object_printtree(Object *self, int depth);
void del_Object(Object *self);

int SymbolObject_hash(SymbolObject *);
void SymbolObject_link(SymbolObject *self, Object *object);

void StringObject_concat(StringObject *a, StringObject *b);

void ListObject_append(ListObject *self, Object *elem);
void ListObject_set(ListObject *self, int index, Object *elem);
Object *ListObject_get(ListObject *self, int index);
Object *ListObject_pop(ListObject *self, int index);

// macros for casting between object pointer types
#define OBJ(x) ((Object *) x)
#define SYM(x) ((SymbolObject *) x)
#define NUM(x) ((NumberObject *) x)
#define STR(x) ((StringObject *) x)
#define LST(x) ((ListObject *) x)
#define FUN(x) ((FunctionObject *) x)

#endif //OBJECT_H