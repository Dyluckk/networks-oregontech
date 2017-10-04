#define MUNIT_ENABLE_ASSERT_ALIASES
#include "../unit/munit.h"
#include "../lab1/encode.h"
#include "../lab1/nameserver.h"

#if defined(_MSC_VER)
#pragma warning(disable: 4127)
#endif

static MunitResult
test1() {
  assert_true(2 == 2);
  return MUNIT_OK;
}

static MunitResult
test1() {
  assert_true(2 == 2);
  return MUNIT_OK;
}

/* functions to run */
static MunitTest test_suite_tests[] = {
  { (char*) "/lab1/test1", test1, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
  { (char*) "/lab1/test2", test2, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
  /* REQUIRED NULL TERMINATOR */
  { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
};

static const MunitSuite test_suite = {
  (char*) "Test Suite Name",
  test_suite_tests,
  NULL,
  0,
  MUNIT_SUITE_OPTION_NONE
};

#include <stdlib.h>
int main(int argc, char* argv[]) {
  /* Use µnit here. */
  return munit_suite_main(&test_suite, (void*) "µnit", argc, argv);
}
