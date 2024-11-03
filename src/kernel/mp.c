#include "../../src/include/sysvar.h"
#include "../../src/include/mp.h"
#include "../../src/include/acpi.h"
#include "../../src/include/graphics.h"

void init_cpus() {
    if (!(acpi.Flags & 0b001)) {
        print("ERROR: No MADT table present");
        while(1);
    }
    uint8_t* base = (uint8_t*)acpi.MADT;
    int offset = 0x2c;
    while (offset < acpi.MADT->Length) {
        if (base[offset] == 0x0) {
            MADT_processor_local_APIC* entry0 = (MADT_processor_local_APIC*)(base+offset);
            print("Processor ");
            printdec(entry0->APIC_ID);
            if (entry0->Flags & 0b11) {
                print(": online capable\n");
            } else {
                print(": not online capable\n");
            }
            offset+= entry0->Length;
        } else {
            offset += base[offset+1];
        }
    }
}
