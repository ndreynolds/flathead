// jslite.h

#include "../ext/uthash.h"

typedef int bool;
#define true 1
#define false 0

enum JLType {
  JL_NUMBER,
  JL_STRING,
  JL_BOOLEAN,
  JL_ARRAY,
  JL_NULL,
  JL_UNDEF
};

struct JLNumber {
  double value;
};

struct JLString {
  long len;
  char *ptr;
};

struct JLBoolean {
  bool value;
};

struct JLArray {
  long len;
  void *ptr;
};

struct JLValue {
  union {
    struct JLNumber number;
    struct JLString string;
    struct JLArray array;
    struct JLBoolean boolean;
  } as;
  enum JLType type;
};

struct JLVariable {
  const char *name;
  struct JLValue *ptr;
  UT_hash_handle hh;
};
