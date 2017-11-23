#include <stdio.h>
#include <stdint.h>
#include "keyboard/keyboard.h"
#include "ia32/ia32-io.h"

// Data and command ports
static const uint8_t DATA_PORT = 0x60;
static const uint8_t STATUS_PORT_READ = 0x64;
static const uint8_t COMMAND_PORT_WRITE = 0x64;

// Commands
static const uint8_t CMD_READ_CONTROLLER_CONFIGURATION = 0x20;
static const uint8_t CMD_WRITE_CONTROLLER_CONFIGURATION = 0x60;

static const uint8_t CMD_DISABLE_FIRST_PS2_PORT = 0xAD;
static const uint8_t CMD_DISABLE_SECOND_PS2_PORT = 0xA7;

static const uint8_t CMD_ENABLE_FIRST_PS2_PORT = 0xAE;
static const uint8_t CMD_ENABLE_SECOND_PS2_PORT = 0xA8;

static const uint8_t CMD_SELF_TEST = 0xAA;
static const uint8_t RSP_SELF_TEST_PASSED = 0x55;
static const uint8_t CMD_TEST_FIRST_PORT = 0xAB;
static const uint8_t RSP_PORT_TEST_PASSED = 0x00;


#define BITNO(n) (1 << (n))

// Configuration bits
static const uint8_t ENABLE_FIRST_PORT_INTERRUPTS =  BITNO(0);
static const uint8_t ENABLE_SECOND_PORT_INTERRUPTS = BITNO(1);
static const uint8_t SYSTEM_FLAG =  BITNO(2);
static const uint8_t DISABLE_FIRST_CLOCK =  BITNO(4);
static const uint8_t DISABLE_SECOND_CLOCK = BITNO(5);
static const uint8_t ENABLE_FIRST_PORT_TRANSLATION = BITNO(6);

void disable_devices()
{
	outb(COMMAND_PORT_WRITE, CMD_DISABLE_FIRST_PS2_PORT);
	outb(COMMAND_PORT_WRITE, CMD_DISABLE_SECOND_PS2_PORT);
}

uint8_t read_configuration()
{
	outb(COMMAND_PORT_WRITE, CMD_READ_CONTROLLER_CONFIGURATION);
	uint8_t conf_byte = inb(DATA_PORT);
	return conf_byte;
}

void write_configuration(uint8_t configuration)
{
	outb(COMMAND_PORT_WRITE, CMD_WRITE_CONTROLLER_CONFIGURATION);
	outb(DATA_PORT, configuration);
}

bool initialize_keyboard_controller()
{
	disable_devices();

	// Flush output buffer
	inb(DATA_PORT);

	// Disable interrupts and translation
	uint8_t conf_byte = read_configuration();
	conf_byte &= ~(ENABLE_FIRST_PORT_INTERRUPTS | ENABLE_SECOND_PORT_INTERRUPTS | ENABLE_FIRST_PORT_TRANSLATION);
	write_configuration(conf_byte);

	// Controller self test
	outb(COMMAND_PORT_WRITE, CMD_SELF_TEST);
	uint8_t test_result = inb(DATA_PORT);
	if (test_result != RSP_SELF_TEST_PASSED)
	{
		disable_devices();
		return false;
	}

	// Port tests
	outb(COMMAND_PORT_WRITE, CMD_TEST_FIRST_PORT);
	test_result = inb(DATA_PORT);
	if (test_result != RSP_PORT_TEST_PASSED)
	{
		disable_devices();
		return false;
	}

	// Enable devices
	outb(COMMAND_PORT_WRITE, CMD_ENABLE_FIRST_PS2_PORT);
	outb(COMMAND_PORT_WRITE, CMD_ENABLE_SECOND_PS2_PORT);

	// Enable interrupts
	conf_byte = read_configuration();
	conf_byte |= (ENABLE_FIRST_PORT_INTERRUPTS | ENABLE_SECOND_PORT_INTERRUPTS);
	write_configuration(conf_byte);

	return true;
}

bool read_scancode(uint8_t *scancode_out)
{
	uint8_t status = inb(STATUS_PORT_READ);
	if (status & 0x01)
	{
		*scancode_out = inb(DATA_PORT);
		return true;
	}
	return false;
}
