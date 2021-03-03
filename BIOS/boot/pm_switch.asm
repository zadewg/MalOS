[bits 16]			; still in 16-bit RM at this point

; Switch to protected mode
switch_to_pm :
	cli 			; critical steps ahead, so disable interrupts.

	lgdt [gdt_descriptor]  	; Load the GDT
	
	mov eax, cr0 		; Switch to protected mode setting cr0 register to 1
	or eax, 0x1 		; set to 1 
	mov cr0, eax		; cr0 cant be set directly

	jmp CODE_SEG:init_pm 	; Modern processors use pipelining, jmp far ahead 
				; ( into a different segment ) so the CPU flushes  
				; all pending operations. This ensures all following
				; code is executed in 32-bit protected mode

[bits 32]			; from now onwards we are in 32-bit mode

; Initialise registers and stack
init_pm :
	mov ax, DATA_SEG 	; memory segmentation now works differently, so update
	mov ds, ax 		; registers to point to the new descriptors in the GDT
	mov ss, ax		; our segments overlap and are not protected so either  
	mov es, ax		; is fine here.
	mov fs, ax
	mov gs, ax
	
	mov ebp, 0x90000 	; Set the stack
	mov esp, ebp 		
	
	call BEGIN_PM 		; go
