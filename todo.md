- If framebuffer reaches its limit a page fault is triggered. Adding protection.
- If there happens a line break in textmode the first character in the next line gets overwritten.
- LOCAL APIC base is outside the physical RAM in MMIO (maybe also some other memory mapped devices). If the kernel tries to configure the timer for example. A page fault is triggered.
- Framebuffer virtual address is currently set to 0x4000000. Not very elegant.