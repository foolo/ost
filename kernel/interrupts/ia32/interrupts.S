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


.global gpf_handler_wrapper
gpf_handler_wrapper:
	pushal
	cld

	# push error code
	movl 40(%esp),%eax
	pushl %eax
	call gpf_handler
	popl %eax

	popal
	iret


.global page_fault_handler_wrapper
page_fault_handler_wrapper:

	pushl %ds
	pushl %es
	pushl %fs
	pushl %gs
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
return_to_userspace:
	popl %eax
	popl %eax
	popl %eax

	popal
	popl %gs
	popl %fs
	popl %es
	popl %ds

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
	cld
	pushl %gs
    pushl %fs
    pushl %es
    pushl %ds
    pushl %ebp
	pushl %edx
	pushl %ecx
	pushl %ebx
	pushl %eax
	call syscall_handler
	addl $4, %esp #do not overwrite eax which contains the return value
	popl %ebx
	popl %ecx
	popl %edx
	popl %ebp
	popl %ds
	popl %es
	popl %fs
	popl %gs
	iret
