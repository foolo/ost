#include "interrupts.h"
#include "ia32-interrupts.h"
#include "keyboard.h"
#include "ia32/ia32-io.h"
#include "newlib/i686-ost/syscalls.h"
#include "syscall-handler.h"

static const int IDT_SIZE = 256;

struct IDT_entry {
	uint16_t offset_lowerbits;
	uint16_t selector;
	uint8_t zero;
	uint8_t type_attr;
	uint16_t offset_higherbits;
};

struct IDT_entry IDT[IDT_SIZE];

extern "C" void load_idt(uint32_t *idt_ptr);

namespace kernel
{

void create_idt_entry(uint32_t callback_function_pointer, uint8_t vector_index)
{
	static const uint8_t KERNEL_CODE_SEGMENT_OFFSET = 0x08;
	static const uint8_t INTERRUPT_GATE = 0x8e;

	IDT_entry *entry = IDT + vector_index;

	/* populate IDT entry of keyboard's interrupt */
	entry->offset_lowerbits = callback_function_pointer & 0xffff;
	entry->selector = KERNEL_CODE_SEGMENT_OFFSET;
	entry->zero = 0;
	entry->type_attr = INTERRUPT_GATE;
	entry->offset_higherbits = (callback_function_pointer & 0xffff0000) >> 16;

	// prepare address for
	uint32_t idt_address = (uint32_t)IDT;
	uint32_t idt_ptr[2];
	idt_ptr[0] = (sizeof (struct IDT_entry) * IDT_SIZE) + ((idt_address & 0xffff) << 16);
	idt_ptr[1] = idt_address >> 16 ;

	// fill the IDT descriptor
	load_idt(idt_ptr);
}

void register_callback(uint32_t callback_function_pointer, uint8_t irq)
{
	uint8_t vector_index = IRQ_0_VECTOR_START + irq;
	create_idt_entry(callback_function_pointer, vector_index);

	// enable the irq
	uint8_t pic_addr = (irq < 8) ? PIC1_DATA : PIC2_DATA;
	uint8_t mask = inb(pic_addr);
	mask &= ~(1 << irq); // Clear bit number irq
	outb(pic_addr, mask);
}

extern "C" void keyboard_handler_wrapper(void);

extern "C" void keyboard_handler(void)
{
	kernel::PIC_sendEOI(KEYBOARD_IRQ);
	keyboard::handle_keyboard_scancode();
}

extern "C" void syscall_handler_wrapper(void);

extern "C" void syscall_handler(uint32_t syscall_id, uint32_t param1, uint32_t param2, uint32_t param3)
{
	if (syscall_id == SYSCALL_EXIT)
	{
	}
	else if (syscall_id == SYSCALL_CLOSE)
	{
	}
	else if (syscall_id == SYSCALL_EXECVE)
	{
	}
	else if (syscall_id == SYSCALL_FORK)
	{
	}
	else if (syscall_id == SYSCALL_FSTAT)
	{
	}
	else if (syscall_id == SYSCALL_GETPID)
	{
	}
	else if (syscall_id == SYSCALL_ISATTY)
	{
	}
	else if (syscall_id == SYSCALL_KILL)
	{
	}
	else if (syscall_id == SYSCALL_LINK)
	{
	}
	else if (syscall_id == SYSCALL_LSEEK)
	{
	}
	else if (syscall_id == SYSCALL_OPEN)
	{
	}
	else if (syscall_id == SYSCALL_READ)
	{
	}
	else if (syscall_id == SYSCALL_SBRK)
	{
	}
	else if (syscall_id == SYSCALL_STAT)
	{
	}
	else if (syscall_id == SYSCALL_TIMES)
	{
	}
	else if (syscall_id == SYSCALL_UNLINK)
	{
	}
	else if (syscall_id == SYSCALL_WRITE)
	{
		int file = (int)param1;
		char* buffer = (char*)param2;
		int length = (int)param3;
		handle_syscall_write(file, buffer, length);
	}
	else if (syscall_id == SYSCALL_GETTIMEOFDAY)
	{
	}
}

void initialize_IDT()
{
	register_callback((uint32_t)keyboard_handler_wrapper, KEYBOARD_IRQ);
}

void initialize_software_interrupts()
{
	create_idt_entry((uint32_t)syscall_handler_wrapper, SYSCALL_INTERRUPT_VECTOR);
}


} // namespace
