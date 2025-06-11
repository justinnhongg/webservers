#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/stat.h>

#define PORT 8080
#define BUFFER_SIZE 4096

void send_response(int client_socket, const char *filepath);

int main() {
    int server_fd, client_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE];

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    listen(server_fd, 10);

    while (1) {
        client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
        if (client_socket < 0) {
            perror("accept failed");
            continue;
        }

        int read_size = read(client_socket, buffer, BUFFER_SIZE - 1);
        buffer[read_size] = '\0';

        // Parse HTTP GET request
        char method[8], path[1024];
        sscanf(buffer, "%s %s", method, path);

        // Remove leading slash
        char *file_to_serve = path + 1;
        if (strlen(file_to_serve) == 0)
            file_to_serve = "index.html";

        send_response(client_socket, file_to_serve);
        close(client_socket);
    }

    return 0;
}
void send_response(int client_socket, const char *filepath) {
    char buffer[BUFFER_SIZE];
    FILE *file = fopen(filepath, "r");

    if (file == NULL) {
        // File not found
        const char *not_found = "HTTP/1.1 404 Not Found\r\n"
                                "Content-Length: 13\r\n"
                                "Content-Type: text/plain\r\n"
                                "Connection: close\r\n\r\n"
                                "404 Not Found";
        write(client_socket, not_found, strlen(not_found));
        return;
    }

    // Get file size
    fseek(file, 0, SEEK_END);
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
