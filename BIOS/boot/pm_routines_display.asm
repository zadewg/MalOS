[bits 32] 			;32 bit protected mode

VIDEO_MEMORY equ 0xb8000 	; memory mapped display
WHITE_ON_BLACK equ 0x0f		; text mode character attributes		

; params: null terminated string pointed to by EDX
; string overwrites top left pixels of the display. No scrollling
print_string_pm :
	pusha
	mov edx , VIDEO_MEMORY 	; Set edx to the start of video memory mem.

; same logic as BIOS interrupt based real mode routine
print_string_pm_loop :
	mov al, [ebx] 		; ebx is the pointer to the string, load first character
	mov ah, WHITE_ON_BLACK 	
	
	cmp al, 0 		; catch null terminator
	je print_string_pm_done ; exit
	
	mov [edx], ax 		; Store char and attributes at current character cell.
				
	add ebx, 1 		; Increment EBX pointer.
	add edx, 2 		; Move to next cell in video memory.
	
	jmp print_string_pm_loop 

print_string_pm_done :
	popa
	ret 

