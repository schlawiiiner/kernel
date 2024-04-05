#include "../../src/include/font.h"
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;

struct __attribute__((packed)) BootInformationFormat {
    uint32_t flags;

    uint32_t mem_lower;
    uint32_t mem_upper;

    uint32_t boot_device;

    uint32_t cmdline;

    uint32_t mods_count;
    uint32_t mods_addr;

    uint32_t syms[4];

    uint32_t mmap_length;
    uint32_t mmap_addr;

    uint32_t drives_length;
    uint32_t drives_addr;

    uint32_t config_table;

    uint32_t boot_loader_name;

    uint32_t apm_table;

    uint32_t vbe_control_info;
    uint32_t vbe_mode_info;
    uint16_t vbe_mode;
    uint16_t vbe_interface_seg;
    uint16_t vbe_interface_off;
    uint16_t vbe_interface_len;

    uint64_t framebuffer_addr;
    uint32_t framebuffer_pitch;
    uint32_t framebuffer_width;
    uint32_t framebuffer_height;
    uint8_t framebuffer_bpp;
    uint8_t framebuffer_type;
    uint8_t color_info[6];
};

typedef struct BootInformationFormat BootInformationFormat;

void put_pixel(BootInformationFormat* multiboot_structure, int x, int y, uint32_t color) {
    int width = (int)(multiboot_structure->framebuffer_width);
    uint32_t *framebuffer = (uint32_t*)(multiboot_structure->framebuffer_addr);
    framebuffer[x + width*y] = color;
}

void put_char(BootInformationFormat* multiboot_structure, int pos, uint32_t foreground, uint32_t background, char character) {
    for (int i = 1; i <= 8; i++) {
        for (int j = 0; j < 16; j++) {
            char* bitmap = (ascii_chars + character)[0];
            int bit = (bitmap[j] >> (8-i)) & 0x1;
            if (bit) {
                put_pixel(multiboot_structure, pos*8 + i, j, foreground);
            } else {
                put_pixel(multiboot_structure, pos*8+i, j, background);
            }
        }
    }
}

void printf(char* string, BootInformationFormat* multiboot_structure ,uint32_t foreground, uint32_t background) {
    int i = 0;
    while (string[i] != '\0') {
        put_char(multiboot_structure, i, foreground, background, string[i]);
        i++;
    }
}
void write_char_to_serial(char character) {
    asm("movw $0x3F8, %dx"); // Move 0x3F8 into DX
    asm("movb %0, %%al" : : "r" (character)); // Move character into AL
    asm("out %al, %dx");     // Output the contents of AX to the I/O port specified by DX
}

void write_string_to_serial(char* string) {
    int i = 0;
    while (string[i] != '\0') {
        write_char_to_serial(string[i]);
        i++;
    }
}



void kernelmain(BootInformationFormat* multiboot_structure, unsigned int magicnumber) {
    if (magicnumber != 0x2badb002) {
        write_string_to_serial("Magic number is wrong");
    }
    if ((multiboot_structure->flags & 0x01)) {
        //mem flag is set
    }
    if ((multiboot_structure->flags & (1 << 1)) >> 1) {
        //boot_device flag is set
    }
    if ((multiboot_structure->flags & (1 << 12)) >> 12) {
        uint8_t type = multiboot_structure->framebuffer_type;
        if (type == 0x0) {
            //return;
        } else if (type == 0x1) {
            for (int x = 0; x < multiboot_structure->framebuffer_width; x++) {
                for (int y = 0; y < multiboot_structure->framebuffer_height; y++) {
                    put_pixel(multiboot_structure, x, y, 0x0);
                }
            }
            printf("Hello World", multiboot_structure, 0x00ff00, 0x00);
        } else if (type = 0x2) {
            //return;
        }
        while(1);
    }
    //return;
}