// console.h
// ---------

#ifndef JS_CONSOLE_H
#define JS_CONSOLE_H

#include "../runtime.h"

js_val * console_log(js_val *, js_args *, eval_state *);
js_val * console_error(js_val *, js_args *, eval_state *);
js_val * console_info(js_val *, js_args *, eval_state *);
js_val * console_assert(js_val *, js_args *, eval_state *);
js_val * console_time(js_val *, js_args *, eval_state *);

void log_args(FILE *, js_args *);
js_val * bootstrap_console(void);

#endif
