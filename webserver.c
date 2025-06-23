//importing libraries to use library functions
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <fcntl.h>
#include <sys/stat.h>

//defining https port and setting buffer size
#define PORT 4433
#define BUFFER_SIZE 4096

//function declarations 
void send_response(int client_socket, const char *filepath);
void init_openssl();
void cleanup_openssl();
SSL_CTX *create_context();
void configure_context(SSL_CTX *ctx);

int main() {
    //Declaring server file decriptor or an integer that refers to a listening socket
    int server_fd; /*sockaddr_in address is used specificly for IPv4 address and port; sockaddr is
    necessary for functions like bind(), connect() or accept() since functions are 
    protocol agnostic. */
    struct sockaddr_in address; 
    socklen_t addrlen = sizeof(address);
    
    //Intitializing OpenSSL
    init_openssl(); //loads SSL algorithms and error strings
    SSL_CTX *ctx = create_context(); //creates a new tls/ssl context
    configure_context(ctx; // loads cert and key
    
    //creating server socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    int opt = 1; //"I want to enable a socket option thats passed onto setsockopt()"
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    /*bind() associates server_fd (server socket) with an ipv4 address + port stored 
    in addr struct. However bind expects a generic sockaddr, we typecase sockaddr_in 
    to match the expected type. This if checks if bind was successful.*/
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    //sets server_fd as a listening socket
    //10 sets maximum number of pending connections in the queue
    listen(server_fd, 10);
    printf("🔐 HTTPS Server listening on port %d...\n", PORT); //for logging purposes
    
    //accept loop
    while (1) {
        struct sockaddr_in client_addr; //client_addr only used fo each inidvidual connection
        socklen_t len = sizeof(client_addr);
        
        int client_socket = accept(server_fd, (struct sockaddr *)&client_addr, &len);
        if (client_socket < 0) {
            perror("accept failed");
            continue;
        }

        SSL *ssl = SSL_new(ctx); /*creates a new SSL connection object based on configurations
        stored in ctx which refers to ssl context which is a struct that holds configs for
        SSL/TLS connections.*/
        SSL_set_fd(ssl, client_socket); /*Links the SSL object to the TCP socket (client_socket)
        that you got from accept()*/
        
        /* starts the TLS/SSL handshake with the client on the socket that ssl is attached too.
        This is where OpenSSL decides the encryption protocol and verifies certificates and
        establishes keys*/
        if (SSL_accept(ssl) <= 0) {
            ERR_print_errors_fp(stderr); //prints descriptor of error to stderr
            SSL_free(ssl); //frees SSL object and memory associated with it
            close(client_socket); //closes the TCP socket
            continue; //skips the loop and goes back to listening
        }

        char buffer[BUFFER_SIZE]; //allocates buffer in mem to hold incoming client data
        int read_size = SSL_read(ssl, buffer, sizeof(buffer) - 1);/*reads the data from 
        client through the TLS session, decrpts it and stores it in buffer. The -1 ensures
        space for \0 at the end. read_size holds the number of bytes read which can differ
        from buffersize*/
        buffer[read_size] = '\0'; //adds \9 at the end of data to make the buffer a valid C string

        char method[8], path[1024]; //method will hold the HTTP request method and path holds requested url path
        sscanf(buffer, "%s %s", method, path); //parses buffer tokens that have HTTP request line into method and path
        char *file_to_serve = path + 1; //skips the / in the path string
        if (strlen(file_to_serve) == 0) // if path is just root of website then index.html (homepage) is served by default
            file_to_serve = "index.html";

        send_response(ssl, file_to_serve); //sents the requested file back to client over SSL connection

        SSL_shutdown(ssl); /*closes the SSL connection by sending a "close_notify" alert to the
        client. This ensures both sides properly finish the TLS session*/
        SSL_free(ssl); //frees memory for the SSL connection to avoid memory leaks
        close(client_socket); //closes underlying TCP socket ending the non-SSL connection
    }

    return 0;
}
void send_response(SSL *ssl, const char *filepath) {
    char buffer[BUFFER_SIZE];
    FILE *file = fopen(filepath, "r");

    if (file == NULL) {
        // File not found
        const char *not_found = "HTTP/1.1 404 Not Found\r\n"
                                "Content-Length: 13\r\n"
                                "Content-Type: text/plain\r\n"
                                "Connection: close\r\n\r\n"
                                "404 Not Found";
        SSL_write(ssl, not_found, strlen(not_found));
        return;
    }

    // Get file size
    fseek(file, 0, SEEK_END); //moves file pointer to end of the file (0)
    long filesize = ftell(file);
    rewind(file);

    // Send headers
    snprintf(buffer, sizeof(buffer),
             "HTTP/1.1 200 OK\r\n"
             "Content-Length: %ld\r\n"
             "Content-Type: text/html\r\n"  // You can improve MIME type detection
             "Connection: close\r\n\r\n", filesize);
    write(client_socket, buffer, strlen(buffer));

    // Send file content
    size_t n;
    while ((n = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        write(client_socket, buffer, n);
    }

    fclose(file);
}

