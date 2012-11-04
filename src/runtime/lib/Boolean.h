// Boolean.h
// ---------
// Boolean properties, methods, and prototype

#ifndef JS_BOOL_H
#define JS_BOOL_H

#include "../runtime.h"

js_val * bool_proto_to_string(js_val *, js_args *, eval_state*);
js_val * bool_proto_value_of(js_val *, js_args *, eval_state*);

js_val * bootstrap_boolean(void);

#endif
