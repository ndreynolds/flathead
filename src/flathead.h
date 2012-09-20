/*
 * flathead.h -- Memory management, casting, and debug helpers
 *
 * Copyright (c) 2012 Nick Reynolds
 *  
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *  
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef FLATHEAD_H
#define FLATHEAD_H

#include "../ext/uthash.h"
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <assert.h>
#include <float.h>

typedef int bool;
#define true 1
#define false 0

struct JSArgs;
struct JSValue;
struct Node;

typedef struct State {
  int line;
  int column;
} State;

typedef enum Signal {
  S_BREAK = 1,
  S_NOOP
} Signal;

typedef enum JSType {
  T_NUMBER,
  T_STRING,
  T_BOOLEAN,
  T_OBJECT,
  T_FUNCTION,
  T_NULL,
  T_UNDEF
} JSType;

typedef enum JSErrorType {
  E_TYPE,
  E_SYNTAX,
  E_EVAL,
  E_RANGE,
  E_REFERENCE,
  E_ASSERTION,
  E_PARSE,
  E_ERROR
} JSErrorType;

typedef struct JSArgs {
  struct JSValue *arg;
  struct JSArgs *next;
  struct State *state;
} JSArgs;

typedef struct JSProp {
  char *name;
  bool writable;
  bool enumerable;
  bool configurable;
  bool circular;
  struct JSValue *ptr;
  UT_hash_handle hh;
} JSProp;

struct JSNumber {
  double val;
  bool is_nan;
  bool is_inf;
  bool is_neg;
};

struct JSString {
  long len;
  char *ptr;
};

struct JSBoolean {
  bool val;
};

struct JSObject {
  bool is_array;
  bool frozen;
  bool sealed;
  bool extensible;
  bool enumerable;
  struct JSValue *parent;
  JSProp *map;
};

/**
 * The standard API for natively defined functions provides an instance (when
 * applicable), the arguments as values in linked-list format, and the evaluation
 * state, which contains information that may be used for error reporting.
 */
typedef struct JSValue * (*JSNativeFunction)(struct JSValue *, JSArgs *, State *); 

struct JSFunction {
  bool is_native;
  struct Node *node; 
  struct JSValue *closure;
  struct JSValue *prototype;
  struct JSValue *instance;
  JSNativeFunction native;
};

typedef struct JSValue {
  union {
    struct JSNumber number;
    struct JSString string;
    struct JSBoolean boolean;
    struct JSObject object;
    struct JSFunction function;
  };
  JSType type;
  Signal signal;
  struct JSValue *proto;
  bool marked;
} JSValue;

void fh_set(JSValue *, char *, JSValue *);
void fh_set_rec(JSValue *, char *, JSValue *);
JSProp * fh_get_prop(JSValue *, char *);
JSProp * fh_get_prop_rec(JSValue *, char *);
JSProp * fh_get_prop_proto(JSValue *, char *);
JSValue * fh_get(JSValue *, char *);
JSValue * fh_get_proto(JSValue *, char *);
JSValue * fh_get_rec(JSValue *, char *);

JSValue * fh_new_val(JSType);
JSValue * fh_new_number(double, bool, bool, bool);
JSValue * fh_new_string(char *);
JSValue * fh_new_boolean(bool);
JSValue * fh_new_object();
JSValue * fh_new_function(struct Node *);
JSValue * fh_new_native_function(JSNativeFunction);
JSValue * fh_get_arg(JSArgs*, int);
JSArgs * fh_new_args();
State * fh_new_state(int, int);

JSValue * fh_cast(JSValue *, JSType);
char * fh_typeof(JSValue *);
char * fh_str_concat(char *, char *);
void fh_error(State *, JSErrorType, const char *, ...);
void fh_debug_obj(FILE *, JSValue *, int);
void fh_debug_arr(FILE *, JSValue *, int);
void fh_debug_args(FILE *, JSArgs *);
void fh_debug(FILE *, JSValue *, int, bool);
int fh_arg_len(JSArgs*);

#define JSBOOL(x)     fh_new_boolean(x)
#define JSSTR(x)      fh_new_string(x)
#define JSNULL()      fh_new_val(T_NULL)
#define JSUNDEF()     fh_new_val(T_UNDEF)
#define JSNUM(x)      fh_new_number(x,0,0,0)
#define JSNAN()       fh_new_number(0,1,0,0)
#define JSINF(n)      fh_new_number(DBL_MAX,0,1,0)
#define JSNINF(n)     fh_new_number(DBL_MIN,0,1,1)
#define JSOBJ()       fh_new_object()
#define JSFUNC(x)     fh_new_function(x)
#define JSNFUNC(x)    fh_new_native_function(x)
#define JSCAST(x,t)   fh_cast((x),(t))
#define JSDEBUG(x)    fh_debug(stdout,(x),0,1);

#define ARG0(args)    ((args)->arg == NULL ? JSUNDEF() : (args)->arg)       
#define ARGN(args, n) fh_get_arg((args), (n))
#define ARGLEN(args)  fh_arg_len((args))

#define STREQ(a,b)    (strcmp((a),(b)) == 0)

#define OBJ_ITER(o,p) JSProp *_tmp; HASH_ITER(hh,(o)->object.map,p,_tmp)

#endif
