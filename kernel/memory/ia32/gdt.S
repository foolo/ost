 .global load_gdt
 load_gdt:
	cli
	movl 4(%esp),%edx
	lgdt (%edx)
	sti
	ret

 .global reloadSegments
 reloadSegments:
    # Reload CS register containing code selector:
    # 0x08 points at the new code selector
    ljmp $0x0008, $reload_CS
 reload_CS:
    # Reload data segment registers:
    movw  $0x10,%ax # 0x10 points at the new data selector
    movw  %ax,%ds
    movw  %ax,%es
    movw  %ax,%fs
    movw  %ax,%gs
    movw  %ax,%ss
    ret


 .global activate_tss
 activate_tss:
	# Load the index of our TSS structure - The index is
	# 0x28, as it is the 5th selector and each is 8 bytes
	# long, but we set the bottom two bits (making 0x2B)
	# so that it has an RPL of 3, not zero.
	movw 4(%esp),%ax
	movw %ax,%bx
	ltrw %ax          # Load 0x2B into the task state register.
	ret


.global get_esp
get_esp:
	movl %esp,%eax
	ret

.global jump_usermode
.extern sample_user_function
jump_usermode:
	movw $0x23,%ax
	movw %ax,%ds
	movw %ax,%es
	movw %ax,%fs
	movw %ax,%gs #we don't need to worry about SS. it's handled by iret


	pushl $0x23 #user data segment with bottom 2 bits set for ring 3
	movl $0xFFFFFFFC,%eax
	pushl %eax #user stack pointer to 0xFFFFFFFC
	pushf
	pushl $0x1B # ;user code segment with bottom 2 bits set for ring 3
	push $0x8048080

	movl $0,%eax
	movl %eax, %edi;
	movl %eax, %esi;
	movl %eax, %ebp;
	movl %eax, %ebx;
	movl %eax, %edx;
	movl %eax, %ecx;
	movl %eax, %eax;

	iret
