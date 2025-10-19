#include "page.h"
#include "rprintf.h"
#include <stdint.h>

//array representing all pages
static struct ppage physical_page_array[NUM_PHYSICAL_PAGES];

//head of the free list
struct ppage *free_page_list = 0;

void init_pfa_list(void) {
    //initialize all pages and link them together
    for (int i = 0; i < NUM_PHYSICAL_PAGES; i++) {
        physical_page_array[i].physical_addr = (void *)((uintptr_t)i * PAGE_SIZE_BYTES);
        physical_page_array[i].next = (i < NUM_PHYSICAL_PAGES - 1) ? &physical_page_array[i + 1] : 0;
        physical_page_array[i].prev = (i > 0) ? &physical_page_array[i - 1] : 0;
    }
    free_page_list = &physical_page_array[0];
}

//allocate npages from the front of the free list
struct ppage *allocate_physical_pages(unsigned int npages) {
    if (!free_page_list || npages == 0)
        return 0;

    //check if there's enough free pages
    unsigned int count = 0;
    struct ppage *cur = free_page_list;
    while (cur && count < npages) {
        count++;
        cur = cur->next;
    }
    if (count < npages)
        //return 0 if there's not enough pages
        return 0; 

    //detach npages from the free list
    struct ppage *alloc_head = free_page_list;
    struct ppage *alloc_tail = alloc_head;
    for (unsigned int i = 1; i < npages; i++) {
        alloc_tail = alloc_tail->next;
    }

    free_page_list = alloc_tail->next;
    if (free_page_list)
        free_page_list->prev = 0;

    alloc_tail->next = 0;
    alloc_head->prev = 0;

    return alloc_head;
}

//return a list of pages to the free list
void free_physical_pages(struct ppage *ppage_list) {
    if (!ppage_list)
        return;

    //find the end of the list being freed
    struct ppage *tail = ppage_list;
    while (tail->next)
        tail = tail->next;

    //attach the free list to the tail
    if (free_page_list)
        free_page_list->prev = tail;

    tail->next = free_page_list;
    ppage_list->prev = 0;
    free_page_list = ppage_list;
}

//print helper
extern int vga_putc(int c);
#define HEXPTR(x) ((unsigned)((uintptr_t)(x)))

void print_pfa_state(void) {
    struct ppage *cur = free_page_list;
    esp_printf(vga_putc, "\nFree list:\n");
    while (cur) {
        esp_printf(vga_putc, "  Page @ 0x%08x -> phys=0x%08x\n",
                   HEXPTR(cur), HEXPTR(cur->physical_addr));
        cur = cur->next;
    }
    esp_printf(vga_putc, "(end of free list)\n");
}
