#include <stdlib.h>
#include <string.h>

struct String {
  char* data;
  int32_t length;
};

struct String* string_new(char data[]) {
  struct String* str = malloc(sizeof(struct String));

  const int len = strlen(data);
  str->length = len;
  str->data = malloc(len + 1);

  strcpy(str->data, data);

  return str;
}

void string_free(struct String* str) {
  free(str->data);
  free(str);
}

int string_length(struct String* str) {
  return str->length;
}

void string_append(struct String* str, const char* s) {
  str->length += strlen(s);

  char* new_data = malloc(str->length + 1);
  strcpy(new_data, str->data);
  strcat(new_data, s);

  free(str->data);
  str->data = new_data;
}
