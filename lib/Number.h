// Number.h
// --------

#include "../src/flathead.h"

JSValue * number_proto_to_exponential(JSArgs *, State *);
JSValue * number_proto_to_fixed(JSArgs *, State *);
JSValue * number_proto_to_locale_string(JSArgs *, State *);
JSValue * number_proto_to_precision(JSArgs *, State *);
JSValue * number_proto_to_string(JSArgs *, State *);
JSValue * number_proto_value_of(JSArgs *, State *);

JSValue * bootstrap_number(void);
