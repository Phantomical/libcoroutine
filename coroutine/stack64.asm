; stack64.asm - Implements jmp_stack and init_stack for the x86-64 architecture

.CODE

; Overview of MS x64 calling convention
; Parameters:
;    Integer Params: 
;        rcx, rdx, r8, r9
;    Floating Point Params:
;        xmm0, xmm1, xmm2, xmm3
;    Volatile Registers:
;        rax, r10, r11, xmm4, xmm5
;    Non-volatile Registers:
;        All Other registers
;    Stack Pointer:
;        Aligned to 16 bytes (rsp)

; jmp_stack:
;    Switches between two stacks, saving all registers 
;    before changing the stack pointer
;    This function complies to the microsoft
;    x64 calling convention
; Arguments: (x64 calling convention)
;    RCX - new stack pointer (void*)
;    RDX - address of old stack pointer (void**)
@@jmp_stack proc
	; Save callee-save gp registers
	sub  rsp, 64  ; Allocate stack space
	mov  [rsp+56], rbx
	mov  [rsp+48], rsi
	mov  [rsp+40], rdi
	mov  [rsp+32], rbp
	mov  [rsp+24], r12
	mov  [rsp+16], r13
	mov  [rsp+8],  r14
	mov  [rsp],    r15

	; Save callee-save xmm registers
	sub  rsp, 160 ; Allocate stack space
	movdqu [rsp+144], xmm15
	movdqu [rsp+128], xmm14
	movdqu [rsp+112], xmm13
	movdqu [rsp+96],  xmm12
	movdqu [rsp+80],  xmm11
	movdqu [rsp+64],  xmm10
	movdqu [rsp+48],  xmm9
	movdqu [rsp+32],  xmm8
	movdqu [rsp+16],  xmm7
	movdqu [rsp],     xmm6

	; Actual code to switch the stacks
	mov  [rdx], rsp  ; Save stack pointer to provided address
	mov  rsp, rcx    ; Set stack pointer to given new stack pointer

	; Restore xmm registers
	movdqu [rsp],     xmm6
	movdqu [rsp+16],  xmm7
	movdqu [rsp+32],  xmm8
	movdqu [rsp+48],  xmm9
	movdqu [rsp+64],  xmm10
	movdqu [rsp+80],  xmm11
	movdqu [rsp+96],  xmm12
	movdqu [rsp+112], xmm13
	movdqu [rsp+128], xmm14
	movdqu [rsp+144], xmm15
	add  rsp, 160

	; Restore General pupose registers
	pop  r15
	pop  r14
	pop  r13
	pop  r12
	pop  rbp
	pop  rdi
	pop  rsi
	pop  rbx

	ret
@@jmp_stack endp

; init_stack:
;   Initializes the stack for the coroutine
;   to perform the initial switch
;   This procedure complies to microsoft x64
;   calling convention
; Arguments:
;   RCX - A pointer to a struct of type tmpinfo (tmpinfo*)
;   RDX - A pointer to a stack pointer (void**)
@@init_stack proc
	mov  r8,  [rcx]    ; Load the stack pointer that we are to jump to
	mov  r9,  [rcx+8]  ; Load the function pointer that we will call
	mov  rax, rcx      ; Save a copy of the tmpinfo pointer
	mov  rcx, r8       ; Set the stack pointer argument

	sub  rcx, 232      ; Add a bunch of free space to the coroutine stack
	                   ; so that when jmp_stack pops off a whole bunch
	                   ; of nonexistent variables we don't fall off the 
	                   ; bottom of the stack. 232 = 16*10 + 8*8 + 8

	mov  r10, coroutine_start
	mov [rcx+224], r10 ; Set the return address for jmp_stack
                       ; so that it returns to where it would
                       ; normally return in this function

	call @@jmp_stack   ; Branch off to our new coroutine
	                   ; This clobbers all non-volatile register
					   ; and jumps to coroutine_start when starting
					   ; the couroutine, returning as usual when
					   ; resumed from the coroutine

	ret

coroutine_start:       ; Our coroutine effectively starts here 
	                   ; (All non-volatile registers contain garbage)

	push rdx           ; Save return stack pointer address
	push r8            ; Save the coroutine stack pointer
	mov  rcx, rax      ; Load the tmpinfo pointer into rcx

	sub  rsp, 32       ; Allocate shadow space for parameters
	call r9            ; Call the coroutine function with rcx
	add  rsp, 32       ; Free the shadow space for the parameters

	pop  rdx           ; Retrieve coroutine stack pointer
	pop  r9            ; Retrieve old stack pointer
	sub  rdx, 8        ; Bring rdx back into the stack space

	mov  rcx, [r9]     ; Get the top of the original stack
	                   ; jmp_stack will clobber the bottom 
					   ; of the coroutine stack but it isn't
					   ; being used so this is fine

	call @@jmp_stack   ; Exit the coroutine

	int 3              ; If this gets executed then there is a bug in the program
@@init_stack endp

coroutine_jmp_stack proc
	jmp @@jmp_stack
coroutine_jmp_stack endp

coroutine_init_stack proc
	jmp @@init_stack
coroutine_init_stack endp

END
