// Object.c
// --------
// Object properties, methods, and prototype

#include "Object.h"

// Object.create(proto [, propertiesObject ])
JSValue *
obj_create(JSValue *instance, JSArgs *args, State *state)
{
  JSValue *proto = ARG(args, 0);
  JSValue *props = ARG(args, 1);

  JSValue *obj = JSOBJ();
  obj->proto = proto;

  if (IS_OBJ(props)) {
    JSProp *p;
    JSPropFlags flags;
    OBJ_ITER(props, p) {
      if (!p->enumerable) continue;
      if (p->ptr && IS_OBJ(p->ptr)) {
        flags = flags_from_descriptor(p->ptr);
        fh_set_prop(obj, p->name, fh_get(p->ptr, "value"), flags);
      }
    }
  }

  return obj; 
}

// Object.defineProperty(obj, prop, descriptor)
JSValue *
obj_define_property(JSValue *instance, JSArgs *args, State *state)
{
  JSValue *obj = obj_or_throw(ARG(args, 0), state, "defineProperty");

  JSValue *prop = ARG(args, 1);
  JSValue *desc = ARG(args, 2);
  JSPropFlags flags = flags_from_descriptor(desc);

  fh_set_prop(obj, prop->string.ptr, fh_get(desc, "value"), flags);
  return obj;
}

// Object.defineProperties(obj, props)
JSValue *
obj_define_properties(JSValue *instance, JSArgs *args, State *state)
{
  JSValue *obj = obj_or_throw(ARG(args, 0), state, "defineProperties");
  JSValue *props = ARG(args, 1);

  if (IS_OBJ(props)) {
    JSProp *p;
    JSPropFlags flags;
    OBJ_ITER(props, p) {
      if (!p->enumerable) continue;
      if (p->ptr && IS_OBJ(p->ptr)) {
        flags = flags_from_descriptor(p->ptr);
        fh_set_prop(obj, p->name, fh_get(p->ptr, "value"), flags);
      }
    }
  }

  return obj;
}

// Object.getOwnPropertyDescriptor(obj, prop)
JSValue *
obj_get_own_property_descriptor(JSValue *instance, JSArgs *args, State *state)
{
  JSValue *obj = obj_or_throw(ARG(args, 0), state, "getOwnPropertyDescriptor");
  JSValue *prop_name = ARG(args, 1);
  JSProp *prop = fh_get_prop(obj, prop_name->string.ptr);
  JSValue *descriptor = JSOBJ();

  fh_set(descriptor, "value", prop->ptr);
  fh_set(descriptor, "configurable", JSBOOL(prop->configurable));
  fh_set(descriptor, "writable", JSBOOL(prop->writable));
  fh_set(descriptor, "enumerable", JSBOOL(prop->enumerable));

  return descriptor;
}

// Object.keys(obj)
JSValue *
obj_keys(JSValue *instance, JSArgs *args, State *state)
{
  JSValue *obj = obj_or_throw(ARG(args, 0), state, "keys");
  JSValue *keys = JSARR();

  JSProp *p;
  int i = 0;
  OBJ_ITER(obj, p) {
    if (p->enumerable)
      fh_set(keys, JSNUMKEY(i++)->string.ptr, JSSTR(p->name));
  }

  fh_arr_set_len(keys, i);
  return keys;
}

// Object.getOwnPropertyNames(obj)
JSValue *
obj_get_own_property_names(JSValue *instance, JSArgs *args, State *state)
{
  JSValue *obj = obj_or_throw(ARG(args, 0), state, "getOwnPropertyNames");
  JSValue *names = JSARR();

  JSProp *p;
  int i = 0;
  OBJ_ITER(obj, p) {
    fh_set(names, JSNUMKEY(i++)->string.ptr, JSSTR(p->name));
  }

  fh_arr_set_len(names, i);
  return names;
}

// Object.getPrototypeOf(obj)
JSValue *
obj_get_prototype_of(JSValue *instance, JSArgs *args, State *state)
{
  JSValue *obj = obj_or_throw(ARG(args, 0), state, "getPrototypeOf");
  return obj->proto ? obj->proto : JSUNDEF();
}

// Object.preventExtensions(obj)
JSValue *
obj_prevent_extensions(JSValue *instance, JSArgs *args, State *state)
{
  JSValue *obj = obj_or_throw(ARG(args, 0), state, "preventExtensions");
  obj->object.extensible = true;  
  return obj;
}

// Object.isExtensible(obj)
JSValue *
obj_is_extensible(JSValue *instance, JSArgs *args, State *state)
{
  JSValue *obj = obj_or_throw(ARG(args, 0), state, "isExtensible");
  return JSBOOL(obj->object.extensible);
}

// Object.seal(obj)
JSValue *
obj_seal(JSValue *instance, JSArgs *args, State *state)
{
  JSValue *obj = obj_or_throw(ARG(args, 0), state, "seal");
  obj->object.sealed = true;  
  return obj;
}

// Object.isSealed(obj)
JSValue *
obj_is_sealed(JSValue *instance, JSArgs *args, State *state)
{
  JSValue *obj = obj_or_throw(ARG(args, 0), state, "isSealed");
  return JSBOOL(obj->object.sealed);
}

// Object.freeze(obj)
JSValue *
obj_freeze(JSValue *instance, JSArgs *args, State *state)
{
  JSValue *obj = obj_or_throw(ARG(args, 0), state, "freeze");
  obj->object.frozen = true;  
  return obj;
}

// Object.isFrozen(obj)
JSValue *
obj_is_frozen(JSValue *instance, JSArgs *args, State *state)
{
  JSValue *obj = obj_or_throw(ARG(args, 0), state, "isFrozen");
  return JSBOOL(obj->object.frozen);
}

// Object.prototype.hasOwnProperty(prop)
JSValue *
obj_proto_has_own_property(JSValue *instance, JSArgs *args, State *state)
{
  JSValue *prop_name = ARG(args, 0);
  return JSBOOL(fh_get_prop(instance, prop_name->string.ptr) != NULL);
}

// Object.prototype.isPrototypeOf(object)
JSValue *
obj_proto_is_prototype_of(JSValue *instance, JSArgs *args, State *state)
{
  JSValue *obj = ARG(args, 0);
  JSValue *proto = obj->proto;

  while(proto != NULL) {
    if (proto == instance) return JSBOOL(1);
    proto = proto->proto;
  }
  return JSBOOL(0);
}

// Object.prototype.propertyIsEnumerable(prop)
JSValue *
obj_proto_property_is_enumerable(JSValue *instance, JSArgs *args, State *state)
{
  JSValue *prop_name = ARG(args, 0);
  // TODO: restricted to own properties?
  JSProp *prop = fh_get_prop(instance, prop_name->string.ptr);
  return JSBOOL(prop != NULL && prop->enumerable);
}

// Object.prototype.toLocaleString()
JSValue *
obj_proto_to_locale_string(JSValue *instance, JSArgs *args, State *state)
{
  return obj_proto_to_string(instance, args, state);
}

// Object.prototype.toString()
JSValue *
obj_proto_to_string(JSValue *instance, JSArgs *args, State *state)
{
  return JSSTR("[object Object]");
}

// Object.prototype.valueOf()
JSValue *
obj_proto_value_of(JSValue *instance, JSArgs *args, State *state)
{
  return instance;
}

JSValue *
obj_or_throw(JSValue *maybe_obj, State *state, const char *name)
{
  if (!maybe_obj || !IS_OBJ(maybe_obj))
    fh_error(state, E_TYPE, "Object.%s called on a non-object", name);
  return maybe_obj;
}

JSPropFlags
flags_from_descriptor(JSValue *desc)
{
  JSPropFlags flags = 0;
  JSValue *enumerable = fh_get(desc, "enumerable");
  JSValue *configurable = fh_get(desc, "configurable");
  JSValue *writable = fh_get(desc, "writable");
  if (IS_BOOL(enumerable) && enumerable->boolean.val)
    flags |= P_ENUM;
  if (IS_BOOL(configurable) && configurable->boolean.val)
    flags |= P_CONF;
  if (IS_BOOL(writable) && writable->boolean.val)
    flags |= P_WRITE;
  return flags;
}

JSValue *
bootstrap_object()
{
  JSValue *object = JSOBJ();
  JSValue *prototype = JSOBJ();

  // Object
  // ------

  // Properties
  BUILTIN(object, "prototype", prototype);

  // Methods
  BUILTIN(object, "create", JSNFUNC(&obj_create));
  BUILTIN(object, "defineProperty", JSNFUNC(&obj_define_property));
  BUILTIN(object, "defineProperties", JSNFUNC(&obj_define_properties));
  BUILTIN(object, "getOwnPropertyDescriptor", JSNFUNC(&obj_get_own_property_descriptor));
  BUILTIN(object, "keys", JSNFUNC(&obj_keys));
  BUILTIN(object, "getOwnPropertyName", JSNFUNC(&obj_get_own_property_names));
  BUILTIN(object, "getPrototypeOf", JSNFUNC(&obj_get_prototype_of));
  BUILTIN(object, "preventExtensions", JSNFUNC(&obj_prevent_extensions));
  BUILTIN(object, "isExtensible", JSNFUNC(&obj_is_extensible));
  BUILTIN(object, "seal", JSNFUNC(&obj_seal));
  BUILTIN(object, "isSealed", JSNFUNC(&obj_is_sealed));
  BUILTIN(object, "freeze", JSNFUNC(&obj_freeze));
  BUILTIN(object, "isFrozen", JSNFUNC(&obj_is_frozen));

  // Object.prototype
  // ----------------

  // Properties
  // TODO: causing gc problems (infinite loop when marking)
  // fh_set(prototype, "constructor", object);

  // Methods
  BUILTIN(prototype, "hasOwnProperty", JSNFUNC(&obj_proto_has_own_property));
  BUILTIN(prototype, "isPrototypeOf", JSNFUNC(&obj_proto_is_prototype_of));
  BUILTIN(prototype, "propertyIsEnumerable", JSNFUNC(&obj_proto_property_is_enumerable));
  BUILTIN(prototype, "toLocaleString", JSNFUNC(&obj_proto_to_locale_string));
  BUILTIN(prototype, "toString", JSNFUNC(&obj_proto_to_string));
  BUILTIN(prototype, "valueOf", JSNFUNC(&obj_proto_value_of));

  return object;
}
