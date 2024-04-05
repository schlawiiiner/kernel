LDGLAGS = -melf_i386
NASMFLAGS = -felf64
GCCFLAGS = -nostdlib -fno-builtin -fno-exceptions -ffreestanding -mno-red-zone -fno-leading-underscore 
INCLUDES = -I$(PWD)

objects = bin/loader.o bin/kernel.o

bin/kernel.o: src/kernel/kernel.c
	@gcc $(GCCFLAGS) $(INCLUDES) -o $@ -c $<  

bin/loader.o: src/boot/loader.asm
	@nasm $(NASMFLAGS) $< -o $@ 


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
	@qemu-system-x86_64 -cdrom test/boot/mykernel.iso -accel kvm -serial file:serial.log
	@rm -r test

