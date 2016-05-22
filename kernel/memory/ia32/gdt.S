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
