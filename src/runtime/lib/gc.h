// gc.h
// ----

#ifndef JS_GC_H
#define JS_GC_H

#include "../runtime.h"

js_val * gc_run(js_val *, js_args *, eval_state *);
js_val * gc_info(js_val *, js_args *, eval_state *);
js_val * gc_spy(js_val *, js_args *, eval_state *);

js_val * bootstrap_gc(void);

#endif
