#pragma once
//******************************************************
// getport.h
//
// Author: Philip Howard
// Date: 10/15/2017
//
// Functions for requesting ports and doing lookups on assigned ports.
//

// Set the host name and port number for the name servier
// host: null terminated string indicating the nameserver's host
//       A value of NULL will use the default "unix.cset.oit.edu"
// port: The port number for the nameserver.
//       A value of zero will use the default port 35000
int setup_ns(const char *host, int port);

/* was missing from .h */
int do_request(const char *service_name, int type, int port);

// Define the port number being used by a service
// service_name: null terminated string < 50 characters long
// port: The number of the port being used
// return value: The assigned port. A value < 0 means an error occurred
int define_port(const char *service_name, int port);

// Get a port for use by a service.
// service_name: null terminated string < 50 characters long
// return value: The assigned port. A value < 0 means an error occurred
int request_port(const char *service_name);

// Look up a port number for the specified name
// service_name: null terminated string < 50 characters long
// return value: The assigned port. A value < 0 means an error occurred
int lookup_port(const char *service_name);

// Release a port back into the pool
// service_name: null terminated string < 50 characters long
// port: the port number the service is running on
// return value: 0 = success, non-zero = error
int release_port(const char *service_name, int port);

void Fatal_Error(const char *msg, ...);
