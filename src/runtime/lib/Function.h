// Function.h
// ----------
// Function properties, methods, and prototype

#ifndef JS_FUNC_H
#define JS_FUNC_H

#include "../runtime.h"

JSValue * func_new(JSValue *, JSArgs *, State *);

JSValue * func_proto_apply(JSValue *, JSArgs *, State *);
JSValue * func_proto_bind(JSValue *, JSArgs *, State *);
JSValue * func_proto_call(JSValue *, JSArgs *, State *);
JSValue * func_proto_is_generator(JSValue *, JSArgs *, State *);
JSValue * func_proto_to_string(JSValue *, JSArgs *, State *);

JSValue * bootstrap_function(void);

#endif
