#include <stdio.h>
#include <unistd.h>
#include "ide_controller.h"
#include "ia32/ia32-io.h"

// Source: http://wiki.osdev.org/PCI_IDE_Controller

namespace kernel {

unsigned char ide_buf[2048] = { 0 };

struct IDEChannelRegisters {
	unsigned short base;  // I/O Base.
	unsigned short ctrl;  // Control Base
	unsigned short bmide; // Bus Master IDE
	unsigned char nIEN;  // nIEN (No Interrupt);
} channels[2];

struct ide_device {
	unsigned char Reserved;    // 0 (Empty) or 1 (This Drive really exists).
	unsigned char Channel;     // 0 (Primary Channel) or 1 (Secondary Channel).
	unsigned char Drive;       // 0 (Master Drive) or 1 (Slave Drive).
	unsigned short Type;        // 0: ATA, 1:ATAPI.
	unsigned short Signature;   // Drive Signature
	unsigned short Capabilities;   // Features.
	unsigned int CommandSets; // Command Sets Supported.
	unsigned int Size;        // Size in Sectors.
	unsigned char Model[41];   // Model in string.
} ide_devices[4];

void ide_write(unsigned char channel, unsigned char reg, unsigned char data) {
	if (reg > 0x07 && reg < 0x0C) {
		ide_write(channel, ATA_REG_CONTROL, 0x80 | channels[channel].nIEN);
	}
	if (reg < 0x08) {
		outb(channels[channel].base + reg - 0x00, data);
	}
	else if (reg < 0x0C) {
		outb(channels[channel].base + reg - 0x06, data);
	}
	else if (reg < 0x0E) {
		outb(channels[channel].ctrl + reg - 0x0A, data);
	}
	else if (reg < 0x16) {
		outb(channels[channel].bmide + reg - 0x0E, data);
	}
	if (reg > 0x07 && reg < 0x0C) {
		ide_write(channel, ATA_REG_CONTROL, channels[channel].nIEN);
	}
}

unsigned char ide_read(unsigned char channel, unsigned char reg) {
	unsigned char result;
	if (reg > 0x07 && reg < 0x0C) {
		ide_write(channel, ATA_REG_CONTROL, 0x80 | channels[channel].nIEN);
	}
	if (reg < 0x08) {
		result = inb(channels[channel].base + reg - 0x00);
	}
	else if (reg < 0x0C) {
		result = inb(channels[channel].base + reg - 0x06);
	}
	else if (reg < 0x0E) {
		result = inb(channels[channel].ctrl + reg - 0x0A);
	}
	else if (reg < 0x16) {
		result = inb(channels[channel].bmide + reg - 0x0E);
	}
	if (reg > 0x07 && reg < 0x0C) {
		ide_write(channel, ATA_REG_CONTROL, channels[channel].nIEN);
	}
	return result;
}

void ide_read_buffer(unsigned char channel, unsigned char reg, unsigned int buffer, unsigned int quads) {
	/* WARNING: This code contains a serious bug. The inline assembly trashes ES and
	 *           ESP for all of the code the compiler generates between the inline
	 *           assembly blocks.
	 */
	if (reg > 0x07 && reg < 0x0C) {
		ide_write(channel, ATA_REG_CONTROL, 0x80 | channels[channel].nIEN);
	}
	asm("pushw %es; movw %ds, %ax; movw %ax, %es");
	if (reg < 0x08) {
		insl(channels[channel].base + reg - 0x00, buffer, quads);
	}
	else if (reg < 0x0C) {
		insl(channels[channel].base + reg - 0x06, buffer, quads);
	}
	else if (reg < 0x0E) {
		insl(channels[channel].ctrl + reg - 0x0A, buffer, quads);
	}
	else if (reg < 0x16) {
		insl(channels[channel].bmide + reg - 0x0E, buffer, quads);
	}
	asm("popw %es;");
	if (reg > 0x07 && reg < 0x0C) {
		ide_write(channel, ATA_REG_CONTROL, channels[channel].nIEN);
	}
}

void ide_initialize(unsigned int BAR0, unsigned int BAR1, unsigned int BAR2, unsigned int BAR3, unsigned int BAR4) {

	int count = 0;

	// 1- Detect I/O Ports which interface IDE Controller:
	channels[ATA_PRIMARY].base = (BAR0 & 0xFFFFFFFC) + 0x1F0 * (!BAR0);
	channels[ATA_PRIMARY].ctrl = (BAR1 & 0xFFFFFFFC) + 0x3F6 * (!BAR1);
	channels[ATA_SECONDARY].base = (BAR2 & 0xFFFFFFFC) + 0x170 * (!BAR2);
	channels[ATA_SECONDARY].ctrl = (BAR3 & 0xFFFFFFFC) + 0x376 * (!BAR3);
	channels[ATA_PRIMARY].bmide = (BAR4 & 0xFFFFFFFC) + 0; // Bus Master IDE
	channels[ATA_SECONDARY].bmide = (BAR4 & 0xFFFFFFFC) + 8; // Bus Master IDE

	// 2- Disable IRQs:
	ide_write(ATA_PRIMARY, ATA_REG_CONTROL, 2);
	ide_write(ATA_SECONDARY, ATA_REG_CONTROL, 2);

	// 3- Detect ATA-ATAPI Devices:
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) {
			unsigned char err = 0, type = IDE_ATA, status;
			ide_devices[count].Reserved = 0; // Assuming that no drive here.

			// (I) Select Drive:
			ide_write(i, ATA_REG_HDDEVSEL, 0xA0 | (j << 4)); // Select Drive.
			sleep(1); // Wait 1ms for drive select to work.

			// (II) Send ATA Identify Command:
			ide_write(i, ATA_REG_COMMAND, ATA_CMD_IDENTIFY);
			sleep(1); // This function should be implemented in your OS. which waits for 1 ms.
			          // it is based on System Timer Device Driver.

			// (III) Polling:
			if (ide_read(i, ATA_REG_STATUS) == 0) {
				continue; // If Status = 0, No Device.
			}

			while (1) {
				status = ide_read(i, ATA_REG_STATUS);
				if ((status & ATA_SR_ERR)) {
					err = 1;
					break;
				} // If Err, Device is not ATA.
				if (!(status & ATA_SR_BSY) && (status & ATA_SR_DRQ)) {
					break; // Everything is right.
				}
			}

			// (IV) Probe for ATAPI Devices:
			if (err != 0) {
				unsigned char cl = ide_read(i, ATA_REG_LBA1);
				unsigned char ch = ide_read(i, ATA_REG_LBA2);

				if (cl == 0x14 && ch == 0xEB) {
					type = IDE_ATAPI;
				}
				else if (cl == 0x69 && ch == 0x96) {
					type = IDE_ATAPI;
				}
				else {
					continue; // Unknown Type (may not be a device).
				}

				ide_write(i, ATA_REG_COMMAND, ATA_CMD_IDENTIFY_PACKET);
				sleep(1);
			}

			// (V) Read Identification Space of the Device:
			ide_read_buffer(i, ATA_REG_DATA, (unsigned int) ide_buf, 128);

			// (VI) Read Device Parameters:
			ide_devices[count].Reserved = 1;
			ide_devices[count].Type = type;
			ide_devices[count].Channel = i;
			ide_devices[count].Drive = j;
			ide_devices[count].Signature = *((unsigned short *) (ide_buf + ATA_IDENT_DEVICETYPE));
			ide_devices[count].Capabilities = *((unsigned short *) (ide_buf + ATA_IDENT_CAPABILITIES));
			ide_devices[count].CommandSets = *((unsigned int *) (ide_buf + ATA_IDENT_COMMANDSETS));

			// (VII) Get Size:
			if (ide_devices[count].CommandSets & (1 << 26)) {
				// Device uses 48-Bit Addressing:
				ide_devices[count].Size = *((unsigned int *) (ide_buf + ATA_IDENT_MAX_LBA_EXT));
			}
			else {
				// Device uses CHS or 28-bit Addressing:
				ide_devices[count].Size = *((unsigned int *) (ide_buf + ATA_IDENT_MAX_LBA));
			}

			// (VIII) String indicates model of device (like Western Digital HDD and SONY DVD-RW...):
			for (int k = 0; k < 40; k += 2) {
				ide_devices[count].Model[k] = ide_buf[ATA_IDENT_MODEL + k + 1];
				ide_devices[count].Model[k + 1] = ide_buf[ATA_IDENT_MODEL + k];
			}
			ide_devices[count].Model[40] = 0; // Terminate String.

			count++;
		}
	}

	// 4- Print Summary:
	for (int i = 0; i < 4; i++) {
		if (ide_devices[i].Reserved == 1) {
			unsigned int size_mb = ide_devices[i].Size / 1024 / 2;
			bool is_ata = (ide_devices[i].Type == 0);
			printf(" Found %s Drive %dMiB - %s\n", (is_ata ? "ATA" : "ATAPI"), size_mb, ide_devices[i].Model);
		}
	}
}

} // namespace kernel
