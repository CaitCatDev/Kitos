.code32

.equ KERNEL_VIRT, 0xFFFFFF8000000000;



.align 16
.section .multiboot, "a"
	.4byte 0xE85250D6
	.4byte 0
	.4byte 0x10
	.4byte 0x100000000 - 0xE85250e6

	/*
	.2byte 5
	.2byte 0
	.4byte 20
	.4byte 1024
	.4byte 712
	.4byte 24
*/
	.align 8	
	.2byte 0
	.2byte 0
	.4byte 8

.text
.global _start

_start:
	cli
	cld

	/*This should never happen. but still just check it
	 *Cause if we got this far we can't be sure registers have
	 *correct Multiboot2 structs
	 */	
	mov $_not_multiboot2_error_string - KERNEL_VIRT,%esi
	cmp $0x36d76289,%eax
	jne _puts32

	lgdt _GDTR - KERNEL_VIRT

	jmp $0x18,$_flush_segments - KERNEL_VIRT

_flush_segments:
	mov $0x20,%ax
	mov %ax,%ds
	mov %ax,%es
	mov %ax,%ss
	mov %ax,%fs
	mov %ax,%gs

	mov $_stack_top - KERNEL_VIRT,%esp
	push %ebx /*Save Mb2 data struct addr*/

_is_x86_64: #Check if this CPU is 64 or just 32bit
	mov $_cpuid_error_string - KERNEL_VIRT,%esi
	pushfl
	pop %eax
	mov %eax,%ebx
	xor $1<<21,%eax
	push %eax
	popfl
	pushfl
	pop %eax
	cmp %eax,%ebx
	jz _puts32

	mov $0x80000000,%eax
	cpuid
	cmp $0x80000001,%eax
	jb _puts32
	
	mov $_cpuid_error_string - KERNEL_VIRT,%esi
	mov $0x80000001,%eax
	cpuid
	test $1<<29,%edx
	jz _puts32

_paging_init32:
	xor %eax,%eax
	mov $0x1200,%ecx
	mov $pml4 - KERNEL_VIRT,%edi
	rep stosl
	mov $pml4 - KERNEL_VIRT,%edi
	mov %edi,%cr3
	mov $pdp - KERNEL_VIRT,%ebx
	
	mov %ebx,(%edi)
	orl $0x3,(%edi)
	add $0x1000-8,%edi
	mov %ebx,(%edi)
	orl $0x3,(%edi)


	mov $pdp - KERNEL_VIRT,%edi
	mov $pd - KERNEL_VIRT,%ebx
	
	mov %ebx,(%edi)
	orl $0x3,(%edi)

	mov $pd - KERNEL_VIRT,%edi
	mov $pt1 - KERNEL_VIRT,%ebx
	
	mov %ebx,(%edi)
	orl $0x3,(%edi)
	add $8,%edi
	mov $pt2 - KERNEL_VIRT,%ebx
	mov %ebx,(%edi)
	orl $0x3,(%edi)	
	
	mov $pt1 - KERNEL_VIRT,%edi
	xor %ebx,%ebx
	orl $0x3,%ebx
	mov $1024,%ecx

	.fill_tables:
	mov %ebx,(%edi)
	add $8,%edi
	add $0x1000,%ebx
	loop .fill_tables
	
	mov $0x20,%eax
	mov %eax,%cr4

	mov $0xC0000080,%ecx
	rdmsr
	or $0x100,%eax
	wrmsr

	pop %edi

	mov %cr0,%eax
	or $1<<31,%eax
	mov %eax,%cr0
	jmp $0x28,$_lm_start - KERNEL_VIRT

_puts32:
	mov $0x4f,%ah
	mov $0xb8000,%edi
	_puts32_loop:
	lodsb
	cmp $0x00,%al
	je _hlt_forever
	stosw
	jmp _puts32_loop

_hlt_forever:
	cli
	hlt
	jmp _hlt_forever

.code64

_lm_start:
	mov $0x30,%ax
	mov %ax,%ds
	mov %ax,%ss
	mov %ax,%es
	mov %ax,%gs
	mov %ax,%fs

	movabs $KERNEL_VIRT,%rax
	add %rax,%rsp
	add %rax,%rdi

	movabs $_GDTR,%rax
	lgdt (%rax)

/*Not sure if this is really needed
	but we did reload GDT with the new
	higher half address so just feels like
	something I should do
	*/
	pushq $0x28
	movabs $_higher_half_start,%rax
	push %rax

	lretq

_higher_half_start:
	mov $0x30,%ax
	mov %ax,%ds
	mov %ax,%ss
	mov %ax,%es
	mov %ax,%gs
	mov %ax,%fs

	push %rdi
	callq idt_init
	pop %rdi

	movabs $pml4,%rsi
	xor %rax,%rax
	mov %rax,(%rsi)
	call multiboot2_cinit

halt_loop:
	cli
	hlt
	jmp halt_loop

.section .rodata
_lm_error_string:
	.asciz "Long Mode Unsupported"

_cpuid_error_string:
	.asciz "CPUID Unsupported"

_not_multiboot2_error_string:
	.asciz "Multiboot magic incorrect"

.data
_GDT:
	.null:
	.quad 0x00
	.code16:
	.word 0xffff
	.word 0x0000
	.byte 0x00
	.byte 0x9a
	.byte 0x8f
	.byte 0x00
	.data16:
	.word 0xffff
	.word 0x0000
	.byte 0x00
	.byte 0x92
	.byte 0x8f
	.byte 0x00
	.code32:
	.word 0xffff
	.word 0x0000
	.byte 0x00
	.byte 0x9a
	.byte 0xcf
	.byte 0x00
	.data32:
	.word 0xffff
	.word 0x0000
	.byte 0x00
	.byte 0x92
	.byte 0xcf
	.byte 0x00
	.code64:
	.word 0x0000
	.word 0x0000
	.byte 0x00
	.byte 0x9a
	.byte 0x20
	.byte 0x00
	.data64:
	.word 0x0000
	.word 0x0000
	.byte 0x00
	.byte 0x92
	.byte 0x00
	.byte 0x00	
.end:


_GDTR:
	.word 0x37
	.quad _GDT

.bss
.align 0x1000
.lcomm stack,0x4000
_stack_top:

.extern pml4
.extern pdp
.extern pd
.extern pt1
.extern pt2
