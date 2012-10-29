// console.h
// ---------

#ifndef JS_CONSOLE_H
#define JS_CONSOLE_H

#include "../runtime.h"

JSValue * console_log(JSValue *, JSArgs *, State *);
JSValue * console_error(JSValue *, JSArgs *, State *);
JSValue * console_assert(JSValue *, JSArgs *, State *);
JSValue * console_time(JSValue *, JSArgs *, State *);

void log_args(FILE *, JSArgs *);
JSValue * bootstrap_console(void);

#endif
