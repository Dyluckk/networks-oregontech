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

struct _dns_header_t{
    unsigned short trans_id; // identification number

    unsigned char rd :1; // recursion desired
    unsigned char tc :1; // truncated message
    unsigned char aa :1; // authoritive answer
    unsigned char opcode :4; // purpose of message
    unsigned char qr :1; // query/response flag

    unsigned char r_code :4; // response code
    unsigned char cd :1; // checking disabled
    unsigned char ad :1; // authenticated data
    unsigned char z :1; // its z! reserved
    unsigned char ra :1; // recursion available

    unsigned short q_count; // number of question entries
    unsigned short ans_count; // number of answer entries
    unsigned short auth_count; // number of authority entries
    unsigned short add_count; // number of resource entries


    // unsigned short trans_id; /* used by the client to match up replies with queries.*/
    //
    // unsigned char rd:1; /* should pursue the query recursively */
    // unsigned char tc:1; /* is message truncated */
    // unsigned char aa:1; /* is responding ns authority for the domain name */
    // unsigned char opcode:4; /* kind of query (0 indicates standard query.) */
    // unsigned char qr:1; /*  whether this message is a query 0, or a response 1 */
    //
    // unsigned char  r_code:4; /* response code */
    // unsigned char  z:3; /* reserved for future use. Set this field to 0. */
    // unsigned char  ra:1; /* whether recursive query support is available in the ns */
    //
    // unsigned short q_count; /* # of entries in the question section */
    // unsigned short ans_count; /* # of resource records in the answer section */
    // unsigned short auth_count; /* # of ns records in the authority records section */
    // unsigned short add_count; /* # of ns records in the additional records section */
    // // char dns_query[255];
    // // char name[255];
    // // unsigned short q_type;
    // // unsigned short q_class;
};

struct QUESTION
{
    unsigned short qtype;
    unsigned short qclass;
};

typedef struct
{
    unsigned char *name;
    struct QUESTION *ques;
} QUERY_t;
