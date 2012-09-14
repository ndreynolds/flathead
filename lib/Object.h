// Object.h
// --------

#include "../src/flathead.h"

JSValue * obj_create(JSArgs *, State *);
JSValue * obj_define_property(JSArgs *, State *);
JSValue * obj_define_properties(JSArgs *, State *);
JSValue * obj_get_own_property_descriptor(JSArgs *, State *);
JSValue * obj_keys(JSArgs *, State *);
JSValue * obj_get_own_property_names(JSArgs *, State *);
JSValue * obj_get_prototype_of(JSArgs *, State *);
JSValue * obj_prevent_extensions(JSArgs *, State *);
JSValue * obj_is_extensible(JSArgs *, State *);
JSValue * obj_seal(JSArgs *, State *);
JSValue * obj_is_sealed(JSArgs *, State *);
JSValue * obj_freeze(JSArgs *, State *);
JSValue * obj_is_frozen(JSArgs *, State *);

JSValue * obj_proto_has_own_property(JSArgs *, State *);
JSValue * obj_proto_is_prototype_of(JSArgs *, State *);
JSValue * obj_proto_property_is_enumerable(JSArgs *, State *);
JSValue * obj_proto_to_locale_string(JSArgs *, State *);
JSValue * obj_proto_to_string(JSArgs *, State *);
JSValue * obj_proto_value_of(JSArgs *, State *);

JSValue * bootstrap_object(void);
