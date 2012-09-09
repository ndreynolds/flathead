// console.h
// ---------

#include "../src/flathead.h"

JSValue * console_log(JSArgs *, State *);
JSValue * console_error(JSArgs *, State *);
JSValue * console_assert(JSArgs *, State *);
JSValue * console_time(JSArgs *, State *);
JSValue * bootstrap_console(void);
