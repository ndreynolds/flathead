// Array.h
// -------

#include "../runtime.h"

JSValue * arr_new(JSValue *, JSArgs *, State *);
JSValue * arr_is_array(JSValue *, JSArgs *, State *);

JSValue * arr_proto_pop(JSValue *, JSArgs *, State *);
JSValue * arr_proto_push(JSValue *, JSArgs *, State *);
JSValue * arr_proto_reverse(JSValue *, JSArgs *, State *);
JSValue * arr_proto_shift(JSValue *, JSArgs *, State *);
JSValue * arr_proto_sort(JSValue *, JSArgs *, State *);
JSValue * arr_proto_splice(JSValue *, JSArgs *, State *);
JSValue * arr_proto_unshift(JSValue *, JSArgs *, State *);

JSValue * arr_proto_concat(JSValue *, JSArgs *, State *);
JSValue * arr_proto_join(JSValue *, JSArgs *, State *);
JSValue * arr_proto_slice(JSValue *, JSArgs *, State *);
JSValue * arr_proto_to_string(JSValue *, JSArgs *, State *);
JSValue * arr_proto_index_of(JSValue *, JSArgs *, State *);
JSValue * arr_proto_last_index_of(JSValue *, JSArgs *, State *);

JSValue * arr_proto_filter(JSValue *, JSArgs *, State *);
JSValue * arr_proto_for_each(JSValue *, JSArgs *, State *);
JSValue * arr_proto_every(JSValue *, JSArgs *, State *);
JSValue * arr_proto_map(JSValue *, JSArgs *, State *);
JSValue * arr_proto_some(JSValue *, JSArgs *, State *);
JSValue * arr_proto_reduce(JSValue *, JSArgs *, State *);
JSValue * arr_proto_reduce_right(JSValue *, JSArgs *, State *);

void arr_merge_sort(JSProp **, int);
void arr_merge(JSProp **, JSProp **, int, int, JSProp **);
void arr_recur(JSProp **, JSProp **, int);
int arr_cmp(JSProp *, JSProp *);
int arr_cmp_js(JSProp *, JSProp *);
int arr_lex_sort(JSProp *, JSProp *);
int arr_key_sort(JSProp *, JSProp *);
JSValue * arr_do_join(JSValue *, JSValue *);

JSValue * bootstrap_array(void);
