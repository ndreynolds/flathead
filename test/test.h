#include <assert.h>

int tests;

#define TEST_START(title) do { \
  printf("%s\n\n", title); \
  int tests = 0; \
} while (0)

#define TEST(exp) do { \
  assert(exp); tests++; \
  printf(" ✓ %s\n", __func__); \
} while (0)

#define TEST_SUMMARY(i)  printf("\n%d tests passed. 0 failures.\n", i)
