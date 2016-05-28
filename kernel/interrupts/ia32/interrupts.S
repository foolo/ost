.global load_idt
load_idt:
	movl 4(%esp),%edx
	lidt (%edx)
	sti 				#turn on interrupts
	ret

.global div0_handler_wrapper
div0_handler_wrapper:
	pushal
	cld
	call div0_handler
	popal
	iret

.global page_fault_handler_wrapper
page_fault_handler_wrapper:
	pushal
	cld


	# push stack address
	movl %esp,%eax
	pushl %eax

	# push cr2 (Page Fault Linear Address)
	movl %cr2,%eax
	pushl %eax


	# push error code
	movl 40(%esp),%eax
	pushl %eax

	call page_fault_handler

	popl %eax
	popl %eax
	popl %eax

	popal
	iret

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
	pushl %edx
	pushl %ecx
	pushl %ebx
	pushl %eax
	call syscall_handler
	popl %eax
	popl %eax
	popl %eax
	popl %eax
	popal
	iret
