// Math.h
// ------

#ifndef JS_MATH_H
#define JS_MATH_H

#include "../runtime.h"

js_val * math_abs(js_val *, js_args *, eval_state *);
js_val * math_acos(js_val *, js_args *, eval_state *);
js_val * math_asin(js_val *, js_args *, eval_state *);
js_val * math_atan(js_val *, js_args *, eval_state *);
js_val * math_atan2(js_val *, js_args *, eval_state *);
js_val * math_ceil(js_val *, js_args *, eval_state *);
js_val * math_cos(js_val *, js_args *, eval_state *);
js_val * math_exp(js_val *, js_args *, eval_state *);
js_val * math_floor(js_val *, js_args *, eval_state *);
js_val * math_log(js_val *, js_args *, eval_state *);
js_val * math_max(js_val *, js_args *, eval_state *);
js_val * math_min(js_val *, js_args *, eval_state *);
js_val * math_pow(js_val *, js_args *, eval_state *);
js_val * math_random(js_val *, js_args *, eval_state *);
js_val * math_round(js_val *, js_args *, eval_state *);
js_val * math_sin(js_val *, js_args *, eval_state *);
js_val * math_sqrt(js_val *, js_args *, eval_state *);
js_val * math_tan(js_val *, js_args *, eval_state *);
js_val * bootstrap_math(void);

#endif
