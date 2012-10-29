// Number.h
// --------

#ifndef JS_NUM_H
#define JS_NUM_H

#include "../runtime.h"

JSValue * number_new(JSValue *, JSArgs *, State *);

JSValue * number_proto_to_exponential(JSValue *, JSArgs *, State *);
JSValue * number_proto_to_fixed(JSValue *, JSArgs *, State *);
JSValue * number_proto_to_locale_string(JSValue *, JSArgs *, State *);
JSValue * number_proto_to_precision(JSValue *, JSArgs *, State *);
JSValue * number_proto_to_string(JSValue *, JSArgs *, State *);
JSValue * number_proto_value_of(JSValue *, JSArgs *, State *);

JSValue * bootstrap_number(void);

#endif
