# wildcard files description
C_SOURCES = $(wildcard kernel/*.c drivers/*.c libc/*.c)
C_HEADERS = $(wildcard kernel/*.h drivers/*.h libc/*.h)
OBJECTS   = $(C_SOURCES:.c=.o kernel/interrupt.o)

# cross compiler binaries
CC = ~/opt/cross/bin/i686-elf-gcc
LD = ~/opt/cross/bin/i686-elf-ld
AS = /usr/bin/nasm
EM = /usr/bin/qemu-system-x86_64

#CFLAGS = $($(CC) -Q --help=warning | sed -e 's/^\s*\(\-\S*\)\s*\[\w*\]/\1 /gp;d' | tr -d '\n')

# Defaul build target
all: malos-disk

# Emulator
run: all
	$(EM) malos-disk -curses

# Generate disk image
malos-disk: boot/boot_sector.bin kernel/kernel.bin
	cat $^ > $@
	truncate $@ -s 17409 #39 sectors + 1 byte
	@echo -e "\n\n\033[1;33mDone. \033[0mTo start the emulator: make run"
	@echo "alt+2 'quit' to exit it."

# Build kernel binary
%.bin: kernel/kernel_entry.o $(OBJECTS)
	$(LD) -o $@ -Ttext 0x1000 $^ --oformat binary

# Generic source -> object target
%.o: %.c $(C_HEADERS)
	$(CC) -ffreestanding -c $< -o $@ -nostdlib -Wall -Wextra -pedantic -Wcast-align -Wcast-qual -Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op -Wmissing-include-dirs -Wredundant-decls -Wshadow -Wstrict-overflow=5 -Wundef -Wno-unused -Wno-variadic-macros -Wno-parentheses -fdiagnostics-show-option   -Wstrict-prototypes


# Assemble kernel_entry.
%.o: %.asm
	$(AS) $< -f elf -o $@

# Assemble boot sector
%.bin: %.asm
	$(AS) $< -f bin -I './boot/' -o $@
	@#$(AS) $< -f bin -o $@
clean:
	rm -fr *.bin *.dis *.o malos-disk
	rm -fr kernel/*.o kernel/*.bin boot/*.bin drivers/*.o libc/*.o

