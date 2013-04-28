// Object.c
// --------
// Object properties, methods, and prototype

#include "Object.h"

static js_prop_flags flags_from_descriptor(js_val *desc);
static js_val * obj_or_throw(js_val *, eval_state *, const char *);


// new Object([value])
js_val *
obj_new(js_val *instance, js_args *args, eval_state *state)
{
  js_val *value = ARG(args, 0);
  js_val *obj = state->construct ? state->this : JSOBJ();

  if (IS_OBJ(value)) return value;
  if (IS_STR(value) || IS_BOOL(value) || IS_NUM(value)) {
    js_val *res = TO_OBJ(value);
    res->proto = value->proto;
    return res;
  }
  return obj;
}

// Object.create(proto [, propertiesObject ])
js_val *
obj_create(js_val *instance, js_args *args, eval_state *state)
{
  js_val *proto = ARG(args, 0);
  js_val *props = ARG(args, 1);

  js_val *obj = JSOBJ();
  obj->proto = proto;

  if (IS_OBJ(props)) {
    js_prop *p;
    js_prop_flags flags;
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
js_val *
obj_define_property(js_val *instance, js_args *args, eval_state *state)
{
  js_val *obj = obj_or_throw(ARG(args, 0), state, "defineProperty");

  js_val *prop = ARG(args, 1);
  js_val *desc = ARG(args, 2);
  js_prop_flags flags = flags_from_descriptor(desc);

  fh_set_prop(obj, prop->string.ptr, fh_get(desc, "value"), flags);
  return obj;
}

// Object.defineProperties(obj, props)
js_val *
obj_define_properties(js_val *instance, js_args *args, eval_state *state)
{
  js_val *obj = obj_or_throw(ARG(args, 0), state, "defineProperties");
  js_val *props = ARG(args, 1);

  if (IS_OBJ(props)) {
    js_prop *p;
    js_prop_flags flags;
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
js_val *
obj_get_own_property_descriptor(js_val *instance, js_args *args, eval_state *state)
{
  js_val *obj = obj_or_throw(ARG(args, 0), state, "getOwnPropertyDescriptor");
  js_val *prop_name = ARG(args, 1);
  js_prop *prop = fh_get_prop(obj, prop_name->string.ptr);
  js_val *descriptor = JSOBJ();

  fh_set(descriptor, "value", prop->ptr);
  fh_set(descriptor, "configurable", JSBOOL(prop->configurable));
  fh_set(descriptor, "writable", JSBOOL(prop->writable));
  fh_set(descriptor, "enumerable", JSBOOL(prop->enumerable));

  return descriptor;
}

// Object.keys(obj)
js_val *
obj_keys(js_val *instance, js_args *args, eval_state *state)
{
  js_val *obj = obj_or_throw(ARG(args, 0), state, "keys");
  js_val *keys = JSARR();

  js_prop *p;
  int i = 0;
  OBJ_ITER(obj, p) {
    if (p->enumerable)
      fh_set(keys, JSNUMKEY(i++)->string.ptr, JSSTR(p->name));
  }

  fh_set_len(keys, i);
  return keys;
}

// Object.getOwnPropertyNames(obj)
js_val *
obj_get_own_property_names(js_val *instance, js_args *args, eval_state *state)
{
  js_val *obj = obj_or_throw(ARG(args, 0), state, "getOwnPropertyNames");
  js_val *names = JSARR();

  js_prop *p;
  int i = 0;
  OBJ_ITER(obj, p) {
    fh_set(names, JSNUMKEY(i++)->string.ptr, JSSTR(p->name));
  }

  fh_set_len(names, i);
  return names;
}

// Object.getPrototypeOf(obj)
js_val *
obj_get_prototype_of(js_val *instance, js_args *args, eval_state *state)
{
  js_val *obj = obj_or_throw(ARG(args, 0), state, "getPrototypeOf");
  return obj->proto ? obj->proto : JSUNDEF();
}

// Object.preventExtensions(obj)
js_val *
obj_prevent_extensions(js_val *instance, js_args *args, eval_state *state)
{
  js_val *obj = obj_or_throw(ARG(args, 0), state, "preventExtensions");
  obj->object.extensible = true;  
  return obj;
}

// Object.isExtensible(obj)
js_val *
obj_is_extensible(js_val *instance, js_args *args, eval_state *state)
{
  js_val *obj = obj_or_throw(ARG(args, 0), state, "isExtensible");
  return JSBOOL(obj->object.extensible);
}

// Object.seal(obj)
js_val *
obj_seal(js_val *instance, js_args *args, eval_state *state)
{
  js_val *obj = obj_or_throw(ARG(args, 0), state, "seal");
  js_prop *prop;
  OBJ_ITER(obj, prop) {
    prop->configurable = false;
  }
  obj->object.extensible = false;  
  return obj;
}

// Object.isSealed(obj)
js_val *
obj_is_sealed(js_val *instance, js_args *args, eval_state *state)
{
  js_val *obj = obj_or_throw(ARG(args, 0), state, "isSealed");
  js_prop *prop;
  OBJ_ITER(obj, prop) {
    if (prop->configurable) return JSBOOL(0);
  }
  return JSBOOL(!obj->object.extensible);
}

// Object.freeze(obj)
js_val *
obj_freeze(js_val *instance, js_args *args, eval_state *state)
{
  js_val *obj = obj_or_throw(ARG(args, 0), state, "freeze");
  js_prop *prop;
  OBJ_ITER(obj, prop) {
    prop->configurable = false;
    prop->writable = false;
  }
  obj->object.extensible = false;  
  return obj;
}

// Object.isFrozen(obj)
js_val *
obj_is_frozen(js_val *instance, js_args *args, eval_state *state)
{
  js_val *obj = obj_or_throw(ARG(args, 0), state, "isFrozen");
  js_prop *prop;
  OBJ_ITER(obj, prop) {
    if (prop->configurable) return JSBOOL(0);
    if (prop->writable) return JSBOOL(0);
  }
  return JSBOOL(!obj->object.extensible);
}

// Object.prototype.hasOwnProperty(prop)
js_val *
obj_proto_has_own_property(js_val *instance, js_args *args, eval_state *state)
{
  js_val *prop_name = ARG(args, 0);
  return JSBOOL(fh_get_prop(instance, prop_name->string.ptr) != NULL);
}

// Object.prototype.isPrototypeOf(object)
js_val *
obj_proto_is_prototype_of(js_val *instance, js_args *args, eval_state *state)
{
  js_val *obj = ARG(args, 0);
  js_val *proto = obj->proto;

  while (proto != NULL) {
    if (proto == instance) return JSBOOL(1);
    proto = proto->proto;
  }
  return JSBOOL(0);
}

// Object.prototype.propertyIsEnumerable(prop)
js_val *
obj_proto_property_is_enumerable(js_val *instance, js_args *args, eval_state *state)
{
  js_val *prop_name = ARG(args, 0);
  js_prop *prop = fh_get_prop(instance, prop_name->string.ptr);
  return JSBOOL(prop != NULL && prop->enumerable);
}

// Object.prototype.toLocaleString()
js_val *
obj_proto_to_locale_string(js_val *instance, js_args *args, eval_state *state)
{
  return obj_proto_to_string(instance, args, state);
}

// Object.prototype.toString()
js_val *
obj_proto_to_string(js_val *instance, js_args *args, eval_state *state)
{
  char *class = TO_OBJ(state->this)->object.class;
  return JSSTR(fh_str_concat(fh_str_concat("[object ", class), "]"));
}

// Object.prototype.valueOf()
js_val *
obj_proto_value_of(js_val *instance, js_args *args, eval_state *state)
{
  return instance;
}

static js_val *
obj_or_throw(js_val *maybe_obj, eval_state *state, const char *name)
{
  if (!maybe_obj || !IS_OBJ(maybe_obj))
    fh_throw(state, fh_new_error(E_TYPE, "Object.%s called on a non-object", name));
  return maybe_obj;
}

static js_prop_flags
flags_from_descriptor(js_val *desc)
{
  js_prop_flags flags = 0;
  js_val *enumerable = fh_get(desc, "enumerable");
  js_val *configurable = fh_get(desc, "configurable");
  js_val *writable = fh_get(desc, "writable");
  if (IS_BOOL(enumerable) && enumerable->boolean.val)
    flags |= P_ENUM;
  if (IS_BOOL(configurable) && configurable->boolean.val)
    flags |= P_CONF;
  if (IS_BOOL(writable) && writable->boolean.val)
    flags |= P_WRITE;
  return flags;
}

js_val *
bootstrap_object()
{
  js_val *object = JSNFUNC(obj_new, 1);
  js_val *prototype = JSOBJ();

  // Object
  // ------

  // Properties
  DEF(object, "prototype", prototype);

  // Methods
  DEF(object, "create", JSNFUNC(obj_create, 2));
  DEF(object, "defineProperty", JSNFUNC(obj_define_property, 3));
  DEF(object, "defineProperties", JSNFUNC(obj_define_properties, 2));
  DEF(object, "getOwnPropertyDescriptor", JSNFUNC(obj_get_own_property_descriptor, 2));
  DEF(object, "keys", JSNFUNC(obj_keys, 1));
  DEF(object, "getOwnPropertyNames", JSNFUNC(obj_get_own_property_names, 1));
  DEF(object, "getPrototypeOf", JSNFUNC(obj_get_prototype_of, 1));
  DEF(object, "preventExtensions", JSNFUNC(obj_prevent_extensions, 1));
  DEF(object, "isExtensible", JSNFUNC(obj_is_extensible, 1));
  DEF(object, "seal", JSNFUNC(obj_seal, 1));
  DEF(object, "isSealed", JSNFUNC(obj_is_sealed, 1));
  DEF(object, "freeze", JSNFUNC(obj_freeze, 1));
  DEF(object, "isFrozen", JSNFUNC(obj_is_frozen, 1));

  // Object.prototype
  // ----------------

  // Properties
  DEF(prototype, "constructor", JSNFUNC(obj_new, 1));

  // Methods
  DEF(prototype, "hasOwnProperty", JSNFUNC(obj_proto_has_own_property, 1));
  DEF(prototype, "isPrototypeOf", JSNFUNC(obj_proto_is_prototype_of, 1));
  DEF(prototype, "propertyIsEnumerable", JSNFUNC(obj_proto_property_is_enumerable, 1));
  DEF(prototype, "toLocaleString", JSNFUNC(obj_proto_to_locale_string, 0));
  DEF(prototype, "toString", JSNFUNC(obj_proto_to_string, 0));
  DEF(prototype, "valueOf", JSNFUNC(obj_proto_value_of, 0));

  fh->object_proto = prototype;

  return object;
}
