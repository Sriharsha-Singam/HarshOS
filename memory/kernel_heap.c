//
// Created by INT_ACC on 6/17/2020.
//

#include "kernel_heap.h"
#include "kernel_malloc.h"
#include "mem_operations.h"
#include "page_frame_handler.h"
#include "../drivers/screen_control.h"

///**
// *  This address will change each time more of the heap is allocated/deallocated.
// *  The HEAP will move backwards from address 0xC0000000 for EVERY PROCESS -- not just the kernel.
// *  But the HEAP will be mapped to different PHYSICAL PAGES with similar/same VIRTUAL ADDRESSES
// */
//u32 CURRENT_HEAP_ADDRESS = mallocable_address;

///**
// * This variable will be used to hold to location for HEAP Specified Pages.
// * These page may leave out massive chunks of memory in between. This might be
// * a good way to keep track of that.
// *
// * Another option is to potentially check the last address of the last
// * heap_entry but this might lead to some inconsistencies.
// */
//u32 CURRENT_HEAP_ADDRESS_PAGE_ALIGNED = HEAP_START_POINT;


heap_entry_linked_list* HEAP_LINKED_LIST_HEAD;
heap_entry_linked_list* HEAP_LINKED_LIST_LAST;

//heap_page_entry_linked_list* HEAP_PAGE_ALIGNED_LINKED_LIST_HEAD = (heap_page_entry_linked_list*) NULL;
//heap_page_entry_linked_list* HEAP_PAGE_ALIGNED_LINKED_LIST_LAST = (heap_page_entry_linked_list*) NULL;
heap_page_entry_linked_list* HEAP_PAGE_ALIGNED_LINKED_LIST_HEAD;
heap_page_entry_linked_list* HEAP_PAGE_ALIGNED_LINKED_LIST_LAST;

/**
 * This function will insert a new Linked List Heap Entry
 *
 * @param size This will be the size of the space that needs to be
 *             heap malloced. IN -- BYTES
 *
 * @return HEAP_ENTRY_LINKED_LIST POINTER
 */
heap_entry_linked_list* insert_heap_entry(u32 size) {

    heap_entry_linked_list* new_heap_entry_linked_list = (heap_entry_linked_list*) mallocable_address;

    /**
     * Check if there is an existing HEAP ENTRY LINKED LIST
     */
    if (!HEAP_LINKED_LIST_HEAD) {

        // Create the new HEAP ENTRY LINKED LIST since none exists yet
        HEAP_LINKED_LIST_HEAD = new_heap_entry_linked_list;
        new_heap_entry_linked_list->previous = (heap_entry_linked_list*) NULL;
        new_heap_entry_linked_list->next = (heap_entry_linked_list*) NULL;
        HEAP_LINKED_LIST_LAST = new_heap_entry_linked_list;

    } else {

        /**
         * Add NEW HEAP ENTRY since no existing heap entry can be used for the newly requested size.
         * The new heap entry will be added at the end of the linked list and will be sorted afterwards.
         */

        // Get the last Heap Entry to append the New Heap Entry at the end of it
        heap_entry_linked_list* linked_list_copy = HEAP_LINKED_LIST_LAST;

        linked_list_copy->next = new_heap_entry_linked_list;
        new_heap_entry_linked_list->next = (heap_entry_linked_list*) NULL;
        new_heap_entry_linked_list->previous = linked_list_copy;

        // Setup the new HEAP LINKED LIST LAST Pointer
        HEAP_LINKED_LIST_LAST = new_heap_entry_linked_list;

    }

    // Setup other values for the newly created HEAP ENTRY
    new_heap_entry_linked_list->is_used = USED;
    new_heap_entry_linked_list->heap_entry_size = size;
    new_heap_entry_linked_list->magic_number = HEAP_MAGIC_NUMBER;

    // Adjust the mallocable address to allow for the latest HEAP ENTRY HEADER
    mallocable_address = mallocable_address + sizeof(heap_entry_linked_list);

    // Set the Heap Location to the newly calculated pointer
    new_heap_entry_linked_list->heap_entry_location = (void*) mallocable_address;

    // Adjust the mallocable address to allow for the latest HEAP ENTRY MEMORY LOCATION
    mallocable_address = mallocable_address + size;

    // Setup the NEWLY CREATED HEAP ENTRY FOOTER
    heap_entry_footer_t* heap_entry_footer = (heap_entry_footer_t*) (mallocable_address);
    heap_entry_footer->magic_number = HEAP_FOOTER_MAGIC_NUMBER;
    heap_entry_footer->header_location = new_heap_entry_linked_list;

    // Adjust the mallocable address to allow for the latest HEAP ENTRY FOOTER
    mallocable_address = mallocable_address + sizeof(heap_entry_footer_t);

    // Sort the updated list
    sort_heap_entry_linked_list();

    return new_heap_entry_linked_list;
}

/**
 * Find any existing Empty Heap Entries to use. Also split up any existing "very" large FREE Heap Entries.
 *
 * @param size Size That is Being Requested -- BYTES
 *
 * @return Returning the address for the FREE Heap Entry Linked List
 */
heap_entry_linked_list* find_empty_heap_entry(u32 size) {

    /**
         * Check if there is an already existing Heap Entry that is free and that has enough space
         * for the requested malloc size.
         */
    heap_entry_linked_list* heap_start = HEAP_LINKED_LIST_HEAD;
    while (heap_start) {
        if (heap_start->is_used == FREE) {
            /**
             * Check is there is enough space in the current Heap Entry to allocate for a heap entry of the requested size
             * as well as keep an extra one to prevent loss of excessive heap space.
             */
            if (heap_start->heap_entry_size > (sizeof(heap_entry_linked_list) + size + sizeof(heap_entry_footer_t) + 4)) {

                /**
                 * Add Footer to Existing Heap Entry Space. This footer will be the footer for the heap
                 * entry for the requested size.
                 */
                heap_entry_footer_t* new_footer = (heap_entry_footer_t*) (heap_start->heap_entry_location + size);
                new_footer->header_location = heap_start;
                new_footer->magic_number = HEAP_FOOTER_MAGIC_NUMBER;

                /**
                 * Add a new Heap Entry Header for the remaining space after the allocation that
                 * happened after the above allocation.
                 */
                heap_entry_linked_list* new_heap_entry = (heap_entry_linked_list*) ((u32)new_footer + sizeof(heap_entry_footer_t));
                new_heap_entry->magic_number = HEAP_MAGIC_NUMBER;
                new_heap_entry->heap_entry_size = heap_start->heap_entry_size - sizeof(heap_entry_linked_list) - sizeof(heap_entry_footer_t) - size;
                new_heap_entry->is_used = FREE;

                // Set up new Heap Entries Next and Previous and Heap Location pointers
                new_heap_entry->next = heap_start->next;
                new_heap_entry->previous = heap_start;
                new_heap_entry->heap_entry_location = (void*) ((u32)new_heap_entry + sizeof(heap_entry_linked_list));

                // Edit existing Heap Entries Next Pointer and the Size
                heap_start->next = new_heap_entry;
                heap_start->heap_entry_size = size;

                // Edit the old footer to point to the newly created Heap Entry
                heap_entry_footer_t* old_footer = (heap_entry_footer_t*) (new_heap_entry->heap_entry_location + new_heap_entry->heap_entry_size);
                old_footer->header_location = new_heap_entry;

                // Setup newly created Heap Entry as USED
                heap_start->is_used = USED;

                // Sort the updated list. This will be necessary for the reducing the size of existing big Heap Entries
                sort_heap_entry_linked_list();

                return heap_start;
            }
                /**
                 * Check is there is enough space in the Heap Entry to allocate for the requested size only.
                 */
            else if (heap_start->heap_entry_size >= size) {

                // Setup newly created Heap Entry as USED
                heap_start->is_used = USED;
                return heap_start;
            }
        }
        heap_start = heap_start->next;
    }

    return NULL;
}

/**
 * Malloc a new Heap Entry Space
 *
 * @param size Size That is Being Requested -- BYTES
 *
 * @return Returning the address for the Heap Malloced Space that can be used by the Kernel/User
 */
void* kernel_heap_malloc(u32 size) {

    // Find any existing Empty Heap Entries to use. Also split up any existing "very" large FREE Heap Entries.
    heap_entry_linked_list* heap_entry = find_empty_heap_entry(size);

    /**
     * If there is no existing Heap Entry to use -- INSERT a new Linked List Heap Entry
     */
    if (heap_entry == NULL) {

        // Insert Heap Entry
        heap_entry = insert_heap_entry(size);
    }

    // Making sure the Heap Entry being Malloced is set to USED -- preventing stupid errors for previous functions
    heap_entry->is_used = USED;

    // Returning the address for the Heap Malloced Space that can be used by the Kernel/User
    return ((void*)heap_entry->heap_entry_location);
}

/**
 * Calloc a new Heap Entry Space
 *
 * @param size Size That is Being Requested -- BYTES
 * @param value Value to set the new Allocated Heap Location
 *
 * @return Returning the address for the Heap Calloced Space that can be used by the Kernel/User
 */
void* kernel_heap_calloc(u32 size, u8 value) {

    // Using Malloc to allocate the Heap Entry
    void* heap_entry_location = kernel_heap_malloc(size);

    // Set the value for the space that was newly allocated
    memory_set(heap_entry_location, value, size);

    // Returning the address for the Heap Calloced Space that can be used by the Kernel/User
    return heap_entry_location;
}

/**
 * Remove a Heap Entry.
 * TODO: THIS DOES NOT CHANGE THE MALLOCABLE ADDRESS. IT ONLY REMOVES/LOSSES THE HEAP ENTRY LOCATION
 * TODO: THIS FUNCTION NEEDS TO TAKE INTO ACCOUNT THE MALLOCABLE ADDRESS TO PREVENT LOSS OF MEMORY SPACE OVER TIME/USE.
 *
 * @param remove Heap Entry that needs to be Removed
 */
void remove_heap_entry(heap_entry_linked_list* remove) {

    heap_entry_linked_list* prev = remove->previous;
    heap_entry_linked_list* next = remove->next;

    /**
     * Set the NEXT and PREVIOUS pointers for surrounding Heap Entries
     * This will by definition remove the Heap Entry from the Linked List Mappings.
     */

    if (prev != NULL) prev->next = next;
    else HEAP_LINKED_LIST_HEAD = next;

    if (next != NULL) next->previous = prev;
    else HEAP_LINKED_LIST_LAST = prev;
}

/**
 * Merge 2 Heap Entries.
 *
 * @param heap_entry_list THIS PARAMETER SHOULD BE THE !!FIRST!! HEAP ENTRY OF THE 2 THAT NEED TO BE MERGED
 *
 * @return Returns 0={for successful Merging of the 2 Heap Entries}  1={for unsuccessful Merging}
 */
int merge_two_heap_entries_linked_list(heap_entry_linked_list* heap_entry_list, heap_entry_linked_list* heap_entry_list1) {

    //Find the previous Heap Entry Footer -- (This is to make sure that the 2 Heap Entries to be merged are next to each other)
    heap_entry_footer_t* footer = (heap_entry_footer_t*) ((u32)heap_entry_list1 - 8);
    if (footer->magic_number != HEAP_FOOTER_MAGIC_NUMBER) return 1;

    // Remove the Second Heap Entry to merge with the First Heap Entry
    remove_heap_entry(heap_entry_list1);

    // Get the pointer for the Overall Heap Space Location
    void* heap_entry_1 = heap_entry_list->heap_entry_location;

    // Calculate the overall size of the newly merged space
    u32 size_of_new_space = heap_entry_list->heap_entry_size + sizeof(heap_entry_footer_t) + sizeof(heap_entry_linked_list) + heap_entry_list1->heap_entry_size;

    // Set the merged space to 0
    memory_set(heap_entry_1, 0, size_of_new_space);

    // Set the size for the Heap Entry for the overall merged space
    heap_entry_list->heap_entry_size = size_of_new_space;

    // Use the second footer of the heap_entry_list1 -- also set the overall heap_entry address
    heap_entry_footer_t* heap_entry_2_footer = heap_entry_1 + size_of_new_space;
    heap_entry_2_footer->header_location = heap_entry_list;

    // Remove the First Heap Entry to append at the end of the Linked List
    remove_heap_entry(heap_entry_list);

    // Append at the end of the Linked List
    heap_entry_linked_list* list = HEAP_LINKED_LIST_LAST;

    /**
     * If the Heap Linked List Last Pointer is NOT NULL then setup the merged space at the end of the Linked List
     * If the Heap Linked List Last Pointer is NULL then setup the merged space at the end of the Linked List AND the START of the Linked List
     */
    if (list) {
        list->next = heap_entry_list;
        heap_entry_list->previous = list;
        heap_entry_list->next = NULL;
    } else {
        heap_entry_list->previous = NULL;
        heap_entry_list->next = NULL;
        if (!HEAP_LINKED_LIST_HEAD) HEAP_LINKED_LIST_HEAD = heap_entry_list;
    }

    // Set the last Linked list entry to the merged one
    HEAP_LINKED_LIST_LAST = heap_entry_list;

    // Sort the newly adjusted linked list
    sort_heap_entry_linked_list();

    return 0;
}

/**
 * Swap 2 continuous sets of Heap Entry Linked List Entries
 *
 * @param prev The left/previous Linked List Entry
 * @param next The right/next Linked List Entry
 */
void swap_heap_entry_linked_list(heap_entry_linked_list* prev, heap_entry_linked_list* next) {

    heap_entry_linked_list* prev_prev = prev->previous;
    heap_entry_linked_list* next_next = next->next;

    if (prev_prev != NULL) {
        prev_prev->next = next;
    } else {
        HEAP_LINKED_LIST_HEAD = next;
    }

    if (next_next != NULL) {
        next_next->previous = prev;
    } else {
        HEAP_LINKED_LIST_LAST = prev;
    }

    prev->next = next_next;
    prev->previous = next;

    next->next = prev;
    next->previous = prev_prev;
}

/**
 * Sort the entire Heap Linked List by ascending order
 */
void sort_heap_entry_linked_list() {

    heap_entry_linked_list* heap_entry_list = HEAP_LINKED_LIST_HEAD;

    while (heap_entry_list) {

        if ((heap_entry_list->next != NULL)
            && (heap_entry_list->heap_entry_size > heap_entry_list->next->heap_entry_size)) {

            swap_heap_entry_linked_list(heap_entry_list, heap_entry_list->next);

            heap_entry_list = HEAP_LINKED_LIST_HEAD;

        } else {
            heap_entry_list = heap_entry_list->next;
        }
    }


}

/**
 * THIS FUNCTION GOES THROUGH ALL THE HEAP ENTRIES IN THE LINKED LIST
 * AND CHECKS IF THE ANY CONTINUOUS HEAP ENTRIES ARE FREE AND THEN MERGES
 * 2 HEAP ENTRIES AT A TIME.
 */
 // TODO: USE MORE ROBUST FREEING ALGO. TESTED MORE RIGOUROSLY AND FAILS -- USE THE CURRENT TEST-HEAP-INSTRUCTION
void merge_heap_entries_in_entire_linked_list() {

     heap_entry_linked_list* heap_entry_list = HEAP_LINKED_LIST_HEAD;

     while (heap_entry_list) {

         heap_entry_footer_t* footer = (heap_entry_footer_t*) ((u32)heap_entry_list - 8);

         if ((heap_entry_list->is_used == FREE)
            && (footer->magic_number == HEAP_FOOTER_MAGIC_NUMBER)
            && (footer->header_location->is_used == FREE)) {

             merge_two_heap_entries_linked_list(footer->header_location, heap_entry_list);
             sort_heap_entry_linked_list();

             heap_entry_list = HEAP_LINKED_LIST_HEAD;
         } else {
             heap_entry_list = heap_entry_list->next;
         }
     }
     sort_heap_entry_linked_list();
}

/**
 * Free a Heap Linked List Entry. This sets the IS_USED FLAG to FREE -- For future allocation
 *
 * @param address Address of the Heap Space to be Freed
 *
 * @return 0={Successfully Freed} 1={NOT Successfully Freed}
 */
u32 kernel_heap_free(void* address) {

    merge_heap_entries_in_entire_linked_list();

    heap_entry_linked_list* heap_entry_list = HEAP_LINKED_LIST_HEAD;

    while (heap_entry_list) {
        if (heap_entry_list->heap_entry_location == (heap_entry_linked_list*)address) {
            heap_entry_list->is_used = FREE;

            merge_heap_entries_in_entire_linked_list();

            return 0;
        }
        heap_entry_list = heap_entry_list->next;
    }

    return 1;
}

/**
 * Find the Length of the overall Heap Linked List -- Number of Heap Linked List Entries
 *
 * @return Number of Heap Linked List Entries
 */
u32 length_of_heap_entries_list() {

    heap_entry_linked_list* heap_entry = HEAP_LINKED_LIST_HEAD;

    u32 counter = 0;

    while(heap_entry) {
        counter++;
        heap_entry = heap_entry->next;
    }

    return counter;
}

/**
 * Get the Heap Entry based on the Index.
 *
 * @param index Index of the Heap Linked List Entry to be returned
 *
 * @return Heap Entry Address -- Based on Index
 */
heap_entry_linked_list* get_heap_entry(u32 index) {

    if (index >= length_of_heap_entries_list()) return NULL;

    heap_entry_linked_list* heap_entry_list = HEAP_LINKED_LIST_HEAD;

//    LOG_DEBUG("Looking For Heap Entry Starting From: ", (u32)heap_entry_list);

    while(index) {
        heap_entry_list = heap_entry_list->next;
        index--;
    }

    return heap_entry_list;
}

/**
 * Get the Heap Entry based on the Heap Space Address.
 *
 * @param address Address of the Heap Linked List Entry Space to be returned
 *
 * @return Heap Entry Address -- Based on Heap Space Address
 */
heap_entry_linked_list* get_heap_entry_address(void* address) {

    heap_entry_linked_list* heap_entry_list = HEAP_LINKED_LIST_HEAD;

    while(heap_entry_list) {
        if (heap_entry_list->heap_entry_location == address) return heap_entry_list;
        heap_entry_list = heap_entry_list->next;
    }

    return NULL;
}

/**
 * Check if a Heap Entry exists and is USED based on the Heap Space address
 *
 * @param address Address of the Heap Entries Heap Space
 *
 * @return NULL={Heap Entry Does Not Exist OR is Not Used} Heap_Entry_Address={Heap Address of the Heap Entry}
 */
heap_entry_linked_list* does_heap_entry_exist(void* address) {

    heap_entry_linked_list* entry = get_heap_entry_address(address);
    if (entry->is_used == USED) return entry;

    return NULL;
}

/**
 * Print All the Heap Entries
 */
void print_all_heap_entries() {

    heap_entry_linked_list* heap_entry_list = HEAP_LINKED_LIST_HEAD;

    u32 counter = 0;

    while(heap_entry_list) {
        kernel_print_string("Heap Entry ");
        kernel_print_hex_value((u32)heap_entry_list);
        kernel_print_string(": Address: ");
        kernel_print_hex_value((u32)heap_entry_list->heap_entry_location);
        kernel_print_string("; Size: ");
        kernel_print_hex_value((u32)heap_entry_list->heap_entry_size);
        kernel_print_string("; ");

        if (heap_entry_list->is_used == USED) kernel_print_string("USED\n");
        else kernel_print_string("FREE\n");


        heap_entry_list = heap_entry_list->next;
        counter++;
    }

}

/**
 * Print All the Heap Entries WITH MORE DETAIL
 */
void print_all_full_heap_entries() {

    heap_entry_linked_list* heap_entry_list = HEAP_LINKED_LIST_HEAD;

    kernel_print_string("HEAD: ");
    kernel_print_hex_value((u32)HEAP_LINKED_LIST_HEAD);
    kernel_print_string(" ");
    kernel_print_hex_value((u32)HEAP_LINKED_LIST_HEAD->heap_entry_size);
    kernel_print_string("; LAST: ");
    kernel_print_hex_value((u32)HEAP_LINKED_LIST_LAST);
    kernel_print_string(" ");
    kernel_print_hex_value((u32)HEAP_LINKED_LIST_LAST->heap_entry_size);
    kernel_print_string("\n");

    u32 counter = 0;

    while(heap_entry_list) {
        kernel_print_string("Heap Entry ");
        kernel_print_hex_value((u32)heap_entry_list);
        kernel_print_string(": Address: ");
        kernel_print_hex_value((u32)heap_entry_list->heap_entry_location);
        kernel_print_string("; Size: ");
        kernel_print_hex_value((u32)heap_entry_list->heap_entry_size);
        kernel_print_string("; Next: ");
        if (heap_entry_list->next == NULL) {
            kernel_print_string("NULL");
        } else {
            kernel_print_hex_value((u32)heap_entry_list->next->heap_entry_size);
        }

        kernel_print_string("; Prev: ");

        if (heap_entry_list->previous == NULL) {
            kernel_print_string("NULL");
        } else {
            kernel_print_hex_value((u32)heap_entry_list->previous->heap_entry_size);
        }
        kernel_print_string("; ");

        if (heap_entry_list->is_used == USED) kernel_print_string("USED\n");
        else kernel_print_string("FREE\n");


        heap_entry_list = heap_entry_list->next;
        counter++;
    }

}

/**
 * This function will create Page Aligned Heap Entries. Will implement this function
 * upon implementing paging in User Mode. Will need to account for Pgae Directory,
 * Page Directory Entries and Page Table Entries.
 *
 * @return POINTER TO THE PAGE ALIGNED HEAP ENTRY.
 */
//heap_page_entry_linked_list* insert_heap_page_aligned_entry() {
//
//    heap_page_entry_linked_list* new_heap_page_entry_linked_list = (heap_page_entry_linked_list*) mallocable_address;
//
//    if (!HEAP_PAGE_ALIGNED_LINKED_LIST_HEAD) {
//        HEAP_PAGE_ALIGNED_LINKED_LIST_HEAD = new_heap_page_entry_linked_list;
//        new_heap_page_entry_linked_list->previous = (heap_page_entry_linked_list*) NULL;
//        new_heap_page_entry_linked_list->next = (heap_page_entry_linked_list*) NULL;
//    } else {
//
//        heap_page_entry_linked_list* linked_list_copy = HEAP_PAGE_ALIGNED_LINKED_LIST_HEAD;
//
//        while (linked_list_copy->next) {
//            linked_list_copy = linked_list_copy->next;
//        }
//
//        new_heap_page_entry_linked_list->previous = linked_list_copy;
//        new_heap_page_entry_linked_list->next = NULL;
//    }
//
//    new_heap_page_entry_linked_list->magic_number = HEAP_PAGE_MAGIC_NUMBER;
//
//    u32 page_entry_index = (HEAP_START_POINT >> 22) - 50;
//
//    kernel_set_any_page_table_entry()
//
//    mallocable_address = mallocable_address - sizeof(heap_page_entry_linked_list);
//
//    return new_heap_page_entry_linked_list;
//
//}
