// console.h
// ---------

#include "../src/jslite.h"

JLVALUE * console_log(JLARGS *);
JLVALUE * console_error(JLARGS *);
JLVALUE * console_time(JLARGS *);
JLVALUE * console_assert(JLARGS *);
JLVALUE * bootstrap_console(void);
