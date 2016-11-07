; stack.asm - Implements jmp_stack and init_stack 

.CODE

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

; init_stack:
;   Initializes the stack for the coroutine
;   to perform the initial switch
; Arguments:
;   RCX - A pointer to a struct of type tmpinfo (tmpinfo*)
;   RDX - A pointer to a stack pointer (void**)
PROC @16init_stack
	mov  r8,  [rcx]    ; Load the stack pointer that we are to jump to
	mov  r9,  [rcx+8]  ; Load the function pointer that we will call
	mov  rax, rcx      ; Save a copy of the tmpinfo pointer
	mov  rcx, r8       ; Set the stack pointer argument

	add  rcx, 200      ; Add a bunch of free space to the stack
	                   ; so that when jmp_stack pops off a whole bunch
	                   ; of nonexistent variables we don't fall off the 
	                   ; bottom of the stack. 200 = 16*8 + 8*8 + 8

	mov  [rax], coroutine_start ; Set the return address for jmp_stack
                                ; so that it returns to where it would
                                ; normally return in this function

	call jmp_stack     ; Branch off to our new coroutine
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
	call r9            ; Call the coroutine function with rcx, rdx

	pop  rdx           ; Retrieve coroutine stack pointer
	pop  r9            ; Retrieve old stack pointer

	mov  rcx, [r9]     ; Get the top of the original stack
	                   ; jmp_stack will clobber the bottom 
					   ; of the coroutine stack but it isn't
					   ; being used so this is fine

	call jmp_stack     ; Exit the coroutine

	int 3              ; If this gets executed then there is a bug in the program
ENDP @16init_stack

END