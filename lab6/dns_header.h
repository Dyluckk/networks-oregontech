/* for r_code */
#define NO_ERROR 0;
#define FORMAT_ERROR 1;
#define SERVER_FAILURE 2;
#define NAME_ERROR 3;
#define NOT_IMPL 4;
#define DNS_REFUSED 5;
#define QUERY_TYPE_A 1 /* Ipv4 addresses */

#define QR_QUERY 0;
#define QR_RESPONSE 1;

typedef struct {
    uint16_t trans_id; /* used by the client to match up replies with queries.*/

    uint16_t rd:1; /* should pursue the query recursively */
    uint16_t tc:1; /* is message truncated */
    uint16_t aa:1; /* is responding ns authority for the domain name */
    uint16_t opcode:4; /* kind of query (0 indicates standard query.) */
    uint16_t qr:1; /*  whether this message is a query 0, or a response 1 */

    uint16_t r_code:4; /* response code */
    uint16_t z:3; /* reserved for future use. Set this field to 0. */
    uint16_t ra:1; /* whether recursive query support is available in the ns */

    uint16_t q_count; /* # of entries in the question section */
    uint16_t ans_count; /* # of resource records in the answer section */
    uint16_t auth_count; /* # of ns records in the authority records section */
    uint16_t add_count; /* # of ns records in the additional records section */
} _dns_header_t;

typedef struct {
    u_short qtype;
    u_short qclass;
} _dns_question_t;

typedef struct  {
    char* qname;
    _dns_question_t* ques;
} _dns_query_t;

typedef struct {
     _dns_header_t* header;
     _dns_query_t* query;
} _dns_packet_t;
