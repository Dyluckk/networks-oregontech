#define MUNIT_ENABLE_ASSERT_ALIASES
#include "../unit/munit.h"
#include "../lab1/encode.h"
#include "../lab1/nameserver.h"
#include "../lab1/readline.h"

#if defined(_MSC_VER)
#pragma warning(disable: 4127)
#endif

/*
* desribe: should pass when buff == request after encode
*/
static MunitResult
encode_success() {
  request_t* buff = malloc(sizeof(request_t));
  request_t* request = malloc(sizeof(request_t));
  request->msg_type = 1;
  request->status = 0;
  request->port = 22;
  strncpy(request->service_name, "Hello", sizeof(request->service_name));

  buff = encode(request, buff);
  assert_true(buff == request);

  return MUNIT_OK;
}

/*
* desribe: should pass when buff == null after a failed encode
*/
static MunitResult
encode_fail() {
  request_t* buff = malloc(sizeof(request_t));
  request_t* request = malloc(sizeof(request_t));
  request->msg_type = 1;
  request->status = 11;
  request->port = 22;
  strncpy(request->service_name, "Hello", sizeof(request->service_name));

  buff = encode(request, buff);
  assert_true(buff == NULL);

  return MUNIT_OK;
}

/*
* desribe: should pass when buff == request after a successful decode
*/
static MunitResult
decode_success() {
  request_t* buff = malloc(sizeof(request_t));
  request_t* request = malloc(sizeof(request_t));
  request->msg_type = 1;
  request->status = 1;
  request->port = 22;
  strncpy(request->service_name, "Hello", sizeof(request->service_name));

  buff = decode(buff, request);
  assert_true(buff == request);

  return MUNIT_OK;
}

/*
* desribe: should pass when buff == null after a failed decode
*/
static MunitResult
decode_fail() {
  request_t* buff = malloc(sizeof(request_t));
  request_t* request = malloc(sizeof(request_t));
  request->msg_type = 1;
  request->status = 11;
  request->port = 22;
  strncpy(request->service_name, "Hello", sizeof(request->service_name));

  buff = decode(buff, request);
  assert_true(buff == NULL);

  return MUNIT_OK;
}

/*
* desribe: should pass when a valid msg_type and status
*           are passed through the request struct
*/
static MunitResult
invalid_test_success() {
  request_t* request = malloc(sizeof(request_t));
  request->msg_type = 1;
  request->status = 0;
  request->port = 22;
  strncpy(request->service_name, "Hello", sizeof(request->service_name));

  int validated_request = is_invalid(request);

  assert_true(validated_request == 0);

  return MUNIT_OK;
}

/*
*  desribe: should pass when an invalid status is passed
*/
static MunitResult
invalid_status_test() {
  request_t* request = malloc(sizeof(request_t));
  request->msg_type = 1;
  request->status = 9;
  request->port = 22;
  strncpy(request->service_name, "Hello", sizeof(request->service_name));

  int validated_request = is_invalid(request);
  assert_true(validated_request == 1);

  return MUNIT_OK;
}

/*
*  desribe: should pass when an invalid msg_type is passed
*/
static MunitResult
invalid_msg_type_test() {
  request_t* request = malloc(sizeof(request_t));
  request->msg_type = 20;
  request->status = 2;
  request->port = 22;
  strncpy(request->service_name, "Hello", sizeof(request->service_name));

  int validated_request = is_invalid(request);
  assert_true(validated_request == 1);

  return MUNIT_OK;
}

/*
* desribe: should pass when the expected lines are returned
*/
static MunitResult
readline_test() {
  int fd;
  char* file_name = "my_file";
  char buff[32];

  char expectedLines[8][12] = {"hello_san1\n", "hello_san2\n", "hello_san3\n",
                               "hello_san4\n", "hello_san5\n", "hello_san6\n",
                               "hello_san7\n", "hello_san8\n"};

  if ((fd = open_blocks(file_name)) < 0) {
      perror("Problem in opening the file");
      exit(1);
  }

  int lines_read = 0;
  while(readline(buff, sizeof(buff), fd) != 0){
    assert_true(strcmp(buff, expectedLines[lines_read]) == 0);
    lines_read++;
  }

  close_blocks(fd);
  return MUNIT_OK;
}

/* functions to run */
static MunitTest test_suite_tests[] = {
  { (char*) "encode/invalid/valid-request", invalid_test_success, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
  { (char*) "encode/invalid/invalid_status_test", invalid_status_test, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
  { (char*) "encode/invalid/invalid_status_test", invalid_msg_type_test, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
  { (char*) "encode/encode/encode_success", encode_success, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
  { (char*) "encode/encode/encode_fail", encode_fail, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
  { (char*) "encode/decode/decode_success", decode_success, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
  { (char*) "encode/decode/decode_fail", decode_fail, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
  { (char*) "encode/readline/test1", readline_test, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
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
  /* Use µnit here. */
  return munit_suite_main(&test_suite, (void*) "µnit", argc, argv);
}
