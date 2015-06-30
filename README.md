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


# Notes

The following integer symbols are defined in the Eclipse project.
They are used by stdint-gcc.h

__INT8_TYPE__ signed char
__UINT8_TYPE__ unsigned char
__INT16_TYPE__ short
__UINT16_TYPE__ unsigned short
__INT32_TYPE__ long
__UINT32_TYPE__ unsigned long
__INT64_TYPE__ long long
__INT64_TYPE__ unsigned long long

