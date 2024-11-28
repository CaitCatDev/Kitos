.code64
.text
.global isr_functions
	
.macro isr_no_error no
_isr\no:
	pushq $0x00 /*Dummy Error code*/
	pushq $\no
	jmp isr_common
.endm

.macro isr_error no
_isr\no:
	pushq $\no
	jmp isr_common
.endm

.macro isr_ptr no
	.quad _isr\no
.endm

.extern isr_handler


isr_no_error 0
isr_no_error 1
isr_no_error 2
isr_no_error 3
isr_no_error 4
isr_no_error 5
isr_no_error 6
isr_no_error 7
isr_error 8
isr_no_error 9
isr_error 10
isr_error 11
isr_error 12
isr_error 13
isr_error 14
isr_no_error 15
isr_no_error 16
isr_error 17
isr_no_error 18
isr_no_error 19
isr_no_error 20
isr_error 21
isr_no_error 22
isr_no_error 23
isr_no_error 24
isr_no_error 25
isr_no_error 26
isr_no_error 27
isr_no_error 28
isr_error 29
isr_error 30
isr_no_error 31


isr_common:
	pushq %rbp
	mov %rsp,%rbp
	pushq %rax
	pushq %rbx
	pushq %rcx
	pushq %rdx
	pushq %rsi
	pushq %rdi
	pushq %r8
	pushq %r9
	pushq %r10
	pushq %r11
	pushq %r12
	pushq %r13
	pushq %r14
	pushq %r15
	
	mov 8(%rbp),%rdi /*Vector No*/
	mov 16(%rbp),%rsi /*Error Code*/
	mov %rsp,%rdx /*Registers*/
	call isr_handler

	popq %r15
	popq %r14
	popq %r13
	popq %r12
	popq %r11
	popq %r10
	popq %r9
	popq %r8
	popq %rdi
	popq %rsi
	popq %rdx
	popq %rcx
	popq %rbx
	popq %rax
	popq %rbp
	add $0x16,%rsp
	iretq

.data
isr_functions:
isr_ptr 0
isr_ptr 1
isr_ptr 2
isr_ptr 3
isr_ptr 4 
isr_ptr 5 
isr_ptr 6 
isr_ptr 7 
isr_ptr 8 
isr_ptr 9 
isr_ptr 10 
isr_ptr 11
isr_ptr 12
isr_ptr 13
isr_ptr 14
isr_ptr 15
isr_ptr 16
isr_ptr 17
isr_ptr 18
isr_ptr 19
isr_ptr 20
isr_ptr 21
isr_ptr 22
isr_ptr 23
isr_ptr 24
isr_ptr 25
isr_ptr 26
isr_ptr 27
isr_ptr 28
isr_ptr 29
isr_ptr 30
isr_ptr 31
