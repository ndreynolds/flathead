/*
 * jslite.h -- Memory management, casting, and debug helpers
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

#ifndef JSLITE_H
#define JSLITE_H

#include "../ext/uthash.h"
#include <stdlib.h>
#include <stdio.h>

typedef int bool;
#define true 1
#define false 0

bool jldebug;

#define STREQ(a,b)  (strcmp((a),(b)) == 0)

typedef enum JLTYPE {
  T_NUMBER,
  T_STRING,
  T_BOOLEAN,
  T_OBJECT,
  T_FUNCTION,
  T_NULL,
  T_UNDEF
} JLTYPE;

typedef struct JLARGS {
  void *arg;
  void *next;
} JLARGS;

typedef struct JLPROP {
  char *name;
  bool writable;
  bool enumerable;
  bool configurable;
  bool circular;
  void *ptr;
  UT_hash_handle hh;
} JLPROP;

struct JLNumber {
  double val;
  bool is_nan;
  bool is_inf;
};

struct JLString {
  long len;
  char *ptr;
};

struct JLBoolean {
  bool val;
};

struct JLArray {
  long len;
  void *ptr;
};

struct JLObject {
  bool is_array;
  void *proto;
  void *parent;
  JLPROP *map;
};

struct JLFunction {
  bool is_native;
  void *node; 
  void *closure;
  void *native;
};

typedef struct JLVALUE {
  union {
    struct JLNumber number;
    struct JLString string;
    struct JLArray array;
    struct JLBoolean boolean;
    struct JLObject object;
    struct JLFunction function;
  };
  JLTYPE type;
} JLVALUE;

typedef JLVALUE * (*JLNATVFUNC)(JLARGS *); 

void jl_gc(void);
void jl_set(JLVALUE *, char *, JLVALUE *);
void jl_set_rec(JLVALUE *, char *, JLVALUE *);
JLPROP * jl_get_prop(JLVALUE *, char *);
JLPROP * jl_get_prop_rec(JLVALUE *, char *);
JLVALUE * jl_get(JLVALUE *, char *);
JLVALUE * jl_get_rec(JLVALUE *, char *);

JLVALUE * jl_alloc_val();
JLVALUE * jl_new_val(JLTYPE);
JLVALUE * jl_new_number(double, bool, bool);
JLVALUE * jl_new_string(char *);
JLVALUE * jl_new_boolean(bool);
JLVALUE * jl_new_object();
JLVALUE * jl_new_function(void *);
JLVALUE * jl_new_native_function(JLNATVFUNC);
JLARGS * jl_new_args();

JLVALUE * jl_cast(JLVALUE *, JLTYPE);
char * jl_typeof(JLVALUE *);
char * jl_str_concat(char *, char *);
void jl_debug_obj(JLVALUE *, int);
void jl_debug_arr(JLVALUE *, int);
void jl_debug_args(JLARGS *);
void jl_debug(JLVALUE *, int, bool);

#define JLBOOL(x)     jl_new_boolean((x))
#define JLSTR(x)      jl_new_string((x))
#define JLNULL()      jl_new_val(T_NULL)
#define JLUNDEF()     jl_new_val(T_UNDEF)
#define JLNUM(x)      jl_new_number((x),0,0)
#define JLNAN()       jl_new_number(0,1,0)
#define JLINF()       jl_new_number(0,0,1)
#define JLOBJ()       jl_new_object()
#define JLFUNC(x)     jl_new_function(x)
#define JLNFUNC(x)    jl_new_native_function(x)

#define JLCAST(x, t)  jl_cast((x), (t))
#define JLDEBUG(x)    jl_debug((x),0,1);

#define GETARG(args)  ((args)->arg == NULL ? JLUNDEF() : (args)->arg)       

#endif
