#include "../../src/include/gdt.h"

void make_GDT_entry(GlobalDescriptorTable* gdt, uint16_t selector, uint32_t limit, uint32_t base, uint8_t access_byte, uint8_t flags) {
    SegmentDescriptor* descr = (SegmentDescriptor*)((uint64_t)gdt + selector);
    descr->Base0 = base & 0xffff;
    descr->Base1 = (base >> 16) & 0xff;
    descr->Base2 = (base >> 24) & 0xff;
    descr->Limit0 = limit & 0xffff;
    descr->Limit1_Flags = ((limit >> 16) & 0xf) | ((flags & 0xf) << 4);
    descr->AccessByte = access_byte;
}

void make_GDT_default_entries(GlobalDescriptorTable* gdt) {
    make_GDT_entry(gdt, 0x0, 0x0, 0x0, 0x0, 0x0);
    make_GDT_entry(gdt, 0x8, 0xfffff, 0x0, GDT_PRESENT | DPL_RING0 | SEGMENT_DESCRIPTOR | READABLE_WRITABLE | EXECUTABLE, GRANULARITY | LONG_MODE);
    make_GDT_entry(gdt, 0x10, 0xfffff, 0x0, GDT_PRESENT | DPL_RING0 | SEGMENT_DESCRIPTOR | READABLE_WRITABLE, GRANULARITY | SIZE);
    make_GDT_entry(gdt, 0x18, 0xfffff, 0x0, GDT_PRESENT | DPL_RING3 | SEGMENT_DESCRIPTOR | READABLE_WRITABLE | EXECUTABLE, GRANULARITY | LONG_MODE);
    make_GDT_entry(gdt, 0x20, 0xfffff, 0x0, GDT_PRESENT | DPL_RING3 | SEGMENT_DESCRIPTOR | READABLE_WRITABLE, GRANULARITY | SIZE);
}

void make_TSS_entry(GlobalDescriptorTable* gdt, uint8_t n, TaskStateSegment* tss) {
    uint32_t limit = sizeof(TaskStateSegment) - 1;
    uint64_t base = (uint64_t)tss;
    gdt->TSS[n].Limit0 = limit & 0xffff;
    gdt->TSS[n].Base0 = base & 0xffff;
    gdt->TSS[n].Base1 = (base >> 16) & 0xff;
    gdt->TSS[n].Base2 = (base >> 24) & 0xff;
    gdt->TSS[n].Base3 = (base >> 32);
    gdt->TSS[n].Limit1_Flags = ((limit >> 16) & 0xf);
    gdt->TSS[n].AccessByte = GDT_PRESENT | DPL_RING0 | TSS_AVAILABLE_TYPE;
}

void load_GDT(GDTR* gdtr) {
    asm volatile("cli");
    asm volatile("lgdt %0" :: "m" ((uint64_t)gdtr) : "memory");
    asm volatile("sti");
}