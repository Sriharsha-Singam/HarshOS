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

/**
 * Malloc a new Heap Entry Space
 *
 * @param size Size That is Being Requested -- BYTES
 *
 * @return Returning the address for the Heap Malloced Space that can be used by the Kernel/User
 */
void* kernel_heap_malloc(u32 size);
void* kernel_heap_malloc_page_aligned(u32 size); /**TODO: NOT IMPLEMENTED YET*/

/**
 * Calloc a new Heap Entry Space
 *
 * @param size Size That is Being Requested -- BYTES
 * @param value Value to set the new Allocated Heap Location
 *
 * @return Returning the address for the Heap Calloced Space that can be used by the Kernel/User
 */
void* kernel_heap_calloc(u32 size, u8 value);
void* kernel_heap_calloc_page_aligned(u32 size); /**TODO: NOT IMPLEMENTED YET*/

/**
 * Free a Heap Linked List Entry. This sets the IS_USED FLAG to FREE -- For future allocation
 *
 * @param address Address of the Heap Space to be Freed
 *
 * @return 0={Successfully Freed} 1={NOT Successfully Freed}
 */
u32 kernel_heap_free(void* address);

/**
 * Find the Length of the overall Heap Linked List -- Number of Heap Linked List Entries
 *
 * @return Number of Heap Linked List Entries
 */
u32 length_of_heap_entries_list();

/**
 * Get the Heap Entry based on the Index.
 *
 * @param index Index of the Heap Linked List Entry to be returned
 *
 * @return Heap Entry Address -- Based on Index
 */
heap_entry_linked_list* get_heap_entry(u32 index);

/**
 * Get the Heap Entry based on the Heap Space Address.
 *
 * @param address Address of the Heap Linked List Entry Space to be returned
 *
 * @return Heap Entry Address -- Based on Heap Space Address
 */
heap_entry_linked_list* get_heap_entry_address(void* address);

/**
 * Check if a Heap Entry exists and is USED based on the Heap Space address
 *
 * @param address Address of the Heap Entries Heap Space
 *
 * @return NULL={Heap Entry Does Not Exist OR is Not Used} Heap_Entry_Address={Heap Address of the Heap Entry}
 */
heap_entry_linked_list* does_heap_entry_exist(void* address);

/**
 * Print All the Heap Entries
 */
void print_all_heap_entries();

/**
 * Print All the Heap Entries WITH MORE DETAIL
 */
void print_all_full_heap_entries();

/**
 * Sort the entire Heap Linked List by ascending order
 */
void sort_heap_entry_linked_list();

/**
 * Swap 2 continuous sets of Heap Entry Linked List Entries
 *
 * @param prev The left/previous Linked List Entry
 * @param next The right/next Linked List Entry
 */
void swap_heap_entry_linked_list(heap_entry_linked_list* prev, heap_entry_linked_list* next);

/**
 * THIS FUNCTION GOES THROUGH ALL THE HEAP ENTRIES IN THE LINKED LIST
 * AND CHECKS IF THE ANY CONTINUOUS HEAP ENTRIES ARE FREE AND THEN MERGES
 * 2 HEAP ENTRIES AT A TIME.
 */
void merge_heap_entries_in_entire_linked_list();

/**
 * Merge 2 Heap Entries.
 *
 * @param heap_entry_list THIS PARAMETER SHOULD BE THE !!FIRST!! HEAP ENTRY OF THE 2 THAT NEED TO BE MERGED
 *
 * @return Returns 0={for successful Merging of the 2 Heap Entries}  1={for unsuccessful Merging}
 */
int merge_two_heap_entries_linked_list(heap_entry_linked_list* heap_entry_list, heap_entry_linked_list* heap_entry_list1);

/**
 * This function will insert a new Linked List Heap Entry
 *
 * @param size This will be the size of the space that needs to be
 *             heap malloced. IN -- BYTES
 *
 * @return HEAP_ENTRY_LINKED_LIST POINTER
 */
heap_entry_linked_list* insert_heap_entry(u32 size);

/**
 * Remove a Heap Entry.
 *
 * @param remove Heap Entry that needs to be Removed
 */
void remove_heap_entry(heap_entry_linked_list* heap_entry);

#endif //HARSHOS_KERNEL_HEAP_H
