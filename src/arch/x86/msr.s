.code64
.text

.global readmsr
.global writemsr

readmsr:
	mov %rdi,%rcx
	rdmsr
	shl $32,%rdx
	or %rdx,%rax
	retq

writemsr:
	mov %rdi,%rcx
	mov %esi,%eax
	shr $32,%rsi
	mov %esi,%edx
	wrmsr
	retq
