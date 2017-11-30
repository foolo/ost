#pragma once
#include <stdint.h>

static const uint8_t PIC1_COMMAND  = 0x20;   // Master PIC IO base address
static const uint8_t PIC1_DATA     = 0x21;
static const uint8_t PIC2_COMMAND  = 0xA0;   // Slave PIC IO base address
static const uint8_t PIC2_DATA     = 0xA1;

static const uint8_t DIVIDE_BY_ZERO_EXCEPTION_VECTOR = 0x00;
static const uint8_t GPF_EXCEPTION_VECTOR = 0x0d;
static const uint8_t PAGE_FAULT_EXCEPTION_VECTOR = 0x0e;

static const uint8_t IRQ_0_VECTOR_START = 0x20;

static const uint8_t KEYBOARD_IRQ = 1;

static const uint8_t SYSCALL_INTERRUPT_VECTOR = 0x80;
