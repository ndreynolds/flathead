/*
 * regexp.h -- PCRE wrapper
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

#ifndef fh_no_regexp
#include <pcre.h>
#endif

#include "regexp.h"
#include "flathead.h"


const int regexp_vector_len = 30;


// Gateway to the PCRE library. When compiled with fh_no_regexp, this function
// is still available, but will throw an error when called.
int *
fh_regexp(char *str, char *pattern, int *count, int offset, bool caseless)
{
#ifndef fh_no_regexp
  const char *error;
  int rc;
  int error_offset;
  int *output_vector = malloc(regexp_vector_len * sizeof(int));
  int options = PCRE_JAVASCRIPT_COMPAT;
  if (caseless) 
    options |= PCRE_CASELESS;

  pcre *regexp = pcre_compile(pattern, options, &error, &error_offset, NULL);
  if (!regexp)
    fh_error(NULL, E_SYNTAX, "Regular expression is not valid");

  rc = pcre_exec(regexp, NULL, str, strlen(str), offset, 0, 
                 output_vector, regexp_vector_len);

  if (count != NULL)
    *count = rc;

  pcre_free(regexp);

  if (rc < 0) {
    free(output_vector);
    return NULL;
  }
  
  return output_vector;
#else
  fh_error(NULL, E_ERROR, "Regular expressions are not available");
  return NULL;
#endif
}
