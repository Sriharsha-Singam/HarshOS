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

    if (!HEAP_LINKED_LIST_HEAD) {
        HEAP_LINKED_LIST_HEAD = new_heap_entry_linked_list;
//        LOG_DEBUG("Inserting First Heap Entry: ", (u32)HEAP_LINKED_LIST_HEAD);
        new_heap_entry_linked_list->previous = (heap_entry_linked_list*) NULL;
        new_heap_entry_linked_list->next = (heap_entry_linked_list*) NULL;
        HEAP_LINKED_LIST_LAST = new_heap_entry_linked_list;
    } else {

        heap_entry_linked_list* linked_list_copy = HEAP_LINKED_LIST_LAST;

        linked_list_copy->next = new_heap_entry_linked_list;
        new_heap_entry_linked_list->next = (heap_entry_linked_list*) NULL;
        new_heap_entry_linked_list->previous = linked_list_copy;
        HEAP_LINKED_LIST_LAST = new_heap_entry_linked_list;

    }

    new_heap_entry_linked_list->is_used = USED;
    new_heap_entry_linked_list->heap_entry_size = size;
    new_heap_entry_linked_list->magic_number = HEAP_MAGIC_NUMBER;

    mallocable_address = mallocable_address + sizeof(heap_entry_linked_list);

    new_heap_entry_linked_list->heap_entry_location = (void*) mallocable_address;

    mallocable_address = mallocable_address + size;

    heap_entry_footer_t* heap_entry_footer = (heap_entry_footer_t*) (mallocable_address);
    heap_entry_footer->magic_number = HEAP_FOOTER_MAGIC_NUMBER;
    heap_entry_footer->header_location = new_heap_entry_linked_list;

    mallocable_address = mallocable_address + sizeof(heap_entry_footer_t);

    sort_heap_entry_linked_list();

    return new_heap_entry_linked_list;
}


heap_entry_linked_list* find_empty_heap_entry(u32 size) {

    heap_entry_linked_list* starting_heap_entry = HEAP_LINKED_LIST_HEAD;

    while (starting_heap_entry) {
        if (((starting_heap_entry->is_used) == FREE)
            && (starting_heap_entry->heap_entry_size >= size)
            && (starting_heap_entry->heap_entry_size < (size+4)))
        {
            return starting_heap_entry;
        }
        starting_heap_entry = starting_heap_entry->next;
    }

    return NULL;
}

void* kernel_heap_malloc(u32 size) {

    heap_entry_linked_list* heap_entry = find_empty_heap_entry(size);

    if (heap_entry == NULL) {
        heap_entry = insert_heap_entry(size);
    }

    heap_entry->is_used = USED;
    return ((void*)heap_entry->heap_entry_location);
}

void* kernel_heap_calloc(u32 size) {
    void* heap_entry_location = kernel_heap_malloc(size);
    memory_set(heap_entry_location, 0, size);
    return heap_entry_location;
}

void remove_heap_entry(heap_entry_linked_list* remove) {

    heap_entry_linked_list* prev = remove->previous;
    heap_entry_linked_list* next = remove->next;

    if (prev != NULL) prev->next = next;
    else HEAP_LINKED_LIST_HEAD = next;

    if (next != NULL) next->previous = prev;
    else HEAP_LINKED_LIST_LAST = prev;
}

/**
 * Merge 2 Heap Entries.
 *
 * @param heap_entry_list THIS PARAMETER SHOULD BE THE !!FIRST!! HEAP ENTRY OF THE 2 THAT NEED TO BE MERGED
 */
int merge_two_heap_entries_linked_list(heap_entry_linked_list* heap_entry_list, heap_entry_linked_list* heap_entry_list1) {

    //Find the previous Heap Entry Footer -- (This is to make sure that the 2 Heap Entries to be merged are next to each other)
    heap_entry_footer_t* footer = (heap_entry_footer_t*) ((u32)heap_entry_list1 - 8);
    if (footer->magic_number != HEAP_FOOTER_MAGIC_NUMBER) return 1;

    remove_heap_entry(heap_entry_list1);

    void* heap_entry_1 = heap_entry_list->heap_entry_location;
    u32 size_of_new_space = heap_entry_list->heap_entry_size + sizeof(heap_entry_footer_t) + sizeof(heap_entry_linked_list) + heap_entry_list1->heap_entry_size;

    memory_set(heap_entry_1, 0, size_of_new_space);

    heap_entry_list->heap_entry_size = size_of_new_space;

    heap_entry_footer_t* heap_entry_2_footer = heap_entry_1 + size_of_new_space;

    heap_entry_2_footer->header_location = heap_entry_list;

    remove_heap_entry(heap_entry_list);

    heap_entry_linked_list* list = HEAP_LINKED_LIST_LAST;

    if (list) {
        list->next = heap_entry_list;
        heap_entry_list->previous = list;
        heap_entry_list->next = NULL;
    } else {
        heap_entry_list->previous = NULL;
        heap_entry_list->next = NULL;
        if (!HEAP_LINKED_LIST_HEAD) HEAP_LINKED_LIST_HEAD = heap_entry_list;
    }

    HEAP_LINKED_LIST_LAST = heap_entry_list;

    sort_heap_entry_linked_list();

    return 0;
}

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

     while (heap_entry_list->next) {

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



int kernel_heap_free(void* address) {

    merge_heap_entries_in_entire_linked_list();

    heap_entry_linked_list* heap_entry_list = HEAP_LINKED_LIST_HEAD;

    while (heap_entry_list) {
        if (heap_entry_list->heap_entry_location == (heap_entry_linked_list*)address) {
            heap_entry_list->is_used = FREE;

            merge_heap_entries_in_entire_linked_list();

            return 1;
        }
        heap_entry_list = heap_entry_list->next;
    }

    return 0;
}


u32 length_of_heap_entries_list() {

    heap_entry_linked_list* heap_entry = HEAP_LINKED_LIST_HEAD;

    u32 counter = 0;

    while(heap_entry) {
        counter++;
        heap_entry = heap_entry->next;
    }

    return counter;
}

void merge_incomplete() {
    heap_entry_linked_list* list = HEAP_LINKED_LIST_HEAD;

    while (list) {
        if (list->is_used == FREE) {
            heap_entry_footer_t* footer = (heap_entry_footer_t*) ((u32)list - 8);

            if (footer->magic_number == HEAP_FOOTER_MAGIC_NUMBER && footer->header_location->is_used == FREE) {
                LOG_DEBUG("Merging", footer->header_location->heap_entry_size);
                merge_two_heap_entries_linked_list(footer->header_location, list);
            }
        }

        list = list->next;
    }
}

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

heap_entry_linked_list* get_heap_entry_address(void* address) {

    heap_entry_linked_list* heap_entry_list = HEAP_LINKED_LIST_HEAD;

    while(heap_entry_list) {
        if (heap_entry_list->heap_entry_location == address) return heap_entry_list;
        heap_entry_list = heap_entry_list->next;
    }

    return NULL;
}

u32 does_heap_entry_exist(void* address) {

    heap_entry_linked_list* heap_entry_list = HEAP_LINKED_LIST_HEAD;

    while(heap_entry_list) {
        if ((heap_entry_list->heap_entry_location == address) && (heap_entry_list->is_used == USED)) return 1;
        heap_entry_list = heap_entry_list->next;
    }

    return 0;
}

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
