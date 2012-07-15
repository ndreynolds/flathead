// jslite.h

#include "../ext/uthash.h"
#include <stdlib.h>

typedef int bool;
#define true 1
#define false 0

typedef enum JLTYPE {
  T_NUMBER,
  T_STRING,
  T_BOOLEAN,
  T_ARRAY,
  T_NULL,
  T_UNDEF
} JLTYPE;

struct JLNumber {
  double val;
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

struct JLVariable {
  const char *name;
  struct JLValue *ptr;
  UT_hash_handle hh;
};

typedef struct JLVALUE {
  union {
    struct JLNumber number;
    struct JLString string;
    struct JLArray array;
    struct JLBoolean boolean;
  };
  JLTYPE type;
} JLVALUE;

JLVALUE *
alloc_val()
{
  JLVALUE *val = malloc(sizeof(JLVALUE));
  val->type = T_NULL;
}

JLVALUE *
new_number(int x)
{
  JLVALUE *val = alloc_val();
  val->number.val = x;
  val->type = T_NUMBER;
}

JLVALUE *
new_string(char *x)
{
  JLVALUE *val = alloc_val();
  val->string.ptr = x;
  val->type = T_STRING;
}

JLVALUE *
new_boolean(bool x)
{
  JLVALUE *val = alloc_val();
  val->type = T_BOOLEAN;
  val->boolean.val = x;
}

#define JLBOOL(x) new_boolean(x)
#define JLNUM(x) new_number(x)
#define JLSTR(x) new_string(x)
