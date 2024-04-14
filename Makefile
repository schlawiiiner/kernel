LDGLAGS = -melf_i386
NASMFLAGS = -felf64
GCCFLAGS = -nostdlib -fno-builtin -fno-exceptions -ffreestanding -mno-red-zone -fno-leading-underscore 
INCLUDES = -I$(PWD)

objects = bin/loader.o bin/kernel.o bin/interrupts.o bin/graphics.o bin/font.o bin/pic.o
asm_files = src/boot/check.asm src/boot/interrupts.asm src/boot/loader.asm src/boot/multiboot2.asm src/boot/paging.asm src/boot/sysvar.asm

bin/kernel.o: src/kernel/kernel.c
	@gcc $(GCCFLAGS) $(INCLUDES) -O2 -o $@ -c $<  

bin/interrupts.o: src/kernel/interrupts.c
	@gcc $(GCCFLAGS) $(INCLUDES) -O2 -o $@ -c $<  

bin/graphics.o: src/kernel/graphics.c
	@gcc $(GCCFLAGS) $(INCLUDES) -O2 -o $@ -c $< 

bin/font.o: src/kernel/font.c
	@gcc $(GCCFLAGS) $(INCLUDES) -O2 -o $@ -c $< 

bin/pic.o: src/kernel/pic.c
	@gcc $(GCCFLAGS) $(INCLUDES) -O2 -o $@ -c $< 

bin/loader.o: $(asm_files)
	@nasm $(NASMFLAGS) src/boot/loader.asm -o $@ 

bin/mykernel.bin: linker.ld $(objects)
	@ld $(LDFLAGS) -T linker.ld -o $@ $(objects)


build: bin/mykernel.bin
	@make -s bin/mykernel.bin

install: bin/mykernel.bin
	@sudo cp $< /boot/mykernel.bin
	@sudo update-grub

clean:
	@cd bin && rm *

qemu:
	@make -s build
	@mkdir -p test/boot/grub
	@cp grub.cfg test/boot/grub/
	@cp bin/mykernel.bin test/boot/
	@grub-mkrescue -o test/boot/mykernel.iso test 2> /dev/null
	@qemu-system-x86_64 -bios /usr/share/ovmf/OVMF.fd -cdrom test/boot/mykernel.iso -accel kvm -serial file:serial.log
	@rm -r test

dissasemble: bin/mykernel.bin
	@objdump -s -D $< > dissasembly.txt

