//
// Created by INT_ACC on 6/17/2020.
//

#ifndef HARSHOS_KERNEL_HEAP_H
#define HARSHOS_KERNEL_HEAP_H

#include "../cpu/types.h"

#define HEAP_MAGIC_NUMBER 0xF92BD3A5
#define HEAP_FOOTER_MAGIC_NUMBER 0xB18AD0C3
#define HEAP_PAGE_MAGIC_NUMBER 0xA83F0D7D
#define HEAP_START_POINT 0xC0000000
//#define KHEAP_INITIAL_SIZE  0x100000
//#define HEAP_INDEX_SIZE   0x20000
//#define HEAP_MIN_SIZE     0x70000

#define FREE 0x0
#define USED 0x1

extern u32 CURRENT_HEAP_ADDRESS;



/******************************************************************************************/
/**
 * This is a Linked List for the normal Heap Entries
 *
 * The Space of Memory will be right after the Heap Linked List Entry
 *
 * This is what the data structure will look like:
 *
 *       ---------------------------
 *      | heap_entry_linked_list* |
 *      ---------------------------
 *      | MALLOCED FREE SPACE     |
 *      ---------------------------
 *      | heap_entry_footer_t*    |
 *      ---------------------------
 *
 */
typedef struct _heap_entry_linked_list heap_entry_linked_list;

struct _heap_entry_linked_list {
    u32 magic_number;
    void* heap_entry_location;
    u32 heap_entry_size;
    u8 is_used;
    heap_entry_linked_list* next;
    heap_entry_linked_list* previous;
};
/******************************************************************************************/


/******************************************************************************************/
/**
 * Page Aligned Heap Entries will need a separate type of Linked List in order
 * to minimize space use (above and below an actual page) and it just seems easier
 */
typedef struct _heap_page_entry_linked_list heap_page_entry_linked_list;

struct _heap_page_entry_linked_list {
    u32 magic_number;
    void* heap_page_entry_location;
    heap_page_entry_linked_list* next;
    heap_page_entry_linked_list* previous;
};
/******************************************************************************************/

//extern heap_entry_linked_list* HEAP_PAGE_ALIGNED_LINKED_LIST_HEAD;
//extern heap_entry_linked_list* HEAP_PAGE_ALIGNED_LINKED_LIST_LAST;

//typedef struct _heap_entry_header_t heap_entry_header_t;
//
//struct _heap_entry_header_t{
//    u32 magic_number;
//    u32 size;
//};

/******************************************************************************************/
/**
 * Footer is necessary to make sure that 2 Heap Linked List
 * Entries are next to each other -- otherwise they cannot be
 * merged
 */
typedef struct _heap_entry_footer_t heap_entry_footer_t;

struct _heap_entry_footer_t {
    u32 magic_number;
    heap_entry_linked_list* header_location;
};
/******************************************************************************************/

void* kernel_heap_malloc(u32 size);
void* kernel_heap_malloc_page_aligned(u32 size);
void* kernel_heap_calloc(u32 size);
void* kernel_heap_calloc_page_aligned(u32 size);

int kernel_heap_free(void* address);

u32 length_of_heap_entries_list();
heap_entry_linked_list* get_heap_entry(u32 index);
heap_entry_linked_list* get_heap_entry_address(void* address);
u32 does_heap_entry_exist(void* address);
void print_all_heap_entries();
void print_all_full_heap_entries();
void sort_heap_entry_linked_list();

heap_entry_linked_list* insert_heap_entry(u32 size);
int remove_heap_entry(heap_entry_linked_list* heap_entry);

#endif //HARSHOS_KERNEL_HEAP_H
