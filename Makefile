# Constants
MAX_RAMSIZE = 0x400000000
SCREEN_X = 1920
SCREEN_Y = 1080

# Flags
GCC_WARNINGS = #-Wunused-result -Wall -Wextra -Werror=implicit-function-declaration
LDFLAGS = --oformat elf64-x86-64
NASMFLAGS = -f elf64 -DMAX_RAMSIZE=$(MAX_RAMSIZE) -DSCREEN_X=$(SCREEN_X) -DSCREEN_Y=$(SCREEN_Y)
GCCFLAGS = -O2 -fno-pic -fno-pie -mcmodel=large -nostdlib -fno-exceptions -ffreestanding -mno-red-zone -fno-leading-underscore  -DMAX_RAMSIZE=$(MAX_RAMSIZE) $(GCC_WARNINGS)
INCLUDES = -I$(PWD)

# Directories and sources
SRC_DIR := src
INCLUDE_DIR := src/include
BIN_DIR := bin

C_SRCS := $(shell find $(SRC_DIR) -type f -name '*.c')
ASM_SRCS := $(shell find $(SRC_DIR) -type f -name '*.asm')

C_OBJS := $(patsubst $(SRC_DIR)/%.c, $(BIN_DIR)/%.o, $(C_SRCS))
ASM_OBJS := $(patsubst $(SRC_DIR)/%.asm, $(BIN_DIR)/%.o, $(ASM_SRCS))

OBJS := $(C_OBJS) $(ASM_OBJS)
SRCS := $(C_SRCS) $(ASM_SRCS)

TARGET := $(BIN_DIR)/mykernel.bin

PROGRESS := 0
TOTAL := $(shell bash count_tasks.sh $(SRCS))

$(TARGET): linker.ld $(OBJS)
	$(eval PROGRESS=$(shell echo $(PROGRESS)+1 | bc))
	@echo "($(PROGRESS)/$(TOTAL)) Linking kernel..."
	@ld $(LDFLAGS) -T linker.ld -o $@ $(OBJS)

$(BIN_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(eval PROGRESS=$(shell echo $(PROGRESS)+1 | bc))
	@echo "($(PROGRESS)/$(TOTAL)) Compiling $<..."
	@gcc $(GCCFLAGS) $(INCLUDES) -o $@ -c $<

$(BIN_DIR)/%.o: $(SRC_DIR)/%.asm
	@mkdir -p $(dir $@)
	$(eval PROGRESS=$(shell echo $(PROGRESS)+1 | bc))
	@echo "($(PROGRESS)/$(TOTAL)) Assembling $<..."
	@nasm $(NASMFLAGS) $< -o $@

clean:
	@echo "Cleaning up build directory..."
	@rm -rf $(BIN_DIR)

build: $(TARGET)
	@python hack.py $(BIN_DIR)/mykernel.bin load.txt

.PHONY: all clean build hello


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
	@objdump -s -x -D $< > dissasembly.txt