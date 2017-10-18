#include "ns_handler.h"

// *************************************
// See the header file for documentation
time_t get_keep_alive_time(int keep_alive_time) {
    /* get current EPOCH */
    time_t current_epoch = time(0);

    /* value is the experation of the service */
    current_epoch += keep_alive_time;
    return current_epoch;
}

// *************************************
// See the header file for documentation
request_t* update_service(request_t       *decoded_request,
                          ns_lookup_table& lookup_table,
                          int              keep_alive_time) {
    service_info_t new_service;

    new_service.timeout = get_keep_alive_time(keep_alive_time);
    new_service.port    = decoded_request->port;

    /* convert cstring to string */
    string service_name(decoded_request->service_name);

    /* whether or not the service exists already, insert into the table */
    lookup_table.insert({ service_name, new_service });

    /* change flags for response */
    decoded_request->msg_type = RESPONSE;
    decoded_request->status   = SUCCESS;
    return decoded_request;
}

request_t* keep_alive(request_t       *decoded_request,
                      ns_lookup_table& lookup_table,
                      int              keep_alive_time) {
    /* check if exists */
    string service_to_find(decoded_request->service_name);

    unordered_map<std::string,
                  service_info_t>::const_iterator search = lookup_table.find(
        service_to_find);

    if (search != lookup_table.end()) {
        /* found, check if port matches */
        service_info_t lookup_values;
        lookup_values = search->second;

        /* if timeout, respond with an error
         * due to the service already being defined */
        if (lookup_values.port == decoded_request->port) {
            decoded_request = update_service(decoded_request,
                                             lookup_table,
                                             keep_alive_time);
        } else {
            /* wrong port return */
            decoded_request->msg_type = RESPONSE;
            decoded_request->status   = INVALID_ARG;
        }
    }
    else {
        /* not found, return */
        decoded_request->msg_type = RESPONSE;
        decoded_request->status   = INVALID_ARG;
    }
    return decoded_request;
}

// *************************************
// See the header file for documentation
request_t* define_service(request_t       *decoded_request,
                          ns_lookup_table& lookup_table,
                          int              keep_alive_time) {
    /* check if exists */
    string service_to_find(decoded_request->service_name);

    unordered_map<std::string,
                  service_info_t>::const_iterator search = lookup_table.find(
        service_to_find);

    if (search != lookup_table.end()) {
        /* found, check if service has timedout */
        service_info_t lookup_values;
        lookup_values = search->second;

        /* if not timedout, respond with an error
         * due to the service already being defined */
        time_t current_epoch = time(0);

        if (lookup_values.timeout > current_epoch) {
            /* change flags for response */
            decoded_request->msg_type = RESPONSE;
            decoded_request->status   = SERVICE_IN_USE;

            /* service_name found, but timedout, so it can be re-defined */
        } else {
            decoded_request = update_service(decoded_request,
                                             lookup_table,
                                             keep_alive_time);
        }
    }
    else {
        /* not found, define service and port in table */
        decoded_request = update_service(decoded_request,
                                         lookup_table,
                                         keep_alive_time);
    }

    return decoded_request;
}

// *************************************
// See the header file for documentation
request_t* lookup_service_port(request_t       *decoded_request,
                               ns_lookup_table& lookup_table) {
    if (decoded_request->port != 0) {
        decoded_request->msg_type = RESPONSE;
        decoded_request->status   = INVALID_ARG;
        return decoded_request;
    }

    service_info_t lookup_values;
    unordered_map<std::string,
                  service_info_t>::const_iterator search = lookup_table.find(
        decoded_request->service_name);

    /* the found port */
    if (search != lookup_table.end()) {
        lookup_values = search->second;

        /* check if timedout */
        time_t current_epoch = time(0);

        if (lookup_values.timeout < current_epoch) {
            decoded_request->msg_type = RESPONSE;
            decoded_request->status   = SERVICE_NOT_FOUND;
            string service_to_remove = decoded_request->service_name;

            /* remove from table */
            lookup_table.erase(service_to_remove);
        }
        else {
            decoded_request->port     = lookup_values.port;
            decoded_request->msg_type = RESPONSE;
            decoded_request->status   = SUCCESS;
        }
    }

    /* service not found */
    else {
        decoded_request->msg_type = RESPONSE;
        decoded_request->status   = SERVICE_NOT_FOUND;
    }
    return decoded_request;
}

// *************************************
// See the header file for documentation
request_t* remove_service(request_t       *decoded_request,
                          ns_lookup_table& lookup_table) {
    string service_name_to_remove(decoded_request->service_name);

    service_info_t lookup_values;

    unordered_map<std::string,
                  service_info_t>::const_iterator search = lookup_table.find(
        decoded_request->service_name);

    /* the found port */
    if (search != lookup_table.end()) {
        lookup_values = search->second;

        /* check if port matches */

        // printf("%d\n", decoded_request->port);
        if (lookup_values.port == decoded_request->port) {
            lookup_table.erase(service_name_to_remove);
            decoded_request->msg_type = RESPONSE;
            decoded_request->status   = SUCCESS;

            /* error on non matching port */
        } else {
            decoded_request->msg_type = RESPONSE;
            decoded_request->status   = INVALID_ARG;
        }
    }

    /* service not found */
    else {
        decoded_request->msg_type = RESPONSE;
        decoded_request->status   = SERVICE_NOT_FOUND;
    }
    return decoded_request;
}

/* function used to wipe timedout entries */
void clear_timedout_services(ns_lookup_table& lookup_table,
                             int              keep_alive_time) {
    unordered_map<std::string,
                  service_info_t>::iterator it = lookup_table.begin();

    while (it != lookup_table.end())
    {
        time_t current_epoch = time(0);

        if ((it->second).timeout < current_epoch) {
            string service_name_to_remove = it->first;
            it++;
            lookup_table.erase(service_name_to_remove);
        } else {
            it++;
        }
    }
}

// *************************************
// See the header file for documentation
request_t* create_ports_full_response(request_t *decoded_request) {
    decoded_request->msg_type = RESPONSE;
    decoded_request->status   = ALL_PORTS_BUSY;
    return decoded_request;
}

// *************************************
// See the header file for documentation
request_t* create_bad_request_response(request_t *decoded_request) {
    decoded_request->msg_type = RESPONSE;
    decoded_request->status   = INVALID_ARG;
    return decoded_request;
}

// *************************************
// See the header file for documentation
request_t* create_shutdown_request_response(request_t *decoded_request) {
    decoded_request->msg_type = RESPONSE;
    decoded_request->status   = SUCCESS;
    return decoded_request;
}
