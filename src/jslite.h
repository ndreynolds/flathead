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

typedef struct JLVALUE {
  union {
    struct JLNumber number;
    struct JLString string;
    struct JLArray array;
    struct JLBoolean boolean;
  };
  JLTYPE type;
} JLVALUE;

struct JLVariable {
  const char *name;
  JLVALUE *ptr;
  UT_hash_handle hh;
};

JLVALUE * new_number(double);
JLVALUE * new_string(char *);
JLVALUE * new_null(void);
JLVALUE * new_boolean(bool);
char * jl_typeof(JLVALUE *);
char * jl_str_concat(char *, char*);

void jl_debug_value(JLVALUE *);

#define JLBOOL(x) new_boolean(x)
#define JLNUM(x) new_number(x)
#define JLSTR(x) new_string(x)
#define JLNULL() new_null()
