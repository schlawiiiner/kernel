MAX_RAMSIZE = 0x400000000
SCREEN_X = 1024
SCREEN_Y = 600

LDFLAGS = --oformat elf64-x86-64
NASMFLAGS = -felf64 -DMAX_RAMSIZE=$(MAX_RAMSIZE) -DSCREEN_X=$(SCREEN_X) -DSCREEN_Y=$(SCREEN_Y)
GCCFLAGS = -mcmodel=large -nostdlib -fno-builtin -fno-exceptions -ffreestanding -mno-red-zone -fno-leading-underscore -DMAX_RAMSIZE=$(MAX_RAMSIZE)
INCLUDES = -I$(PWD)

objects = bin/loader.o bin/kernel.o bin/interrupts.o bin/graphics.o bin/font.o bin/serial_port.o bin/acpi.o bin/apic.o bin/ioapic.o bin/paging.o bin/pci.o bin/xhci.o bin/msi.o bin/allocator.o bin/utils.o bin/thread.o bin/gdt.o bin/memory.o bin/vmem.o
asm_files = src/boot/check.asm src/boot/interrupts.asm src/boot/loader.asm src/boot/multiboot2.asm src/boot/paging.asm src/boot/apic.asm src/boot/mp.asm

bin/kernel.o: src/kernel/kernel.c
	@gcc $(GCCFLAGS) $(INCLUDES) -O2 -o $@ -c $<  

bin/interrupts.o: src/kernel/interrupts.c
	@gcc $(GCCFLAGS) $(INCLUDES) -O2 -o $@ -c $<  

bin/graphics.o: src/kernel/graphics.c
	@gcc $(GCCFLAGS) $(INCLUDES) -O2 -o $@ -c $< 

bin/font.o: src/kernel/font.c
	@gcc $(GCCFLAGS) $(INCLUDES) -O2 -o $@ -c $< 

bin/serial_port.o: src/kernel/serial_port.c
	@gcc $(GCCFLAGS) $(INCLUDES) -O2 -o $@ -c $< 

bin/acpi.o: src/kernel/acpi.c
	@gcc $(GCCFLAGS) $(INCLUDES) -O2 -o $@ -c $< 

bin/paging.o: src/mm/paging.c
	@gcc $(GCCFLAGS) $(INCLUDES) -O2 -o $@ -c $< 

bin/apic.o: src/kernel/apic.c
	@gcc $(GCCFLAGS) $(INCLUDES) -O2 -o $@ -c $< 

bin/ioapic.o: src/kernel/ioapic.c
	@gcc $(GCCFLAGS) $(INCLUDES) -O2 -o $@ -c $< 

bin/pci.o: src/kernel/pci.c
	@gcc $(GCCFLAGS) $(INCLUDES) -O2 -o $@ -c $<

bin/xhci.o: src/driver/xhci.c
	@gcc $(GCCFLAGS) $(INCLUDES) -O2 -o $@ -c $<

bin/msi.o: src/kernel/msi.c
	@gcc $(GCCFLAGS) $(INCLUDES) -O2 -o $@ -c $<

bin/allocator.o: src/mm/allocator.c
	@gcc $(GCCFLAGS) $(INCLUDES) -O2 -o $@ -c $<

bin/utils.o: src/mm/utils.c
	@gcc $(GCCFLAGS) $(INCLUDES) -O2 -o $@ -c $<

bin/thread.o: src/kernel/thread.c
	@gcc $(GCCFLAGS) $(INCLUDES) -O2 -o $@ -c $<

bin/gdt.o: src/kernel/gdt.c 
	@gcc $(GCCFLAGS) $(INCLUDES) -O2 -o $@ -c $<

bin/memory.o: src/mm/memory.c 
	@gcc $(GCCFLAGS) $(INCLUDES) -O2 -o $@ -c $<

bin/vmem.o: src/mm/vmem.c 
	@gcc $(GCCFLAGS) $(INCLUDES) -O2 -o $@ -c $<


bin/loader.o: $(asm_files)
	@nasm $(NASMFLAGS) src/boot/loader.asm -o $@ 

bin/mykernel.bin: linker.ld $(objects)
	@ld $(LDFLAGS) -T linker.ld -o $@ $(objects)


build: bin/mykernel.bin
	@make -s bin/mykernel.bin
	@python hack.py bin/mykernel.bin load.txt

install: bin/mykernel.bin
	@sudo cp $< /boot/mykernel.bin
	@sudo update-grub

clean:
	@cd bin && rm *

qemu:
	@make -s build
	@rm -rf test/
	@mkdir -p test/boot/grub
	@cp grub.cfg test/boot/grub/
	@cp bin/mykernel.bin test/boot/
	@grub-mkrescue -o test/boot/mykernel.iso test 2> /dev/null
	@qemu-system-x86_64 \
	-machine q35  \
	-m 2G \
	-smp cores=2 \
	-device qemu-xhci \
	-device usb-kbd \
	-device usb-mouse \
	-bios /usr/share/ovmf/OVMF.fd \
	-cdrom test/boot/mykernel.iso \
	-accel kvm \
	-serial file:serial.log \
	-cpu max
	@rm -r test

dissasemble: bin/mykernel.bin
	@objdump -s -D $< > dissasembly.txt

