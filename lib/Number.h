// Number.h
// --------

#include "../src/flathead.h"
#include "../src/props.h"

JSValue * number_proto_to_exponential(JSValue *, JSArgs *, State *);
JSValue * number_proto_to_fixed(JSValue *, JSArgs *, State *);
JSValue * number_proto_to_locale_string(JSValue *, JSArgs *, State *);
JSValue * number_proto_to_precision(JSValue *, JSArgs *, State *);
JSValue * number_proto_to_string(JSValue *, JSArgs *, State *);
JSValue * number_proto_value_of(JSValue *, JSArgs *, State *);

JSValue * bootstrap_number(void);
