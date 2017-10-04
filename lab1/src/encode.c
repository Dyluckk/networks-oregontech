//******************************************************
// Declare functions for opening and reading files a buffer at a time
//
// Author: Zachary Wentworth
// Email:  zachary.wentworth@oit.edu
//
//******************************************************
#include "encode.h"
#include <stdio.h>

// *************************************
// See the header file for documentation
void* encode(request_t* request, void* buff) {
  /* flip port h2ns */
  request->port = htons(request->port);

  /* NULL fill the service_name of request */
  char * null_position;
  int ptr_offset = 0;
  null_position = (char*) memchr (request->service_name, '\0', sizeof(request->service_name));
  if (null_position != NULL) {
      ptr_offset = null_position-request->service_name;
      memset(null_position, '\0', sizeof(request->service_name)-ptr_offset);
  }

  /* check status, return NULL to indicate ERROR */
  if(request->msg_type < DEFINE_PORT || request->msg_type > STOP) return NULL;
  /* check message type, return NULL to indicate ERROR */
  if(request->status < SUCCESS || request->status > UNDEFINED_ERROR) return NULL;

  /* assign request to buff */
  if(buff != request) {
    buff = request;
  }
  return buff;
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
      for(int i = ptr_offset; i < sizeof(request->service_name); i++) {
          if(request->service_name[i] != '\0') return 1;
      }
  }

  return validCheck;
}

// *************************************
// See the header file for documentation
request_t* decode(void* buff, request_t* decoded) {
  /* check if valid */
  int valid = is_invalid(decoded);
  if(valid > 0) return NULL;
  /* flip port */
  decoded->port = ntohs(decoded->port);
  /* assign decoded to buff and return */
  if(buff != decoded) {
    buff = decoded;
  }
  return decoded;
}
