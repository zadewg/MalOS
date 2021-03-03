;
; Global Descriptor Table
;

; we are in 32 bit mode, so:
;	db: 8  bits (1 byte)
; 	dw: 32 bits (2 bytes)
;	dd: 64 bits (4 bytes)
; 
; Note that the scrambled layout is not for pleasure.
; https://stackoverflow.com/questions/30253678/x86-segment-descriptor-layout-why-is-it-weird

; basic flat mode configuration.
;
; overlapping code and data segments covering the full 4GB of addressable memory
; no r/w/x protection, ring 0 privilege, no virtual memory

; FLAGS DESCRIPTION:
; 
; These are single bits, padding may be used to fit byte sizes.
;
; A)
; Present: 	          1 ( segment is present for memory ), 0 ( not present ) - this is used for VM
; Privilege: 		  0 ( ring zero - highest avaliable ), 1 ,2, 3
; Descriptor type: 	  1 ( code or data segment ), 0 ( used for traps )
; 
; B)
; Code: 		  1 ( code segment ), 0 ( data segment )
; CODE ONLY: Conforming:  1 ( segments with lower privilege may call code in this segment ), 0
; CODE ONLY: Readable: 	  1 ( readable - can read constants ), 0 ( execute only )
; DATA ONLY: Expand Down: 1 ( segment grows towards lower addresses ), 0 ( up ) - for stacks
; DATA ONLY: Writeable:   1 (segment may be written to ), 0 ( read only )
; Accessed:               0 (CPU sets this bit when it accesed the segment) 1
;
; C)
; Granularity: 		  1 ( multiplies limit by 16^3 ) 0xfffff -> 0xfffff000 ( 4GB ), 0 
; 32-bit default: 	  1 ( segment holds 32 bit code ), 0 ( 16 bit ) - sets data unit size for operations.
; 64-bit code segment: 	  Same as above, but for 64 bit systems. 
; AVL: 			  ignored by hardware, for software use.  


gdt_start :		; Entry point

gdt_null : 		; invalid null descriptor ( required by the CPU for sanity )
	dd 0x0 		
	dd 0x0


gdt_code :		; code segment descriptor
			; base=0x0 , limit=0xfffff ( when offsetted by granularity -> 4GB )
			; A) -> 1001b
			; B) -> 1010b
			; C) -> 1100b
	dw 0xffff 	; Limit (bits 0-15)
	dw 0x0 		; Base (bits 0-15)
	db 0x0 		; Base (bits 16-23)
	
	db 10011010b 	; A), B) Flags
	db 11001111b 	; C) flags ( Padded ), Limit ( bits 16-19)
	db 0x0 		; Base (bits 24-31)


gdt_data : 		; data segment descriptor
			; same as code segment, except:
			; B) -> 0010b
	dw 0xffff 	
	dw 0x0 		
	db 0x0 		
	db 10010010b 	
	db 11001111b 	
	db 0x0 		

gdt_end : 		; label used by assembler to calculate GDT size in the descriptor.


gdt_descriptor :
	dw gdt_end - gdt_start - 1 ; GDT ( always one less than true size )
	dd gdt_start 		   ; GDT Start address


; Segment descriptor offset constants.
; Each segment is 16 bytes, so: 0x0 -> NULL: 0x08 -> CODE: 0x10 -> DATA
; i.e, to reference the data segment, set DS (segment register) to 0x10
CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start
