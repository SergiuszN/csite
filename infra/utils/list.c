// Define the list structure
typedef struct {
    void *data; // Pointer to the array of elements
    size_t elementSize; // Size of each element in bytes
    size_t size; // Number of elements currently in the list
    size_t capacity; // Total capacity of the list
} List;

// Initialize a new list
List *list_create(size_t elementSize, size_t initialCapacity) {
    List *list = malloc(sizeof(List));
    if (!list) return NULL;

    list->data = malloc(elementSize * initialCapacity);
    if (!list->data) {
        free(list);
        return NULL;
    }

    list->elementSize = elementSize;
    list->size = 0;
    list->capacity = initialCapacity;

    return list;
}

// Add an element to the end of the list
int list_append(List *list, const void *element) {
    // Check if we need to resize
    if (list->size >= list->capacity) {
        size_t newCapacity = list->capacity * 2;
        void *newData = realloc(list->data, list->elementSize * newCapacity);
        if (!newData) return 0;

        list->data = newData;
        list->capacity = newCapacity;
    }

    // Copy the new element to the end of the list
    memcpy(
        (char*)list->data + (list->size * list->elementSize),
        element,
        list->elementSize
    );
    list->size++;

    return 1;
}

// Get an element at a specific index
void *list_get(List *list, size_t index) {
    if (index >= list->size) return NULL;
    return (char *) list->data + (index * list->elementSize);
}

// Remove an element at a specific index
int list_remove(List *list, size_t index) {
    if (index >= list->size) return 0;

    // Move all subsequent elements one position back
    if (index < list->size - 1) {
        memmove(
            (char*)list->data + (index * list->elementSize),
            (char*)list->data + ((index + 1) * list->elementSize),
            (list->size - index - 1) * list->elementSize
        );
    }

    list->size--;
    return 1;
}

// Free the list and its data
void list_free(List *list) {
    if (list) {
        free(list->data);
        free(list);
    }
}
