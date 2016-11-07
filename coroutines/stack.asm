; stack.asm - Implements jmp_stack and init_stack 

.CODE

PROC save_gp_regs
	push rbx
	push rsi
	push rdi
	push rbp
	push r12
	push r13
	push r14
	push r15
	ret
ENDP save_gp_regs

PROC restore_gp_regs
	pop  rbx
	pop  rsi
	pop  rdi
	pop  rbp
	pop  r12
	pop  r13
	pop  r14
	pop  r15
	ret
ENDP restore_gp_regs

; jmp_stack:
;    Switches between two stacks, saving all registers 
;    before changing the stack pointer
; Arguments: (x64 calling convention)
;    RCX - new stack pointer (void*)
;    RDX - address of old stack pointer (void**)
PROC @16jmp_stack
	; Save callee-save xmm registers
	push xmm15
	push xmm14
	push xmm13
	push xmm12
	push xmm11
	push xmm10
	push xmm9
	push xmm8
	push xmm7
	push xmm6

	; Save callee-save gp registers
	push rbx
	push rsi
	push rdi
	push rbp
	push r12
	push r13
	push r14
	push r15

	; Actual code to switch the stacks
	mov  [rdx], rsp  ; Save stack pointer to provided address
	mov  rsp, rcx    ; Set stack pointer to given new stack pointer

	; Restore General pupose registers
	pop  r15
	pop  r14
	pop  r13
	pop  r12
	pop  rbp
	pop  rdi
	pop  rsi
	pop  rbx

	; Restore xmm registers
	pop  xmm6
	pop  xmm7
	pop  xmm8
	pop  xmm9
	pop  xmm10
	pop  xmm11
	pop  xmm12
	pop  xmm14
	pop  xmm15

	ret
ENDP @16jmp_stack


END