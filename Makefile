CC=$(HOME)/opt/cross/bin/i686-elf-gcc
AS=$(HOME)/opt/cross/bin/i686-elf-as

all: tetrisos.iso

tetrisos.iso: isodir
	grub-mkrescue -o tetrisos.iso isodir

isodir: tetrisos
	mkdir -p isodir/boot/grub
	cp grub.cfg isodir/boot/grub
	cp tetrisos isodir/boot/tetrisos

tetrisos: boot.o kernel.o
	$(CC) -T linker.ld -o tetrisos -ffreestanding -O2 -nostdlib boot.o kernel.o -lgcc

boot.o: boot.s
	$(AS) boot.s -o boot.o 

kernel.o: kernel.c
	$(CC) -c kernel.c -o kernel.o -ffreestanding -O2 -std=gnu99 -Wall -Wextra


clean:
	rm -rf isodir boot.o kernel.o tetrisos