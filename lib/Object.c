// Object.c
// --------
// Object properties, methods, and prototype

#include "Object.h"

// Object.create(proto [, propertiesObject ])
JSValue *
obj_create(JSArgs *args, State *state)
{
  // TODO: stub
}

// Object.defineProperty(obj, prop, descriptor)
JSValue *
obj_define_property(JSArgs *args, State *state)
{
  // TODO: stub
}

// Object.defineProperties(obj, props)
JSValue *
obj_define_properties(JSArgs *args, State *state)
{
  // TODO: stub
}

// Object.getOwnPropertyDescriptor(obj, prop)
JSValue *
obj_get_own_property_descriptor(JSArgs *args, State *state)
{
  // TODO: stub
}

// Object.keys(obj)
JSValue *
obj_keys(JSArgs *args, State *state)
{
  // TODO: stub
}

// Object.getOwnPropertyNames(obj)
JSValue *
obj_get_own_property_names(JSArgs *args, State *state)
{
  // TODO: stub
}

// Object.getPrototypeOf(obj)
JSValue *
obj_get_prototype_of(JSArgs *args, State *state)
{
  // TODO: stub
}

// Object.preventExtensions(obj)
JSValue *
obj_prevent_extensions(JSArgs *args, State *state)
{
  // TODO: stub
}

// Object.isExtensible(obj)
JSValue *
obj_is_extensible(JSArgs *args, State *state)
{
  // TODO: stub
}

// Object.seal(obj)
JSValue *
obj_seal(JSArgs *args, State *state)
{
  // TODO: stub
}

// Object.isSealed(obj)
JSValue *
obj_is_sealed(JSArgs *args, State *state)
{
  // TODO: stub
}

// Object.freeze(obj)
JSValue *
obj_freeze(JSArgs *args, State *state)
{
  // TODO: stub
}

// Object.isFrozen(obj)
JSValue *
obj_is_frozen(JSArgs *args, State *state)
{
  // TODO: stub
}

// Object.prototype.hasOwnProperty(prop)
JSValue *
obj_proto_has_own_property(JSArgs *args, State *state)
{
  // TODO: stub
}

// Object.prototype.isPrototypeOf(object)
JSValue *
obj_proto_is_prototype_of(JSArgs *args, State *state)
{
  // TODO: stub
}

// Object.prototype.propertyIsEnumerable(prop)
JSValue *
obj_proto_property_is_enumerable(JSArgs *args, State *state)
{
  // TODO: stub
}

// Object.prototype.toLocaleString()
JSValue *
obj_proto_to_locale_string(JSArgs *args, State *state)
{
  // TODO: stub
}

// Object.prototype.toString()
JSValue *
obj_proto_to_string(JSArgs *args, State *state)
{
  // TODO: stub
}

// Object.prototype.valueOf()
JSValue *
obj_proto_value_of(JSArgs *args, State *state)
{
  // TODO: stub
}

JSValue *
bootstrap_object()
{
  JSValue *object = JSOBJ();
  JSValue *prototype = JSOBJ();

  // Object
  // ------

  // Properties
  fh_set(object, "prototype", prototype);

  // Methods
  fh_set(object, "create", JSNFUNC(&obj_create));
  fh_set(object, "defineProperty", JSNFUNC(&obj_define_property));
  fh_set(object, "defineProperties", JSNFUNC(&obj_define_properties));
  fh_set(object, "getOwnPropertyDescriptor", JSNFUNC(&obj_get_own_property_descriptor));
  fh_set(object, "keys", JSNFUNC(&obj_keys));
  fh_set(object, "getOwnPropertyName", JSNFUNC(&obj_get_own_property_names));
  fh_set(object, "getPrototypeOf", JSNFUNC(&obj_get_prototype_of));
  fh_set(object, "preventExtensions", JSNFUNC(&obj_prevent_extensions));
  fh_set(object, "isExtensible", JSNFUNC(&obj_is_extensible));
  fh_set(object, "seal", JSNFUNC(&obj_seal));
  fh_set(object, "isSealed", JSNFUNC(&obj_is_sealed));
  fh_set(object, "freeze", JSNFUNC(&obj_freeze));
  fh_set(object, "isFrozen", JSNFUNC(&obj_is_frozen));

  // Object.prototype
  // ----------------

  // Properties
  // TODO: causing gc problems (infinite loop when marking)
  // fh_set(prototype, "constructor", object);

  // Methods
  fh_set(prototype, "hasOwnProperty", JSNFUNC(&obj_proto_has_own_property));
  fh_set(prototype, "isPrototypeOf", JSNFUNC(&obj_proto_is_prototype_of));
  fh_set(prototype, "propertyIsEnumerable", JSNFUNC(&obj_proto_property_is_enumerable));
  fh_set(prototype, "toLocaleString", JSNFUNC(&obj_proto_to_locale_string));
  fh_set(prototype, "toString", JSNFUNC(&obj_proto_to_string));
  fh_set(prototype, "valueOf", JSNFUNC(&obj_proto_value_of));

  return object;
}
