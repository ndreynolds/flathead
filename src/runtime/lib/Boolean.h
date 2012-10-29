// Boolean.h
// ---------
// Boolean properties, methods, and prototype

#ifndef JS_BOOL_H
#define JS_BOOL_H

#include "../runtime.h"

JSValue * bool_proto_to_string(JSValue *, JSArgs *, State *);
JSValue * bool_proto_value_of(JSValue *, JSArgs *, State *);

JSValue * bootstrap_boolean(void);

#endif
