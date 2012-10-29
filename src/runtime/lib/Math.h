// Math.h
// ------

#ifndef JS_MATH_H
#define JS_MATH_H

#include "../runtime.h"

JSValue * math_abs(JSValue *, JSArgs *, State *);
JSValue * math_acos(JSValue *, JSArgs *, State *);
JSValue * math_asin(JSValue *, JSArgs *, State *);
JSValue * math_atan(JSValue *, JSArgs *, State *);
JSValue * math_atan2(JSValue *, JSArgs *, State *);
JSValue * math_ceil(JSValue *, JSArgs *, State *);
JSValue * math_cos(JSValue *, JSArgs *, State *);
JSValue * math_exp(JSValue *, JSArgs *, State *);
JSValue * math_floor(JSValue *, JSArgs *, State *);
JSValue * math_log(JSValue *, JSArgs *, State *);
JSValue * math_max(JSValue *, JSArgs *, State *);
JSValue * math_min(JSValue *, JSArgs *, State *);
JSValue * math_pow(JSValue *, JSArgs *, State *);
JSValue * math_random(JSValue *, JSArgs *, State *);
JSValue * math_round(JSValue *, JSArgs *, State *);
JSValue * math_sin(JSValue *, JSArgs *, State *);
JSValue * math_sqrt(JSValue *, JSArgs *, State *);
JSValue * math_tan(JSValue *, JSArgs *, State *);
JSValue * bootstrap_math(void);

#endif
