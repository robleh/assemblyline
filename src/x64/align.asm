; Author:       Matthew Graeber (@mattifestation)
; License:      BSD 3-Clause
; Syntax:       MASM
; Build Syntax: ml64 /c /Cx AdjustStack.asm
; Output:       AdjustStack.obj
; Notes: I really wanted to avoid having this external dependency but I couldn't
; come up with any other way to guarantee 16-byte stack alignment in 64-bit
; shellcode written in C.
; https://github.com/mattifestation/PIC_Bindshell/blob/master/PIC_Bindshell/AdjustStack.asm

EXTRN entry:PROC
PUBLIC align_stack

_TEXT SEGMENT

; AlignRSP is a simple call stub that ensures that the stack is 16-byte aligned prior
; to calling the entry point of the payload. This is necessary because 64-bit functions
; in Windows assume that they were called with 16-byte stack alignment. When amd64
; shellcode is executed, you can't be assured that you stack is 16-byte aligned. For example,
; if your shellcode lands with 8-byte stack alignment, any call to a Win32 function will likely
; crash upon calling any ASM instruction that utilizes XMM registers (which require 16-byte)
; alignment.

align_stack PROC
	push rsi
	mov rsi, rsp
	and rsp, 0FFFFFFFFFFFFFFF0h
	sub rsp, 020h
	call entry
	mov rsp, rsi
	pop rsi
	ret
align_stack ENDP

_TEXT ENDS

END
