#include <stdlib.h>
#include <string.h>

typedef struct String {
    char *data;
    int32_t length;
} String;

String *string_new(char data[]) {
    String *str = malloc(sizeof(String));

    const int len = strlen(data);
    str->length = len;
    str->data = malloc(len + 1);

    strcpy(str->data, data);

    return str;
}

void string_free(String *str) {
    free(str->data);
    free(str);
}

void string_list_free(List *list) {
    if (!list) return;

    for (size_t i = 0; i < list->size; i++) {
        String **str_ptr = list_get(list, i);
        if (str_ptr && *str_ptr) {
            string_free(*str_ptr);
        }
    }

    list_free(list);
}

int string_length(String *str) {
    return str->length;
}

void string_append(String *str, const char *s) {
    int new_len = str->length + strlen(s);
    char *new_data = realloc(str->data, new_len + 1);
    if (new_data) {
        str->data = new_data;
        strcat(str->data, s);
        str->length = new_len;
    }
}

List *string_explode(String *str, const char *delimiter) {
    // Create a new list to store String pointers
    List *result = list_create(sizeof(String *), 10);
    if (!result) return NULL;

    // Make a copy of the string data for strtok
    char *tmp = strdup(str->data);
    if (!tmp) {
        free(tmp);
        list_free(result);
        return NULL;
    }

    // Split the string
    char *token = strtok(tmp, delimiter);
    while (token) {
        printf("Token found: %s\n", token);
        // Create new String from token
        String *part = string_new(token);
        if (!part) {
            // Handle error: clean up and return
            for (size_t i = 0; i < result->size; i++) {
                String **str_ptr = list_get(result, i);
                if (str_ptr && *str_ptr) {
                    string_free(*str_ptr);
                }
            }
            list_free(result);
            free(tmp);
            return NULL;
        }

        // Add the String pointer to the list
        if (!list_append(result, &part)) {
            // Handle error: clean up and return
            string_free(part);
            for (size_t i = 0; i < result->size; i++) {
                String **str_ptr = list_get(result, i);
                if (str_ptr && *str_ptr) {
                    string_free(*str_ptr);
                }
            }
            list_free(result);
            free(tmp);
            return NULL;
        }

        token = strtok(NULL, delimiter);
    }

    free(tmp);
    return result;
}

String *string_implode(List *list, const char *delimiter) {
    if (list->size == 0) return NULL;

    // Calculate total length needed
    size_t total_len = 0;
    size_t delim_len = strlen(delimiter);

    for (size_t i = 0; i < list->size; i++) {
        String **str_ptr = list_get(list, i);
        if (str_ptr && *str_ptr) {
            total_len += (*str_ptr)->length;
            if (i < list->size - 1) {
                total_len += delim_len;
            }
        }
    }

    // Create result buffer
    char *result = malloc(total_len + 1);
    if (!result) return NULL;
    result[0] = '\0';

    // Build the joined string
    for (size_t i = 0; i < list->size; i++) {
        String **str_ptr = list_get(list, i);
        if (str_ptr && *str_ptr) {
            strcat(result, (*str_ptr)->data);
            if (i < list->size - 1) {
                strcat(result, delimiter);
            }
        }
    }

    String *joined = string_new(result);
    free(result);
    return joined;
}

int string_position(String *str, const char *substr) {
    char *pos = strstr(str->data, substr);
    if (pos == NULL) return -1;
    return pos - str->data;
}

int string_contains(String *str, const char *substr) {
    return strstr(str->data, substr) != NULL;
}

String *string_replace(String *str, const char *search, const char *replace) {
    char *pos = strstr(str->data, search);
    if (pos == NULL) return string_new(str->data);

    int search_len = strlen(search);
    int replace_len = strlen(replace);
    int prefix_len = pos - str->data;

    // Calculate new string length
    int new_len = str->length - search_len + replace_len;
    char *result = malloc(new_len + 1);

    // Copy prefix
    strncpy(result, str->data, prefix_len);
    // Copy replacement
    strcpy(result + prefix_len, replace);
    // Copy suffix
    strcpy(result + prefix_len + replace_len, pos + search_len);

    String *new_str = string_new(result);
    free(result);
    return new_str;
}

String *string_replace_all(String *str, const char *search, const char *replace) {
    String *result = string_new(str->data);
    String *temp;

    while (string_contains(result, search)) {
        temp = string_replace(result, search, replace);
        string_free(result);
        result = temp;
    }

    return result;
}
