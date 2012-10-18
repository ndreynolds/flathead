// String.c
// ----------
// String properties, methods, and prototype

#include <ctype.h>
#include <math.h>

#include "String.h"

JSValue *
str_new(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

// String.fromCharCode(num1, ..., numN)
JSValue *
str_from_char_code(JSValue *instance, JSArgs *args, State *state)
{
  fh_error(state, E_EVAL, "Unicode is not supported");
  return JSUNDEF();
}

// String.prototype.charAt(index)
JSValue *
str_proto_char_at(JSValue *instance, JSArgs *args, State *state)
{
  int index = TO_NUM(ARG(args, 0))->number.val;

  if (index < 0 || index >= instance->string.length)
    return JSSTR("");
  char *str = malloc(2);
  sprintf(str, "%c", instance->string.ptr[index]);
  return JSSTR(str);
}

// String.prototype.charCodeAt(index)
JSValue *
str_proto_char_code_at(JSValue *instance, JSArgs *args, State *state)
{
  fh_error(state, E_EVAL, "Unicode is not supported");
  return JSUNDEF();
}

// String.prototype.concat(string2, string3[, ..., stringN])
JSValue *
str_proto_concat(JSValue *instance, JSArgs *args, State *state)
{
  JSValue *new = JSSTR(instance->string.ptr);

  JSValue *arg;
  char *tmp;
  int i;
  for (i=0; i<ARGLEN(args); i++) {
    arg = TO_STR(ARG(args, i));
    tmp = new->string.ptr;
    new->string.ptr = fh_str_concat(new->string.ptr, arg->string.ptr);
    // Free the old string (GC can't reach it anymore).
    free(tmp);
  }

  new->string.length = strlen(new->string.ptr);
  return new;
}

// String.prototype.indexOf(searchValue[, fromIndex])
JSValue *
str_proto_index_of(JSValue *instance, JSArgs *args, State *state)
{
  JSValue *search_str = TO_STR(ARG(args, 0));

  char *haystack = instance->string.ptr;
  char *needle = search_str->string.ptr;
  JSValue *from = ARG(args, 1);

  int i = IS_NUM(from) ? from->number.val : 0;
  int match = 0;

  // Searching for an empty string returns the fromIndex when less than
  // the instance string length, and the instance string length otherwise.
  if (strlen(needle) == 0)
    return JSNUM(i <= strlen(haystack) ? i : strlen(haystack));

  if (strlen(needle) > strlen(haystack) || i > strlen(haystack))
    return JSNUM(-1);

  for (; i<strlen(haystack); i++) {
    if (haystack[i] == needle[match])
      match++;
    else
      match = haystack[i] == needle[0] ? 1 : 0;
    if (match == strlen(needle))
      return JSNUM(i - strlen(needle) + 1);
  }

  return JSNUM(-1);
}

// String.prototype.lastIndexOf(searchValue[, fromIndex])
JSValue *
str_proto_last_index_of(JSValue *instance, JSArgs *args, State *state)
{
  JSValue *search_str = TO_STR(ARG(args, 0));
  JSValue *from = ARG(args, 1);

  char *haystack = instance->string.ptr;
  char *needle = search_str->string.ptr;

  int max = IS_NUM(from) ? from->number.val : strlen(haystack) - 1;
  int match = strlen(needle) - 1;
  int i = strlen(haystack) - 1;

  if (max < 0) max = 0;

  if (strlen(needle) == 0)
    return JSNUM(max <= strlen(haystack) ? max : strlen(haystack));

  if (strlen(needle) > strlen(haystack))
    return JSNUM(-1);

  for (; i>=0; i--) {
    if (haystack[i] == needle[match])
      match--;
    else
      match = strlen(needle) - (haystack[i] == needle[0] ? 2 : 1);
    if (match < 0 && i <= max)
      return JSNUM(i);
  }

  return JSNUM(-1);
}

// String.prototype.localeCompare(compareString)
JSValue *
str_proto_locale_compare(JSValue *instance, JSArgs *args, State *state)
{
  JSValue *compare_str = TO_STR(ARG(args, 0));
  return JSNUM(strcmp(instance->string.ptr, compare_str->string.ptr));
}

// String.prototype.match(regexp)
JSValue *
str_proto_match(JSValue *instance, JSArgs *args, State *state)
{
  JSValue *regexp = ARG(args, 0);
  JSValue *arr = JSARR();

  int count;
  char *str = instance->string.ptr;
  int *matches = fh_regexp(str, regexp->string.ptr, &count);

  if (!matches) 
    return arr;

  int i;
  for (i = 0; i < count; i++) {
    JSValue *match = JSSTR(fh_str_slice(str, matches[2*i], matches[2*i+1]));
    fh_set(arr, JSNUMKEY(i)->string.ptr, match);
  }
  free(matches);
  fh_arr_set_len(arr, i);
  return arr;
}

// String.prototype.replace(regexp|substr, newSubStr|function[, flags])
JSValue *
str_proto_replace(JSValue *instance, JSArgs *args, State *state)
{
  // NOTE: flags parameter is non-standard
  // TODO: requires regex
  return JSUNDEF();
}

// String.prototype.search(regexp)
JSValue *
str_proto_search(JSValue *instance, JSArgs *args, State *state)
{
  JSValue *regexp = ARG(args, 0);

  int count;
  char *str = instance->string.ptr;
  int *matches = fh_regexp(str, regexp->string.ptr, &count);

  if (!matches) 
    return JSNUM(-1);

  JSValue *result = JSNUM(matches[0]);
  free(matches);
  return result;
}

// String.prototype.slice(beginSlice[, endSlice])
JSValue *
str_proto_slice(JSValue *instance, JSArgs *args, State *state)
{
  JSValue *end_arg = ARG(args, 1);
  int len = strlen(instance->string.ptr);
  int end = IS_UNDEF(end_arg) ? len : TO_NUM(end_arg)->number.val;
  int start = TO_NUM(ARG(args, 0))->number.val;

  if (start < 0) start = len + start > 0 ? len + start : 0;
  if (end < 0) end = len + end > 0 ? len + end : 0;
  if (end > len) end = len;
  if (end - start == 0) return JSSTR("");

  return JSSTR(fh_str_slice(instance->string.ptr, start, end));
}

// String.prototype.split([separator][, limit])
JSValue *
str_proto_split(JSValue *instance, JSArgs *args, State *state)
{
  // TODO: regex separators
  
  JSValue *sep_arg = ARG(args, 0);
  JSValue *limit_arg = ARG(args, 1);

  JSValue *arr = JSARR();

  int limit = IS_UNDEF(limit_arg) ? pow(2, 32) - 1 : TO_NUM(limit_arg)->number.val;

  if (limit == 0) 
    return arr;

  if (IS_UNDEF(sep_arg)) { 
    fh_set(arr, "0", instance);
    fh_arr_set_len(arr, 1);
    return arr;
  }

  char *split;                         // store the split substring
  char *sep = sep_arg->string.ptr;     // separator string
  char *str = instance->string.ptr;    // instance string
  int len = instance->string.length;   // instance string length
  int match = 0;                       // number of chars of sep matched
  int start = 0;                       // start index to split from
  int index = 0;                       // result array index
  int i;                               // instance string index

  for (i=0; i<strlen(str); i++) {
    if (str[i] == sep[match])
      match++;
    else
      match = str[i] == sep[0] ? 1 : 0;
    if (match == strlen(sep)) {
      split = fh_str_slice(str, start, i - strlen(sep) + 1);
      fh_set(arr, JSNUMKEY(index++)->string.ptr, JSSTR(split));
      start = i + 1;
      match = 0;
      if (--limit == 0) break;
    }
  }

  // Move the remaining string (possibly all of it) into the array.
  if (limit > 0) {
    split = fh_str_slice(str, start, len);
    fh_set(arr, JSNUMKEY(index++)->string.ptr, JSSTR(split));
  }

  fh_arr_set_len(arr, index);
  return arr;
}

// String.prototype.substr(start[, length])
JSValue *
str_proto_substr(JSValue *instance, JSArgs *args, State *state)
{
  int strlen = instance->string.length;
  int start = TO_NUM(ARG(args, 0))->number.val;
  int length = IS_UNDEF(ARG(args, 1)) ? strlen : TO_NUM(ARG(args, 1))->number.val;

  if (start < 0) 
    start = MAX(start + strlen, 0);

  if (MIN(MAX(length, 0), strlen - start) <= 0)
    return JSSTR("");

  int end = MIN(strlen, start + length);
  return JSSTR(fh_str_slice(instance->string.ptr, start, end));
}

// String.prototype.substring(start[, end])
JSValue *
str_proto_substring(JSValue *instance, JSArgs *args, State *state)
{
  int len = instance->string.length;
  int start = TO_NUM(ARG(args, 0))->number.val;
  int end = IS_UNDEF(ARG(args, 1)) ? len : TO_NUM(ARG(args, 1))->number.val;

  start = MIN(MAX(start, 0), len);
  end = MIN(MAX(end, 0), len);

  int from = MIN(start, end);
  int to = MAX(start, end);

  return JSSTR(fh_str_slice(instance->string.ptr, from, to));
}

// String.prototype.toLocaleLowerCase()
JSValue *
str_proto_to_locale_lower_case(JSValue *instance, JSArgs *args, State *state)
{
  return str_proto_to_lower_case(instance, args, state);
}

// String.prototype.toLocaleUpperCase()
JSValue *
str_proto_to_locale_upper_case(JSValue *instance, JSArgs *args, State *state)
{
  return str_proto_to_upper_case(instance, args, state);
}

// String.prototype.toLowerCase()
JSValue *
str_proto_to_lower_case(JSValue *instance, JSArgs *args, State *state)
{
  JSValue *new = JSSTR(instance->string.ptr);
  char *str = new->string.ptr;
  int i = 0;
  while(str[i]) {
    str[i] = tolower(str[i]);
    i++;
  }
  return new;
}

// String.prototype.toString()
JSValue *
str_proto_to_string(JSValue *instance, JSArgs *args, State *state)
{
  return instance;
}

// String.prototype.toUpperCase()
JSValue *
str_proto_to_upper_case(JSValue *instance, JSArgs *args, State *state)
{
  JSValue *new = JSSTR(instance->string.ptr);
  char *str = new->string.ptr;
  int i = 0;
  while(str[i]) {
    str[i] = toupper(str[i]);
    i++;
  }
  return new;
}

// String.prototype.trim()
JSValue *
str_proto_trim(JSValue *instance, JSArgs *args, State *state)
{
  JSValue *rtrimmed = str_proto_trim_right(instance, args, state);
  return str_proto_trim_left(rtrimmed, args, state);
}

// String.prototype.trimLeft()
JSValue *
str_proto_trim_left(JSValue *instance, JSArgs *args, State *state)
{
  JSValue *new = JSSTR(instance->string.ptr);
  char *str = new->string.ptr;
  int n = 0;
  while(str[n] != '\0' && isspace((unsigned char)str[n]))
    n++;
  memmove(str, str + n, strlen(str) - n + 1);
  new->string.length = strlen(str);
  return new;
}

// String.prototype.trimRight()
JSValue *
str_proto_trim_right(JSValue *instance, JSArgs *args, State *state)
{
  JSValue *new = JSSTR(instance->string.ptr);
  char *str = new->string.ptr;
  int n = strlen(str);
  while(n > 0 && isspace((unsigned char)str[n - 1]))
    n--;
  str[n] = '\0';
  new->string.length = n;
  return new;
}

// String.prototype.valueOf()
JSValue *
str_proto_value_of(JSValue *instance, JSArgs *args, State *state)
{
  return instance;
}

JSValue *
bootstrap_string()
{
  JSValue *string = JSOBJ();
  JSValue *prototype = JSOBJ();

  // String
  // ------

  BUILTIN(string, "prototype", prototype);
  BUILTIN(string, "fromCharCode", JSNFUNC(&str_from_char_code));

  // String.prototype
  // ----------------

  // Properties

  BUILTIN(prototype, "constructor", JSNFUNC(&str_new)); 
  BUILTIN(prototype, "length", JSNUM(0));

  // Methods
  
  BUILTIN(prototype, "charAt", JSNFUNC(&str_proto_char_at));
  BUILTIN(prototype, "charCodeAt", JSNFUNC(&str_proto_char_code_at));
  BUILTIN(prototype, "concat", JSNFUNC(&str_proto_concat));
  BUILTIN(prototype, "indexOf", JSNFUNC(&str_proto_index_of));
  BUILTIN(prototype, "lastIndexOf", JSNFUNC(&str_proto_last_index_of));
  BUILTIN(prototype, "localeCompare", JSNFUNC(&str_proto_locale_compare));
  BUILTIN(prototype, "match", JSNFUNC(&str_proto_match));
  BUILTIN(prototype, "replace", JSNFUNC(&str_proto_replace));
  BUILTIN(prototype, "search", JSNFUNC(&str_proto_search));
  BUILTIN(prototype, "slice", JSNFUNC(&str_proto_slice));
  BUILTIN(prototype, "split", JSNFUNC(&str_proto_split));
  BUILTIN(prototype, "substr", JSNFUNC(&str_proto_substr));
  BUILTIN(prototype, "substring", JSNFUNC(&str_proto_substring));
  BUILTIN(prototype, "toLocaleLowerCase", JSNFUNC(&str_proto_to_locale_lower_case));
  BUILTIN(prototype, "toLocaleUpperCase", JSNFUNC(&str_proto_to_locale_upper_case));
  BUILTIN(prototype, "toLowerCase", JSNFUNC(&str_proto_to_lower_case));
  BUILTIN(prototype, "toString", JSNFUNC(&str_proto_to_string));
  BUILTIN(prototype, "toUpperCase", JSNFUNC(&str_proto_to_upper_case));
  BUILTIN(prototype, "trim", JSNFUNC(&str_proto_trim));
  BUILTIN(prototype, "trimLeft", JSNFUNC(&str_proto_trim_left));
  BUILTIN(prototype, "trimRight", JSNFUNC(&str_proto_trim_right));
  BUILTIN(prototype, "valueOf", JSNFUNC(&str_proto_value_of));

  return string;
}
