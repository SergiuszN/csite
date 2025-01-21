#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

struct Request {
    char *method;
    char *path;
    char *contentType;
    char *cookies;
    char *body;
};

struct Response {
    struct String *statusCode;
    struct String *redirectUri;
    struct String *body;
};

void server_init() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

    // Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Socket creation failed");
        return;
    }

    // Configure server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind socket
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        return;
    }

    // Listen for connections
    if (listen(server_fd, 10) < 0) {
        perror("Listen failed");
        return;
    }

    printf("Server listening on port %d...\n", PORT);

    while (1) {
        // Accept connection
        client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
        if (client_fd < 0) {
            perror("Accept failed");
            continue;
        }

        // Read request
        memset(buffer, 0, BUFFER_SIZE);
        read(client_fd, buffer, BUFFER_SIZE - 1);
        printf("Received request:\n%s\n", buffer);

        struct Response *_response = malloc(sizeof(struct Response));
        _response->statusCode = string_new("200");
        _response->redirectUri = string_new("");
        _response->body = string_new("<html><body><h1>Hello</h1></body></html>");

        char *responseBuffer = malloc(sizeof (char) * (_response->body->length + RESPONSE_HEADER_SIZE));

        // Prepare and send response
        sprintf(responseBuffer, "HTTP/1.1 %s\r\n"
                             "Content-Type: text/html\r\n"
                             "Content-Length: %d\r\n"
                             "\r\n"
                             "%s", _response->statusCode->data, _response->body->length, _response->body->data);

        printf("%s\n\n", responseBuffer);
        write(client_fd, responseBuffer, strlen(responseBuffer));

        // Close client connection
        close(client_fd);
    }

    // Close server socket
    close(server_fd);
}