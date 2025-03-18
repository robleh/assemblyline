public get_module_handle
public get_module_hash
public get_proc_address
public get_proc_hash

.code

; GetModuleHandleW case insensitive implementation
;
; Params:
;     rcx const wchar_t* name
; Return:
;     rax void* PLDR_DATA_TABLE_ENTRY->DllBase
;
get_module_handle proc
    ; Prolog
    push rdi
    push rsi
    push rbp
    mov rsi, rcx
    xor rax, rax
    xor rcx, rcx
    xor rbp, rbp
    xor rdx, rdx
    xor r10, r10
    xor r11, r11
    jmp count_string

    increment:
        add  bp, 2
        inc  cl

    count_string:
        mov  dx, word ptr [rsi + rcx * 2] ; Load utf-16 character at the given index
        test dx, dx                       ; Check for end of string
        jnz  increment
        mov r9b, cl

    ; Get the InLoadOrderModuleList 
    mov rdx, qword ptr gs:[60h]           ; PEB
    mov rdx, qword ptr [rdx + 18h]        ; Ldr
    add rdx, 10h                          ; Head of InLoadOrderModuleList
    mov r8, rdx                           ; Current entry

    get_next_module:
        cmp [r8], rdx                     ; Check if the next Flink circles back to the head
        je  not_found
        mov r8, [r8]                      ; Dereference InLoadOrderLinks to next entry

    compare_module_name:
        cmp  bp, word ptr [r8 + 58h]      ; Compare length of name to current BaseDllName.Length
        jne  get_next_module 
        mov  rdi, qword ptr [r8 + 60h]    ; BaseDllName.Buffer
        mov  cl,  r9b                     ; Reset loop
        sub  rdi, rsi

    compare_utf16_strings:
        mov r10w, word ptr [rsi]
        mov r11w, word ptr [rsi + rdi]
        ; Convert to upper
        cmp r10w, 61h
        jle convert_entry_upper
        sub r10w, 20h

    convert_entry_upper:
        cmp r11w, 61h
        jle compare_utf16
        sub r11w, 20h

    ; Compare 
    compare_utf16:
        cmp r10w, r11w
        jne get_next_module
        add rsi, 2
        loop compare_utf16_strings
        jmp found

    not_found:
        xor rax, rax

    ; Epilog (leaf function)
    epilog:
        pop rbp
        pop rsi
        pop rdi
        ret

    ; The module was found return its base address
    found:
        mov rax, [r8 + 30h]               ; DllBase
        jmp epilog
get_module_handle endp

; Retrieve the base of a module whose name matches the given hash
;
; Params:
;   rcx uint32 hash
;   rdx uint32 (*hasher)(const wchar_t*)
; 
; Return:
;   rax returns void* to DllBase
;
get_module_hash proc
    ; Prolog
    mov qword ptr [rsp + 8], rbx
    mov qword ptr [rsp + 10h], rbp
    mov qword ptr [rsp + 18h], rsi
    push rdi
    sub rsp, 20h

    ; Parse InLoadOrderModuleList
    mov rax, qword ptr gs:[60h]
    mov rsi, rdx
    mov ebp, ecx
    mov rdi, qword ptr [rax + 18h]
    add rdi, 10h
    mov rbx, qword ptr [rdi]
    jmp is_next_last

    next_entry:
        mov rcx, qword ptr [rbx + 60h]
        call rsi
        cmp ebp, eax
        je found
        mov rbx, qword ptr [rbx]

    is_next_last:
        cmp qword ptr [rbx], rdi
        jne next_entry
        xor eax, eax

    epilog:
        mov rbx, qword ptr [rsp + 30h]
        mov rbp, qword ptr [rsp + 38h]
        mov rsi, qword ptr [rsp + 40h]
        add rsp, 20h
        pop rdi
        ret

    found:
        mov rax, qword ptr [rbx + 30h]
        jmp epilog
get_module_hash endp

; GetProcAddress implementation
;
; Params:
;     rcx void* module
;     rdx const char* name
; 
; Return:
;     rax void* proc
;
get_proc_address proc
    ; Prolog
    mov rax,rsp                              ; Store stack pointer in rax
    ; Could these not just be push? is the opcode smaller?
    mov qword ptr [rax + 8], rbx             ; Store non-volatile registers on stack
    mov qword ptr [rax + 10h], rbp
    mov qword ptr [rax + 18h], rsi
    mov qword ptr [rax + 20h], rdi

    ; Parse PE headers
    movsxd rax, dword ptr [rcx + 3Ch]        ; e_lfanew in rax
    mov rbp, rdx                             ; name parameter to rbp
    mov r9, rcx                              ; module base in r9
    mov r8d, dword ptr [rax + rcx + 88h]     ; DATA_DIRECTORY.VirtualAddress
    xor eax, eax                             ; wipe lower bytes of rax
    add r8, rcx                              ; base + virtualaddress = ExportDirectory in r8
    mov edi, dword ptr [r8 + 20h]            ; offset 20h is RVA AddressOfNames in edi
    mov r11d, dword ptr [r8 + 1Ch]           ; offset 1Ch is RVA AddressOfFunctions in r11
    add rdi, rcx                             ; RVA AddressOfNames + base = Name table in rdi
    mov ebx, dword ptr [r8 + 24h]            ; offset 24h is RVA AddressOfNameOrdinals
    add r11, rcx                             ; RVA AddressOfFunction + base = Function table r11
    mov esi, dword ptr [r8 + 18h]            ; offset 18h is NumberOfNames
    add rbx, rcx                             ; RVA of AddressOfNameOrdinals + base = Ordinal table rbx
    test rsi, rsi                            ; check if number of names is 0
    je not_found                             ; if no named exports jump to not found

    name_loop:
        mov edx, dword ptr [rdi + rax * 4]   ; dereference name table index to get RVA to name
        mov rcx, rbp                         ; move name parameter into rcx
        add rdx, r9                          ; name RVA + base = name in rdx
        sub rdx, rbp                         ; subtract name parameter from current name in loop? this looks like an index maybe? or whoa... get the distance between the two strings in memory!? cool

    next_char:
        movzx r8d, byte ptr [rcx]            ; move first byte of name parameter into r8d
        movzx r10d, byte ptr [rcx + rdx]     ; move corresponding byte of current export name into r10d
        sub r8d, r10d
        jne not_equal                        ; if the characters are not equaljump to L2
        inc rcx                              ; increment the string index
        test r10d, r10d                      ; check if the export name is null-byte
        jne next_char                        ; if it's not the end of the string move to the next character

    not_equal:
        test r8d, r8d                        ; check for null-byte
        je found
        inc rax                              ; increment the name table index
        cmp rax, rsi                         ; check if index in name table is equal to the number of names in the table
        jb name_loop                         ; if it's still below move to next name in table

    not_found:
        xor eax, eax

    epilog:
        ; Could these not just be pop
        mov rbx, qword ptr [rsp + 8]         ; restore non-volatile registers from stack
        mov rbp, qword ptr [rsp + 10h]
        mov rsi, qword ptr [rsp + 18h]
        mov rdi, qword ptr [rsp + 20h]
        ret

    found:
        movzx eax, word ptr [rbx + rax * 2]
        mov eax, dword ptr [r11 + rax * 4]
        add rax, r9
        jmp epilog
get_proc_address endp

; Retrieve the address of proc whose name matches the given hash
;
; Params:
;   rcx void* module base
;   rdx uint32 hash
;   r8  uint32 (*hasher)(const wchar_t*)
;
; Return:
;   rax returns void* to proc
;
get_proc_hash proc
    ; Prolog
    mov qword ptr [rsp + 8], rbx
    mov qword ptr [rsp + 10h], rbp
    mov qword ptr [rsp + 18h], rsi
    push rdi
    push r12
    push r13
    push r14
    push r15
    sub rsp, 20h

    ; Parse EAT
    movsxd rax, dword ptr [rcx + 3Ch]
    xor ebx, ebx
    mov r12, r8
    mov r13d, edx
    mov rdi, rcx
    mov esi, dword ptr [rax + rcx + 88h]
    add rsi, rcx
    mov r15d, dword ptr [rsi + 20h]
    mov ebp, dword ptr [rsi + 1Ch]
    add r15, rcx
    mov r14d, dword ptr [rsi + 24h]
    add rbp, rcx
    add r14, rcx
    cmp dword ptr [rsi + 18h], ebx
    jbe not_found

    name_loop:
        mov ecx, dword ptr [r15 + rbx * 4]
        add rcx, rdi
        call r12
        cmp r13d, eax
        je found
        mov eax, dword ptr [rsi + 18h]
        inc rbx
        cmp rbx, rax
        jb name_loop

    not_found:
        xor eax, eax

    epilog:
        mov rbx, qword ptr [rsp + 50h]
        mov rbp, qword ptr [rsp + 58h]
        mov rsi, qword ptr [rsp + 60h]
        add rsp, 20h
        pop r15
        pop r14
        pop r13
        pop r12
        pop rdi
        ret

    found:
        movzx eax, word ptr [r14 + rbx * 2]
        mov eax, dword ptr [rbp + rax * 4]
        add rax, rdi
        jmp epilog
get_proc_hash endp

end
