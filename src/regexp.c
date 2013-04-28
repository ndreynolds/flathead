/*
 * regexp.h -- PCRE wrapper
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

#ifndef FH_NO_REGEXP
#include <pcre.h>
#endif

#include "regexp.h"
#include "flathead.h"


const int regexp_vector_len = 30;


/* Gateway to the PCRE library. Compiles a regular expression pattern and
 * returns matches. When compiled with FH_NO_REGEXP, this function is still
 * available, but will throw an error when called. */
int *
fh_regexp(char *str, char *pattern, int *count, int offset, bool caseless)
{
#ifndef FH_NO_REGEXP
  const char *error;
  int rc;
  int error_offset;
  int *output_vector = malloc(regexp_vector_len * sizeof(int));
  int options = PCRE_JAVASCRIPT_COMPAT;
  if (caseless) 
    options |= PCRE_CASELESS;

  pcre *regexp = pcre_compile(pattern, options, &error, &error_offset, NULL);
  if (!regexp) {
    char *fmt = "Invalid Regular Expression:\n  %s at offset %d";
    fh_throw(NULL, fh_new_error(E_SYNTAX, fmt, error, error_offset));
  }

  rc = pcre_exec(regexp, NULL, str, strlen(str), offset, 0, 
                 output_vector, regexp_vector_len);

  if (count != NULL)
    *count = rc > 0 ? rc : 0;

  pcre_free(regexp);

  if (rc < 0) {
    free(output_vector);
    return NULL;
  }
  
  return output_vector;
#else
  fh_error(NULL, E_ERROR, "Regular expressions are not available");
  UNREACHABLE();
#endif
}

/* Get the number of capturing subpatterns in the regular expression. This is
 * used to get the `NCapturingParens` value used in parts of the ECMA spec */
int 
fh_regexp_ncaptures(char *pattern)
{
#ifndef FH_NO_REGEXP
  const char *error;
  int error_offset;
  int options = PCRE_JAVASCRIPT_COMPAT;
  int captures;
  pcre *regexp = pcre_compile(pattern, options, &error, &error_offset, NULL);
  if (!regexp) {
    char *fmt = "Invalid Regular Expression:\n  %s at offset %d";
    fh_throw(NULL, fh_new_error(E_SYNTAX, fmt, error, error_offset));
  }
  pcre_fullinfo(regexp, NULL, PCRE_INFO_CAPTURECOUNT, &captures);
  return captures;
#else
  fh_error(NULL, E_ERROR, "Regular expressions are not available");
  UNREACHABLE();
#endif
}
