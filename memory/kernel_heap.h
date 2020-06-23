//
// Created by INT_ACC on 6/17/2020.
//

#ifndef HARSHOS_KERNEL_HEAP_H
#define HARSHOS_KERNEL_HEAP_H

#include "../cpu/types.h"

#define HEAP_MAGIC_NUMBER 0xF92BD3A5
#define HEAP_START_POINT 0xC0000000
//#define KHEAP_INITIAL_SIZE  0x100000
//#define HEAP_INDEX_SIZE   0x20000
//#define HEAP_MIN_SIZE     0x70000

#define FREE 0x0
#define USED 0x1

extern u32 CURRENT_HEAP_ADDRESS;

typedef struct _heap_entry_linked_list heap_entry_linked_list;

struct _heap_entry_linked_list {
    u32 magic_number;
    void* heap_entry_location;
    u32 heap_entry_size;
    u8 is_used;
    heap_entry_linked_list* next;
    heap_entry_linked_list* previous;
};


//typedef struct _heap_entry_header_t heap_entry_header_t;
//
//struct _heap_entry_header_t{
//    u32 magic_number;
//    u32 size;
//};


typedef struct _heap_entry_footer_t heap_entry_footer_t;

struct _heap_entry_footer_t {
    u32 magic_number;
    heap_entry_linked_list* header_location;
};


void* kernel_heap_malloc(u32 size);
void* kernel_heap_malloc_page_aligned(u32 size);
void* kernel_heap_calloc(u32 size);
void* kernel_heap_calloc_page_aligned(u32 size);

void kernel_heap_free(void* address);

heap_entry_linked_list* insert_heap_entry(u32 size);
int remove_heap_entry(heap_entry_linked_list* heap_entry);

#endif //HARSHOS_KERNEL_HEAP_H
