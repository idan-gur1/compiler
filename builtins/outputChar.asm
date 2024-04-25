outputChar:
push rbp
mov rbp, rsp
mov rax, 1           ; syscall number for sys_write
mov rdi, 1           ; file descriptor 1 (stdout)
lea rsi, [rbp + 16]  ; pointer to the param
mov rdx, 1           ; bytes to write
syscall
leave
ret 1