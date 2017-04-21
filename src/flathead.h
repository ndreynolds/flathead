/*
 * flathead.h -- Core types, constructors, casting, and debug.
 *
 * Copyright (c) 2012-2017 Nick Reynolds
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
#include <setjmp.h>
#include <assert.h>
#include <limits.h>

#include "../ext/uthash.h"
#include "version.h"

#ifndef INFINITY
#define INFINITY       (1.0/0.0)
#endif

#define MAX_ARENAS     10

#define JSBOOL(x)      fh_new_boolean(x)
#define JSSTR(x)       fh_new_string(x)
#define JSNULL()       fh_new_val(T_NULL)
#define JSUNDEF()      fh_new_val(T_UNDEF)
#define JSNUM(x)       fh_new_number((x),0,0,0)
#define JSNAN()        fh_new_number(0,1,0,0)
#define JSINF()        fh_new_number(INFINITY,0,1,0)
#define JSNINF()       fh_new_number(-INFINITY,0,1,1)
#define JSOBJ()        fh_new_object()
#define JSARR()        fh_new_array()
#define JSFUNC(x)      fh_new_function(x)
#define JSNFUNC(x,n)   fh_new_native_function((x),(n))
#define JSRE(x)        fh_new_regexp(x)
#define JSNUMKEY(x)    fh_cast(JSNUM(x), T_STRING)

#define IS_STR(x)      ((x)->type == T_STRING)
#define IS_NUM(x)      ((x)->type == T_NUMBER)
#define IS_BOOL(x)     ((x)->type == T_BOOLEAN)
#define IS_NULL(x)     ((x)->type == T_NULL)
#define IS_UNDEF(x)    ((x)->type == T_UNDEF)
#define IS_OBJ(x)      ((x)->type == T_OBJECT)
#define IS_FUNC(x)     ((x)->type == T_OBJECT && STREQ((x)->object.class, "Function"))
#define IS_ARR(x)      ((x)->type == T_OBJECT && STREQ((x)->object.class, "Array"))
#define IS_REGEXP(x)   ((x)->type == T_OBJECT && STREQ((x)->object.class, "RegExp"))
#define IS_DATE(x)     ((x)->type == T_OBJECT && STREQ((x)->object.class, "Date"))
#define IS_NAN(x)      ((x)->type == T_NUMBER && (x)->number.is_nan)
#define IS_INF(x)      ((x)->type == T_NUMBER && (x)->number.is_inf)

#define TO_STR(x)      fh_cast((x),T_STRING)
#define TO_NUM(x)      fh_cast((x),T_NUMBER)
#define TO_BOOL(x)     fh_cast((x),T_BOOLEAN)
#define TO_OBJ(x)      fh_cast((x),T_OBJECT)

#define TO_INT(x)      fh_to_int(x)
#define TO_INT32(x)    fh_to_int32(x)
#define TO_UINT32(x)   fh_to_uint32(x)

#define E_ERROR        "Error"
#define E_EVAL         "EvalError"
#define E_RANGE        "RangeError"
#define E_REFERENCE    "ReferenceError"
#define E_SYNTAX       "SyntaxError"
#define E_TYPE         "TypeError"
#define E_URI          "URIError"

#define ARG(args,n)    args_get((args), (n))
#define ARGLEN(args)   args_len(args)

#define STREQ(a,b)     (strcmp((a),(b)) == 0)
#define OBJ_ITER(o,p)  js_prop *_tmp; HASH_ITER(hh,(o)->map,(p),_tmp)

#define DEF(o,k,v)     fh_set_prop((o),(k),(v),P_BUILTIN)
#define DEF2(o,k,v,f)  fh_set_prop((o),(k),(v),(f))

#define DEBUG(x)       fh_debug(stdout,(x),0,1)
#define UNREACHABLE()  assert(0)

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))


struct js_val;
struct js_args;
struct ast_node;
struct gc_arena;

typedef enum {
  GC_STATE_STARTING,
  GC_STATE_MARK,
  GC_STATE_SWEEP,
  GC_STATE_NONE
} gc_state;

typedef enum {
  S_BREAK = 1,
  S_NOOP,
  S_NONE
} ctl_signal;

typedef enum {
  T_NUMBER = 1,
  T_STRING,
  T_BOOLEAN,
  T_OBJECT,
  T_NULL,
  T_UNDEF
} js_type;

typedef enum {
  P_NONE    = 0x00,
  P_WRITE   = 0x01,
  P_ENUM    = 0x02,
  P_CONF    = 0x04,
  P_IGNORE  = 0x08,
  P_BUILTIN = P_WRITE | P_CONF,
  P_DEFAULT = P_WRITE | P_ENUM | P_CONF
} js_prop_flags;

typedef struct {
  gc_state gc_state;
  struct gc_arena *gc_arenas[MAX_ARENAS];
  int gc_num_arenas;
  int gc_runs;
  long gc_last_start;
  long gc_time;

  bool opt_interactive;
  bool opt_print_tokens;
  bool opt_print_ast;
  bool opt_keep_history_file;
  const char *opt_history_filename;

  jmp_buf repl_jmp;                   // used to handle errors within REPL
  char *script_name;
  struct eval_state *callstack;

  struct js_val *function_proto;    // cache prototype pointers
  struct js_val *object_proto;
  struct js_val *array_proto;
  struct js_val *global;
} fh_state;

typedef struct eval_state {
  int line;
  int column;
  char *caller_info;
  char *script_name;
  bool construct;
  bool catch;
  struct js_val *ctx;
  struct js_val *this;
  struct js_val *scope;
  jmp_buf jmp;
  struct eval_state *parent;
} eval_state;

typedef struct {
  char *name;
  bool writable;
  bool enumerable;
  bool configurable;
  bool circular;
  struct js_val *ptr;
  UT_hash_handle hh;
} js_prop;

typedef struct {
  double val;
  bool is_nan;
  bool is_inf;
  bool is_neg;
} js_number;

typedef struct {
  unsigned long length;
  char *ptr;
} js_string;

typedef struct {
  bool val;
} js_boolean;

/* The standard API for natively defined functions provides an instance (when
 * applicable), the arguments as values in linked-list format, and the evaluation
 * state, which contains information that may be used for error reporting.
 */
typedef struct js_val * (js_native_function)(struct js_val *, struct js_args *, eval_state *);

typedef struct {
  bool native;
  bool generator;
  bool provide_this;
  bool extensible;            // [[Extensible]]
  char class[10];             // [[Class]]
  struct js_val *primitive;   // [[PrimitiveValue]]
  struct js_val *bound_this;  // [[BoundThis]]
  struct js_args *bound_args; // [[BoundArguments]]
  struct js_val *scope;       // [[Scope]]
  struct js_val *instance;
  struct js_val *parent;
  struct ast_node *node;
  unsigned long length;
  js_native_function *nativefn;
} js_object;

/* TODO: Store non-objects more efficiently. A lot of space is currenlty wasted
 * for JS primitives. `js_val` should only store a pointer to the `js_obj`.
 * Maybe the GC can keep separate heaps for objects and values. */

typedef struct js_val {
  js_number number;
  js_string string;
  js_boolean boolean;
  js_object object;
  js_type type;
  ctl_signal signal;
  struct js_val *proto;
  bool marked;
  bool flagged;
  js_prop *map;
} js_val;

js_val * fh_new_val(js_type);
js_val * fh_new_number(double, bool, bool, bool);
js_val * fh_new_string(char *);
js_val * fh_new_boolean(bool);
js_val * fh_new_object();
js_val * fh_new_array();
js_val * fh_new_function(struct ast_node *);
js_val * fh_new_native_function(js_native_function, int);
js_val * fh_new_regexp(char *);
js_val * fh_new_error(char *, const char *, ...);

js_prop * fh_new_prop(js_prop_flags);
fh_state * fh_new_global_state();

eval_state * fh_new_state(int, int);
void fh_push_state(eval_state *);
void fh_pop_state();

js_val * fh_eval_file(FILE *, js_val *);
js_val * fh_eval_string(char *, js_val *);
js_val * fh_try_get_proto(char *);

bool fh_is_callable(js_val *);
js_val * fh_to_primitive(js_val *, js_type);
js_val * fh_to_number(js_val *);
js_val * fh_to_int(js_val *);
js_val * fh_to_int32(js_val *);
js_val * fh_to_uint32(js_val *);
js_val * fh_to_string(js_val *);
js_val * fh_to_boolean(js_val *);
js_val * fh_to_object(js_val *);
js_val * fh_cast(js_val *, js_type);

js_val * fh_has_instance(js_val *, js_val *);
js_val * fh_implicit_this_value(js_val *obj);
js_val * fh_has_property(js_val *, char *);
char * fh_typeof(js_val *);
void fh_set_len(js_val *, unsigned long);
void fh_set_class(js_val *, char *);
void fh_throw(eval_state *, js_val *);

extern fh_state *fh;

#endif
