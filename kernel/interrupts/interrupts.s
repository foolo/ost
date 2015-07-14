.global load_idt
load_idt:
	movl 4(%esp),%edx
	lidt (%edx)
	sti 				#turn on interrupts
	ret

.global keyboard_handler
keyboard_handler:
	pushal
	cld
	call keyboard_handler_main
	popal
	iret
