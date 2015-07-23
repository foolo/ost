.global load_idt
load_idt:
	movl 4(%esp),%edx
	lidt (%edx)
	sti 				#turn on interrupts
	ret

.global keyboard_handler_wrapper
keyboard_handler_wrapper:
	pushal
	cld
	call keyboard_handler
	popal
	iret

.global syscall_handler_wrapper
syscall_handler_wrapper:
	pushal
	cld
	call syscall_handler
	popal
	iret
