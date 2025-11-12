; Note: To assemble, link, and run:
;   nasm -f elf64 -o q6.o q6.asm
;   ld -o q6 q6.o
;   ./q6

; === READ-ONLY DATA ===
section .data    ; Recall: Data = Initialized global variables
    FILEPATH: db "/usr/share/dict/words", 0
    LINES_TO_READ: equ 5


; === UNINITIALIZED GLOBAL VARS ===
section .bss 
    c: resb 1


; === SECTION FOR CODE ===
section .text
    global _start   ; Entry point for linker

_start:
    ; openat(AT_FDCWD, FILEPATH, O_RDONLY, 0)
    mov rax, 257    ; Syscall number for "openat"
    mov rdi, -100    ; Arg #1: AT_FDCWD
    mov rsi, FILEPATH    ; Arg #2: Pointer to file path
    mov rdx, 0    ; Arg #3: O_RDONLY
    mov r10, 0    ; Arg #4: Mode (not used for O_RDONLY)
    syscall    ; Make the system call

    ; "rax" now holds the return value of syscall (either fd or -1 for error)
    cmp rax, 0
    jl .exit_error    ; If rax < 0, jump to error/exit

    mov rbx, rax

    mov r12, LINES_TO_READ
    mov r13, 0    ; Current line_count

.read_loop:
    ; read(fd, &c, 1)
    mov rax, 0    ; Syscall number for "read"
    mov rdi, rbx    ; Arg #1: File descriptor
    mov rsi, c    ; Arg #2: Pointer to 1-byte buffer
    mov rdx, 1    ; Arg #3: Read 1 byte
    syscall

    cmp rax, 1
    jne .exit_success    ; If fd != -1, we are done reading

    ; Printing the character
    mov rax, 1
    mov rdi, 1
    mov rsi, c
    mov rdx, 1
    syscall

    mov al, [c]    ; Get the byte from memory into the 'al' register
    cmp al, 10    ; Check if char == '\n' (ASCII for '\n' is 10)
    jne .check_loop_end

    inc r13    ; Increment line_count if char == '\n'

.check_loop_end:    ; For checking if line_count == LINES_TO_READ
    cmp r13, r12
    je .exit_success
    jne .read_loop

.exit_success:
    ; close(fd)
    mov rax, 3
    mov rdi, rbx
    syscall

    ; exit(0)
    mov rax, 60
    mov rdi, 0
    syscall

.exit_error:
    ; exit(1)
    mov rax, 60
    mov rdi, 1
    syscall
