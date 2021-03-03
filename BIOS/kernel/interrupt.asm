[extern isr_handler] ; isr.c
[EXTERN irq_handler] ; isr.c (make an irq.c for this one)

; Common ISR, IRQ stubs. Saves processor state, sets
; up for kernel mode segments, calls C-level fault handler,
; and restores the stack frame.
;
; Note that this context segment switching will only be relevant 
; when different privileges (user mode per say) are implemented. 
; At the moment only kernel runs.
isr_common_stub:
   pusha                    ; Pushes edi,esi,ebp,esp,ebx,edx,ecx,eax

   mov ax, ds               ; Lower 16-bits of eax = ds.
   push eax                 ; save the data segment descriptor

   mov ax, 0x10  	    ; load the kernel data segment descriptor; try extern DATA_SEG
   mov ds, ax
   mov es, ax
   mov fs, ax
   mov gs, ax
   push esp		    ; registers_t *r

   cld 			    ; C code following the sysV ABI requires DF 
			    ; to be clear on function entry
   call isr_handler

   pop eax	
   pop eax        	    ; reload the original data segment descriptor
  
   mov ds, ax
   mov es, ax
   mov fs, ax
   mov gs, ax

   popa                     ; Pops edi,esi,ebp...
   add esp, 8     	    ; Cleans up the pushed error code and pushed ISR number
   sti			    ; redundant
   iret           	    ; pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP  


irq_common_stub:
   pusha                    ; Pushes edi,esi,ebp,esp,ebx,edx,ecx,eax

   mov ax, ds               ; Lower 16-bits of eax = ds.
   push eax                 ; save the data segment descriptor

   mov ax, 0x10  	    ; load the kernel data segment descriptor; try extern DATA_SEG
   mov ds, ax
   mov es, ax
   mov fs, ax
   mov gs, ax
   push esp

   cld
   call irq_handler

   
   pop ebx        	    ; reload the original data segment descriptor
   pop ebx
   mov ds, bx
   mov es, bx
   mov fs, bx
   mov gs, bx

   popa                     ; Pops edi,esi,ebp...
   add esp, 8     	    ; Cleans up the pushed error code and pushed ISR number
   sti                      ; redundant 
   iret          	    ; pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP


; NASM macros (these expand a few hundred lines) , %1 accesses the first param
%macro ISR_ERRCODE 1
  [GLOBAL isr%1]
  isr%1:
    cli
    push byte %1
    jmp isr_common_stub
%endmacro

; can not common handler function without common stack frame, so push dummy err
%macro ISR_NOERRCODE 1 
  [GLOBAL isr%1]        
  isr%1:
    cli
    push byte 0
    push byte %1
    jmp isr_common_stub
%endmacro

; %1 -> IRQ number, %2 ISR number it is mapped to
%macro IRQ 2
  global irq%1
  irq%1:
    cli
    push byte 0
    push byte %2
    jmp irq_common_stub
%endmacro

; IRQs (Standard ISA)
IRQ 0, 32		      ; PIT Interrupt
IRQ 1, 33		      ; Keyboard Interrupt
IRQ 2, 34		      ; Cascade (used internally by the two PICs, never raised)
IRQ 3, 35		      ; COM2 (if enabled) 
IRQ 4, 36		      ; COM1 (if enabled)
IRQ 5, 37		      ; LPT2 (if enabled)
IRQ 6, 38		      ; Floppy Disk
IRQ 7, 39		      ; LPT1 / unreliable "supurious" interrupt (google race conditions on PIC)
IRQ 8, 40		      ; CMOS RTC (if enabled)
IRQ 9, 41		      ; Free for periphereals / legacy SCSI / NIC
IRQ 10, 42		      ; Free for periphereals / SCSI / NIC
IRQ 11, 43		      ; Free for periphereals / SCSI / NIC
IRQ 12, 44 		      ; PS2 mouse
IRQ 13, 45		      ; FPU / Coprocessor / Inter-processor
IRQ 14, 46	 	      ; Primary ATA Hard Disk
IRQ 15, 47		      ; Secondary ATA Hard Disk


; interrupts 8, [10, 14] , 17, 30 push error codes.
ISR_ERRCODE 8		      ; Double Fault Exception
ISR_ERRCODE 10		      ; Bad TSS Exception
ISR_ERRCODE 11		      ; Segment Not Present Exception
ISR_ERRCODE 12		      ; Stack-Segment Fault Exception
ISR_ERRCODE 13		      ; General Protection Fault Exception
ISR_ERRCODE 14		      ; Page Fault Exception
ISR_ERRCODE 17		      ; Alignment Check Exception
ISR_ERRCODE 30		      ; Security Exception

ISR_NOERRCODE 0		      ; Divide By Zero Exception 
ISR_NOERRCODE 1		      ; Debug Exception
ISR_NOERRCODE 2		      ; NMI Exception
ISR_NOERRCODE 3		      ; INT3 Breakpoint Exception
ISR_NOERRCODE 4		      ; INT0 Exception
ISR_NOERRCODE 5		      ; Out Of Bounds Exception
ISR_NOERRCODE 6		      ; Invalid Opcode Exception 
ISR_NOERRCODE 7		      ; Coprocessor Not Avaliable Exception

ISR_NOERRCODE 9		      ; Coprocessor Segment Overrun Exception

ISR_NOERRCODE 15	      ; Reserved
ISR_NOERRCODE 16              ; Floating Point Exception

ISR_NOERRCODE 18	      ; Machine Check Exception
ISR_NOERRCODE 19	      ; Reserved 
ISR_NOERRCODE 20	      ; Reserved
ISR_NOERRCODE 21	      ; Reserved
ISR_NOERRCODE 22	      ; Reserved
ISR_NOERRCODE 23	      ; Reserved
ISR_NOERRCODE 24	      ; Reserved
ISR_NOERRCODE 25	      ; Reserved
ISR_NOERRCODE 26	      ; Reserved
ISR_NOERRCODE 27	      ; Reserved
ISR_NOERRCODE 28	      ; Reserved
ISR_NOERRCODE 29	      ; Reserved

ISR_NOERRCODE 31	      ; Reserved

