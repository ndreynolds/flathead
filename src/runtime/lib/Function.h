// Function.h
// ----------
// Function properties, methods, and prototype

#ifndef JS_FUNC_H
#define JS_FUNC_H

#include "../runtime.h"

js_val * func_new(js_val *, js_args *, eval_state*);

js_val * func_proto_apply(js_val *, js_args *, eval_state*);
js_val * func_proto_bind(js_val *, js_args *, eval_state*);
js_val * func_proto_call(js_val *, js_args *, eval_state*);
js_val * func_proto_is_generator(js_val *, js_args *, eval_state*);
js_val * func_proto_to_string(js_val *, js_args *, eval_state*);

js_val * bootstrap_function(void);

#endif
