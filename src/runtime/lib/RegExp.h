// RegExp.h
// --------

#include "../runtime.h"

JSValue * regexp_new(JSValue *, JSArgs *, State *);
JSValue * regexp_proto_exec(JSValue *, JSArgs *, State *);
JSValue * regexp_proto_test(JSValue *, JSArgs *, State *);
JSValue * regexp_proto_to_string(JSValue *, JSArgs *, State *);

JSValue * bootstrap_regexp(void);
