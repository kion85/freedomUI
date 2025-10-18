NASM = nasm
CC   = gcc
LD   = ld

CFLAGS = -ffreestanding -O2 -Wall -Wextra -m64 -nostdlib
LDFLAGS = -n -T linker.ld

SRC = src/kernel.c src/common.c
OBJ = $(SRC:.c=.o) boot/boot.o

all: kernel.bin

boot/boot.o: boot/boot.s
	$(NASM) -f elf64 boot/boot.s -o boot/boot.o

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

kernel.bin: $(OBJ)
	$(LD) $(LDFLAGS) -o kernel.bin $(OBJ)

clean:
	rm -f $(OBJ) kernel.bin

