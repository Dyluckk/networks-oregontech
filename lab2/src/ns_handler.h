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
/* inserts a service into the table */
request_t* define_service(request_t* decoded_request, ns_lookup_table& lookup_table, int keep_alive_time);
/* looks up a port number for the corresponding service */
request_t* lookup_service_port(request_t* decoded_request, ns_lookup_table& lookup_table);
/* removes service from the table */
request_t* remove_service(request_t* decoded_request, ns_lookup_table& lookup_table);
