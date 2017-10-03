//******************************************************
// Declare functions for opening and reading files a buffer at a time
//
// Author: Philip Howard
// Email:  phil.howard@oit.edu
//
//******************************************************
#include "encode.h"

/* TODO how to determine a failure? */

// *************************************
// See the header file for documentation
void *encode(request_t* request, void *buff) {
  /* flip port h2ns */
  request->port = htons(request->port);
  int found_null = 0;

  /* zero out the rest of service_name */
  for (int i = 0; i < MAX_SERVICE_NAME_LEN; i++) {
    /* find zero */
    if(request->service_name[i] == 0) found_null = 1;
    /* after zero is found zero out all */
    if(found_null == 1 && request->service_name[i] != 0) request->service_name[i] = 0;
  }

  /* assign request to buff and return buff */
  buff = request;
  return buff;
}

// *************************************
// See the header file for documentation
int is_invalid(request_t* request) {
  int validCheck = 0;
  if(request->msg_type < 1 || request->msg_type > 6) validCheck = 1;
  if(request->status < 0 || request->status > 5) validCheck = 1;

  return validCheck;
}

// *************************************
// See the header file for documentation
request_t *decode(void *buff, request_t* decoded) {
  /* check if valid */
  int valid = is_invalid(decoded);
  if(valid > 0) return NULL;
  /* flip port */
  decoded->port = ntohs(decoded->port);
  /* assign decoded to buff and return */
  buff = decoded;
  return decoded;
}
