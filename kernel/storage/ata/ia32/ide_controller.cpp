#include <stdio.h>
#include <unistd.h>
#include "ide_controller.h"
#include "ia32/ia32-io.h"

// Source: http://wiki.osdev.org/PCI_IDE_Controller

namespace kernel {

enum AtaStatus {
	ATA_STATUS_OK,
	ATA_STATUS_DRIVE_FAULT,
	ATA_STATUS_ERROR,
	ATA_STATUS_DRQ_NOT_SET,
};

uint8_t ide_buf[2048] = { 0 };
unsigned static char ide_irq_invoked = 0;

struct IDEChannelRegisters {
	uint16_t base;  // I/O Base.
	uint16_t ctrl;  // Control Base
	uint16_t bmide; // Bus Master IDE
	uint8_t nIEN;  // nIEN (No Interrupt);
} channels[2];

struct ide_device {
	uint8_t Reserved;    // 0 (Empty) or 1 (This Drive really exists).
	uint8_t Channel;     // 0 (Primary Channel) or 1 (Secondary Channel).
	uint8_t Drive;       // 0 (Master Drive) or 1 (Slave Drive).
	unsigned short Type;        // 0: ATA, 1:ATAPI.
	unsigned short Signature;   // Drive Signature
	unsigned short Capabilities;   // Features.
	uint32_t CommandSets; // Command Sets Supported.
	uint32_t Size;        // Size in Sectors.
	uint8_t Model[41];   // Model in string.
} ide_devices[4];

void ide_write(uint8_t channel, uint8_t reg, uint8_t data) {
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

uint8_t ide_read(uint8_t channel, uint8_t reg) {
	uint8_t result;
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

AtaStatus ide_polling(uint8_t channel, bool advanced_check) {
	// (I) Delay 400 nanosecond for BSY to be set:
	for (int i = 0; i < 4; i++) {
		ide_read(channel, ATA_REG_ALTSTATUS); // Reading the Alternate Status port wastes 100ns; loop four times.
	}
	while (ide_read(channel, ATA_REG_STATUS) & ATA_SR_BSY) {
		// Wait for BSY to be zero.
	}
	if (advanced_check) {
		uint8_t status = ide_read(channel, ATA_REG_STATUS); // Read Status Register.
		if (status & ATA_SR_ERR) {
			return ATA_STATUS_ERROR;
		}
		if (status & ATA_SR_DF) {
			return ATA_STATUS_DRIVE_FAULT;
		}
		// BSY, DF and ERR are all zero, so DRQ should be set
		if ((status & ATA_SR_DRQ) == 0) {
			return ATA_STATUS_DRQ_NOT_SET;
		}
	}
	return ATA_STATUS_OK;
}

AtaResult ide_get_result(unsigned int drive, AtaStatus status) {
	if (status == ATA_STATUS_OK) {
		return ATA_RESULT_OK;
	}
	if (status == ATA_STATUS_DRIVE_FAULT) {
		return ATA_RESULT_DRIVE_FAULT;
	}
	else if (status == ATA_STATUS_ERROR) {
		uint8_t error_flags = ide_read(ide_devices[drive].Channel, ATA_REG_ERROR);
		if (error_flags & ATA_ER_AMNF)  return ATA_RESULT_no_address_mark_Found;
		if (error_flags & ATA_ER_TK0NF) return ATA_RESULT_no_media_or_media_error;
		if (error_flags & ATA_ER_ABRT)  return ATA_RESULT_command_aborted;
		if (error_flags & ATA_ER_MCR)   return ATA_RESULT_no_media_or_media_error;
		if (error_flags & ATA_ER_IDNF)  return ATA_RESULT_id_mark_not_Found;
		if (error_flags & ATA_ER_MC)    return ATA_RESULT_no_media_or_media_error;
		if (error_flags & ATA_ER_UNC)   return ATA_RESULT_uncorrectable_data_error;
		if (error_flags & ATA_ER_BBK)   return ATA_RESULT_bad_sectors;
	}
	else if (status == ATA_STATUS_DRQ_NOT_SET) {
		return ATA_RESULT_DRQ_NOT_SET;
	}
	return ATA_RESULT_OK;
}

void ide_read_buffer(uint8_t channel, uint8_t reg, unsigned int buffer, unsigned int quads) {
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
			uint8_t err = 0, type = IDE_ATA, status;
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
				uint8_t cl = ide_read(i, ATA_REG_LBA1);
				uint8_t ch = ide_read(i, ATA_REG_LBA2);

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
			ide_devices[count].CommandSets = *((uint32_t *) (ide_buf + ATA_IDENT_COMMANDSETS));

			// (VII) Get Size:
			if (ide_devices[count].CommandSets & (1 << 26)) {
				// Device uses 48-Bit Addressing:
				ide_devices[count].Size = *((uint32_t *) (ide_buf + ATA_IDENT_MAX_LBA_EXT));
			}
			else {
				// Device uses CHS or 28-bit Addressing:
				ide_devices[count].Size = *((uint32_t *) (ide_buf + ATA_IDENT_MAX_LBA));
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
			uint32_t size_mb = ide_devices[i].Size / 1024 / 2;
			bool is_ata = (ide_devices[i].Type == 0);
			printf("Found %s Drive %lu MiB (%lu bytes) - %s\n", (is_ata ? "ATA" : "ATAPI"), size_mb, ide_devices[i].Size/2,  ide_devices[i].Model);
		}
	}
}

AtaStatus ide_ata_access(AtaDirection direction, uint8_t drive, uint32_t lba, uint8_t numsects, unsigned short selector, uint32_t edi) {
	uint8_t lba_mode; // 0: CHS, 1:LBA28, 2: LBA48
	uint8_t lba_io[6];
	uint8_t channel = ide_devices[drive].Channel; // Read the Channel.
	uint8_t slavebit = ide_devices[drive].Drive; // Read the Drive [Master/Slave]
	uint32_t bus = channels[channel].base; // Bus Base, like 0x1F0 which is also data port.
	uint32_t words = 256; // Almost every ATA drive has a sector-size of 512-byte.
	uint8_t head, sect;

	// Disable IRQ
	ide_write(channel, ATA_REG_CONTROL, channels[channel].nIEN = (ide_irq_invoked = 0x0) + 0x02);

	// (I) Select one from LBA28, LBA48 or CHS;
	if (lba >= 0x10000000) { // Sure Drive should support LBA in this case, or you are
	                         // giving a wrong LBA.
		// LBA48:
		lba_mode = 2;
		lba_io[0] = (lba & 0x000000FF) >> 0;
		lba_io[1] = (lba & 0x0000FF00) >> 8;
		lba_io[2] = (lba & 0x00FF0000) >> 16;
		lba_io[3] = (lba & 0xFF000000) >> 24;
		lba_io[4] = 0;
		lba_io[5] = 0;
		head = 0; // Lower 4-bits of HDDEVSEL are not used here.
	}
	else if (ide_devices[drive].Capabilities & 0x200) { // Drive supports LBA?
		// LBA28:
		lba_mode = 1;
		lba_io[0] = (lba & 0x000000FF) >> 0;
		lba_io[1] = (lba & 0x0000FF00) >> 8;
		lba_io[2] = (lba & 0x00FF0000) >> 16;
		lba_io[3] = 0;
		lba_io[4] = 0;
		lba_io[5] = 0;
		head = (lba & 0xF000000) >> 24;
	}
	else {
		// CHS:
		lba_mode = 0;
		sect = (lba % 63) + 1;
		unsigned short cyl = (lba + 1 - sect) / (16 * 63);
		lba_io[0] = sect;
		lba_io[1] = (cyl >> 0) & 0xFF;
		lba_io[2] = (cyl >> 8) & 0xFF;
		lba_io[3] = 0;
		lba_io[4] = 0;
		lba_io[5] = 0;
		head = (lba + 1 - sect) % (16 * 63) / (63); // Head number is written to HDDEVSEL lower 4-bits.
	}

	// (III) Wait if the drive is busy;
	while (ide_read(channel, ATA_REG_STATUS) & ATA_SR_BSY){
		// Wait if busy.
	}

	// (IV) Select Drive from the controller;
	if (lba_mode == 0) {
		ide_write(channel, ATA_REG_HDDEVSEL, 0xA0 | (slavebit << 4) | head); // Drive & CHS.
	}
	else {
		ide_write(channel, ATA_REG_HDDEVSEL, 0xE0 | (slavebit << 4) | head); // Drive & LBA
	}

	// (V) Write Parameters;
	if (lba_mode == 2) {
		ide_write(channel, ATA_REG_SECCOUNT1, 0);
		ide_write(channel, ATA_REG_LBA3, lba_io[3]);
		ide_write(channel, ATA_REG_LBA4, lba_io[4]);
		ide_write(channel, ATA_REG_LBA5, lba_io[5]);
	}
	ide_write(channel, ATA_REG_SECCOUNT0, numsects);
	ide_write(channel, ATA_REG_LBA0, lba_io[0]);
	ide_write(channel, ATA_REG_LBA1, lba_io[1]);
	ide_write(channel, ATA_REG_LBA2, lba_io[2]);

	// (VI) Select the command and send it;
	uint8_t cmd;
	if (lba_mode == 0 && direction == ATA_READ) cmd = ATA_CMD_READ_PIO;
	if (lba_mode == 1 && direction == ATA_READ) cmd = ATA_CMD_READ_PIO;
	if (lba_mode == 2 && direction == ATA_READ) cmd = ATA_CMD_READ_PIO_EXT;
	if (lba_mode == 0 && direction == ATA_WRITE) cmd = ATA_CMD_WRITE_PIO;
	if (lba_mode == 1 && direction == ATA_WRITE) cmd = ATA_CMD_WRITE_PIO;
	if (lba_mode == 2 && direction == ATA_WRITE) cmd = ATA_CMD_WRITE_PIO_EXT;

	ide_write(channel, ATA_REG_COMMAND, cmd); // Send the Command.

	if (direction == ATA_READ) {
		for (int i = 0; i < numsects; i++) {
			AtaStatus result = ide_polling(channel, true);
			if (result != ATA_STATUS_OK) {
				return result;
			}
			asm("pushw %es");
			asm("mov %%ax, %%es" : : "a"(selector));
			asm("rep insw" : : "c"(words), "d"(bus), "D"(edi));
			// Receive Data.
			asm("popw %es");
			edi += (words * 2);
		}
	}
	else {
		// ATA_WRITE
		for (int i = 0; i < numsects; i++) {
			ide_polling(channel, false);
			asm("pushw %ds");
			asm("mov %%ax, %%ds"::"a"(selector));
			asm("rep outsw"::"c"(words), "d"(bus), "S"(edi));
			// Send Data
			asm("popw %ds");
			edi += (words * 2);
		}
		ide_write(channel, ATA_REG_COMMAND, (uint8_t[] ) { ATA_CMD_CACHE_FLUSH,
				ATA_CMD_CACHE_FLUSH,
				ATA_CMD_CACHE_FLUSH_EXT } [lba_mode]);
		ide_polling(channel, 0);
	}
	return ATA_STATUS_OK;
}

void ide_wait_irq() {
	while (!ide_irq_invoked) {
		// Wait
	}
	ide_irq_invoked = 0;
}

void ide_irq() {
	ide_irq_invoked = 1;
}

AtaResult ide_read_sectors(uint8_t drive, uint8_t numsects, uint32_t lba, unsigned short es, uint32_t edi) {

	if (drive > 3 || ide_devices[drive].Reserved == 0) {
		return ATA_RESULT_DRIVE_NOT_FOUND;
	}
	else if (((lba + numsects) > ide_devices[drive].Size) && (ide_devices[drive].Type == IDE_ATA)) {
		return ATA_RESULT_INVALID_POSITION;
	}
	// 3: Read in PIO Mode through Polling & IRQs:
	if (ide_devices[drive].Type == IDE_ATA) {
		AtaStatus status = ide_ata_access(ATA_READ, drive, lba, numsects, es, edi);
		return ide_get_result(drive, status);
	}
	else if (ide_devices[drive].Type == IDE_ATAPI) {
		printf("ATAPI read not implemented\n");
		/*for (int i = 0; i < numsects; i++) {
			err = ide_atapi_read(drive, lba + i, 1, es, edi + (i * 2048));
			printf("ok\n");
		}*/
	}
	return ATA_RESULT_unknown;
}

} // namespace kernel
