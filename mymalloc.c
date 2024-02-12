#include "mymalloc.h"
#include <stddef.h>

#define MAX_MEMORY 25000
#define METADATA_SIZE sizeof(struct block)

struct block {
    size_t size;
    int free;
    struct block* next;
};

struct block* freeList = (void*)memory;
char memory[MAX_MEMORY];

void initialize() {
    freeList->size = MAX_MEMORY - METADATA_SIZE;
    freeList->free = 1;
    freeList->next = NULL;
}

void split(struct block* fitting_slot, size_t size) {
    struct block* new = (void*)((void*)fitting_slot + size + METADATA_SIZE);
    new->size = (fitting_slot->size) - size - METADATA_SIZE;
    new->free = 1;
    new->next = fitting_slot->next;
    fitting_slot->size = size;
    fitting_slot->free = 0;
    fitting_slot->next = new;
}

void* MyMalloc(size_t noOfBytes) {
    struct block* curr = freeList;
    if (!((void*)memory <= (void*)curr && (void*)curr <= (void*)(memory + MAX_MEMORY))) {
        initialize();
        curr = freeList;
    }
    while ((((curr->size) < noOfBytes) || ((curr->free) == 0)) && (curr->next != NULL)) {
        curr = curr->next;
    }
    if ((curr->size) == noOfBytes) {
        curr->free = 0;
        return (void*)(++curr);
    } else if ((curr->size) > (noOfBytes + METADATA_SIZE)) {
        split(curr, noOfBytes);
        return (void*)(++curr);
    }
    return NULL;
}

void merge() {
    struct block* curr = freeList;
    while ((curr->next) != NULL) {
        if ((curr->free) && (curr->next->free)) {
            curr->size += (curr->next->size) + METADATA_SIZE;
            curr->next = curr->next->next;
        }
        curr = curr->next;
    }
}

void MyFree(void* ptr) {
    if (((void*)memory <= ptr) && (ptr <= (void*)(memory + MAX_MEMORY))) {
        struct block* curr = ptr;
        --curr;
        curr->free = 1;
        merge();
    }
}
