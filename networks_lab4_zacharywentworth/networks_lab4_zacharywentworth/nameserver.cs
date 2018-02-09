using System;
using System.Net;

namespace networks_lab4_zacharywentworth
{
    public class nameserver
    {
        public struct request_t  
        {  
            public uint port;            // net byte order port num
            public byte msg_type;        // see #defines below
            public byte status;          // see #defines below
            public string service_name;  // null terminated name 
            /* struct ctor */
            public request_t(uint port, byte msg_type, byte status, string service_name)
            {
                this.port = port;
                this.msg_type = msg_type;
                this.status = status;
                this.service_name = service_name;
            }
        }  

        // Valid msg_type values
        public const int DEFINE_PORT = 1;
        public const int LOOKUP_PORT = 2;
        public const int KEEP_ALIVE = 3;
        public const int CLOSE_PORT = 4;
        public const int RESPONSE = 5;
        public const int STOP = 6;
        public const int REQUEST_PORT = 7;

        // Valid status values
        public const int SUCCESS = 0;
        public const int SERVICE_IN_USE = 1;
        public const int SERVICE_NOT_FOUND = 2;
        public const int ALL_PORTS_BUSY = 3;
        public const int INVALID_ARG = 4;
        public const int UNDEFINED_ERROR = 5;

        /* Encode */
        public request_t Encode_request(request_t request) {
            /* htons */
            long converted_port = request.port;
            converted_port = IPAddress.HostToNetworkOrder(converted_port);
            request.port = (uint)converted_port;

            return request;
        }

        /* Decode */
        public request_t Decode_request(request_t request)
        {
            /* ntohs */
            long converted_port = request.port;
            converted_port = IPAddress.NetworkToHostOrder(converted_port);
            request.port = (uint)converted_port;

            return request;
        }
    }
}
