#include <iostream>
#include <time.h>
#include <string>
#include <unordered_map>

#include "nameserver.h"
#include "encode.h"

using std::string;
using std::cout;
using std::endl;
using std::unordered_map;

/* struct for service info */
typedef struct {
    int port;        // the service names port#
    time_t timeout;  // the time in which the entry will expire
} service_info_t;

/* table used for service_name->port lookups */
typedef unordered_map<std::string, service_info_t> ns_lookup_table;
/* get keep alive time for service */
time_t get_keep_alive_time(int keep_alive_time);
/* updates a services timeout */
request_t* update_service(request_t* decoded_request, ns_lookup_table& lookup_table, int keep_alive_time);
/* send keep alive signal for a service */
request_t* keep_alive(request_t* decoded_request, ns_lookup_table& lookup_table, int keep_alive_time);
/* inserts a service into the table */
request_t* define_service(request_t* decoded_request, ns_lookup_table& lookup_table, int keep_alive_time);
/* looks up a port number for the corresponding service */
request_t* lookup_service_port(request_t* decoded_request, ns_lookup_table& lookup_table);
/* removes service from the table */
request_t* remove_service(request_t* decoded_request, ns_lookup_table& lookup_table);
/* removes timedout services */
void clear_timedout_services( ns_lookup_table& lookup_table, int keep_alive_time);
/* generates a response for a request that failed due to max ports being reached */
request_t* create_ports_full_response(request_t* decoded_request);
/* generates a response for a bad request */
request_t* create_bad_request_response(request_t* decoded_request);
/* generates a response for a shutdown request */
request_t* create_shutdown_request_response(request_t* decoded_request);
