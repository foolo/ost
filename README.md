# Make

	make

# Make a bootable cdrom .iso

Install prerequisites

	sudo apt-get install xorriso

Build the iso

	make myiso.iso

# Run

	qemu-system-i386 -kernel myos.bin

or for .iso files

	qemu-system-i386 -cdrom myos.iso

