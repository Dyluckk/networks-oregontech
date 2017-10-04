#define MUNIT_ENABLE_ASSERT_ALIASES
#include "../unit/munit.h"
#include "../lab1/encode.h"
#include "../lab1/nameserver.h"
#include "../lab1/readline.h"

#if defined(_MSC_VER)
#pragma warning(disable: 4127)
#endif

/*
* desribe: should pass when a valid msg_type and status
*           are passed through the request struct
*/
static MunitResult
invalid_test1() {
  request_t *request = malloc(sizeof(request_t));
  request->msg_type = 1;
  request->status = 0;

  int validated_request = is_invalid(request);
  assert_true(validated_request == 0);

  return MUNIT_OK;
}

/*
*  desribe: should pass when a valid msg_type and status
*           are passed through the request struct
*/
static MunitResult
invalid_test2() {
  request_t *request = malloc(sizeof(request_t));
  request->msg_type = 9;
  request->status = 9;

  int validated_request = is_invalid(request);
  assert_true(validated_request == 1);

  return MUNIT_OK;
}

/* functions to run */
static MunitTest test_suite_tests[] = {
  { (char*) "encode/invalid/valid-request", invalid_test1, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
  { (char*) "encode/invalid/invalid-request", invalid_test2, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
  /* REQUIRED NULL TERMINATOR */
  { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
};

static const MunitSuite test_suite = {
  (char*) "Lab 1 Unit Tests",
  test_suite_tests,
  NULL,
  0,
  MUNIT_SUITE_OPTION_NONE
};

#include <stdlib.h>
#include <fcntl.h>

int main(int argc, char* argv[]) {

  int fd;
  char buff[4];

  //TODO move to unit
  /*opening the file in read-only mode*/
  // if ((fd = open_blocks(argv[1])) < 0) {
  //     perror("Problem in opening the file");
  //     exit(1);
  // }
  //
  // while(readline(buff, sizeof(buff), fd) != 0){
  //   printf("\nreadline returned ---> %s\n", buff);
  // }

  /* Use µnit here. */
  return munit_suite_main(&test_suite, (void*) "µnit", argc, argv);
}
