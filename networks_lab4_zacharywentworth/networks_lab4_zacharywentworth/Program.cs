using System;
using System.IO;
using System.Net;
using System.Text;
using System.Net.Sockets;
using System.Security.Cryptography;
using System.Runtime.InteropServices;
using System.Runtime.Serialization.Formatters.Binary;
using System.Runtime.Serialization;

public class TCP_file_transfer_client
{
    public const int MAX_SERVICE_NAME_LEN = 49;

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

    [Serializable]
    public struct request_t
    {
        public UInt16 port;            // net byte order port num
        public byte msg_type;        // see #defines below
        public byte status;          // see #defines below
        public char[] service_name;  // null terminated name 
                                     /* struct ctor */
        public request_t(UInt16 port, byte msg_type, byte status, char[] service_name)
        {
            this.port = port;
            this.msg_type = msg_type;
            this.status = status;
            this.service_name = new char[MAX_SERVICE_NAME_LEN + 1];
            for (int i = 0; i < service_name.Length; i++)
            {
                this.service_name[i] = service_name[i];
            }
        }

        public byte[] ToArray()
        {
            var stream = new MemoryStream();
            var writer = new BinaryWriter(stream);

            writer.Write(this.port);
            writer.Write(this.msg_type);
            writer.Write(this.status);
            writer.Write(this.service_name);

            return stream.ToArray();
        }

        public static request_t FromArray(byte[] bytes)
        {
            var reader = new BinaryReader(new MemoryStream(bytes));

            var s = default(request_t);

            s.port = reader.ReadUInt16();
            s.msg_type = reader.ReadByte();
            s.status = reader.ReadByte();
            s.service_name = reader.ReadChars(MAX_SERVICE_NAME_LEN + 1);

            return s;
        }
    }

    /* Encode */
    public static request_t Encode_request(request_t request)
    {
        /* htohs */
        byte[] bytes = BitConverter.GetBytes(request.port);
        Array.Reverse(bytes);
        request.port = BitConverter.ToUInt16(bytes, 0);

        return request;
    }

    /* Decode */
    public static request_t Decode_request(request_t request)
    {
        /* ntohs */
        byte[] bytes = BitConverter.GetBytes(request.port);
        Array.Reverse(bytes);
        request.port = BitConverter.ToUInt16(bytes, 0);

        return request;
    }

    public static Int32 GetPort()
    {
        const int NS_PORT = 50050;
        const string NS_IP = "unix.cset.oit.edu";
        const string SERVICE_NAME = "ftp_cal";

        UdpClient ns_client = new UdpClient(NS_IP, NS_PORT);
        request_t port_request = new request_t(0, LOOKUP_PORT, SUCCESS, SERVICE_NAME.ToCharArray());
        int request_size = 54;

        port_request = Encode_request(port_request);

        byte[] converted_request = port_request.ToArray();
        ns_client.Send(converted_request, request_size);

        IPEndPoint RemoteIpEndPoint = new IPEndPoint(IPAddress.Any, 0);

        /* Blocks until a message returns on this socket from a remote host. */
        Byte[] receiveBytes = ns_client.Receive(ref RemoteIpEndPoint);

        request_t ns_response = request_t.FromArray(receiveBytes);
        ns_response = Decode_request(ns_response);


        Console.WriteLine("Contacting nameserver...");
        Console.WriteLine("looking up service name: " + SERVICE_NAME);
        if (ns_response.port != 0) {
            Console.WriteLine("on port: " + ns_response.port + "\n");
        } 
        
        ns_client.Close();

        return ns_response.port;
    }

    static string CalculateMD5(string filename)
    {
        using (var md5 = MD5.Create())
        {
            using (var stream = File.OpenRead(filename))
            {
                var hash = md5.ComputeHash(stream);
                return BitConverter.ToString(hash).Replace("-", "").ToLowerInvariant();
            }
        }
    }

    public static void Main()
    {
        String server_ip = "unix.cset.oit.edu"; //IP server is sitting on
        Int32 server_port = GetPort(); //port server is sitting on

        /* check if port was returned from ns*/
        if (server_port == 0)
        {
            Console.WriteLine("Service name not found on nameserver");
            return;
        } 

        Byte[] response_buffer = new Byte[256]; // Buffer to store the response bytes.

        /* Create a TcpClient */
        TcpClient client = new TcpClient(server_ip, server_port);

        /* client stream for reading and writing */
        NetworkStream stream = client.GetStream();

        while (true)
        {
            /* get message from user to send to server */
            Console.WriteLine("what would you like to do?");
            Console.WriteLine("use: get <filename/dirname> || exit || shutdown");
            String message = Console.ReadLine();
            message = message.Trim();

            /* Translate the passed message into ASCII and store it as a Byte array */
            Byte[] data = System.Text.Encoding.ASCII.GetBytes(message);

            /* Send the message to the connected TcpServer */
            stream.Write(data, 0, data.Length);
            Console.WriteLine("Sent: {0}", message);

            /* messages that kill the client app */
            if (message == "exit") break;
            if (message == "shutdown") break;

            bool done = false;  // to kill get <file> conversations
            string client_checksum = null; //checksum against file on server after transfer
            string full_path = null; //for file creates and deletes 

            /* run forever */
            while (!done) {
                try {
                    /* Read TcpServer response bytes */
                    Int32 bytes = 0;
                    String response_data = String.Empty;
                    bytes = stream.Read(response_buffer, 0, response_buffer.Length);
                    response_data = System.Text.Encoding.ASCII.GetString(response_buffer, 0, bytes);
                    Console.WriteLine("Received: {0}", response_data);

                    /* parse response */
                    string[] response_sub_strings = response_data.Split(' ');

                    /* check if multiple commands in msg */
                    string[] responses = response_data.Split('\n');

                    string first_response = responses[0];
                    string[] first_response_sub_strings = first_response.Split(' ');
                    /* check if MD5 */
                    if (first_response_sub_strings[0] == "md5sum:") {
                        Console.WriteLine("checksum...");
                        string server_checksum = first_response_sub_strings[1];
                        /* check if match, delete file and report on fail */
                        if (server_checksum != client_checksum) {
                            Console.WriteLine("ERROR: checksum on file failed");
                            File.Delete(full_path);
                        }

                        /* point to next response */
                        if (responses.Length > 1) {
                            responses[0] = responses[1];
                            response_sub_strings = responses[0].Split(' ');
                        }
                    }

                    if (response_sub_strings[0] == "done") {
                        done = true;
                        Console.WriteLine("File transfer complete");
                        break;   
                    }

                    /* check if response is error */
                    if (response_sub_strings[0] == "error:") {
                        done = true;
                        break;
                    }

                    /* check if response is is "file:" */
                    if (response_sub_strings[0] == "file:") {
                        
                        string current_file_name = response_sub_strings[1];
                        string path = "./files_from_transfers/";
                        full_path = path + current_file_name;

                        Byte[] file_transfer_buffer = new Byte[256]; // Buffer to store the response bytes.
                        Int32 transfer_port = Convert.ToInt32(response_sub_strings[2]);

                        string transfer_ip = "unix.cset.oit.edu";
                        Console.WriteLine(transfer_port);
                        TcpClient file_transfer_client = new TcpClient(transfer_ip, transfer_port);

                        Byte[] file_stream_bytes = new Byte[256]; // Buffer for reading data
                        String file_stream_data = null;

                        Console.Write("Waiting for a connection... ");
                        NetworkStream file_transfer_stream = file_transfer_client.GetStream();

                        /* if file already exists, delete it before getting the new one from the server */
                        if (File.Exists(full_path)) File.Delete(full_path);

                        FileStream file_write_stream = new FileStream(full_path, FileMode.Append);

                        int i;
                        // Loop to receive all the data sent by the client.
                        while ((i = file_transfer_stream.Read(file_stream_bytes, 0, file_stream_bytes.Length)) != 0)
                        {
                            // Translate data bytes to a ASCII string.
                            file_stream_data = System.Text.Encoding.ASCII.GetString(file_stream_bytes, 0, i);

                            /* for outputting what is in the files */
                            //Console.WriteLine("Received: {0}", file_stream_data);

                            // Process the data sent by the client.
                            byte[] file_content = System.Text.Encoding.ASCII.GetBytes(file_stream_data);
                            /* write bytes to file */
                            file_write_stream.Write(file_content, 0, file_content.Length);
                        }
                        /* Close everything */
                        file_write_stream.Close();
                        file_transfer_stream.Close();
                        file_transfer_client.Close();
                        /* get md5 for recieved file */
                        client_checksum = CalculateMD5(full_path);
                    }
                }
                catch (ArgumentNullException e)
                {
                    Console.WriteLine("ArgumentNullException: {0}", e);
                }
                catch (SocketException e)
                {
                    Console.WriteLine("SocketException: {0}", e);
                }

                Console.WriteLine("\n");
            }
        }
        /* Close everything */
        stream.Close();
        client.Close();
    }
}