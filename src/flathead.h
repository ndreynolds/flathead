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
#include <stdio.h>

typedef int bool;
#define true 1
#define false 0

bool fhdebug;

#define STREQ(a,b)  (strcmp((a),(b)) == 0)

typedef enum JSTYPE {
  T_NUMBER,
  T_STRING,
  T_BOOLEAN,
  T_OBJECT,
  T_FUNCTION,
  T_NULL,
  T_UNDEF
} JSTYPE;

typedef struct JSARGS {
  void *arg;
  void *next;
} JSARGS;

typedef struct JSPROP {
  char *name;
  bool writable;
  bool enumerable;
  bool configurable;
  bool circular;
  void *ptr;
  UT_hash_handle hh;
} JSPROP;

struct JSNumber {
  double val;
  bool is_nan;
  bool is_inf;
};

struct JSString {
  long len;
  char *ptr;
};

struct JSBoolean {
  bool val;
};

struct JSArray {
  long len;
  void *ptr;
};

struct JSObject {
  bool is_array;
  void *proto;
  void *parent;
  JSPROP *map;
};

struct JSFunction {
  bool is_native;
  void *node; 
  void *closure;
  void *native;
};

typedef struct JSVALUE {
  union {
    struct JSNumber number;
    struct JSString string;
    struct JSArray array;
    struct JSBoolean boolean;
    struct JSObject object;
    struct JSFunction function;
  };
  JSTYPE type;
} JSVALUE;

typedef JSVALUE * (*JSNATVFUNC)(JSARGS *); 

void fh_gc(void);
void fh_set(JSVALUE *, char *, JSVALUE *);
void fh_set_rec(JSVALUE *, char *, JSVALUE *);
JSPROP * fh_get_prop(JSVALUE *, char *);
JSPROP * fh_get_prop_rec(JSVALUE *, char *);
JSVALUE * fh_get(JSVALUE *, char *);
JSVALUE * fh_get_rec(JSVALUE *, char *);

JSVALUE * fh_alloc_val();
JSVALUE * fh_new_val(JSTYPE);
JSVALUE * fh_new_number(double, bool, bool);
JSVALUE * fh_new_string(char *);
JSVALUE * fh_new_boolean(bool);
JSVALUE * fh_new_object();
JSVALUE * fh_new_function(void *);
JSVALUE * fh_new_native_function(JSNATVFUNC);
JSARGS * fh_new_args();

JSVALUE * fh_cast(JSVALUE *, JSTYPE);
char * fh_typeof(JSVALUE *);
char * fh_str_concat(char *, char *);
void fh_debug_obj(JSVALUE *, int);
void fh_debug_arr(JSVALUE *, int);
void fh_debug_args(JSARGS *);
void fh_debug(JSVALUE *, int, bool);

#define JSBOOL(x)     fh_new_boolean((x))
#define JSSTR(x)      fh_new_string((x))
#define JSNULL()      fh_new_val(T_NULL)
#define JSUNDEF()     fh_new_val(T_UNDEF)
#define JSNUM(x)      fh_new_number((x),0,0)
#define JSNAN()       fh_new_number(0,1,0)
#define JSINF()       fh_new_number(0,0,1)
#define JSOBJ()       fh_new_object()
#define JSFUNC(x)     fh_new_function(x)
#define JSNFUNC(x)    fh_new_native_function(x)

#define JSCAST(x, t)  fh_cast((x), (t))
#define JSDEBUG(x)    fh_debug((x),0,1);

#define GETARG(args)  ((args)->arg == NULL ? JSUNDEF() : (args)->arg)       

#endif
