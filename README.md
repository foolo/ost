# Prerequisites

	sudo apt-get install xorriso


	i686-elf-as boot.s -o boot.o
	i686-elf-g++ -c kernel.c++ -o kernel.o -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti
	i686-elf-gcc -T linker.ld -o myos.bin -ffreestanding -O2 -nostdlib boot.o kernel.o


	mkdir -p isodir
	mkdir -p isodir/boot
	cp myos.bin isodir/boot/myos.bin
	mkdir -p isodir/boot/grub
	cp grub.cfg isodir/boot/grub/grub.cfg
	grub-mkrescue -o myos.iso isodir




# Run

	qemu-system-i386 -cdrom myos.iso

	qemu-system-i386 -kernel myos.bin

