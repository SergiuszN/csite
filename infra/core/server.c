#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

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
    if (bind(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
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
        client_fd = accept(server_fd, (struct sockaddr *) &client_addr, &client_len);
        if (client_fd < 0) {
            perror("Accept failed");
            continue;
        }

        // Read request
        memset(buffer, 0, BUFFER_SIZE);
        read(client_fd, buffer, BUFFER_SIZE - 1);
        printf("Received request:\n%s\n", buffer);

        // TODO: create request struct from request
        // TODO: execute router with request

        // TODO: replace that with getting response from router
        struct Response *_response = response_ok(string_new("<html><body><h1>Hello</h1></body></html>"));

        char length[10];
        sprintf(length, "%d", _response->body->length);

        struct String *_responseString = string_new("HTTP/1.1 ");
        string_append(_responseString, _response->statusCode->data);
        string_append(_responseString, "\r\nContent-Type: text/html\r\nContent-Length: ");
        string_append(_responseString, length);
        string_append(_responseString, "\r\n\r\n");
        string_append(_responseString, _response->body->data);

        printf("%s\n\n", _responseString->data);
        write(client_fd, _responseString->data, _responseString->length);


        // Free & Close client connection
        string_free(_responseString);
        response_free(_response);
        close(client_fd);
    }

    // Close server socket
    close(server_fd);
}
