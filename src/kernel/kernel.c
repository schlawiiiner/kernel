#include "../../src/include/uint.h"
#include "../../src/include/bootinfo.h"
#include "../../src/include/io.h"
#include "../../src/include/graphics.h"
#include "../../src/include/pic.h"

void write_char_to_serial(char character) {
    outb(0x3f8, character);
}

void write_string_to_serial(char* string) {
    int i = 0;
    while (string[i] != '\0') {
        write_char_to_serial(string[i]);
        i++;
    }
}

void kernelmain(BootInformationFormat* multiboot_structure, unsigned int magicnumber) {
    PIC_remap(0x20, 0x28);
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
            init_framebuffer(multiboot_structure);
            init_text_mode(multiboot_structure);
            fill_screen(0x00);

        } else if (type = 0x2) {
            //return;
        }
        while(1);
    }
}