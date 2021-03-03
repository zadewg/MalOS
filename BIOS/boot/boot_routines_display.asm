; while (string[i] != 0) { print string[i]; i++ }
_mains:
	; load string
	mov al, [bx] 	; base string address
	
	; check for null terminator
	cmp al, 0 
	je _exits

	; print char
	int 0x10

	; increment pointer
	add bx, 1
	jmp _mains

; exit function
_exits: 
	popa		; recover registers
	ret

; iterate through hex values converting them to ASCII
_hex2asciih:
	cmp cx, 4	; loop four times ( 16 bit values )
	je _exith

	mov ax, dx	; load param into working register ax
	and ax, 0x000f  ; isolate lsd ( will work with one digit at a time )

	add al, 0x30	; numeric ASCII: [0x30, 0x39] ( 0 to 9 )
	cmp al, 0x39	; if > 0x39 ( 9 ) not numeric
	jle _outstreamh ; else add it to output stream

	add al, 0x07	; alphabetic ASCII chars start at 0x40, 0x40-0x39=0x07
	;jmp _outstreamh ; add it to output stream

; populates the output stream 
; ( index (bx) = base addr (HEX_STR) + length (5) - current char index (cx)
_outstreamh:
	mov bx, HEX_STREAM + 5
	sub bx, cx

	mov [bx], al 	; copy current char to outstream at index bx
	ror dx, 4	; rotate one position to get the next digit

	add cx, 1	; increment counter
	jmp _hex2asciih	; get next character

; print and exit function
_exith:
	mov bx, HEX_STREAM
	call prints	; print output stream

	popa 		; recover registers
	ret		; exit
	
 
; print string function interface ( string passed in bx )
prints:
        pusha 		; save registers
        mov ah, 0x0e 	; int =10 / ah=0x0e -> BIOS tele-type output 
	jmp _mains

; print new line function interface ( no params )
endl:
	pusha		; save registers

	mov ah, 0x0e	; int=10 / ah=0x0e -| BIOS tele-type output
	mov al, 0x0a	; line feed \n
	int 0x10
	mov al, 0x0d	; carriage return \r
	int 0x10

	popa		; recover registers
	ret		; exit

; print hex function interace - wraps prints - ( value passed in dx )
printh:
	pusha		; save registers
	mov cx, 0	; iterator counter
	jmp _hex2asciih

HEX_STREAM: 
	db '0x0000', 0 ; ouput stream ( reserved memory )
