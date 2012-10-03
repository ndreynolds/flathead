// String.h
// --------
// String properties, methods, and prototype

#include "../runtime.h"

JSValue * str_new(JSValue *, JSArgs *, State *);
JSValue * str_from_char_code(JSValue *, JSArgs *, State *);

JSValue * str_proto_char_at(JSValue *, JSArgs *, State *);
JSValue * str_proto_char_code_at(JSValue *, JSArgs *, State *);
JSValue * str_proto_concat(JSValue *, JSArgs *, State *);
JSValue * str_proto_index_of(JSValue *, JSArgs *, State *);
JSValue * str_proto_last_index_of(JSValue *, JSArgs *, State *);
JSValue * str_proto_locale_compare(JSValue *, JSArgs *, State *);
JSValue * str_proto_match(JSValue *, JSArgs *, State *);
JSValue * str_proto_quote(JSValue *, JSArgs *, State *);
JSValue * str_proto_replace(JSValue *, JSArgs *, State *);
JSValue * str_proto_search(JSValue *, JSArgs *, State *);
JSValue * str_proto_slice(JSValue *, JSArgs *, State *);
JSValue * str_proto_split(JSValue *, JSArgs *, State *);
JSValue * str_proto_substr(JSValue *, JSArgs *, State *);
JSValue * str_proto_substring(JSValue *, JSArgs *, State *);
JSValue * str_proto_to_locale_lower_case(JSValue *, JSArgs *, State *);
JSValue * str_proto_to_locale_upper_case(JSValue *, JSArgs *, State *);
JSValue * str_proto_to_lower_case(JSValue *, JSArgs *, State *);
JSValue * str_proto_to_string(JSValue *, JSArgs *, State *);
JSValue * str_proto_to_upper_case(JSValue *, JSArgs *, State *);
JSValue * str_proto_trim(JSValue *, JSArgs *, State *);
JSValue * str_proto_trim_left(JSValue *, JSArgs *, State *);
JSValue * str_proto_trim_right(JSValue *, JSArgs *, State *);
JSValue * str_proto_value_of(JSValue *, JSArgs *, State *);

JSValue * bootstrap_string(void);
