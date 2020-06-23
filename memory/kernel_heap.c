//
// Created by INT_ACC on 6/17/2020.
//

#include "kernel_heap.h"
#include "../memory/kernel_malloc.h"


/**
 *  This address will change each time more of the heap is allocated/deallocated.
 *  The HEAP will move backwards from address 0xC0000000 for EVERY PROCESS -- not just the kernel.
 *  But the HEAP will be mapped to different PHYSICAL PAGES with similar/same VIRTUAL ADDRESSES
 */
u32 CURRENT_HEAP_ADDRESS = HEAP_START_POINT;

/**
 * This variable will be used to hold to location for HEAP Specified Pages.
 * These page may leave out massive chunks of memory in between. This might be
 * a good way to keep track of that.
 *
 * Another option is to potentially check the last address of the last
 * heap_entry but this might lead to some inconsistencies.
 */
u32 CURRENT_HEAP_ADDRESS_PAGE_ALIGNED = HEAP_START_POINT;


heap_entry_linked_list* HEAP_LINKED_LIST_HEAD;
heap_entry_linked_list* HEAP_LINKED_LIST_LAST;


/**
 * This function will insert a new Linked List Heap Entry
 *
 * @param size This will be the size of the space that needs to be
 *             heap malloced. IN -- BYTES
 *
 * @return HEAP_ENTRY_LINKED_LIST POINTER
 */
heap_entry_linked_list* insert_heap_entry(u32 size) {

    heap_entry_linked_list* new_heap_entry_linked_list = (heap_entry_linked_list*) CURRENT_HEAP_ADDRESS;

    if (!HEAP_LINKED_LIST_HEAD) {
        HEAP_LINKED_LIST_HEAD = new_heap_entry_linked_list;
        new_heap_entry_linked_list->previous = (heap_entry_linked_list*) NULL;
        new_heap_entry_linked_list->next = (heap_entry_linked_list*) NULL;
    } else {

        heap_entry_linked_list* linked_list_copy = HEAP_LINKED_LIST_HEAD;

        while (linked_list_copy) {
            if (size >= linked_list_copy->heap_entry_size) {
                if (linked_list_copy->next && (size < linked_list_copy->next->heap_entry_size)) {
                    new_heap_entry_linked_list->next = linked_list_copy->next;
                    new_heap_entry_linked_list->next->previous = new_heap_entry_linked_list;
                    new_heap_entry_linked_list->previous = linked_list_copy;
                    linked_list_copy->next = new_heap_entry_linked_list;
                    break;
                } else if (!(linked_list_copy->next)) {
                    linked_list_copy->next = new_heap_entry_linked_list;
                    new_heap_entry_linked_list->previous = linked_list_copy;
                    new_heap_entry_linked_list->next = NULL;
                    break;
                }
            } else if ((size < linked_list_copy->heap_entry_size) && !(linked_list_copy->previous)) {
                HEAP_LINKED_LIST_HEAD->previous = new_heap_entry_linked_list;
                new_heap_entry_linked_list->next = HEAP_LINKED_LIST_HEAD;
                HEAP_LINKED_LIST_HEAD = new_heap_entry_linked_list;
                HEAP_LINKED_LIST_HEAD->previous = NULL;
                break;
            }
            linked_list_copy = linked_list_copy->next;
        }

        new_heap_entry_linked_list = NULL;
        new_heap_entry_linked_list = linked_list_copy;

    }

    new_heap_entry_linked_list->is_used = USED;
    new_heap_entry_linked_list->heap_entry_size = size;
    new_heap_entry_linked_list->magic_number = HEAP_MAGIC_NUMBER;

    CURRENT_HEAP_ADDRESS = CURRENT_HEAP_ADDRESS - sizeof(heap_entry_linked_list);

    //TODO: Creating new area under the linkedlist entry for the heap_data_entry itself -- the
    //TODO: Linked List Entry will act as the Heap Data Entry header.
    //TODO: Then will add footer at the end that points to the Linked List Header.
//    new_heap_entry_linked_list->

    return new_heap_entry_linked_list;
}

void* create_heap_entry_space(u32 size, u8 is_page_aligned) {

    if (!is_page_aligned) {

//        void* start_address = (void*) CURRENT_HEAP_ADDRESS;

//        CURRENT_HEAP_ADDRESS = CURRENT_HEAP_ADDRESS - sizeof(heap_entry_header_t) - sizeof(heap_entry_footer_t) - size;
        CURRENT_HEAP_ADDRESS = CURRENT_HEAP_ADDRESS - sizeof(heap_entry_footer_t) - size;


//        heap_entry_header_t* heap_entry_header = (heap_entry_header_t*) start_address;
//        heap_entry_header->size =size;
//        heap_entry_header->magic_number = HEAP_MAGIC_NUMBER;

        heap_entry_footer_t* heap_entry_footer = (heap_entry_footer_t*) (CURRENT_HEAP_ADDRESS + sizeof(heap_entry_footer_t));
        heap_entry_footer->magic_number = HEAP_MAGIC_NUMBER;
//        heap_entry_footer->header_location = heap_entry_header;
    }

    return NULL; // TODO: Change this -- doing this to prevent compiler warning.
}