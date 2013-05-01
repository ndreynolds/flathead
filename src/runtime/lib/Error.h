// Error.h
// -------

#ifndef JS_ERROR_H
#define JS_ERROR_H

#include "../runtime.h"

js_val * error_new(js_val *, js_args *, eval_state *);
js_val * error_eval_new(js_val *, js_args *, eval_state *);
js_val * error_range_new(js_val *, js_args *, eval_state *);
js_val * error_ref_new(js_val *, js_args *, eval_state *);
js_val * error_syntax_new(js_val *, js_args *, eval_state *);
js_val * error_type_new(js_val *, js_args *, eval_state *);
js_val * error_uri_new(js_val *, js_args *, eval_state *);

js_val * error_proto_to_string(js_val *, js_args *, eval_state *);

js_val * bootstrap_error(js_val *);

#endif
