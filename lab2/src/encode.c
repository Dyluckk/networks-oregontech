//******************************************************
// Declare functions for opening and reading files a buffer at a time
//
// Author: Zachary Wentworth
// Email:  zachary.wentworth@oit.edu
//
//******************************************************
#include "encode.h"
#include <stdio.h>
#include <signal.h>

// *************************************
// See the header file for documentation
void* encode(request_t* request, void* buff) {
    /* check if request and buff are pointing to the same request_t */
    if(buff != request) {
        /* copy request into buff */
        ((request_t*)buff)->port = request->port;
        ((request_t*)buff)->msg_type = request->msg_type;
        ((request_t*)buff)->status = request->status;
        // strncpy(((request_t*)buff)->service_name, request->service_name, MAX_SERVICE_NAME_LEN);
    }
    /* flip port h2ns */
    ((request_t*)buff)->port = htons(((request_t*)buff)->port);

    /* NULL fill the service_name of request */
    int null_pos = 0;
    for(unsigned int i = 0; i < sizeof(request->service_name); i++) {
        if(request->service_name[i] == '\0') {
            printf("%d\n", i);
            null_pos = i;
            i = sizeof(request->service_name);
        }
    }

    char null_filled_service_name[MAX_SERVICE_NAME_LEN];
    strncpy(null_filled_service_name, request->service_name, MAX_SERVICE_NAME_LEN);

    if(null_pos != 0) {
        memset(null_filled_service_name+null_pos, '\0', sizeof(null_filled_service_name)-null_pos);
    }

    memcpy(((request_t*)buff)->service_name, null_filled_service_name, sizeof(null_filled_service_name)+1);

    /* check status, return NULL to indicate ERROR */
    if(((request_t*)buff)->msg_type < DEFINE_PORT || ((request_t*)buff)->msg_type > STOP) return NULL;
    /* check message type, return NULL to indicate ERROR */
    if(((request_t*)buff)->status < SUCCESS || ((request_t*)buff)->status > UNDEFINED_ERROR) return NULL;

    return ((request_t*)buff);
}

// *************************************
// See the header file for documentation
int is_invalid(request_t* request) {
    int validCheck = 0;

    if(request->msg_type < DEFINE_PORT || request->msg_type > STOP) return 1;
    if(request->status < SUCCESS || request->status > UNDEFINED_ERROR) return 1;

    /* checks if the service_name is null filled */
    char * null_position;
    int ptr_offset = 0;
    null_position = (char*) memchr (request->service_name, '\0', sizeof(request->service_name));
    if (null_position != NULL) {
        ptr_offset = null_position-request->service_name;
        for(unsigned int i = ptr_offset; i < sizeof(request->service_name); i++) {
            if(request->service_name[i] != '\0') return 1;
        }
    }

    return validCheck;
}

// *************************************
// See the header file for documentation
request_t* decode(void* buff, request_t* decoded) {
    // raise(SIGINT);

    /* check if request and buff are pointing to the same request_t */
    if(buff != decoded) {
        /* copy request into buff */
        ((request_t*)buff)->port = decoded->port;
        ((request_t*)buff)->msg_type = decoded->msg_type;
        ((request_t*)buff)->status = decoded->status;
        strncpy(((request_t*)buff)->service_name, decoded->service_name, MAX_SERVICE_NAME_LEN);
    }

    /* flip port */
    ((request_t*)buff)->port = ntohs(((request_t*)buff)->port);

    /* check if valid */
    int valid = is_invalid(((request_t*)buff));
    if(valid > 0) return NULL;

    return ((request_t*)buff);
}
