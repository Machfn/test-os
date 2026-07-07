CC=gcc
TARGET=custom-os
ASM=nasm
C_FILES=./libc/string/ctos.c \
		./libc/string/memcmp.c \
		./libc/string/memset.c \
		./libc/string/strcat.c \
		./libc/string/strchr.c \
		./libc/string/strcmp.c \
		./libc/string/strcpy.c \
		./libc/string/strlen.c \
		./libc/string/strncmp.c \
		./libc/string/strstr.c \
		./libc/string/strutil.c \
		./libc/string/memcpy.c \
		./kernel/tty.c \
		./kernel/io.c \
		./kernel/multiboot.c \
		./kernel.c
OBJS=$(C_FILES:.c=.o)

all: clean $(TARGET).iso clean-objects
build: $(TARGET).iso
compile: $(TARGET)
.PHONY: all build compile clean clean-objects finale

%.o: %.c
		$(CC) -c $(@:.o=.c) -o $@ -ffreestanding -fno-exceptions -m32

start.o: start.asm
		$(ASM) -f elf start.asm -o start.o

$(TARGET): start.o $(OBJS)
		$(CC) -m32 -nostdlib -nodefaultlibs -lgcc start.o $(OBJS) -T linker.ld -o $(TARGET)

$(TARGET).iso: $(TARGET) iso/boot/grub/grub.cfg
		cp $(TARGET) ./iso/boot/$(TARGET)
		grub2-mkrescue iso --output=$(TARGET).iso

finale: $(TARGET).iso

clean:
		rm -f *.o $(TARGET) $(TARGET).iso
		find . -name \*.o -delete

clean-objects:
		rm -f *.o
		find . -name \*.o -delete
