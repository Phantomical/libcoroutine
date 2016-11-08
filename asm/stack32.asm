; stack32.asm - Implements jmp_stack and init_stack for the x86 architecture

.MODEL FLAT, C
.CODE

; jmp_stack:
;    Switches between two stacks, saving all registers 
;    before changing the stack pointer
; Arguments: (__fastcall calling convention)
;    ECX - new stack pointer (void*)
;    EDX - address of old stack pointer (void**)
@jmp_stack@8 proc
	; Save gp registers
	push ebx
	push esi
	push edi
	push ebp

	; Actual code to switch the stacks
	mov  [edx], esp  ; Sate stack pointer to given address
	mov  esp, ecx    ; Set stack pointer to new stack pointer

	; Restore gp registers
	pop  ebp
	pop  edi
	pop  esi
	pop  ebx

	ret
@jmp_stack@8 endp

; init_stack:
;   Initializes the stack for the coroutine
;   to perform the initial switch
; Arguments:
;   ECX - A pointer to a struct of type tmpinfo (tmpinfo*)
;   EDX - A pointer to a stack pointer (void**)
@init_stack@8 proc
	push ebx
	push esi

	mov  eax, [ecx]    ; Load the stack pointer that we are to jump to
	mov  ebx, [ecx+4]  ; Load the function pointer that we will call
	mov  esi, ecx      ; Save a copy of the tmpinfo pointer
	mov  ecx, eax      ; Set the stack pointer argument
	
	sub  ecx, 20       ; Add a enough free space to the coroutine stack
	                   ; so that when jmp_stack pops off a whole bunch
	                   ; of nonexistent variables we don't fall off the 
	                   ; bottom of the stack. 20 = 4*4 + 4

	mov  [ecx+12], esi ; Save the tmpinfo pointer so that it is
	                   ; in the ESI register when the coroutine starts
	mov  [ecx+16], ebx ; Save the function pointer that we will call so
	                   ; that it is in the EBX register when the coroutine starts

	mov  esi, coroutine_start
	mov  [ecx+16], esi ; Set the return address for jmp_stack
                       ; so that it returns to where it would
                       ; normally return in this function

	pop  esi           ; Restore normal parameters so that jmp_stack can save them
	pop  ebx

	call @jmp_stack@8  ; Branch off to our new coroutine
	                   ; This clobbers all non-volatile register
					   ; and jumps to coroutine_start when starting
					   ; the couroutine, returning as usual when
					   ; resumed from the coroutine

	ret

coroutine_start:       ; Our coroutine effectively starts here 
	
	push edx           ; Save the return stack pointer address
	push eax           ; Save the coroutine stack pointer
	mov  ecx, eax      ; Load the tmpinfo pointer into ecx

	call esi           ; Call the coroutine function with ecx

	pop  eax           ; Retrieve coroutine stack pointer
	pop  edx           ; Retrieve old stack pointer
	sub  eax, 8        ; Bring coroutine stack pointer back into the stack space

	mov  ecx, [edx]    ; Get the top of the original stack
	                   ; jmp_stack will clobber the bottom 
					   ; of the coroutine stack but that
					   ; doesn't matter because it will not
					   ; be used again

	call @jmp_stack@8  ; Exit the coroutine

	int 3              ; Make sure that if jmp_stack actually returns we crash
@init_stack@8 endp

END
