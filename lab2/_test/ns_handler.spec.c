#include "../lib/unity/unity.h"
#include "../src/nameserver.h"
#include "../src/ns_handler.h"

#include <iostream>

using std::string;
using std::cout;
using std::endl;

/* @describe: test used to ensure testing framework is working properly */
void sanity_test() {
  int a = 1;
  TEST_ASSERT( a == 1 ); //this one will pass
}

/* @describe: should pass when epoch+keep_alive is returned */
void get_keep_alive_time_test() {
  int keep_alive = 200;
  TEST_ASSERT( (time(0) + keep_alive) == get_keep_alive_time(keep_alive) ); //this one will pass
}

/* @describe: should pass when entry is updated in table & request respons is a success */
void update_service_test() {
  ns_lookup_table lookup_table;
  int keep_alive_time = 300;
  const int DESIRED_PORT = 30000;
  request_t* request = new request_t();
  /* initialize request */
  request->msg_type = KEEP_ALIVE;
  strncpy(request->service_name, "www.website.com", sizeof(request->service_name));
  request->port = DESIRED_PORT;

  /* send to update handler */
  request = update_service(request, lookup_table, keep_alive_time);

  /* ensure entry is in table */
  string service_to_find(request->service_name);
  auto search = lookup_table.find(service_to_find);
  /* found */
  if(search != lookup_table.end()) {
    TEST_ASSERT(true);
  }
  /* not found */
  else {
    TEST_ASSERT(false);
  }

  /* check if request has correct values */
  TEST_ASSERT(request->msg_type == RESPONSE);
  TEST_ASSERT(request->status == SUCCESS);
  TEST_ASSERT(request->port == DESIRED_PORT);

  delete(request);
}

/* @describe: should pass when a service is successfully put into the lookup_table */
void define_service_valid_test() {
  ns_lookup_table lookup_table;
  int keep_alive_time = 300;
  const int DESIRED_PORT = 30000;
  request_t* request = new request_t();
  /* initialize request */
  request->msg_type = KEEP_ALIVE;
  strncpy(request->service_name, "www.website.com", sizeof(request->service_name));
  request->port = DESIRED_PORT;

  /* send request to define handler */
  request = define_service(request, lookup_table, keep_alive_time);

  /* ensure entry is in table */
  string service_to_find(request->service_name);
  auto search = lookup_table.find(service_to_find);
  /* found */
  if(search != lookup_table.end()) {
    TEST_ASSERT(true);
  }
  /* not found */
  else {
    TEST_ASSERT(false);
  }

  /* check if request has correct values */
  TEST_ASSERT(request->msg_type == RESPONSE);
  TEST_ASSERT(request->status == SUCCESS);
  TEST_ASSERT(request->port == DESIRED_PORT);

  delete(request);
}

/* @describe: should pass when a failure occurs due to the service already being defined */
void define_service_invalid_test() {
  ns_lookup_table lookup_table;
  int keep_alive_time = 300;
  const int DESIRED_PORT = 55555;
  request_t* request = new request_t();
  /* initialize request */
  request->msg_type = KEEP_ALIVE;
  strncpy(request->service_name, "www.website.com", sizeof(request->service_name));
  request->port = DESIRED_PORT;

  /* send request to define handler */
  request = define_service(request, lookup_table, keep_alive_time);

  /* ensure entry is in table */
  string service_to_find(request->service_name);
  auto search = lookup_table.find(service_to_find);
  /* found */
  if(search != lookup_table.end()) {
    TEST_ASSERT(true);
  }
  /* not found */
  else {
    TEST_ASSERT(false);
  }

  /* send request to define handler to trigger invalid */
  define_service(request, lookup_table, keep_alive_time);

  /* check if request has correct values */
  TEST_ASSERT(request->msg_type == RESPONSE);
  TEST_ASSERT(request->status == SERVICE_IN_USE);

  delete(request);
}

/* @describe: should pass when the desired port of a service is returned */
void lookup_service_port_valid_test() {
  ns_lookup_table lookup_table;
  int keep_alive_time = 300;
  const int DESIRED_PORT = 55555;
  request_t* request = new request_t();
  /* initialize request */
  request->msg_type = KEEP_ALIVE;
  strncpy(request->service_name, "www.website.com", sizeof(request->service_name));
  request->port = DESIRED_PORT;

  /* send request to define handler */
  request = define_service(request, lookup_table, keep_alive_time);

  /* lookup_service_port */
  request->msg_type = LOOKUP_PORT;
  strncpy(request->service_name, "www.website.com", sizeof(request->service_name));
  request->port = 0;

  request = lookup_service_port(request, lookup_table);

  TEST_ASSERT(request->msg_type == RESPONSE);
  TEST_ASSERT(request->status == SUCCESS);
  TEST_ASSERT(request->port == DESIRED_PORT);
}

/* @describe: should pass when the desired port of a service isn't returned due to a timeout */
void lookup_service_port_timedout_service_test() {
  ns_lookup_table lookup_table;
  int keep_alive_time = -300;
  const int DESIRED_PORT = 55555;
  request_t* request = new request_t();
  /* initialize request */
  request->msg_type = KEEP_ALIVE;
  strncpy(request->service_name, "www.website.com", sizeof(request->service_name));
  request->port = DESIRED_PORT;

  /* send request to define handler */
  request = define_service(request, lookup_table, keep_alive_time);

  /* lookup_service_port */
  request->msg_type = LOOKUP_PORT;
  strncpy(request->service_name, "www.website.com", sizeof(request->service_name));
  request->port = 0;

  request = lookup_service_port(request, lookup_table);

  TEST_ASSERT(request->msg_type == RESPONSE);
  TEST_ASSERT(request->status == SERVICE_NOT_FOUND);
}

/* @describe: should pass when the a failure occues due to the service not existing */
void lookup_service_port_non_existing_service_test() {
  ns_lookup_table lookup_table;
  request_t* request = new request_t();
  /* initialize request */
  request->msg_type = LOOKUP_PORT;
  strncpy(request->service_name, "www.website.com", sizeof(request->service_name));
  request->port = 0;

  /* lookup_service_port */
  request = lookup_service_port(request, lookup_table);

  TEST_ASSERT(request->msg_type == RESPONSE);
  TEST_ASSERT(request->status == SERVICE_NOT_FOUND);
}

/* @describe: should pass when the a failure occues due to not passing in port 0 */
void lookup_service_port_invalid_port_arg_test() {
  ns_lookup_table lookup_table;
  const int DESIRED_PORT = 55555;
  request_t* request = new request_t();
  /* initialize request */

  /* lookup_service_port */
  request->msg_type = LOOKUP_PORT;
  strncpy(request->service_name, "www.website.com", sizeof(request->service_name));
  request->port = DESIRED_PORT;

  request = lookup_service_port(request, lookup_table);

  TEST_ASSERT(request->msg_type == RESPONSE);
  TEST_ASSERT(request->status == INVALID_ARG);
}

/* @describe: should pass when the service is removed from the lookup_table */
void remove_service_test_valid() {
  ns_lookup_table lookup_table;
  int keep_alive_time = 300;
  const int DESIRED_PORT = 55555;
  request_t* request = new request_t();
  /* initialize request */
  request->msg_type = DEFINE_PORT;
  strncpy(request->service_name, "www.website.com", sizeof(request->service_name));
  request->port = DESIRED_PORT;

  /* send request to define handler */
  request = define_service(request, lookup_table, keep_alive_time);

  /* lookup_service_port */
  request->msg_type = CLOSE_PORT;
  strncpy(request->service_name, "www.website.com", sizeof(request->service_name));
  request->port = DESIRED_PORT;

  request = remove_service(request, lookup_table);

  TEST_ASSERT(request->msg_type == RESPONSE);
  TEST_ASSERT(request->status == SUCCESS);

  /* ensure entry is not in table */
  string service_to_find(request->service_name);
  auto search = lookup_table.find(service_to_find);
  /* found */
  if(search != lookup_table.end()) {
    TEST_ASSERT(false);
  }
  /* not found */
  else {
    TEST_ASSERT(true);

  }
}

/* @describe: should pass when a failure occurs due to a service not existing */
void remove_service_test_invalid() {
  ns_lookup_table lookup_table;
  request_t* request = new request_t();

  /* lookup_service_port */
  request->msg_type = CLOSE_PORT;
  strncpy(request->service_name, "www.website.com", sizeof(request->service_name));
  request->port = 0;

  request = remove_service(request, lookup_table);

  TEST_ASSERT(request->msg_type == RESPONSE);
  TEST_ASSERT(request->status == SERVICE_NOT_FOUND);
}

/* @describe: should pass when the entries are removed that had timedout */
void clear_timedout_services_test() {
    ns_lookup_table lookup_table;
    int keep_alive_time = -1000;
    const int DESIRED_PORT = 55555;
    request_t* request = new request_t();
    /* initialize request */
    request->msg_type = KEEP_ALIVE;
    strncpy(request->service_name, "www.website0.com", sizeof(request->service_name));
    request->port = DESIRED_PORT;

    /* send requests to define handler */
    request = define_service(request, lookup_table, keep_alive_time);

    strncpy(request->service_name, "www.website1.com", sizeof(request->service_name));
    request = define_service(request, lookup_table, keep_alive_time);

    strncpy(request->service_name, "www.website2.com", sizeof(request->service_name));
    request = define_service(request, lookup_table, keep_alive_time);

    strncpy(request->service_name, "www.website3.com", sizeof(request->service_name));
    request = define_service(request, lookup_table, keep_alive_time);

    strncpy(request->service_name, "www.website4.com", sizeof(request->service_name));
    request = define_service(request, lookup_table, keep_alive_time);

    strncpy(request->service_name, "www.website5.com", sizeof(request->service_name));
    request = define_service(request, lookup_table, keep_alive_time);

    strncpy(request->service_name, "www.website6.com", sizeof(request->service_name));
    request = define_service(request, lookup_table, keep_alive_time);

    clear_timedout_services(lookup_table, keep_alive_time);

    TEST_ASSERT(lookup_table.size() == 0);
}

/* @describe: should pass when the entries are removed that had timedout */
void clear_timedout_services_test_no_removals() {
    ns_lookup_table lookup_table;
    int keep_alive_time = 1000;
    const int DESIRED_PORT = 55555;
    request_t* request = new request_t();
    /* initialize request */
    request->msg_type = KEEP_ALIVE;
    strncpy(request->service_name, "www.website0.com", sizeof(request->service_name));
    request->port = DESIRED_PORT;

    /* send requests to define handler */
    request = define_service(request, lookup_table, keep_alive_time);

    strncpy(request->service_name, "www.website1.com", sizeof(request->service_name));
    request = define_service(request, lookup_table, keep_alive_time);

    strncpy(request->service_name, "www.website2.com", sizeof(request->service_name));
    request = define_service(request, lookup_table, keep_alive_time);

    strncpy(request->service_name, "www.website3.com", sizeof(request->service_name));
    request = define_service(request, lookup_table, keep_alive_time);

    strncpy(request->service_name, "www.website4.com", sizeof(request->service_name));
    request = define_service(request, lookup_table, keep_alive_time);

    strncpy(request->service_name, "www.website5.com", sizeof(request->service_name));
    request = define_service(request, lookup_table, keep_alive_time);

    strncpy(request->service_name, "www.website6.com", sizeof(request->service_name));
    request = define_service(request, lookup_table, keep_alive_time);

    clear_timedout_services(lookup_table, keep_alive_time);

    TEST_ASSERT(lookup_table.size() == 7);
}

int main(int argc, char** argv) {
  UNITY_BEGIN();
    RUN_TEST(sanity_test);
    RUN_TEST(get_keep_alive_time_test);
    RUN_TEST(update_service_test);
    RUN_TEST(define_service_valid_test);
    RUN_TEST(define_service_invalid_test);
    RUN_TEST(lookup_service_port_valid_test);
    RUN_TEST(lookup_service_port_timedout_service_test);
    RUN_TEST(lookup_service_port_non_existing_service_test);
    RUN_TEST(lookup_service_port_invalid_port_arg_test);
    RUN_TEST(remove_service_test_valid);
    RUN_TEST(remove_service_test_invalid);
    RUN_TEST(clear_timedout_services_test);
    RUN_TEST(clear_timedout_services_test_no_removals);
  return UNITY_END();
}
