/*
 * debug.c -- debugging functions
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

#include <math.h>

#include "debug.h"
#include "props.h"
#include "cli.h"


// ----------------------------------------------------------------------------
// Value Debugging
// ----------------------------------------------------------------------------

void
fh_debug(FILE *stream, js_val *val, int indent, bool newline)
{
  switch(val->type) {
    case T_BOOLEAN:
      fprintf(stream, "%s", !val->boolean.val ? "false" : "true");
      break;
    case T_NUMBER:
      fh_debug_num(stream, val);
      break;
    case T_STRING:
      if (fh->opt_interactive)
        cfprintf(stream, ANSI_YELLOW, "'%s'", val->string.ptr);
      else
        fprintf(stream, "%s", val->string.ptr);
      break;
    case T_NULL:
      cfprintf(stream, ANSI_GRAY, "null");
      break;
    case T_UNDEF:
      cfprintf(stream, ANSI_GRAY, "undefined");
      break;
    case T_OBJECT:
      if (IS_ARR(val))
        fh_debug_arr(stream, val, indent);
      else if (IS_FUNC(val))
        cfprintf(stream, ANSI_BLUE, "[Function]");
      else
        fh_debug_obj(stream, val, indent, false);
      break;
  }
  if (newline) fprintf(stream, "\n");
}

// Debug an object with extra verbosity, displaying non-enumerable properties.
void
fh_debug_verbose(FILE *stream, js_val *val, int indent)
{
  switch(val->type) {
    case T_BOOLEAN:
      fprintf(stream, "Boolean: (%s)", !val->boolean.val ? "false" : "true");
      break;
    case T_NUMBER:
      fh_debug_num(stream, val);
      break;
    case T_STRING:
      cfprintf(stream, ANSI_YELLOW, "String: '%s'", val->string.ptr);
      break;
    case T_NULL:
      cfprintf(stream, ANSI_GRAY, "null");
      break;
    case T_UNDEF:
      cfprintf(stream, ANSI_GRAY, "undefined");
      break;
    case T_OBJECT:
      if (IS_ARR(val))
        fprintf(stream, "Array: ");
      else if (IS_FUNC(val))
        cfprintf(stream, ANSI_BLUE, "Function: ");
      else
        fprintf(stream, "Object: ");
      break;
  }

  if (IS_OBJ(val))
    fh_debug_obj(stream, val, indent, true);

  fprintf(stream, "\n");
}

void
fh_debug_obj(FILE *stream, js_val *obj, int indent, bool force_enum)
{
  js_prop *x;
  int i;
  bool first = true;

  OBJ_ITER(obj, x) {
    if (!x->enumerable && !force_enum) continue;
    if (first) {
      for (i = 0; i < indent; i++) fprintf(stream, " ");
      fprintf(stream, "{");
      first = false;
    }
    else {
      fprintf(stream, ",\n");
      for (i = 0; i < (indent + 1); i++) fprintf(stream, " ");
    }
    fprintf(stream, " %s: ", x->name);
    if (x->circular)
      cfprintf(stream, ANSI_BLUE, "[Circular]");
    else
      fh_debug(stream, x->ptr, indent+3, false);
  };

  fprintf(stream, first ? "{}" : " }");
}

void
fh_debug_arr(FILE *stream, js_val *arr, int indent)
{
  if (arr->object.length == 0) {
    fprintf(stream, "[]");
    return;
  }
  fprintf(stream, "[ ");

  bool first = true;
  js_prop *prop;
  unsigned long i;

  for (i = 0; i < arr->object.length; i++) {
    prop = fh_get_prop(arr, JSNUMKEY(i)->string.ptr);

    if (!first) 
      fprintf(stream, ", ");
    else
      first = false;

    if (!prop) continue;

    if (prop->circular)
      cfprintf(stream, ANSI_BLUE, "[Circular]");
    else
      fh_debug(stream, prop->ptr, 0, false);
  }

  fprintf(stream, " ]");
}

void
fh_debug_num(FILE *stream, js_val *num)
{
  if (num->number.is_nan)
    cfprintf(stream, ANSI_ORANGE, "NaN");
  else if (num->number.is_inf) 
    cfprintf(stream, ANSI_ORANGE, "%sInfinity", num->number.is_neg ? "-" : "");
  else {
    char *fmt = "%f";
    if (fmod(num->number.val, 1) == 0) 
      fmt = "%.0f";
    if (fabs(num->number.val) > 1e21)
      fmt = "%g";
    cfprintf(stream, ANSI_ORANGE, fmt, num->number.val);
  }
}

void
fh_debug_args(FILE *stream, js_args *args)
{
  unsigned int i;
  for (i = 0; i < ARGLEN(args); i++)
    fh_debug(stream, ARG(args, i), 0, 1);
}
