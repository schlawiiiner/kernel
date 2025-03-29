MAX_RAMSIZE = 0x400000000
SCREEN_X = 1920
SCREEN_Y = 1080

LDFLAGS = --oformat elf64-x86-64
NASMFLAGS = -felf64 -DMAX_RAMSIZE=$(MAX_RAMSIZE) -DSCREEN_X=$(SCREEN_X) -DSCREEN_Y=$(SCREEN_Y)
GCCFLAGS = -mcmodel=large -nostdlib -fno-builtin -fno-exceptions -ffreestanding -mno-red-zone -fno-leading-underscore -Wunused-result -DMAX_RAMSIZE=$(MAX_RAMSIZE)
INCLUDES = -I$(PWD)

objects = bin/loader.o bin/kernel.o bin/interrupts.o bin/graphics.o bin/font.o bin/serial_port.o bin/acpi.o bin/apic.o bin/ioapic.o bin/pci.o bin/msi.o bin/utils.o bin/gdt.o bin/memory.o bin/vmem.o bin/pmem.o bin/mmap.o bin/mp.o bin/slab.o bin/nvme.o bin/io.o
asm_files = src/boot/check.asm src/boot/interrupts.asm src/boot/loader.asm src/boot/multiboot2.asm src/boot/paging.asm src/boot/apic.asm src/boot/mp.asm src/boot/user.asm

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

bin/apic.o: src/kernel/apic.c
	@gcc $(GCCFLAGS) $(INCLUDES) -O2 -o $@ -c $< 

bin/ioapic.o: src/kernel/ioapic.c
	@gcc $(GCCFLAGS) $(INCLUDES) -O2 -o $@ -c $< 

bin/pci.o: src/kernel/pci.c
	@gcc $(GCCFLAGS) $(INCLUDES) -O2 -o $@ -c $<

bin/msi.o: src/kernel/msi.c
	@gcc $(GCCFLAGS) $(INCLUDES) -O2 -o $@ -c $<

bin/utils.o: src/mm/utils.c
	@gcc $(GCCFLAGS) $(INCLUDES) -O2 -o $@ -c $<

bin/gdt.o: src/kernel/gdt.c 
	@gcc $(GCCFLAGS) $(INCLUDES) -O2 -o $@ -c $<

bin/memory.o: src/mm/memory.c 
	@gcc $(GCCFLAGS) $(INCLUDES) -O2 -o $@ -c $<

bin/vmem.o: src/mm/vmem.c 
	@gcc $(GCCFLAGS) $(INCLUDES) -O2 -o $@ -c $<

bin/pmem.o: src/mm/pmem.c 
	@gcc $(GCCFLAGS) $(INCLUDES) -O2 -o $@ -c $<

bin/mmap.o: src/mm/mmap.c 
	@gcc $(GCCFLAGS) $(INCLUDES) -O2 -o $@ -c $<

bin/mp.o: src/kernel/mp.c 
	@gcc $(GCCFLAGS) $(INCLUDES) -O2 -o $@ -c $<

bin/slab.o: src/mm/slab.c 
	@gcc $(GCCFLAGS) $(INCLUDES) -O2 -o $@ -c $<

bin/nvme.o: src/driver/nvme/nvme.c 
	@gcc $(GCCFLAGS) $(INCLUDES) -O2 -c $< -o $@

bin/io.o:  src/driver/nvme/io.c
	@gcc $(GCCFLAGS) $(INCLUDES) -O2 -c $< -o $@

bin/loader.o: $(asm_files)
	@nasm $(NASMFLAGS) src/boot/loader.asm -o $@ 

bin/mykernel.bin: linker.ld $(objects)
	@ld $(LDFLAGS) -T linker.ld -o $@ $(objects)


build: bin/mykernel.bin
	@make -s bin/mykernel.bin
	@python hack.py bin/mykernel.bin load.txt

install: bin/mykernel.bin
	@make build
	@sudo cp $< /boot/mykernel.bin
	@sudo update-grub

clean:
	@cd bin && rm *

disk:
	@qemu-img create -f raw disk.img 1G
	@sudo parted disk.img mklabel gpt
	@sudo parted disk.img mkpart primary ext4 50% 50%

qemu:
	@make -s build
	@rm -rf test/
	@mkdir -p test/boot/grub
	@cp grub.cfg test/boot/grub/
	@cp bin/mykernel.bin test/boot/
	@grub-mkrescue -o test/boot/mykernel.iso test 2> /dev/null
	@qemu-system-x86_64 \
	-machine q35 \
	-m 2G \
	-smp cores=4 \
	-device qemu-xhci \
	-device usb-kbd \
	-device usb-mouse \
	-bios /usr/share/ovmf/OVMF.fd \
	-cdrom test/boot/mykernel.iso \
	-enable-kvm \
	-serial file:serial.log \
	-cpu max -no-reboot \
	-monitor stdio \
	-drive file=disk.img,if=none,id=nvme0,format=raw \
    -device nvme,drive=nvme0,serial=1234 \
	-trace enable=___
	@rm -r test 

dissasemble: bin/mykernel.bin
	@objdump -s -D $< > dissasembly.txt

