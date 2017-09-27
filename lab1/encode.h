#pragma once

// *****************************************************
// Declaration of functions for marshalling request_t structures
//
// Author: Philip Howard
// Email:  phil.howard@oit.edu

#include "nameserver.h"

// *****************************************************
// encode a request_t into network order
//
// request: the request_t to be encoded
// buff: the buffer in which to place the encoded request
//      buff and request can point to the same location. If they don't
//      they must not overlap
//
// return value:
//     on success returns buff
//     on failure returns NULL
void *encode(request_t *request, void *buff);

// *****************************************************
// check's the validity of a request
//
// Checks include:
//      msg_type is a valid value
//      status is a valid value
//      service_name is null terminated and all bytes after the initial null 
//                   are zero.
//      
//      NOTE: is_invalid can be called on a request in either network byte
//      order or host byte order.
//
// return value:
//      if request is valid, return 0
//      else return an error code indicating what was invalid.
int is_invalid(request_t *request);

// *****************************************************
// decode a request_t from network order
//
// buff: the request_t to be decoded. It is assumed that the buff
//       is in network order
// decoded: the buffer in which to place the decoded request
//      buff and decoded can point to the same location. If they don't
//      they must not overlap
//
// return value:
//     on success returns decoded
//     on failure returns NULL
//
// Note: decode calls is_invalid(buff) prior to decoding
request_t *decode(void *buff, request_t *decoded);
