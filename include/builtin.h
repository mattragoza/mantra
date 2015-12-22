#ifndef BUILTIN_H
#define BUILTIN_H

#include "common.h"
#include "error.h"
#include "object.h"
#include "interpreter.h"

Object *Builtin_set(ListObject *arg, ListObject *context);
Object *Builtin_add(ListObject *arg, ListObject *context);
Object *Builtin_subtract(ListObject *arg, ListObject *context);

#endif //BUILTIN_H