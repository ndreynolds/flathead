/*
 * args.c -- singly linked list used internally to represent arguments 
 *
 * Copyright (c) 2013 Nick Reynolds
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

#include "args.h"
#include "flathead.h"

js_args *
args_new()
{
  js_args *args = malloc(sizeof(js_args));
  args->arg = NULL;
  args->next = NULL;
  return args;
}

void
args_append(js_args *args, js_val *val)
{
  if (args->arg == NULL) 
    args->arg = val;
  else {
    while (args->next != NULL) args = args->next;
    args->next = args_new();
    args->next->arg = val;
  }
}

js_val *
args_get(js_args *args, int n)
{
  int i;
  for (i = 0; i <= n; i++) {
    if (i == n) 
      return args->arg ? args->arg : JSUNDEF();
    if (args->next == NULL) 
      break;
    args = args->next;
  }
  return JSUNDEF();
}

unsigned int
args_len(js_args *args)
{
  if (args == NULL) return 0;

  // Although arrays can be up to 2^32 - 1 in length, we limit
  // the number of arguments to 2^16 - 1 (UINT_MAX).
  unsigned long i = 0;
  while (i < UINT_MAX) {
    if (args->arg) i++;
    if (args->next == NULL) break;
    args = args->next;
  }
  if (i >= UINT_MAX)
    fh_error(NULL, E_RANGE, "too many arguments");
  return i;
}
