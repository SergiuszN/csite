// Dependency ----------------------------------------------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

// Global defines ------------------------------------------------------------------------------------------------------
#define PORT 8468
#define BUFFER_SIZE 1024

// Infra ---------------------------------------------------------------------------------------------------------------
#include "infra/utils/string.c"

// Domain --------------------------------------------------------------------------------------------------------------
// nothing to import there for now...

// App -----------------------------------------------------------------------------------------------------------------
// nothing to import there for now...

// Main ----------------------------------------------------------------------------------------------------------------

int main() {
  int server_fd, client_fd;
  struct sockaddr_in server_addr, client_addr;
  socklen_t client_len = sizeof(client_addr);
  char buffer[BUFFER_SIZE];

  // Create socket
  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0) {
    perror("Socket creation failed");
    return 1;
  }

  // Configure server address
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(PORT);

  // Bind socket
  if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
    perror("Bind failed");
    return 1;
  }

  // Listen for connections
  if (listen(server_fd, 10) < 0) {
    perror("Listen failed");
    return 1;
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

    // Prepare and send response
    const char *response = "HTTP/1.1 200 OK\r\n"
                         "Content-Type: text/html\r\n"
                         "Content-Length: 13\r\n"
                         "\r\n"
                         "Hello, World!";

    write(client_fd, response, strlen(response));

    // Close client connection
    close(client_fd);
  }

  // Close server socket
  close(server_fd);
  return 0;
}