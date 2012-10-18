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

#include "debug.h"
#include "cli.h"


// ----------------------------------------------------------------------------
// Value Debugging
// ----------------------------------------------------------------------------

void
fh_debug_obj(FILE *stream, JSValue *obj, int indent)
{
  if (HASH_COUNT(obj->object.map) == 0) {
    fprintf(stream, "{}");
    return;
  }

  JSProp *x;
  int i;
  bool first = true;

  OBJ_ITER(obj, x) {
    if (first) {
      fprintf(stream, "\n");
      for(i=0;i<(indent);i++) fprintf(stream, " ");
      fprintf(stream, "{");
      first = false;
    }
    else {
      fprintf(stream, ",\n");
      for(i=0;i<(indent+1);i++) fprintf(stream, " ");
    }
    fprintf(stream, " %s: ", x->name);
    x->circular ? 
      fprintf(stream, "[Circular]") : 
      fh_debug(stream, x->ptr, indent+3, false);
  };
  fprintf(stream, " }");
}

void
fh_debug_arr(FILE *stream, JSValue *arr, int indent)
{
  if (HASH_COUNT(arr->object.map) == 0) {
    fprintf(stream, "[]");
    return;
  }
  bool first = true;
  JSProp *x, *tmp;
  fprintf(stream, "[ ");
  HASH_ITER(hh, arr->object.map, x, tmp) {
    if (!first) 
      fprintf(stream, ", ");
    else if (x->enumerable)
      first = false;
    if (x->enumerable)
      fh_debug(stream, x->ptr, 0, false);
  };
  fprintf(stream, " ]");
}

void
fh_debug_args(FILE *stream, JSArgs *args)
{
  bool first = true;
  while(first || args->next != NULL)
  {
    if (!first)
      args = args->next;
    if (args->arg != NULL)
      fh_debug(stream, args->arg, 0, 1);
    first = false;
  }
}

void
fh_debug(FILE *stream, JSValue *val, int indent, bool newline)
{
  switch(val->type) {
    case T_BOOLEAN:
      fprintf(stream, "%s", !val->boolean.val ? "false" : "true");
      break;
    case T_NUMBER:
      if (val->number.is_nan)
        cfprintf(stream, ANSI_ORANGE, "NaN");
      else if (val->number.is_inf) 
        cfprintf(stream, ANSI_ORANGE, "%sInfinity", val->number.is_neg ? "-" : "");
      else 
        cfprintf(stream, ANSI_ORANGE, "%g", val->number.val);
      break;
    case T_STRING:
      cfprintf(stream, ANSI_YELLOW, "'%s'", val->string.ptr);
      break;
    case T_NULL:
      cfprintf(stream, ANSI_GRAY, "null");
      break;
    case T_FUNCTION:
      cfprintf(stream, ANSI_BLUE, "[Function]");
      break;
    case T_UNDEF:
      cfprintf(stream, ANSI_GRAY, "undefined");
      break;
    case T_OBJECT:
      if (val->object.is_array)
        fh_debug_arr(stream, val, indent);
      else
        fh_debug_obj(stream, val, indent);
      break;
  }
  if (newline) fprintf(stream, "\n");
}
