#include <iomanip>      // std::setw
#include <iostream>

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

#define IPV4_T 1 //Ipv4 address

typedef struct {
    unsigned short trans_id;   /* used by the client to match up replies with
                                  queries.*/

    unsigned char rd : 1;      /* should pursue the query recursively */
    unsigned char tc : 1;      /* is message truncated */
    unsigned char aa : 1;      /* is responding ns authority for the domain name
                                */
    unsigned char opcode : 4;  /* kind of query (0 indicates standard query.) */
    unsigned char qr     : 1;  /*  whether this message is a query 0, or a
                                  response 1 */
    unsigned char r_code : 4;  /* response code */
    unsigned char z      : 3;  /* reserved for future use. Set this field to
                                  0.*/
    unsigned char ra     : 1;  /* whether recursive query support is available
                                  in the ns */
    unsigned short q_count;    /* # of entries in the question section */
    unsigned short ans_count;  /* # of resource records in the answer section */
    unsigned short auth_count; /* # of ns records in the authority records
                                  section */
    unsigned short add_count;  /* # of ns records in the additional records
                                  section */
    char*          name;

    unsigned short q_type;
    unsigned short q_class;

    /* serializes struct and encodes the name to proper format */
    std::ostream& serialize(std::ostream& os) const {
        os.write((char*)&this[0], 12);
        unsigned char before_dot_count = 0;
        int prev_index = 0;
        int index_counter = 0;
        bool scanning = true;

        bool done = false;
        for(int i = 0; !done; i++) {

            if(scanning) {
                if(name[i] == '.' || name[i]== '\0') {
                    scanning = false;
                    os.put(before_dot_count);
                    before_dot_count = '\x00';
                    i = prev_index - 1;
                    index_counter++;
                }
                else {
                    before_dot_count++;
                    index_counter++;
                }
            }
            else {
                if(name[i] != '.' && name[i] != '\0') {
                    os.write(&name[i],1);
                }
                /* stop copying if null reached */
                else if(name[i] == '\0') {
                    done = true;
                }
                else {
                    prev_index += index_counter;
                    index_counter = 0;
                    scanning = true;
                }
            }
        }
        os.put('\x00');

        os.write((char *) & q_type,     sizeof(q_type));
        os.write((char *) & q_class,    sizeof(q_class));

        return os;
    }
} _dns_header_t;

typedef struct {
    char* r_name;
    unsigned short r_type;
    unsigned short r_class;
    unsigned int r_ttl;
    unsigned short r_data_len;
    char* r_data;
} _res_record_t;
