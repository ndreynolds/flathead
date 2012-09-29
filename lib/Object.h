// Object.h
// --------

#include "../src/flathead.h"
#include "../src/props.h"

JSValue * obj_create(JSValue *, JSArgs *, State *);
JSValue * obj_define_property(JSValue *, JSArgs *, State *);
JSValue * obj_define_properties(JSValue *, JSArgs *, State *);
JSValue * obj_get_own_property_descriptor(JSValue *, JSArgs *, State *);
JSValue * obj_keys(JSValue *, JSArgs *, State *);
JSValue * obj_get_own_property_names(JSValue *, JSArgs *, State *);
JSValue * obj_get_prototype_of(JSValue *, JSArgs *, State *);
JSValue * obj_prevent_extensions(JSValue *, JSArgs *, State *);
JSValue * obj_is_extensible(JSValue *, JSArgs *, State *);
JSValue * obj_seal(JSValue *, JSArgs *, State *);
JSValue * obj_is_sealed(JSValue *, JSArgs *, State *);
JSValue * obj_freeze(JSValue *, JSArgs *, State *);
JSValue * obj_is_frozen(JSValue *, JSArgs *, State *);

JSValue * obj_proto_has_own_property(JSValue *, JSArgs *, State *);
JSValue * obj_proto_is_prototype_of(JSValue *, JSArgs *, State *);
JSValue * obj_proto_property_is_enumerable(JSValue *, JSArgs *, State *);
JSValue * obj_proto_to_locale_string(JSValue *, JSArgs *, State *);
JSValue * obj_proto_to_string(JSValue *, JSArgs *, State *);
JSValue * obj_proto_value_of(JSValue *, JSArgs *, State *);

JSValue * obj_or_throw(JSValue *, State *, const char *);
JSPropFlags flags_from_descriptor(JSValue *);
JSValue * bootstrap_object(void);
