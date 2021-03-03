; load DH sectors to ES:BX from drive DL
disk_load :
	pusha
	push dx        ; Save requested number of sectors for later checks

	mov ah, 0x02   ; BIOS read sector function
	mov al, dh     ; Read DH sectors
	mov ch, 0x00   ; Select cylinder 0
	mov dh, 0x00   ; Select head 0
	mov cl, 0x02   ; Start reading from second sector ( 1st is the MBR )

	int 0x13       ; BIOS interrupt
	jc disk_error  ; catch error ( i.e. carry flag set )

	pop dx	       ; Restore DX from the stack
	cmp al, dh     ; if AL ( sectors read ) != DH ( sectors expected )
	jne sector_error 
	
	popa
	ret

error_traceback:
	call endl
	mov bx, FAULT_CODE_MSG
	call prints
	
	mov dh, ah	; ah=err code, dl=fault disk
	
	push dx
	mov dl, 0x00
	ror dx, 8
	call printh
	
	mov bx, FAULT_DISK_MSG
	call prints

	pop dx
	mov dh, 0x00
	call printh
	
	jmp $		; halt

disk_error :
	call endl	
	mov bx, DISK_ERROR_MSG
	call prints
	
	jmp error_traceback

sector_error:
	call endl
	mov bx, SECTOR_COUNT_ERROR_MSG
	call prints

	jmp error_traceback


; Variables
FAULT_CODE_MSG: db "Error code: ", 0
FAULT_DISK_MSG: db "  From disk: ", 0
DISK_ERROR_MSG: db "Disk read error", 0
SECTOR_COUNT_ERROR_MSG: db "Could not read all requested sectors", 0
