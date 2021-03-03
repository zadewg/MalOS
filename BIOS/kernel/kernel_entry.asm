; ensures we jump into kmain
[bits 32]		; this code runs in PM
[extern kmain]		; reference for the linker (main is defined elsewhere)

call kmain
jmp $			; this should not run. Just in case, hang.


