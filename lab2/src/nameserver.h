#pragma once

//*******************************************************
// nameservice.h
//
// Type definitions for CST 415 Name Service Server interface
//
// Author: Philip Howard phil.howard@oit.edu
//
// Date: 9/25/2017
//
#include <stdint.h>

#define MAX_SERVICE_NAME_LEN 49

typedef struct
{
    uint16_t port;                                  // net byte order port num
    int8_t msg_type;                                // see #defines below
    int8_t status;                                  // see #defines below
    char service_name[MAX_SERVICE_NAME_LEN + 1];    // null terminated name
} request_t;

// Valid msg_type values
#define DEFINE_PORT     1
#define LOOKUP_PORT     2
#define KEEP_ALIVE      3
#define CLOSE_PORT      4
#define RESPONSE        5
#define STOP            6

// Valid status values
#define SUCCESS           0
#define SERVICE_IN_USE    1
#define SERVICE_NOT_FOUND 2
#define ALL_PORTS_BUSY    3
#define INVALID_ARG       4
#define UNDEFINED_ERROR   5
