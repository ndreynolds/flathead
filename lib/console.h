// console.h
// ---------

#include "../src/flathead.h"

JSValue * console_log(JSArgs *, State *);
JSValue * console_error(JSArgs *, State *);
JSValue * console_assert(JSArgs *, State *);
JSValue * console_time(JSArgs *, State *);
void log_args(FILE *, JSArgs *);
JSValue * bootstrap_console(void);
