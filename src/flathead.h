/*
 * flathead.h -- Core types, constructors, casting, and debug.
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

#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <assert.h>
#include <limits.h>

#include "../ext/uthash.h"


#define VERSION        "0.5.1"

#define JSBOOL(x)      fh_new_boolean(x)
#define JSSTR(x)       fh_new_string(x)
#define JSNULL()       fh_new_val(T_NULL)
#define JSUNDEF()      fh_new_val(T_UNDEF)
#define JSNUM(x)       fh_new_number((x),0,0,0)
#define JSNAN()        fh_new_number(0,1,0,0)
#define JSINF(n)       fh_new_number(INT_MAX,0,1,0)
#define JSNINF(n)      fh_new_number(-INT_MAX,0,1,1)
#define JSOBJ()        fh_new_object()
#define JSARR()        fh_new_array()
#define JSFUNC(x)      fh_new_function(x)
#define JSNFUNC(x)     fh_new_native_function(x)
#define JSREGEXP(x)    fh_new_regexp(x)
#define JSNUMKEY(x)    fh_cast(JSNUM((x)), T_STRING)

#define IS_STR(x)      ((x)->type == T_STRING)
#define IS_NUM(x)      ((x)->type == T_NUMBER)
#define IS_FUNC(x)     ((x)->type == T_FUNCTION)
#define IS_BOOL(x)     ((x)->type == T_BOOLEAN)
#define IS_OBJ(x)      ((x)->type == T_OBJECT)
#define IS_NULL(x)     ((x)->type == T_NULL)
#define IS_UNDEF(x)    ((x)->type == T_UNDEF)
#define IS_ARR(x)      ((x)->type == T_OBJECT && (x)->object.is_array)
#define IS_NAN(x)      ((x)->type == T_NUMBER && (x)->number.is_nan)
#define IS_INF(x)      ((x)->type == T_NUMBER && (x)->number.is_inf)

#define TO_STR(x)      fh_cast((x),T_STRING)
#define TO_NUM(x)      fh_cast((x),T_NUMBER)
#define TO_BOOL(x)     fh_cast((x),T_BOOLEAN)
#define TO_OBJ(x)      fh_cast((x),T_OBJECT)

#define ARG(args,n)    fh_get_arg((args), (n))
#define ARGLEN(args)   fh_arg_len(args)

#define STREQ(a,b)     (strcmp((a),(b)) == 0)
#define OBJ_ITER(o,p)  JSProp *_tmp; HASH_ITER(hh,(o)->object.map,p,_tmp)
#define BUILTIN(o,k,v) fh_set_prop((o),(k),(v),P_BUILTIN)

#define DEBUG(x)       fh_debug(stdout,(x),0,1)

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))


struct JSArgs;
struct JSValue;
struct Node;

typedef struct State {
  int line;
  int column;
  struct JSValue *ctx;
} State;

typedef enum Signal {
  S_BREAK = 1,
  S_NOOP,
  S_NONE
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

typedef enum JSPropFlags {
  P_WRITE   = 0x01,
  P_ENUM    = 0x02,
  P_CONF    = 0x04,
  P_BUILTIN = P_WRITE | P_CONF,
  P_DEFAULT = P_WRITE | P_ENUM | P_CONF
} JSPropFlags;

struct JSNumber {
  double val;
  bool is_nan;
  bool is_inf;
  bool is_neg;
};

struct JSString {
  long length;
  char *ptr;
};

struct JSBoolean {
  bool val;
};

struct JSObject {
  int length;
  bool is_array;
  bool is_regexp;
  bool frozen;
  bool sealed;
  bool extensible;
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
  bool is_generator;
  struct Node *node; 
  struct JSValue *closure;
  struct JSValue *bound_this;
  struct JSArgs *bound_args;
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

JSValue * fh_new_val(JSType);
JSValue * fh_new_number(double, bool, bool, bool);
JSValue * fh_new_string(char *);
JSValue * fh_new_boolean(bool);
JSValue * fh_new_object();
JSValue * fh_new_array();
JSValue * fh_new_function(struct Node *);
JSValue * fh_new_native_function(JSNativeFunction);
JSValue * fh_new_regexp(char *);
JSValue * fh_get_arg(JSArgs *, int);
JSArgs * fh_new_args(JSValue *, JSValue *, JSValue *);
JSProp * fh_new_prop(JSPropFlags);
State * fh_new_state(int, int);

JSValue * fh_eval_file(FILE *, JSValue *, int);
JSValue * fh_try_get_proto(char *);
JSValue * fh_cast(JSValue *, JSType);
JSValue * fh_has_instance(JSValue *, JSValue *);
JSValue * fh_has_property(JSValue *, char *);
char * fh_typeof(JSValue *);
void fh_arr_set_len(JSValue *, int);
void fh_error(State *, JSErrorType, const char *, ...);
int fh_arg_len(JSArgs*);

extern int fh_opt_interactive;
extern int fh_opt_print_ast;
extern int fh_opt_print_tokens;

#endif
