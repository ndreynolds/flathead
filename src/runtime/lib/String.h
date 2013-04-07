// String.h
// --------
// String properties, methods, and prototype

#ifndef JS_STRING_H
#define JS_STRING_H

#include "../runtime.h"

js_val * str_new(js_val *, js_args *, eval_state *);
js_val * str_from_char_code(js_val *, js_args *, eval_state *);

js_val * str_proto_char_at(js_val *, js_args *, eval_state *);
js_val * str_proto_char_code_at(js_val *, js_args *, eval_state *);
js_val * str_proto_concat(js_val *, js_args *, eval_state *);
js_val * str_proto_index_of(js_val *, js_args *, eval_state *);
js_val * str_proto_last_index_of(js_val *, js_args *, eval_state *);
js_val * str_proto_locale_compare(js_val *, js_args *, eval_state *);
js_val * str_proto_match(js_val *, js_args *, eval_state *);
js_val * str_proto_replace(js_val *, js_args *, eval_state *);
js_val * str_proto_search(js_val *, js_args *, eval_state *);
js_val * str_proto_slice(js_val *, js_args *, eval_state *);
js_val * str_proto_split(js_val *, js_args *, eval_state *);
js_val * str_proto_substr(js_val *, js_args *, eval_state *);
js_val * str_proto_substring(js_val *, js_args *, eval_state *);
js_val * str_proto_to_locale_lower_case(js_val *, js_args *, eval_state *);
js_val * str_proto_to_locale_upper_case(js_val *, js_args *, eval_state *);
js_val * str_proto_to_lower_case(js_val *, js_args *, eval_state *);
js_val * str_proto_to_string(js_val *, js_args *, eval_state *);
js_val * str_proto_to_upper_case(js_val *, js_args *, eval_state *);
js_val * str_proto_trim(js_val *, js_args *, eval_state *);
js_val * str_proto_trim_left(js_val *, js_args *, eval_state *);
js_val * str_proto_trim_right(js_val *, js_args *, eval_state *);
js_val * str_proto_value_of(js_val *, js_args *, eval_state *);

char * str_splice(char *, char *, int, int);
js_val * str_regexp_splitter(char *, js_val *, int);

js_val * bootstrap_string(void);

#endif
