inputString:
push rbp
mov rbp, rsp
mov rax, 0                   ; syscall number for sys_read
mov rdi, 0                   ; file descriptor 0 (stdin)
mov rsi, QWORD [rbp + 16]    ; buffer to store the character read
movsx rdx, DWORD [rbp + 24]  ; bytes to read
syscall
mov BYTE [rsi + rax - 1], 0
dec rax
leave
ret 12