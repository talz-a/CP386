#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define PAGE_SIZE 256
#define NUM_PAGES 16
#define NUM_FRAMES 8
#define VIRTUAL_ADDRESS_SPACE_SIZE (PAGE_SIZE * NUM_PAGES)
#define PHYSICAL_ADDRESS_SPACE_SIZE (PAGE_SIZE * NUM_FRAMES)
#define NUM_VIRTUAL_ADDRESSES 20

typedef struct {
    int frame_number;
    int present;
    int use_bit;
} PageTableEntry;

PageTableEntry page_table[NUM_PAGES];
int frame_table[NUM_FRAMES];
int clock_pointer = 0;
int page_faults = 0;

void initialize_page_table() {
    for (int i = 0; i < NUM_PAGES; i++) {
        page_table[i].frame_number = -1;
        page_table[i].present = 0;
        page_table[i].use_bit = 0;
    }
}

void initialize_frame_table() {
    for (int i = 0; i < NUM_FRAMES; i++) {
        frame_table[i] = -1;
    }
}

int find_free_frame() {
    for (int i = 0; i < NUM_FRAMES; i++) {
        if (frame_table[i] == -1) {
            return i;
        }
    }
    return -1;
}

int clock_algorithm() {
    while (1) {
        int page_in_frame = frame_table[clock_pointer];
        if (page_table[page_in_frame].use_bit == 0) {
            int victim_frame = clock_pointer;
            page_table[page_in_frame].present = 0;
            page_table[page_in_frame].frame_number = -1;
            frame_table[victim_frame] = -1;
            clock_pointer = (clock_pointer + 1) % NUM_FRAMES;
            return victim_frame;
        } else {
            page_table[page_in_frame].use_bit = 0;
            clock_pointer = (clock_pointer + 1) % NUM_FRAMES;
        }
    }
}

void handle_page_fault(int page_number) {
    int frame_number = find_free_frame();
    if (frame_number == -1) {
        frame_number = clock_algorithm();
        page_faults++;
        printf("Page fault occurred. Replaced page in frame %d.\n", frame_number);
    }
    page_table[page_number].frame_number = frame_number;
    page_table[page_number].present = 1;
    page_table[page_number].use_bit = 1;
    frame_table[frame_number] = page_number;
}

int translate_address(int logical_address) {
    int page_number = (logical_address / PAGE_SIZE) % NUM_PAGES;
    int offset = logical_address % PAGE_SIZE;

    if (page_table[page_number].present == 0) {
        handle_page_fault(page_number);
    }
    page_table[page_number].use_bit = 1;
    int frame_number = page_table[page_number].frame_number;
    int physical_address = frame_number * PAGE_SIZE + offset;
    return physical_address;
}

int generate_random_virtual_address() {
    return rand() % VIRTUAL_ADDRESS_SPACE_SIZE;
}

int main() {
    srand(time(NULL));
    initialize_page_table();
    initialize_frame_table();

    for (int i = 0; i < NUM_VIRTUAL_ADDRESSES; i++) {
        int logical_address = generate_random_virtual_address();
        int physical_address = translate_address(logical_address);
        printf("Logical Address: %d -> Physical Address: %d\n",
               logical_address, physical_address);
    }
    printf("Total page faults: %d\n", page_faults);
    return 0;
}
