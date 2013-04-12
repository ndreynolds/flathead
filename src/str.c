/*
 * str.c -- String functions
 *
 * Copyright (c) 2012-2013 Nick Reynolds
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>

#include "str.h"


/* Returns a newly allocated string that is the concatenation of the two
 * argument strings */
char *
fh_str_concat(char *a, char *b)
{
  size_t size = strlen(a) + strlen(b) + 1;
  char *new = malloc(size);
  snprintf(new, size, "%s%s", a, b);
  return new;
}

/* Returns a newly allocated substring from start and end indices provided */
char *
fh_str_slice(char *str, unsigned start, unsigned end)
{
  if (start > end || end > strlen(str))
    return NULL;
  size_t size = end - start + 1;
  char *new = malloc(size);
  snprintf(new, size, "%.*s", end - start, str + start);
  return new;
}

/* Replace a substring in the given string with a new string. The limit
 * argument indicates the maximum number of replacements that should be made.
 * Pass 0 to for unlimited replacements. The return is a newly allocated string
 * unless no replacements could be made, in which case the original is
 * returned. */
char *
fh_str_replace(char *orig, char *repl, char *new, int limit)
{
  char *result, *ins, *tmp;
  int count, dist;
  bool has_limit = limit > 0;

  if (!(orig && repl && strlen(repl) && new)) return orig;
  if (!(ins = strstr(orig, repl))) return orig;

  int len_new = strlen(new), len_repl = strlen(repl);
  for (count = 0; (tmp = strstr(ins, repl)) && (!has_limit || limit); count++, limit--)
    ins = tmp + len_repl;

  tmp = result = malloc(strlen(orig) + (len_new - len_repl) * count + 1);
  if (!result) return orig;
  
  while (count--) {
    ins = strstr(orig, repl);
    dist = ins - orig;
    tmp = strncpy(tmp, orig, dist) + dist;
    tmp = strcpy(tmp, new) + len_new;
    orig += dist + len_repl;
  }

  strcpy(tmp, orig);
  return result;
}
