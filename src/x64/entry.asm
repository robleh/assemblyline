extrn entry:proc
public align_rsp

.code

; Start of PIC
_al segment byte read execute alias(".al$100") 'code'

; Make sure the stack is 16 byte aligned for XMM instructions
align_rsp proc
    push rsi
    mov rsi, rsp
    and rsp, 0fffffffffffffff0h
    int 3
    call entry
    mov rsp, rsi
    pop rsi
    ret
align_rsp endp

_al ends

end
