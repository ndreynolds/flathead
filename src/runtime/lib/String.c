// String.c
// ----------
// String properties, methods, and prototype

#include <ctype.h>
#include <math.h>

#include "RegExp.h"
#include "String.h"

js_val *
str_new(js_val *instance, js_args *args, eval_state *state)
{
  js_val *value = ARGLEN(args) > 0 ? ARG(args, 0) : JSSTR("");
  if (state->construct)
    state->this->object.primitive = TO_STR(value);
  return TO_STR(value);
}

// String.fromCharCode(num1, ..., numN)
js_val *
str_from_char_code(js_val *instance, js_args *args, eval_state *state)
{
  fh_error(state, E_EVAL, "Unicode is not supported");
  UNREACHABLE();
}

// String.prototype.charAt(index)
js_val *
str_proto_char_at(js_val *instance, js_args *args, eval_state *state)
{
  int index = TO_NUM(ARG(args, 0))->number.val;

  if (index < 0 || index >= instance->string.length)
    return JSSTR("");
  char *str = malloc(2);
  sprintf(str, "%c", instance->string.ptr[index]);
  return JSSTR(str);
}

// String.prototype.charCodeAt(index)
js_val *
str_proto_char_code_at(js_val *instance, js_args *args, eval_state *state)
{
  fh_error(state, E_EVAL, "Unicode is not supported");
  UNREACHABLE();
}

// String.prototype.concat(string2, string3[, ..., stringN])
js_val *
str_proto_concat(js_val *instance, js_args *args, eval_state *state)
{
  js_val *new = JSSTR(instance->string.ptr);

  js_val *arg;
  char *tmp;
  unsigned int i;
  for (i = 0; i < ARGLEN(args); i++) {
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
js_val *
str_proto_index_of(js_val *instance, js_args *args, eval_state *state)
{
  js_val *search_str = TO_STR(ARG(args, 0));

  char *haystack = instance->string.ptr;
  char *needle = search_str->string.ptr;
  js_val *from = ARG(args, 1);

  long i = TO_INT(from)->number.val;
  long match = 0;

  long needle_len = strlen(needle);
  long haystack_len = strlen(haystack);

  // Searching for an empty string returns the fromIndex when less than
  // the instance string length, and the instance string length otherwise.
  if (needle_len == 0)
    return JSNUM(i <= haystack_len ? i : haystack_len);

  if (needle_len > haystack_len || i > haystack_len)
    return JSNUM(-1);

  for (; i < haystack_len; i++) {
    if (haystack[i] == needle[match])
      match++;
    else
      match = haystack[i] == needle[0];
    if (match == needle_len)
      return JSNUM(i - needle_len + 1);
  }

  return JSNUM(-1);
}

// String.prototype.lastIndexOf(searchValue[, fromIndex])
js_val *
str_proto_last_index_of(js_val *instance, js_args *args, eval_state *state)
{
  js_val *search_str = TO_STR(ARG(args, 0));
  js_val *from = ARG(args, 1);

  char *haystack = instance->string.ptr;
  char *needle = search_str->string.ptr;

  double max = IS_NUM(from) ? from->number.val : strlen(haystack) - 1;
  long match = strlen(needle) - 1;
  long i = strlen(haystack) - 1;

  long needle_len = strlen(needle);
  long haystack_len = strlen(haystack);

  if (max < 0) max = 0;
  if (max > haystack_len) max = haystack_len - 1;

  if (needle_len == 0)
    return JSNUM(max <= haystack_len ? max : haystack_len);

  if (needle_len > haystack_len)
    return JSNUM(-1);

  for (; i >= 0; i--) {
    if (haystack[i] == needle[match])
      match--;
    else
      match = needle_len - (haystack[i] == needle[0] ? 2 : 1);
    if (match < 0 && i <= max)
      return JSNUM(i);
  }

  return JSNUM(-1);
}

// String.prototype.localeCompare(compareString)
js_val *
str_proto_locale_compare(js_val *instance, js_args *args, eval_state *state)
{
  js_val *compare_str = TO_STR(ARG(args, 0));
  return JSNUM(strcmp(instance->string.ptr, compare_str->string.ptr));
}

// String.prototype.match(regexp)
js_val *
str_proto_match(js_val *instance, js_args *args, eval_state *state)
{
  js_val *regexp = ARG(args, 0);
  if (!IS_REGEXP(regexp))
    regexp = fh_new_regexp("");

  js_args *exec_args;

  bool global = fh_get_proto(regexp, "global")->boolean.val;
  if (!global) {
    exec_args = fh_new_args(instance, NULL, NULL);
    return regexp_proto_exec(regexp, exec_args, state);
  }

  fh_set(regexp, "lastIndex", JSNUM(0));

  js_val *arr = JSARR();
  int this_ind, prev_last_ind = 0, n = 0;
  bool last_match = true;
  js_val *result, *match_str;

  while (last_match) {
    exec_args = fh_new_args(instance, NULL, NULL);
    result = regexp_proto_exec(regexp, exec_args, state);
    if (IS_NULL(result)) {
      last_match = false;
      break;
    }
    this_ind = TO_NUM(fh_get(regexp, "lastIndex"))->number.val;
    if (this_ind == prev_last_ind) {
      fh_set(regexp, "lastIndex", JSNUM(this_ind + 1));
      prev_last_ind = this_ind + 1;
    }
    else {
      prev_last_ind = this_ind;
    }
    match_str = fh_get(result, "0");
    fh_set(arr, JSNUMKEY(n)->string.ptr, match_str);
    n++;
  }

  if (n == 0)
    return JSNULL();
  fh_set_len(arr, n);
  return arr;
}

// String.prototype.replace(regexp|substr, newSubStr|function)
js_val *
str_proto_replace(js_val *instance, js_args *args, eval_state *state)
{
  // TODO: replace function, replacement substitutions
  char *str = TO_STR(instance)->string.ptr;
  js_val *search_val = ARG(args, 0);
  js_val *replace_val = ARG(args, 1);

  // Not a RegExp
  if (!IS_REGEXP(search_val)) {
    char *search = TO_STR(search_val)->string.ptr;
    char *replace = TO_STR(replace_val)->string.ptr;
    return JSSTR(fh_str_replace(str, search, replace, 1));
  }

  bool global = TO_BOOL(fh_get_proto(search_val, "global"))->boolean.val,
       caseless = TO_BOOL(fh_get_proto(search_val, "ignoreCase"))->boolean.val;

  char *pattern = fh_get(search_val, "source")->string.ptr;
  char *repl = TO_STR(replace_val)->string.ptr;
  int count, *matches;

  fh_set(search_val, "lastIndex", JSNUM(0));

  unsigned i = 0;
  while (i < strlen(str)) {
    matches = fh_regexp(str, pattern, &count, i, caseless);
    if (count == 0) break;
    fh_set(search_val, "lastIndex", JSNUM(matches[1]));
    str = str_splice(str, repl, matches[0], matches[1]);
    i = matches[1] + strlen(repl) - (matches[1] - matches[0]);
    count = 0;
    free(matches);
    if (!global) break;
  }

  if (instance->string.ptr != str) {
    free(instance->string.ptr);
    instance->string.ptr = str;
  }
  return instance;
}

// String.prototype.search(regexp)
js_val *
str_proto_search(js_val *instance, js_args *args, eval_state *state)
{
  char *str = TO_STR(instance)->string.ptr;
  js_val *regexp = ARG(args, 0);

  if (!IS_REGEXP(regexp)) {
    js_val *tmp = TO_STR(regexp);
    regexp = JSRE("");
    fh_set(regexp, "source", tmp);
  }

  char *pattern = fh_get(regexp, "source")->string.ptr;
  bool caseless = fh_get_proto(regexp, "ignoreCase")->boolean.val;
  int count, *matches = fh_regexp(str, pattern, &count, 0, caseless);

  if (!matches) 
    return JSNUM(-1);

  js_val *result = JSNUM(matches[0]);
  free(matches);
  return result;
}

// String.prototype.slice(beginSlice[, endSlice])
js_val *
str_proto_slice(js_val *instance, js_args *args, eval_state *state)
{
  js_val *end_arg = ARG(args, 1);
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
js_val *
str_proto_split(js_val *instance, js_args *args, eval_state *state)
{
  // TODO: regex separators
  
  js_val *sep_arg = ARG(args, 0);
  js_val *limit_arg = ARG(args, 1);

  instance = TO_STR(instance);
  js_val *arr = JSARR();

  unsigned long limit = IS_UNDEF(limit_arg) ? 
    pow(2, 32) - 1 : TO_UINT32(limit_arg)->number.val;

  if (limit == 0) 
    return arr;

  if (IS_UNDEF(sep_arg)) { 
    fh_set(arr, "0", instance);
    fh_set_len(arr, 1);
    return arr;
  }
  else if (IS_REGEXP(sep_arg))
    return str_regexp_splitter(instance->string.ptr, sep_arg, limit);
  else
    sep_arg = TO_STR(sep_arg);

  char *split;                       // store the split substring
  char *str = instance->string.ptr;  // instance string
  char *sep = sep_arg->string.ptr;   // separator string
  int len = instance->string.length; // instance string length
  int match = 0;                     // number of chars of sep matched
  int start = 0;                     // start index to split from
  int index = 0;                     // result array index
  int n = strlen(str);               // len of str
  int i;                             // instance string index
  bool matched_last = false;         // 

  for (i = 0; i < n; i++) {
    matched_last = false;
    if (str[i] == sep[match])
      match++;
    else
      match = str[i] == sep[0];
   
    if (match == (int)strlen(sep)) {
      split = fh_str_slice(str, start, i - strlen(sep) + 1);
      fh_set(arr, JSNUMKEY(index++)->string.ptr, JSSTR(split));
      free(split);
      start = i + 1;
      match = 0;
      matched_last = true;
      if (--limit == 0) break;
    }
  }

  // Move the remaining string (possibly all of it) into the array.
  if (limit > 0) {
    if (start != len) {
      split = fh_str_slice(str, start, len);
      fh_set(arr, JSNUMKEY(index++)->string.ptr, JSSTR(split));
      free(split);
    }
    else if (matched_last && strlen(sep))
      fh_set(arr, JSNUMKEY(index++)->string.ptr, JSSTR(""));
  }

  fh_set_len(arr, index);
  return arr;
}

// String.prototype.substr(start[, length])
js_val *
str_proto_substr(js_val *instance, js_args *args, eval_state *state)
{
  long slen = instance->string.length;
  long start = TO_INT(ARG(args, 0))->number.val;
  long length = IS_UNDEF(ARG(args, 1)) ?  slen : TO_INT(ARG(args, 1))->number.val;

  if (start < 0) 
    start = MAX(start + slen, 0);

  if (MIN(MAX(length, 0), slen - start) <= 0)
    return JSSTR("");

  int end = MIN(slen, start + length);
  return JSSTR(fh_str_slice(instance->string.ptr, start, end));
}

// String.prototype.substring(start[, end])
js_val *
str_proto_substring(js_val *instance, js_args *args, eval_state *state)
{
  long len = instance->string.length;
  long start = TO_INT(ARG(args, 0))->number.val;
  int end = IS_UNDEF(ARG(args, 1)) ? len : TO_INT(ARG(args, 1))->number.val;

  start = MIN(MAX(start, 0), len);
  end = MIN(MAX(end, 0), len);

  int from = MIN(start, end);
  int to = MAX(start, end);

  return JSSTR(fh_str_slice(instance->string.ptr, from, to));
}

// String.prototype.toLocaleLowerCase()
js_val *
str_proto_to_locale_lower_case(js_val *instance, js_args *args, eval_state *state)
{
  return str_proto_to_lower_case(instance, args, state);
}

// String.prototype.toLocaleUpperCase()
js_val *
str_proto_to_locale_upper_case(js_val *instance, js_args *args, eval_state *state)
{
  return str_proto_to_upper_case(instance, args, state);
}

// String.prototype.toLowerCase()
js_val *
str_proto_to_lower_case(js_val *instance, js_args *args, eval_state *state)
{
  js_val *new = JSSTR(instance->string.ptr);
  char *str = new->string.ptr;
  int i = 0;
  while (str[i]) {
    str[i] = tolower(str[i]);
    i++;
  }
  return new;
}

// String.prototype.toString()
js_val *
str_proto_to_string(js_val *instance, js_args *args, eval_state *state)
{
  return instance;
}

// String.prototype.toUpperCase()
js_val *
str_proto_to_upper_case(js_val *instance, js_args *args, eval_state *state)
{
  js_val *new = JSSTR(instance->string.ptr);
  char *str = new->string.ptr;
  int i = 0;
  while (str[i]) {
    str[i] = toupper(str[i]);
    i++;
  }
  return new;
}

// String.prototype.trim()
js_val *
str_proto_trim(js_val *instance, js_args *args, eval_state *state)
{
  js_val *rtrimmed = str_proto_trim_right(instance, args, state);
  return str_proto_trim_left(rtrimmed, args, state);
}

// String.prototype.trimLeft()
js_val *
str_proto_trim_left(js_val *instance, js_args *args, eval_state *state)
{
  js_val *new = JSSTR(instance->string.ptr);
  char *str = new->string.ptr;
  int n = 0;
  while (str[n] != '\0' && isspace((unsigned char)str[n]))
    n++;
  memmove(str, str + n, strlen(str) - n + 1);
  new->string.length = strlen(str);
  return new;
}

// String.prototype.trimRight()
js_val *
str_proto_trim_right(js_val *instance, js_args *args, eval_state *state)
{
  js_val *new = JSSTR(instance->string.ptr);
  char *str = new->string.ptr;
  int n = strlen(str);
  while (n > 0 && isspace((unsigned char)str[n - 1]))
    n--;
  str[n] = '\0';
  new->string.length = n;
  return new;
}

// String.prototype.valueOf()
js_val *
str_proto_value_of(js_val *instance, js_args *args, eval_state *state)
{
  return instance;
}

/* Cut out the string between start and end, replacing it with the new string */
char *
str_splice(char *str, char *rep, int start, int end)
{
  char *front, *back, *tmp, *new;
  front = fh_str_slice(str, 0, start);
  back = fh_str_slice(str, end, strlen(str));
  tmp = fh_str_concat(front, rep);
  new = fh_str_concat(tmp, back);
  free(front);
  free(back);
  free(tmp);
  return new;
}

js_val *
str_regexp_splitter(char *str, js_val *regexp, int limit)
{
  char *source = TO_STR(fh_get_proto(regexp, "source"))->string.ptr;
  bool caseless = TO_BOOL(fh_get_proto(regexp, "ignoreCase"))->boolean.val;
  // int ncaps = fh_regexp_ncaptures(source);
  js_val *arr = JSARR();
  int count, *matches;
  char *tmp;
  unsigned i, j;
  bool matched_last = false;

  for (i = 0, j = 0; i < strlen(str); j++) {
    matched_last = false;
    matches = fh_regexp(str, source, &count, i, caseless);
    if (count == 0) break;
    tmp = fh_str_slice(str, i, matches[0]);
    fh_set(arr, JSNUMKEY(j)->string.ptr, JSSTR(tmp));
    i = matches[1];
    free(matches);
    matched_last = true;
  }

  if (i < strlen(str)) {
    tmp = fh_str_slice(str, i, strlen(str));
    fh_set(arr, JSNUMKEY(j++)->string.ptr, JSSTR(tmp));
  }
  else if (matched_last)
    fh_set(arr, JSNUMKEY(j++)->string.ptr, JSSTR(""));

  fh_set_len(arr, j);
  return arr;
}

js_val *
bootstrap_string()
{
  js_val *string = JSNFUNC(str_new, 1);
  js_val *prototype = JSOBJ();

  // String
  // ------

  DEF(string, "prototype", prototype);
  DEF(string, "fromCharCode", JSNFUNC(str_from_char_code, 1));

  // String.prototype
  // ----------------

  // Properties

  DEF(prototype, "constructor", JSNFUNC(str_new, 1)); 
  DEF(prototype, "length", JSNUM(0));

  // Methods
  
  DEF(prototype, "charAt", JSNFUNC(str_proto_char_at, 1));
  DEF(prototype, "charCodeAt", JSNFUNC(str_proto_char_code_at, 1));
  DEF(prototype, "concat", JSNFUNC(str_proto_concat, 1));
  DEF(prototype, "indexOf", JSNFUNC(str_proto_index_of, 1));
  DEF(prototype, "lastIndexOf", JSNFUNC(str_proto_last_index_of, 1));
  DEF(prototype, "localeCompare", JSNFUNC(str_proto_locale_compare, 1));
  DEF(prototype, "match", JSNFUNC(str_proto_match, 1));
  DEF(prototype, "replace", JSNFUNC(str_proto_replace, 2));
  DEF(prototype, "search", JSNFUNC(str_proto_search, 1));
  DEF(prototype, "slice", JSNFUNC(str_proto_slice, 2));
  DEF(prototype, "split", JSNFUNC(str_proto_split, 2));
  DEF(prototype, "substr", JSNFUNC(str_proto_substr, 2));
  DEF(prototype, "substring", JSNFUNC(str_proto_substring, 2));
  DEF(prototype, "toLocaleLowerCase", JSNFUNC(str_proto_to_locale_lower_case, 0));
  DEF(prototype, "toLocaleUpperCase", JSNFUNC(str_proto_to_locale_upper_case, 0));
  DEF(prototype, "toLowerCase", JSNFUNC(str_proto_to_lower_case, 0));
  DEF(prototype, "toString", JSNFUNC(str_proto_to_string, 0));
  DEF(prototype, "toUpperCase", JSNFUNC(str_proto_to_upper_case, 0));
  DEF(prototype, "trim", JSNFUNC(str_proto_trim, 0));
  DEF(prototype, "trimLeft", JSNFUNC(str_proto_trim_left, 0));
  DEF(prototype, "trimRight", JSNFUNC(str_proto_trim_right, 0));
  DEF(prototype, "valueOf", JSNFUNC(str_proto_value_of, 0));

  fh_attach_prototype(prototype, fh->function_proto);

  return string;
}
