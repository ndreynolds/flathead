// Object.h
// --------

#ifndef JS_OBJ_H
#define JS_OBJ_H

#include "../runtime.h"

js_val * obj_create(js_val *, js_args *, eval_state *);
js_val * obj_define_property(js_val *, js_args *, eval_state *);
js_val * obj_define_properties(js_val *, js_args *, eval_state *);
js_val * obj_get_own_property_descriptor(js_val *, js_args *, eval_state *);
js_val * obj_keys(js_val *, js_args *, eval_state *);
js_val * obj_get_own_property_names(js_val *, js_args *, eval_state *);
js_val * obj_get_prototype_of(js_val *, js_args *, eval_state *);
js_val * obj_prevent_extensions(js_val *, js_args *, eval_state *);
js_val * obj_is_extensible(js_val *, js_args *, eval_state *);
js_val * obj_seal(js_val *, js_args *, eval_state *);
js_val * obj_is_sealed(js_val *, js_args *, eval_state *);
js_val * obj_freeze(js_val *, js_args *, eval_state *);
js_val * obj_is_frozen(js_val *, js_args *, eval_state *);

js_val * obj_proto_has_own_property(js_val *, js_args *, eval_state *);
js_val * obj_proto_is_prototype_of(js_val *, js_args *, eval_state *);
js_val * obj_proto_property_is_enumerable(js_val *, js_args *, eval_state *);
js_val * obj_proto_to_locale_string(js_val *, js_args *, eval_state *);
js_val * obj_proto_to_string(js_val *, js_args *, eval_state *);
js_val * obj_proto_value_of(js_val *, js_args *, eval_state *);

js_val * obj_or_throw(js_val *, eval_state *, const char *);
js_prop_flags flags_from_descriptor(js_val *);
js_val * bootstrap_object(void);

#endif
