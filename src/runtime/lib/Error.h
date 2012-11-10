// Error.h
// -------

#ifndef JS_ERROR_H
#define JS_ERROR_H

#include "../runtime.h"

js_val * error_new(js_val *, js_args *, eval_state *);

js_val * bootstrap_error(void);

#endif
