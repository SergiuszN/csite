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

// Utils ---------------------------------------------------------------------------------------------------------------
#include "infra/utils/list.c"
#include "infra/utils/number.c"
#include "infra/utils/string.c"
#include "infra/utils/hashmap.c"

// Infra ---------------------------------------------------------------------------------------------------------------
#include "infra/core/request_response.c"
#include "infra/core/server.c"

// Domain --------------------------------------------------------------------------------------------------------------
// nothing to import there for now...

// App -----------------------------------------------------------------------------------------------------------------
// nothing to import there for now...

// Main ----------------------------------------------------------------------------------------------------------------

int main() {
  server_init();

  return 0;
}