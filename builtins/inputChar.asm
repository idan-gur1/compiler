inputChar:
push rbp
mov rbp, rsp
sub rsp, 1
mov rax, 0          ; syscall number for sys_read
mov rdi, 0          ; file descriptor 0 (stdin)
lea rsi, [rbp - 1]  ; buffer to store the character read
mov rdx, 1          ; bytes to read
syscall
movsx rax, BYTE [rbp - 1]
leave
ret