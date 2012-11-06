// Number.h
// --------

#ifndef JS_NUM_H
#define JS_NUM_H

#include "../runtime.h"

js_val * number_new(js_val *, js_args *, eval_state *);

js_val * number_proto_to_exponential(js_val *, js_args *, eval_state *);
js_val * number_proto_to_fixed(js_val *, js_args *, eval_state *);
js_val * number_proto_to_locale_string(js_val *, js_args *, eval_state *);
js_val * number_proto_to_precision(js_val *, js_args *, eval_state *);
js_val * number_proto_to_string(js_val *, js_args *, eval_state *);
js_val * number_proto_value_of(js_val *, js_args *, eval_state *);

js_val * bootstrap_number(void);

#endif
