// console.h
// ---------

#include "../src/flathead.h"

JSVALUE * console_log(JSARGS *);
JSVALUE * console_error(JSARGS *);
JSVALUE * console_time(JSARGS *);
JSVALUE * console_assert(JSARGS *);
JSVALUE * bootstrap_console(void);
