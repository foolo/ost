#include "interrupts/interrupts.h"
#include "ia32-interrupts.h"
#include "keyboard/keyboard.h"
#include "ia32/ia32-io.h"
#include "newlib/i686-ost/syscalls.h"
#include "syscalls/syscall-handler.h"

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
}

void load_idt_main()
{
	// prepare address for
	uint32_t idt_address = (uint32_t)IDT;
	uint32_t idt_ptr[2];
	idt_ptr[0] = (sizeof (struct IDT_entry) * IDT_SIZE) + ((idt_address & 0xffff) << 16);
	idt_ptr[1] = idt_address >> 16 ;

	// fill the IDT descriptor
	load_idt(idt_ptr);
}

void enable_irq(uint8_t irq)
{
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
		int file = (int)param1;
		handle_syscall_exit(file);
	}
	else if (syscall_id == SYSCALL_CLOSE)
	{
		int file = (int)param1;
		handle_syscall_close(file);
	}
	else if (syscall_id == SYSCALL_EXECVE)
	{
		char *name = (char *)param1;
		char **argv = (char **)param2;
		char **env = (char **)param3;
		handle_syscall_execve(name, argv, env);
	}
	else if (syscall_id == SYSCALL_FORK)
	{
		handle_syscall_fork();
	}
	else if (syscall_id == SYSCALL_FSTAT)
	{
		int file = (int)param1;
		struct stat *st = (struct stat *)param2;
		handle_syscall_fstat(file, st);
	}
	else if (syscall_id == SYSCALL_GETPID)
	{
		handle_syscall_getpid();
	}
	else if (syscall_id == SYSCALL_ISATTY)
	{
		int file = (int)param1;
		handle_syscall_isatty(file);
	}
	else if (syscall_id == SYSCALL_KILL)
	{
		int pid = (int)param1;
		int sig = (int)param2;
		handle_syscall_kill(pid, sig);
	}
	else if (syscall_id == SYSCALL_LINK)
	{
		char *oldpath = (char *)param1;
		char *newpath = (char *)param2;
		handle_syscall_link(oldpath, newpath);
	}
	else if (syscall_id == SYSCALL_LSEEK)
	{
		int file = (int)param1;
		int ptr = (int)param2;
		int dir = (int)param3;
		handle_syscall_lseek(file, ptr, dir);
	}
	else if (syscall_id == SYSCALL_OPEN)
	{
		char *name = (char *)param1;
		int flags = (int)param2;
		// TODO pass variable argument list
		handle_syscall_open(name, flags);
	}
	else if (syscall_id == SYSCALL_READ)
	{
		int file = (int)param1;
		char * ptr = (char *)param2;
		int len = (int)param3;
		handle_syscall_read(file, ptr, len);
	}
	else if (syscall_id == SYSCALL_SBRK)
	{
		int incr = (int)param1;
		handle_syscall_sbrk(incr);
	}
	else if (syscall_id == SYSCALL_STAT)
	{
		char *file = (char *)param1;
		struct stat *st = (struct stat *)param2;
		handle_syscall_stat(file, st);
	}
	else if (syscall_id == SYSCALL_TIMES)
	{
		struct tms *buf = (struct tms*)param1;
		handle_syscall_times(buf);
	}
	else if (syscall_id == SYSCALL_UNLINK)
	{
		char *name = (char *)param1;
		handle_syscall_unlink(name);
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
		struct timeval *p = (struct timeval*)param1;
		void *z = (void*)param2;
		handle_syscall_gettimeofday(p, z);
	}
	else
	{
		handle_unknown_syscall(syscall_id, param1, param2, param3);
	}
}

void initialize_IDT()
{
	create_idt_entry((uint32_t)keyboard_handler_wrapper, IRQ_0_VECTOR_START + KEYBOARD_IRQ);
	enable_irq(KEYBOARD_IRQ);
	create_idt_entry((uint32_t)syscall_handler_wrapper, SYSCALL_INTERRUPT_VECTOR);
	load_idt_main();
}


} // namespace
