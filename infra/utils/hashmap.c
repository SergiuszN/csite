#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

// Hash table entry structure
typedef struct Entry {
    void *key;
    void *value;
    struct Entry *next;
    size_t keySize;
    size_t valueSize;
} Entry;

// Hash map structure
typedef struct {
    Entry **buckets;
    size_t size; // Number of key-value pairs
    size_t numBuckets; // Number of buckets
    size_t keySize; // Size of each key in bytes
    size_t valueSize; // Size of each value in bytes
} HashMap;

// FNV-1a hash function
uint64_t hash_bytes(const void *data, size_t size) {
    const uint64_t FNV_PRIME = 1099511628211ULL;
    const uint64_t FNV_OFFSET = 14695981039346656037ULL;

    uint64_t hash = FNV_OFFSET;
    const uint8_t *bytes = data;

    for (size_t i = 0; i < size; i++) {
        hash ^= bytes[i];
        hash *= FNV_PRIME;
    }

    return hash;
}

// Create a new hash map
HashMap *hashmap_create(size_t keySize, size_t valueSize, size_t initialBuckets) {
    HashMap *map = malloc(sizeof(HashMap));
    if (!map) return NULL;

    map->buckets = calloc(initialBuckets, sizeof(Entry *));
    if (!map->buckets) {
        free(map);
        return NULL;
    }

    map->size = 0;
    map->numBuckets = initialBuckets;
    map->keySize = keySize;
    map->valueSize = valueSize;

    return map;
}

// Insert or update a key-value pair
int hashmap_put(HashMap *map, const void *key, const void *value) {
    uint64_t hash = hash_bytes(key, map->keySize);
    size_t bucket = hash % map->numBuckets;

    // Check if key already exists
    Entry *entry = map->buckets[bucket];
    while (entry) {
        if (memcmp(entry->key, key, map->keySize) == 0) {
            // Update existing value
            memcpy(entry->value, value, map->valueSize);
            return 1;
        }
        entry = entry->next;
    }

    // Create new entry
    Entry *newEntry = malloc(sizeof(Entry));
    if (!newEntry) return 0;

    newEntry->key = malloc(map->keySize);
    newEntry->value = malloc(map->valueSize);
    if (!newEntry->key || !newEntry->value) {
        free(newEntry->key);
        free(newEntry->value);
        free(newEntry);
        return 0;
    }

    memcpy(newEntry->key, key, map->keySize);
    memcpy(newEntry->value, value, map->valueSize);
    newEntry->keySize = map->keySize;
    newEntry->valueSize = map->valueSize;

    // Add to bucket
    newEntry->next = map->buckets[bucket];
    map->buckets[bucket] = newEntry;
    map->size++;

    return 1;
}

// Get a value by key
void *hashmap_get(HashMap *map, const void *key) {
    uint64_t hash = hash_bytes(key, map->keySize);
    size_t bucket = hash % map->numBuckets;

    Entry *entry = map->buckets[bucket];
    while (entry) {
        if (memcmp(entry->key, key, map->keySize) == 0) {
            return entry->value;
        }
        entry = entry->next;
    }

    return NULL;
}

// Remove a key-value pair
int hashmap_remove(HashMap *map, const void *key) {
    uint64_t hash = hash_bytes(key, map->keySize);
    size_t bucket = hash % map->numBuckets;

    Entry *entry = map->buckets[bucket];
    Entry *prev = NULL;

    while (entry) {
        if (memcmp(entry->key, key, map->keySize) == 0) {
            if (prev) {
                prev->next = entry->next;
            } else {
                map->buckets[bucket] = entry->next;
            }

            free(entry->key);
            free(entry->value);
            free(entry);
            map->size--;
            return 1;
        }
        prev = entry;
        entry = entry->next;
    }

    return 0;
}

// Free the hash map and all entries
void hashmap_free(HashMap *map) {
    if (!map) return;

    for (size_t i = 0; i < map->numBuckets; i++) {
        Entry *entry = map->buckets[i];
        while (entry) {
            Entry *next = entry->next;
            free(entry->key);
            free(entry->value);
            free(entry);
            entry = next;
        }
    }

    free(map->buckets);
    free(map);
}
